/*

ZamEVSE.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamEVSE.h"
#include "ZamFSCC.hpp"
#include "ZamFSCH.hpp"
#include "ZamDEAO.hpp"
#include "ZamDEDO.hpp"
#include "ZamDESE.hpp"
#include "ZamClient.hpp"
#include "ZamReport.hpp"
#include "ZamLPHD.hpp"
#include "ZamLLN0.hpp"
#include "ZamLDEV.hpp"
#include "ZamDEEVP.hpp"
#include "ZamReportena.h"
#include <fmt/format.h>
#include "ZamDPMC.h"
#include "ZamDWMX.h"

CZamEVSE::CZamEVSE()
{
}

CZamEVSE::~CZamEVSE()
{
}

int CZamEVSE::Demarrage()
{
	try
	{
		m_IED = m_zp->m_nomied;
		m_LD = m_IED + m_nom + "/";
		m_lphd = static_pointer_cast<CZamLPHD>(m_de["LPHD1"]);
		m_lln0 = static_pointer_cast<CZamLLN0>(m_de["LLN0"]);
		if (m_lphd->Demarrage()) throw string("Pb lancement lphd");
		if (m_lln0->Demarrage()) throw string("Pb lancement lln0");
		m_dpmc = static_pointer_cast<CZamDPMC>(m_de["DPMC1"]);
		m_dpmc->m_zp = m_zp;
		m_dpmc->m_zl = m_zl;
		m_dwmx = static_pointer_cast<CZamDWMX>(m_de["DWMX1"]);
		m_dwmx->m_evse = m_evse;
		m_dwmx->m_zp = m_zp;
		m_dwmx->m_zl = m_zl;
		m_fsch1 = static_pointer_cast<CZamFSCH>(m_de["FSCH1"]);
		if (m_fsch1->Demarrage()) throw string("Pb evse" + m_sid + " lancement fsch1");
		m_fsch2 = static_pointer_cast<CZamFSCH>(m_de["FSCH2"]);
		if (m_fsch2->Demarrage()) throw string("Pb evse" + m_sid + " lancement fsch2");
		this_thread::sleep_for(chrono::seconds(2));
		m_fscc = static_pointer_cast<CZamFSCC>(m_de["FSCC1"]);
		if (m_fscc->Demarrage()) throw string("Pb evse" + m_sid + "  lancement fschc");
		m_deao = static_pointer_cast<CZamDEAO>(m_de["DEAO1"]);
		m_deao->m_zp = m_zp;
		m_deao->m_zl = m_zl;
		m_deao->m_evse = m_evse;
		m_dedo = static_pointer_cast<CZamDEDO>(m_de["DEDO1"]);
		m_dedo->m_evse = m_evse;
		m_dedo->m_zp = m_zp;
		m_dedo->m_zl = m_zl;
		m_dese = static_pointer_cast<CZamDESE>(m_de["DESE1"]);
		m_dese->m_zp = m_zp;
		m_dese->m_zl = m_zl;
		if (m_deao->Demarrage()) throw string("Pb evse lancement deao");
		if (m_dedo->Demarrage()) throw string("Pb evse lancement dedo");
		if (m_dese->Demarrage()) throw string("Pb evse lancement dese");
		if (Lance()) throw string("Pb lancement evse" + m_sid);
	}
	catch (string exc)
	{
		m_zl->info(fmt::format("Exception dans {} Demarrage : {}", m_sid, exc));
	}
	return 0;
}

int CZamEVSE::Lance()
{
	m_stop = false;
	thread thr(&CZamEVSE::ZamProc, this, m_id);
	thr.detach();
	return 0;
}

bool CZamEVSE::Initialisation()
{
	return true;
}

void CZamEVSE::ZamProc(int i)
{
	try
	{
		if (Initialisation())
		{
			auto rep(make_shared<CZamReportena>());
			rep->m_zl = m_zl;
			float p = 0.;
			while (true)
			{
// Beh
				if (m_ibeh != Liti(m_LD + "LLN0.Beh.stVal"))
				{
					m_ibeh = m_lln0->m_Beh_stVal;
//					m_ldev->m_ibeh = m_ibeh;
					Ecrit(m_LD + "FSCH1.Beh", "stVal", m_ibeh);
					Ecrit(m_LD + "FSCH2.Beh", "stVal", m_ibeh);
					Ecrit(m_LD + "FSCC1.Beh", "stVal", m_ibeh);
					Ecrit(m_LD + "DEAO1.Beh", "stVal", m_ibeh);
					Ecrit(m_LD + "DEDO1.Beh", "stVal", m_ibeh);
				}
				rep->m_adresse = m_zp->m_adresse_cs;
				rep->m_nom_IEDR = "IEDCSD";
				rep->m_evse = m_evse;
				m_zl->info("Attente Station");
				m_zl->info("Waiting Station");
				rep->Demarrage();
				m_zl->info("Connexion a la Station");
				m_zl->info("Connecting to the Station");
				m_zp->m_connect_station = true;
// OFF fonction inactive
				if (m_ibeh == 5) goto finboucle;
// fonction active
			nouveau_vehicule:
				m_adresse_ve.clear();
				m_zp->m_connect_ve = false;
// Attente vehicule
				m_zl->info("Borne en attente de véhicule");
			    m_zl->info("Terminal is waiting a vehicle");
// Connexion VE (venant de Station)
				while (true)
				{
					m_adresse_ve = Lits(m_LD + "DEAO1.EVRef.intAddr");
					if (!m_adresse_ve.empty())
					{
						m_zl->info(fmt::format("Borne connectee en alternatif au vehicule {}", m_adresse_ve));
						m_zl->info(fmt::format("Terminal is  connected in AC  to the vehicle   10.3.141.131 {}", m_adresse_ve));
						break;
					}
					m_adresse_ve = Lits(m_LD + "DEDO1.EVRef.intAddr");
					if (!m_adresse_ve.empty())
					{
						m_zl->info(fmt::format("Borne connectee en continu au vehicule {}", m_adresse_ve));
						m_zl->info(fmt::format(" Terminal is  connected in DC  to the vehicle{}", m_adresse_ve));
						
						break;
					}
					this_thread::sleep_for(chrono::seconds(1));
				}
				p = Litf(m_LD + "DESE1.ChaPwrTgt.setMag.f");
				m_zp->m_sens = 1;
				if (p < 0.) m_zp->m_sens = -1;
				m_zp->m_connect_ve = true;
				m_zp->m_zmp->Ecrit("int_adresseEV", m_adresse_ve);
				m_zl->trace(fmt::format("Ecriture en MP de l'adresse du vehicule {}", m_adresse_ve));
				m_ldev->m_adresse_ve = m_adresse_ve;
				if (m_ldev->Demarrage()) throw string("Pb demarrage de LDEV");
// chargement du fsch1 dans le vehicule
				TransSchd("LDEVSE/FSCH1");
				m_zl->info(fmt::format("Envoi FSCH1 au vehicule {}", m_adresse_ve));
				m_zl->info(fmt::format("FSCH1 is sent to the  vehicle {}", m_adresse_ve));
				m_ref.clear();
				if (m_dpmc->Demarrage()) throw string("Pb evse" + m_sid + " lancement dpmc");
				if (m_dwmx->Demarrage()) throw string("Pb evse" + m_sid + " lancement dwmx1");
// charge/decharge
				while (true)
				{
					if (m_ibeh == 1) FschControl();
					m_ac = Liti("IEDSELDEVSE/DEAO1.PlgStAC.stVal");
					m_dc = Liti("IEDSELDEVSE/DEDO1.PlgStDC.stVal");
// suivi charge ou decharge
					if ((m_ac + m_dc) > +3)
					{
// toujours connecte
// calcul des courants-tensions à affiner CA, CC, mono, tri
//						m_dese->m_ChaV_mag_f = 220; // voir calculs DER
//						m_dese->m_ChaA_mag_f = m_ldev->m_deev->m_WCurrent / 220;
						FschControl();
						m_dpmc->dimax();
						m_dwmx->memax();
					}
					else
					{
						m_zl->info("Deconnexion du cable");
						m_zl->info("The cable is beeing disconnected");
						m_wh = m_ldev->m_wh;
						m_adresse_ve.clear();
						string deref = "";
						m_zp->m_zmp->Ecrit("int_adresseEV", m_adresse_ve);
						Ecrit(m_LD + "DEAO1.EVRef", "intAddr", deref);
						Ecrit(m_LD + "DEDO1.EVRef", "intAddr", deref);
						break;
					}
					this_thread::sleep_for(chrono::seconds(1));
				}
			finboucle:
				this_thread::sleep_for(chrono::seconds(1));
			} // boucle
		}	 // Init
		else
		{
			m_zl->info(fmt::format(" {}: Init impossible", m_nom));
		}
	} // try
	catch (string exc)
	{
		m_zl->info(fmt::format("Exception dans {} transfert schedules : {}", m_inst, exc));
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("Exception dans {} transfert schedules : {}", m_inst, string(e.what())));
	}
	catch (...)
	{
		m_zl->info(fmt::format("Exception dans {} Demarrage : {}", m_inst));
	}
	m_zl->info(fmt::format(" {}: Arret de EVSE {}", m_inst,m_sid));
}

int CZamEVSE::FschControl()
{
	int err = 0;
	string ref = m_fscc->m_ActSchdRef_stVal;
	if (m_ref != ref)
	{
		m_ref = ref;
		TransSchd(m_ref);
		m_zl->info(fmt::format("Copie de {} vers le vehicule connecte", m_ref));
		m_zl->info(fmt::format("{} is beeing copied towards the connected vehicle ", m_ref));
	}
	return err;
}

int CZamEVSE::TransSchd(const string& ref)
{
	int err = 0;
	int numentr;
	float valeur;
	try
	{
		numentr = Liti(m_IED + ref + ".NumEntr.setVal");
//		if (numentr == 0) throw string("FSCH Vide");
		int prio = Liti(m_IED + ref + ".SchdPrio.setVal");
		bool reuse = Liti(m_IED + ref + ".SchdReuse.setVal");
		int intv = Liti(m_IED + ref + ".SchdIntv.setVal");
		int younit = 84;
		unsigned long long tm0 = Liti(m_IED + ref + ".StrTm0.setTm");
		int t1hr = Liti(m_IED + ref + ".StrTm1.setCal.hr");
		int t1mn = Liti(m_IED + ref + ".StrTm1.setCal.mn");
		int t1otyp = Liti(m_IED + ref + ".StrTm1.setCal.occType");
		int t1oper = Liti(m_IED + ref + ".StrTm1.setCal.occPer");
		auto zc(make_shared<CZamClient>("IEDEV", m_adresse_ve));
		zc->getcons();
		string sent = "IEDEVLDEV1/FSCH1.";
		if (!zc->m_con) throw string("Mauvaise connexion dans TransSchd");
		zc->setint(sent + "NumEntr.setVal", "SP", numentr);
		zc->setint(sent + "SchdPrio.setVal", "SP", prio);
		zc->setbool(sent + "SchdReuse.setVal", "SP", reuse);
		zc->setint(sent + "SchdIntv.setVal", "SP", intv);
		zc->setutct(sent + "StrTm0.setTm", "SP", tm0);
		zc->setuint(sent + "StrTm1.setCal.hr", "SP", (unsigned int)t1hr);
		zc->setuint(sent + "StrTm1.setCal.mn", "SP", (unsigned int)t1mn);
		zc->setint(sent + "StrTm1.setCal.occType", "SP", t1otyp);
		zc->setint(sent + "StrTm1.setCal.occPer", "SP", t1oper);
		float f;
		for (int k = 1; k <= numentr; k++)
		{
			zc->setfloat(sent + fmt::format("ValASG{:02d}.setMag.f",k),"SP", Litf(m_IED + ref + fmt::format(".ValASG{:02d}.setMag.f",k)));
		}
		CdeSEBool(m_adresse_ve, sent + "EnaReq", true);
		CdeSEBool(m_adresse_ve, sent + "DsaReq", true);
 		zc->decon();
	}
	catch (string exc)
	{
		m_zl->info(fmt::format("Exception dans {} transfert schedules : {}", m_sid, exc));
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("Exception dans {} transfert schedules : {}", m_sid, string(e.what())));
	}
	catch (...)
	{
		m_zl->info(fmt::format("Exception dans {} Demarrage : {}", m_sid));
	}
	return err;
}

int CZamEVSE::CdeSEBool(const string &address, const string &commande, bool b)
{
	int err = 0;

	try
	{
		auto zc(make_shared<CZamClient>("IEDEV", address));
		zc->m_m = zc;
		zc->m_con = zc->getcon();
		if (!zc->m_con)
			throw string("Probleme de connexion au serveur");
		IedClientError er = zc->cdeBool(commande, b);
		if (er != IED_ERROR_OK) throw string("pb commande fsch1 du vehicule");
		zc->decon();
	}
	catch (string exc)
	{
		m_zl->info(fmt::format("Exception dans {} Demarrage : {}", m_sid, exc));
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("Exception dans {} Demarrage : {}", m_sid, string(e.what())));
	}
	catch (...)
	{
		m_zl->info(fmt::format("Exception dans {} Demarrage : {}", m_sid));
	}
	return err;
}

