/*

ZamLDEV.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamLDEV.hpp"
#include "ZamC118.hpp"
#include "ZamLLN0.hpp"
#include "ZamLPHD.hpp"
#include "ZamDEEV.hpp"
#include "ZamFSCH.hpp"
#include "ZamFSCC.hpp"
#include "ZamXSWI.hpp"
#include "ZamDPMC.h"
#include "ZamDPMC.h"
#include "ZamDWMX.h"
#include <memory>
#include <fmt/format.h>
#include <csignal>
#include "ZamDSTO.h"



using namespace std;

CZamLDEV::CZamLDEV()
{
}

CZamLDEV::~CZamLDEV()
{
}

int CZamLDEV::Demarrage()
{
	m_IED = m_zp->m_nomied;
	m_LD = m_IED + m_nom + "/";
	m_lln0 = static_pointer_cast<CZamLLN0>(m_de["LLN0"]);	
	m_lphd = static_pointer_cast<CZamLPHD>(m_de["LPHD1"]);	
	m_deev = static_pointer_cast<CZamDEEV>(m_de["DEEV1"]);
	m_deev->m_zl = m_zl;
	m_deev->m_zmp = m_zmp;
	m_fsch1 = static_pointer_cast<CZamFSCH>(m_de["FSCH1"]);	
	m_fsch2 = static_pointer_cast<CZamFSCH>(m_de["FSCH2"]);	
	m_fscc = static_pointer_cast<CZamFSCC>(m_de["FSCC1"]);
	m_xswi = static_pointer_cast<CZamXSWI>(m_de["XSWI1"]);
	m_dsto = static_pointer_cast<CZamDSTO>(m_de["DSTO1"]);
	m_dpmc = static_pointer_cast<CZamDPMC>(m_de["DPMC1"]);
	m_dpmc->m_ldev = m_ldev;
	m_dpmc->m_zl = m_zl;
	m_dwmx = static_pointer_cast<CZamDWMX>(m_de["DWMX1"]);
	m_dwmx->m_ldev = m_ldev;
	m_dwmx->m_zl = m_zl;
	m_dsto->m_zmp = m_zmp;
	m_dsto->m_ldev = m_ldev;
	m_dsto->m_deev = m_deev;
	if (m_lln0->Demarrage()) throw string("Erreur lancement lln0");
	if (m_deev->Demarrage()) throw string("Erreur lancement deev");
	if (m_dsto->Demarrage()) throw string("Erreur lancement dsto");
	if (Lance()) throw string("Erreur lancement de ldev");
	return 0;
}

int CZamLDEV::Lance()
{
	m_stop = false;
	thread thr(&CZamLDEV::ZamProc,this, 1);
	thr.detach();
	return 0;
}

bool CZamLDEV::Initialisation()
{
	return true;
} // Initialisation

void CZamLDEV::ZamProc(int i)
{
	if (Initialisation())
	{
		bool bcharge = false;
		bool bconnect = false;
		float f;
		int num;
		bool bcon;
		int numevse;
		int peract1;
		int peract2;
		float Wmax, Wact;
		float Enc;
		int evst;
		auto cc(make_shared<CZamC118>());
		cc->m_zp = m_zp;
		m_c118 = cc;
		m_c118->m_ldev = m_ldev;
		// Beh
		if (m_ibeh != Liti(m_LD + "LLN0.Beh.stVal"))
		{
			m_ibeh = m_lln0->m_Beh_stVal;
			Ecrit(m_LD + "FSCH1.Beh", "stVal", m_ibeh);
			Ecrit(m_LD + "FSCH2.Beh", "stVal", m_ibeh);
			Ecrit(m_LD + "FSCC1.Beh", "stVal", m_ibeh);
			Ecrit(m_LD + "DEEV1.Beh", "stVal", m_ibeh);
		}
// OFF fonction inactive
//		if(m_ibeh == 5) goto finboucle;
// fonction active
// Attente contact WIFI
		m_zl->info("Attente contact WIFI");
		m_zl->info("waiting WIFI contact");
		m_wifi = 0;
		if (m_c118->Demarrage()) throw string("Erreur demarrage client WIFI");
		while (!m_wifi)
		{
			this_thread::sleep_for(chrono::seconds(1));
		}
// Connexion
		m_zl->info("Connexion WIFI établie");
		m_zl->info("WIFI connected ");
		m_zmp->Ecrit("int_adresseEV", m_zp->m_adresseIP_wlan0);
		m_zl->info(fmt::format("Adresse Vehicule: {}", m_zp->m_adresseIP_wlan0));
		m_zl->info(fmt::format(" Vehicle address: {}", m_zp->m_adresseIP_wlan0));
		m_zp->m_pidzam = m_zp->m_zmp->Liti("int_PID");
// Attente branche cable
		this_thread::sleep_for(chrono::seconds(1));
		m_zl->info("Attente branchement du câble");
		m_zl->info("waiting connecting the cable");
		m_adresse_borne.clear();
		while (true)
		{
			m_adresse_borne = m_zs->Lits(m_LD + "DEEV1.EVSERef.intAddr");
			if (!m_adresse_borne.empty()) break;
			this_thread::sleep_for(chrono::seconds(1));
		}
		m_zl->info("Branchement du cable");
		m_zl->info(" cable connecting");
		m_zl->info("Connexion a la borne " + m_adresse_borne);
		m_zl->info("Connecting to the terminal " + m_adresse_borne);
		m_zp->m_zmp->Ecrit("int_adresseSE", m_adresse_borne);
		if (m_dpmc->Demarrage()) throw string("Erreur lancement dpmc");
		if (m_dwmx->Demarrage()) throw string("Erreur lancement dwmx");
		m_zl->info("Attente du message de la CS");
		m_zl->info("waiting message from CS");
		while (true)
		{
			string info = m_zs->Lits(m_LD + "DEEV1.AlmMsg.setVal");
			if (!info.empty()) break;
			this_thread::sleep_for(chrono::seconds(1));
		}
		DAlm();
		m_zl->info(fmt::format("Envoi de la CS: {}", m_alm));
		m_zl->info(fmt::format("sended by CS: {}", m_alm));
		m_bide = false;
		if (!m_energie.empty())
		{ 
			m_bide = true;
		}
		this_thread::sleep_for(chrono::seconds(2));
// Recevoir schedule 1 et lancer FSCH et FSCC
		if (m_fsch1->Demarrage()) throw string("Erreur lancement fsch1");
		if (m_fsch2->Demarrage()) throw string("Erreur lancement fsch2");
		if (m_fscc->Demarrage()) throw string("Erreur lancement fscc");
		bool bch;
		int comm = 0;
		bool boucle = false;
// Attendre que le FSCH soit opérationnel
		while (true)
		{
			if (m_zs->Litb(m_LD + "FSCH1.EnaReq.stVal")) break;
			this_thread::sleep_for(chrono::seconds(1));
		}
// amorce de la boucle de charge/decharge
		while (true)
		{
			if (m_zp->m_stop) break;
			if (m_wifi)
			{
				bcon = true; // la connexion est etablie
				while (bcon)
				{
					if (!boucle) // premier parcours de la boucle
					{
// Attente des fonctions de charge (python)
						while (comm < 2)	// comm est initialement égal à 0
						{
							comm = m_zp->m_zmp->Liti("int_ChargeMarche");
							this_thread::sleep_for(chrono::milliseconds(500));
						}
						m_wcha = 0;
						bch = true;
						if (comm == 3) bch = false;
						m_DptTm = m_zmp->Liti(m_IED + "LDEV1/DEEV1.DptTm.setTm");
						m_zs->Ecrit(m_LD + "DEEV1.DptTm", "setTm", m_DptTm);
						m_StartTm = m_zmp->Liti(m_IED + "LDEV1/DEEV1.StartTm.setTm");
						m_zs->Ecrit(m_LD + "DEEV1.StartTm", "setTm", m_StartTm);
// Déblocage des fonctions de charge/décharge (python)
						m_zmp->Ecrit("int_ChargeMarche", 1);
// Attendre que le véhicule soit en état de charge ou de décharge
						while (true)
						{
							evst = m_zmp->Liti("IEDEVLDEV1/DEEV1.EVSt.stVal");
							if (evst > 1) break;
						}
						m_wcha = 0.;
// Le véhicule est en état de charge/décharge - Attente de l'heure de début du contrat
						while (m_zp->m_temps < m_StartTm)
						{
							this_thread::sleep_for(chrono::milliseconds(500));
						}
						boucle = true; // le premier parcours de la boucle de charge/décharge est terminé
					}	// ! boucle
					m_energy = m_zmp->Litf("IEDEVLDEV1/DEEV1.WhTgt.setMag.f");
					m_zs->Ecrit("IEDEVLDEV1/DEEV1.WhTgt", "setMag.f", m_energy);
					Pmoyenne();
//					m_zl->info(fmt::format("LDEV: Puissance moyenne= {:03.0f} W", m_pmoy));
//					m_zl->info(fmt::format("LDEV: Energie cible = {:03.0f} Wh", m_energy));
// charge
					if (bch)
					{
						m_zp->m_sens = 1;
						ProgramC(); //détermine Wcha = Energie/temps de charge
						m_zmp->Ecrit(m_LD + "DEEV1.WChaTgt.setMag.f", m_Wcha);
						m_zs->Ecrit(m_LD + "DEEV1.WChaTgt", "setMag.f", m_Wcha);
						m_zs->Ecrit(m_LD + "DSTO1.WSpt", "mxVal.f", m_Wcha); // serveur IEC61850
//						m_zl->trace(fmt::format("m_Wcha= {:03.0f} W", m_Wcha));
//						this_thread::sleep_for(chrono::milliseconds(100));
					} // Charge
 // Decharge
					else
					{
						// calcul de puissance en décharge
						m_zp->m_sens = -1;
						ProgramD();
						m_zmp->Ecrit(m_LD + "DEEV1.WChaTgt.setMag.f", m_Wcha); // mémoire partagée
						m_zs->Ecrit(m_LD + "DEEV1.WChaTgt", "setMag.f", m_Wcha);	 // serveur IEC61850
						m_zs->Ecrit(m_LD + "DSTO1.WSpt", "mxVal.f", m_Wcha);		 // serveur IEC61850
//						this_thread::sleep_for(chrono::seconds(1));
					}
					evst = m_zmp->Liti("IEDEVLDEV1/DEEV1.EVSt.stVal");
					if (evst == 1)
					{
						bcon = false;
						m_zl->info("Arret de la charge/decharge");
						evst = 0;
					}
					this_thread::sleep_for(chrono::seconds(1));
				} // evst de 2 à 5
			} // if m_wifi
			while (true)
			{
				if (!m_zmp->Litb("int_wifi")) break;
				this_thread::sleep_for(chrono::seconds(1));
			}
			m_zl->trace("contact wifi perdu");
			m_zp->m_stop = true;
// delai
finboucle:
			this_thread::sleep_for(chrono::seconds(1));
		}	// while true
	} // if init
	else
	{
		m_zl->info(fmt::format("LDEV: Init impossible"));
	}
	m_zl->info(fmt::format("LDEV: Arret du serveur"));
} // LDEV

int CZamLDEV::Program()
{
	int erreur = 0;
	float WhTgt, chamax, chamin, pmoy, t_charge, h_charge, pmin, energy, perestf, hih_charge;
	int him_charge, peract, peresti, h_plus, numentr; 
	m_zs->Ecrit(m_LD + "FSCH2.SchdEntr", "stVal", 1);
	WhTgt = m_zmp->Litf(m_LD + "DEEV1.WhTgt.setMag.f");
// temps de charge (ull) em microsecondes
	t_charge = m_DptTm  - m_zp->m_temps;
// temps de charge en minutes
	h_charge = t_charge / 60000.;
// temps de charge arrondi en minutes arrondis
	him_charge = int(h_charge) +.5;
// temps de charge en heures
	hih_charge = him_charge / 60.;
	// puissance moyenne
	pmoy = WhTgt / hih_charge;
	numentr = 1;
	m_zs->Ecrit(m_LD + "FSCH2.SchdIntv", "setVal", him_charge); //  intervalle
	m_zs->Ecrit(m_LD + "FSCH2.NumEntr", "setVal", numentr);
	for (int k = 1; k <= numentr; k++)
	{
		m_zs->Ecrit(m_LD + fmt::format("FSCH2.ValASG{:02d}",k), "setMag.f", pmoy);
	}
	return erreur;
}

void CZamLDEV::DAlm()
{
	int p, q;
	string salm;
	m_alm = m_zs->Lits(m_LD + "DEEV1.AlmMsg.setVal");
	m_erreur.clear() ;
	p = m_alm.find("!error");
	if (p != string::npos)
	{
		salm = m_alm.substr(p);
		p = salm.find(':');
		m_erreur = salm.substr(p + 1);
	}
	m_borne.clear();
	p = m_alm.find("!evse");
	if (p != string::npos)
	{
		salm = m_alm.substr(p);
		p = salm.find(':');
		q = salm.find('!');
		m_borne = salm.substr(p + 1, q-1);
	}
	m_sens.clear();
	p = m_alm.find("!action");
	if (p != string::npos)
	{
		salm = m_alm.substr(p + 1);
		p = salm.find(':');
		salm = salm.substr(p + 1);
		q = salm.find('!');
		m_sens = salm.substr(0, q);
	}
	m_hdeb.clear();
	p = m_alm.find("!hbegin");
	if (p != string::npos)
	{
		salm = m_alm.substr(p + 1);
		p = salm.find(':');
		salm = salm.substr(p + 1);
		q = salm.find('!');
		m_hdeb = salm.substr(0, q);
	}
	m_hfin.clear();
	p = m_alm.find("!hend");
	if (p != string::npos)
	{
		salm = m_alm.substr(p + 1);
		p = salm.find(':');
		salm = salm.substr(p + 1);
		q = salm.find('!');
		m_hfin = salm.substr(0, q);
	}
	m_energie.clear();
	p = m_alm.find("!Wh");
	if (p != string::npos)
	{
		salm = m_alm.substr(p + 1);
		p = salm.find(':');
		salm = salm.substr(p + 1);
		m_energie = salm;
	}
	return;
}

int CZamLDEV::Pmoyenne()
{
	int err = 0;
	int ihd, ihf, imd, imf;
	ihd = ihf = imd = imf = 0;
	m_pmoy = 0.;
	float duree;
	if (m_hdeb.empty())
	{
		ihd = imd = 0;
	}
	else
	{
		ihd = stoi(m_hdeb.substr(0, 2));
		imd = stoi(m_hdeb.substr(3));
	}
	if (m_hfin.empty())
	{
		ihf = imf = 0;
	}
	else
	{
		ihf = stoi(m_hfin.substr(0, 2));
		imf = stoi(m_hfin.substr(3));
	}
	duree = ((ihf * 60 + imf) - (ihd * 60 + imd)) / 60.;
	if (duree == 0)
	{
		duree = (m_DptTm - m_StartTm) / 3600000.;
	}
	if (duree != 0)
	{
		m_pmoy = m_energy / duree;
	}
	else
	{
		m_pmoy = m_zmp->Litf("int_Wmax");
	}
	return err;
}

int CZamLDEV::ProgramD()
{
	int err = 0;
	bool bred = false;
	m_dpmc->dimax();
	m_dwmx->memax();
	m_Wmax = m_zmp->Litf("int_Wmax");
	m_Wmax *= m_zp->m_sens;
	if (abs(m_Wmax - m_Wmax0) > 1000.)
	{
//		m_zl->info(fmt::format("Puissance Max = {:03.0f} W", m_Wmax));
		m_Wmax0 = m_Wmax;
	}
	m_Wcha = - m_pmoy;
	if (m_Wcha < m_Wmax)
	{
		m_Wcha = m_Wmax;
		bred = true;
	}
	if (abs(m_Wcha - m_wcha) > 1000)
	{
		if (bred) m_zl->info(fmt::format("Puissance de decharge reduite= {:03.0f} W", m_Wcha));
		else m_zl->trace(fmt::format("Puissance de charge= {:03.0f} W", m_Wcha));
		m_wcha = m_Wcha;
	}
	this_thread::sleep_for(chrono::milliseconds(500));
	return err;
}

int CZamLDEV::ProgramC()
{
	int err = 0;
	bool bnorm = false;
	bool bred = false;
	bool bmax = false;
	m_dpmc->dimax();
	m_dwmx->memax();
	m_Wmax = m_zmp->Litf("int_Wmax"); // max determiné par la borne
	m_Wcha = m_pmoy;
	bnorm = true;
	if (m_Wcha > m_Wmax)
	{
		bred = true;
		bnorm = false;
		m_Wcha = m_Wmax;
	}
	if (abs(m_Wcha - m_wcha) > 1000.)
	{
		if (bnorm) m_zl->info(fmt::format("LDEV: Puissance de charge= {:03.0f} W", m_Wcha));
		if (bred) m_zl->info(fmt::format("LDEV: Puissance de charge reduite = {:03.0f} W", m_Wcha));
		if (bmax) m_zl->info(fmt::format("LDEV: Puissance de charge libre max= {:03.0f} W", m_Wcha));
		m_wcha = m_Wcha;
	}
	return err;
}

string CZamLDEV::Sheure()
{
	string rep = "";
	unsigned long long seconde;
	unsigned int minute, heure;
	seconde = m_zp->m_temps / 1000;
	seconde = seconde % 86400;
	minute = seconde / 60;
	heure = minute / 60;
	minute = minute % 60;
	rep = fmt::format("{:02d}h{:02d}", heure, minute);
	return rep;
}
