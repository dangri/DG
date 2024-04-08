/*

ZamLDCS.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamLDCS.hpp"
#include "ZamFSCHP.hpp"
#include "ZamMMXU.hpp"
#include "ZamXSWI.hpp"
#include "ZamLPHD.hpp"
#include "ZamLLN0.hpp"
#include "ZamClient.hpp"
#include "ZamReportCC.hpp"
#include <signal.h>
#include <fmt/format.h>
#include "ZamDECP.h"
#include "ZamCalcul.h"
#include "ZamDVWC.h"
#include "ZamDPMC.h"
#include "ZamDWMX.h"

CZamLDCS::CZamLDCS()
{
}

CZamLDCS::~CZamLDCS()
{
}

int CZamLDCS::Demarrage ()
{
	try
	{
		m_zl->trace("Demarrage LDCS");
		m_IED = m_zp->m_nomied;
		m_LD = m_IED + m_nom + "/";
// pointeurs
		m_decp = static_pointer_cast<CZamDECP>(m_de["DECP1"]);
		m_fsch1 = static_pointer_cast<CZamFSCHP>(m_de["FSCH1"]);
		m_fsch2 = static_pointer_cast<CZamFSCHP>(m_de["FSCH2"]);
//		m_mmxu = static_pointer_cast<CZamMMXU>(m_de["MMXU1"]);
		m_xswi = static_pointer_cast<CZamXSWI>(m_de["XSWI1"]);
		m_xswi->m_zmp = m_zmp;
		m_lphd = static_pointer_cast<CZamLPHD>(m_de["LPHD1"]);
		m_lln0 = static_pointer_cast<CZamLLN0>(m_de["LLN0"]);
		m_dpmc = static_pointer_cast<CZamDPMC>(m_de["DPMC1"]);
		m_dvwc = static_pointer_cast<CZamDVWC>(m_de["DVWC1"]);
		m_dwmx = static_pointer_cast<CZamDWMX>(m_de["DWMX1"]);
		m_dwmx->m_ldcs = m_ldcs;
		// demarrages
		if (m_decp->Demarrage()) throw string("Pb lancement DECP");
		m_zl->trace("Lancement DECP");
		if (m_fsch1->Demarrage()) throw string("Pb lancement fsch1");
		m_zl->trace("Lancement FSCH1");
		if (m_fsch2->Demarrage()) throw string("Pb lancement fsch2");
		m_zl->trace("Lancement FSCH2");
//		if (m_mmxu->Demarrage()) throw string ("Pb lancement mmxu");
		if (m_xswi->Demarrage()) throw string ("Pb lancement xswi");
		m_zl->trace("Lancement XSWI");
		if (m_lphd->Demarrage()) throw string("Pb lancement lphd");
		m_zl->trace("Lancement LPHD");
		if (m_lln0->Demarrage()) throw string("Pb lancement lln0");
		m_zl->trace("Lancement LLN0");
		if (m_dpmc->Demarrage()) throw string("Pb lancement DPMC");
		m_zl->trace("Lancement DPMC");
		if (m_dvwc->Demarrage()) throw string("Pb lancement DVWC");
		m_zl->trace("Lancement DVWC");
// Lancement Calcul
		auto cal(make_shared<CZamCalcul>());
		cal->m_zu = shared_ptr<CZamUnit>(this);
		cal->m_zp = m_zp;
		cal->m_zm = m_zm;
		cal->m_zmp = m_zmp;
		cal->m_zs = m_zs;
		cal->m_nom = "ZamCalcul";
		cal->m_ldcs = m_ldcs;
		cal->m_zl = m_zl;
		m_cal = cal;
		if (m_cal->Demarrage()) throw string("Pb lancement ZamCalcul");
		m_zl->trace("Lancement Calcul"); 
		if (m_dwmx->Demarrage()) throw string("Pb lancement DWMX");
		m_zl->trace("Lancement DWMX");
// Lancement serveur WIFI UDP
		m_bloc_wifi = true;
		m_zl->trace("blocage wifi");
		auto cc(make_shared<CZamS118>());
		cc->m_ldcs = m_ldcs;
		cc->m_zp = m_zp;
		m_serv118 = cc;
		if (m_serv118->Demarrage()) throw string("Probleme au démarrage du serveur S118");
		m_zl->trace("Lancement serveur UDP");
		if (Lance ()) throw string ("Pb lancement boucle LDCS");
		m_zl->trace("Lancement LDCS");
	}
	catch (string exc)
	{
		m_zl->trace("Lancement LLN0");
		m_zl->info(fmt::format("LDCS : Demarrage {}", exc));
	}
	return 0;
}

int CZamLDCS::Lance ()
{
	m_stop= false;
	thread thr(&CZamLDCS::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamLDCS::Initialisation ()
{
	return true;
} // Initialisation

void CZamLDCS::ZamProc (int i)
{
	try
	{
		m_ibeh = 0;
		if (Initialisation ())
		{
			bool bfs1 = false;
			bool bfs2 = false;
			bool afs = false;
			bool bevse1 = false;
			bool bevse2 = false;
			float f;
			bool b;
			bool bena2 = false;
			// Beh
			if (m_ibeh != Liti(m_LD + "LLN0.Beh.stVal"))
			{
				m_ibeh = m_lln0->m_Beh_stVal;
				Ecrit(m_LD + "FSCH1.Beh", "stVal", m_ibeh);
				Ecrit(m_LD + "FSCH2.Beh", "stVal", m_ibeh);
				Ecrit(m_LD + "MMXU1.Beh", "stVal", m_ibeh);
				Ecrit(m_LD + "XSWI1.Beh", "stVal", m_ibeh);
			}
// OFF fonction inactive
			bool baf = false;
//			if (m_ibeh == 5) goto finboucle;
// fonction active
// inventaire des evse
			m_zl->trace("Connexion aux bornes");
			int id = 1;
			int p;
			m_evse_dispo = 0;
			for (auto evs = m_map_evse.begin(); evs != m_map_evse.end(); evs++)
			{
				auto spevse = evs->second;
				spevse->m_nom = evs->first;
				auto zc = make_shared<CZamClient>("IEDSE", spevse->m_adresse);
				//m_zl->trace(fmt::format("Tentative de connexion a la borne {}: ", spevse->m_adresse));
				int bcon = 0;
				bcon = (int) zc->getcons();
				if (bcon)
				{
					spevse->m_id = zc->getstring("IEDSELDEVSE/DESE1.EVSEId.setVal", "SP");
					m_zl->trace("Connexion d'un client a la borne : " + spevse->m_id);
					spevse->m_couleur = "vert";
					spevse->m_dispo = true;
//					LED(spevse);
					zc->decon();
					m_zl->trace("Deconnexion du client de la borne " + spevse->m_id);
					auto zrc(make_shared<CZamReportCC>("IEDSE", spevse->m_adresse));
					zrc->m_ldcs = m_ldcs;
					zrc->m_zl = m_zl;
					spevse->m_zrc = zrc;
					if (zrc->Demarrage()) throw string("pb de demarrage du rcb");
					m_zl->trace("Mise en place du RCB de connexion " + spevse->m_id + "/ev");
					m_zl->info(fmt::format("Borne {} connectee", spevse->m_nom));
					m_zl->info(fmt::format(" {} terminal has been connected", spevse->m_nom));
					m_evse_dispo += 1;
				}
				else
				{
					m_zl->info(fmt::format("Borne {} inactive", spevse->m_nom));
					m_zl->info(fmt::format(" {} idle terminal", spevse->m_nom));
				}
			}
			if (!m_evse_dispo) throw string("Pas de borne active");
			m_zl->info(fmt::format("{} bornes disponibles", m_evse_dispo));
			this_thread::sleep_for(chrono::milliseconds(500));
			afs = false;
			while (!bfs1 || !bfs2)
			{
				if (m_fsch1->m_change)
				{
					for (auto evs = m_map_evse.begin(); evs != m_map_evse.end(); evs++)
					{
						auto pev = evs->second;
						if (pev->m_dispo == true)
						{
							if (TransSchd(m_fsch1, pev->m_adresse, Repart(1))) throw string("Pb transfert fsch1 vers evse1");
							m_fsch1->m_change = false;
							pev->m_nbs += 1;
						}
					}
					bfs1 = true;
					m_zl->info("Chargement du fsch1 dans les bornes");
					m_zl->info("fsch1 is beeing loaded in the EVSEs");
				}
				if (m_fsch2->m_change)
				{
					for (auto evs = m_map_evse.begin(); evs != m_map_evse.end(); evs++)
					{
						auto pev = evs->second;
						if (pev->m_dispo == true)
						{
							if (TransSchd(m_fsch2, pev->m_adresse, Repart(1))) throw string("Pb transfert fsch2 vers evse1");
							m_fsch2->m_change = false;
							pev->m_nbs += 1;
						}
					}
					bfs2 = true;
					m_zl->info("Chargement du fsch2 dans les bornes");
					m_zl->info("fsch2 is beeing loaded in the EVSEs");
				}
// attente chargement des fsch
				m_bloc_wifi = false;
				m_zl->info("Deblocage wifi");
				m_zl->info("wifi has been unblocked");
				if(!afs) m_zl->info("Waiting Schedules loading");;
				afs = true;
				this_thread::sleep_for(chrono::seconds(1));
			}	// fschs
// commande FSCH2
			while (true)
			{
				if (m_fsch2->m_EnaReq_stVal != bena2)
				{
					m_zl->info(fmt::format("commande FSCH2= {}", to_string(!bena2)));
					m_zl->info(fmt::format("FSCH2  command= {}", to_string(!bena2)));
					bena2 = m_fsch2->m_EnaReq_stVal;
					for (auto evs = m_map_evse.begin(); evs != m_map_evse.end(); evs++)
					{
						auto pev = evs->second;
						if (pev->m_dispo == true)
						{
							CdeSEBool(pev->m_adresse, "IEDSELDEVSE/FSCH2.EnaReq", bena2);
							m_zl->trace(fmt::format("commande fsch2= {} sur l'adresse {}", to_string(bena2), pev->m_adresse));
						}
					}					
				}
				if (m_stop != 0) break;
				m_cal->calcul();
				m_dvwc->calmax();
				m_dpmc->dimax();

				this_thread::sleep_for(chrono::seconds(1));
			}
finboucle:
			m_zl->warn("Sortie de LDCS");
		} // if init
		else
		{
			m_zl->critical("Initialisation LDCS impossible");
		}
	}	// try
	catch (string exc)
	{
		m_zl->critical(fmt::format("LDCS : Exception: {}", exc));
	}
	catch (exception e)
	{
		m_zl->critical(fmt::format("LDCS : Exception: {}", e.what()));
	}
	catch (...)
	{
		m_zl->critical("LDCS : Exception");
	}
	m_zl->critical("LDCS : Arret du serveur");
} // ZamProcess

int CZamLDCS::TransSchd(shared_ptr<CZamFSCHP> fsch, const string &address, float ratio)
{
	int err = 0;
	int numentr;
	float valeur;
	try
	{
		numentr = fsch->m_NumEntr_setVal;
		int prio = fsch->m_SchdPrio_setVal;
		bool reuse = fsch->m_SchdReuse_setVal;
		int intv = fsch->m_SchdIntv_setVal;
		int younit = fsch->m_SchdIntv_units_SIUnit;
		unsigned long long tm0 = fsch->m_StrTm0_setTm;
		int t1hr = fsch->m_StrTm1_setCal_hr;
		int t1mn = fsch->m_StrTm1_setCal_mn;
		int t1otyp = fsch->m_StrTm1_setCal_occType;
		int t1oper = fsch->m_StrTm1_setCal_occPer;
		string sent;
		if (fsch == m_fsch1) sent = "IEDSELDEVSE/FSCH1.";
		if (fsch == m_fsch2) sent = "IEDSELDEVSE/FSCH2.";
		auto zc(make_shared<CZamClient>("IEDSE", address));
		zc->m_con = zc->getcon();
		if (!zc->m_con) throw string("Mauvaise connexion");
		zc->setint(sent + "NumEntr.setVal", "SP", numentr);
		zc->setint(sent + "SchdPrio.setVal", "SP", prio);
		zc->setbool(sent + "SchdReuse.setVal", "SP", reuse);
		zc->setint(sent + "SchdIntv.setVal", "SP", intv);
		zc->setutct(sent + "StrTm0.setTm", "SP", tm0);
		zc->setuint(sent + "StrTm1.setCal.hr", "SP", (unsigned int)t1hr);
		zc->setuint(sent + "StrTm1.setCal.mn", "SP", (unsigned int)t1mn);
		zc->setint(sent + "StrTm1.setCal.occType", "SP", t1otyp);
		zc->setint(sent + "StrTm1.setCal.occPer", "SP", t1oper);
		for (int k = 1; k <= numentr; k++)
		{
			zc->setfloat(sent + fmt::format("ValASG{:02d}.setMag.f",k), "SP", fsch->m_mapval[k] * ratio);
		}
		if (fsch == m_fsch1)
		{
			CdeSEBool(address, sent + "EnaReq", true);
		}
		CdeSEBool(address, sent + "DsaReq", true);
		zc->decon();
		m_zl->trace("Propagation de FSCH " + fsch->m_nom + " vers la borne " + address);
	}
	catch (string exc)
	{
		m_zl->info(fmt::format("LCDS: transfert schedules {}", exc));
		m_zl->info(fmt::format("LCDS: {}  schedules are beeing transfered", exc));
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("LCDS: transfert schedules {}", string(e.what())));
		m_zl->info(fmt::format("LCDS: {}  schedules are beeing transfered", e.what()));
	}
	catch (...)
	{
		m_zl->info("LCDS tranfert schedules: Exception inattendue");
	}
	return err;
}

float CZamLDCS::Repart(int k)
{
	// repartition entre les 3 evse
	float f;
	float tp1 = 0.03;
	float tp2 = 0.04;
	float tp3 = 0.05;
	float ratio = 1./3.;
	if (k == 1)
	{
		f = ratio * (1. - tp1);
	}
	if(k == 2)
	{
		f = (1 - ratio) * (1. - tp2);
	}
	if (k == 3)
	{
		f = (1 - ratio) * (1. - tp3);
	}
	return f;
}

int CZamLDCS::CdeSEBool(const string &address, const string &commande, bool b) 
{
	int err = 0;
	try
	{
		auto zc(make_shared<CZamClient>("IEDSE", address));
		zc->m_m = zc;
		zc->m_con = zc->getcon();
		if (!zc->m_con) throw string("Probleme de connexion au serveur");
		zc->cdeBool(commande, b);
		zc->decon();
	}
	catch (string exc)
	{
		m_zl->info(fmt::format("LDCS: Commande bool {}", exc));
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("cdeSEBool {}", string(e.what())));
	}
	catch (...)
	{
		m_zl->info("LCDS, cdeSEbool: Exception inattendue");
	}
	return err;
}

int CZamLDCS::Newve(const string &add_ve)
{
/// Connexion WIFI depuis adresse add_ve
	int err = 0;
	string srep;
	string sr;
	m_erreur = m_borne = m_sens = m_hdeb = m_hfin = m_energie = "";
	int p;
	m_borne_reserv.clear();
	m_zl->trace(fmt::format("Connexion wifi d'un véhicule adresse {}", add_ve));
	try
	{
		if (m_map_conve[add_ve] != nullptr) return -1;
		auto nve(make_shared<CZamConVE>(add_ve));
		m_map_conve[add_ve] = nve;
		auto zc(make_shared<CZamClient>("IEDEV", add_ve));
		zc->m_con = zc->getcon();
		if (!zc->m_con) throw string("Mauvaise connexion dans Newve");
		nve->m_EVId = zc->getstring("IEDEVLDEV1/DEEV1.EVId.setVal", "SP");
		nve->m_EMAId = zc->getstring("IEDEVLDEV1/DEEV1.EMAId.setVal", "SP");
		m_zl->trace(fmt::format("vehicule connecte: {}", nve->m_EMAId));
// Dialogue avec ICSMS
		m_zmp->Ecrit("int_RQSTConnect", nve->m_EMAId);
		m_zl->info("int_RQSTConnect de " + nve->m_EMAId);
		m_zl->info("int_RQSTConnect from " + nve->m_EMAId);
		m_rspcon.clear();
		while (true)
		{
			this_thread::sleep_for(chrono::milliseconds(200));
			srep = m_zmp->Lits("int_RSPConnect");
			if ((!srep.empty()) && (srep != "??????"))
			{
				break;
			}
		}
		m_zmp->Ecrit("int_RSPConnect", string("??????"));
		for (int n = 0; n < 100; n++)
		{
			
		}
		m_rspcon = srep;
		m_zl->info("Lecture reponse de ICSMS: " + srep);
		m_zl->info("answer from ICSMS beeing read: " + srep);
// Refus
		if (srep == "nok")
		{
			m_zl->trace(fmt::format("refus du vehicule {}", nve->m_EMAId));
			m_erreur = "connexion refusee";
			CAlm();
			zc->setstring("IEDEVLDEV1/DEEV1.AlmMsg.setVal", "SP", m_alm);
			m_zl->trace("Message au vehicule du refus de connexion");
			zc->decon();
			m_zl->trace("Deconnexion wifi du véhicule");
			for (auto k = m_map_evse.begin(); k != m_map_evse.end(); k++)
			{
				k->second->m_couleur = "rouge";
			}
			m_zl->trace("Retour de la foction Newve");
			return -1;
		}
// Borne non définie
		if (srep.substr(0,2) == "ok")
		{
			m_borne.clear();
			m_zl->trace("Borne non definie");
		}
		m_borne_reserv.clear();
		// Borne définie
		if (srep.substr(0, 4) == "evse")
		{
			m_borne = srep.substr(0, 5);
			m_borne_reserv = srep;
			m_zl->trace("Borne definie: " + m_borne);
		}
// Charge seule
		p = srep.find(',');
		m_sens = srep.substr(p + 1);
		m_zl->trace("sens: " + m_sens);
		sr = srep.substr(p + 1);
		p = sr.find(',');
		if (p == string::npos)
		{
			m_zl->trace("Pas de reservation");
			m_energie.clear();
		}
		else
		{
			m_sens = sr.substr(0, p);
			sr = sr.substr(p + 1, sr.length());
			p = sr.find(',');
			m_hdeb = sr.substr(0, p);
			sr = sr.substr(p + 1, sr.length());
			p = sr.find(',');
			m_hfin = sr.substr(0, p);
			sr = sr.substr(p + 1, sr.length());
			m_energie = sr;
			m_zl->trace("Reservation: m_sens=" + m_sens + " m_hdeb= " + m_hdeb + " mhfin= " + m_hfin + " energie= " + m_energie);
		}
		m_zl->trace(fmt::format("attente de branchement du vehicule {}", nve->m_EMAId));
		m_adcurrent = add_ve;
		CAlm();
		m_zl->info(fmt::format("{}", m_alm));
		zc->setstring("IEDEVLDEV1/DEEV1.AlmMsg.setVal", "SP", m_alm);
		m_zl->trace(fmt::format("Envoi message {} au vehicule {}", m_alm, add_ve));
		nve->m_id_evse = m_borne;
		for (auto k = m_map_evse.begin(); k != m_map_evse.end(); k++)
		{
			if (m_borne.empty())
			{

				k->second->m_couleur = "vert";
			}
			else
			{
				if (k->first == m_borne)
				{
					k->second->m_couleur = "vert";
					//	LED(k->second);
				}
				else
				{
					k->second->m_couleur = "rouge";
					//	LED(k->second);
				}
			}
		}
		zc->decon();
		m_zl->trace("Deconnexion du client du vehicule");
// Blocage des connexions WIFI
		m_bloc_wifi = true;
		m_zl->trace("Blocage du wifi");
	}
	catch (string exc)
	{
		m_zl->info(fmt::format("LCDS: newve {}", exc));
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("LDCS Newve : Exception : {}", string(e.what())));
	}
	catch (...)
	{
		m_zl->info("LCDS newveException inattendue");
	}
	return err;
}

int CZamLDCS::Conve(const string &add_se, const string& typ, bool b)
{
	int err = 0;
	string envoi;
	m_bad_connect = false;
	m_zl->trace("Reception de la modification de connexion de la borne: " + add_se);
	try
	{
		unsigned int h, mn;
		bool bon_ve = false;
		bool bcon = false;
		string add_ve;
		shared_ptr<CZamConVE> vecon;
		shared_ptr<CZamClient> zcve;
		string sidevse;
		string sibc;
//Connexion
		if (b)
		{
			if (b && (!m_bloc_wifi))
			{
				m_zl->trace("Non prise en compte de la connexion si les connexions wifi ne sont pas bloquees");
				return -1;
			}
			for (auto ive = m_map_conve.begin(); ive != m_map_conve.end(); ++ive)
			{
				vecon = ive->second;
				if (vecon->m_adresse_ve == m_adcurrent)
				{
					bon_ve = true;
					break;
				}
			}
			if (!bon_ve)
			{
				m_zl->err(fmt::format("Un vehicule non autorisé est branché sur la borne {}", add_se));
				return -1;
			}
			sibc = "bc" + add_se.substr(add_se.length() -1);
			vecon->m_adresse_evse = add_se;
			vecon->m_id_evse = m_map_evse_ad[add_se]->m_id;
			if (!m_borne_reserv.empty())
			{
				if (vecon->m_id_evse != m_borne_reserv)
				{
					m_zl->err(fmt::format("un vehicule est banché sur borne {} non prévue", add_se));
					return -1;
				}
			}
			vecon->m_typcon = typ;
			add_ve = vecon->m_adresse_ve;
			m_zl->trace("Branchement a la borne " + add_se + " du vehicule " + add_ve);
			auto zcv(make_shared<CZamClient>("IEDEV", add_ve));
			zcve = zcv;
			while (true)
			{
				zcve->m_con = zcve->getcon();
				if (zcve->m_con) break;
				this_thread::sleep_for(chrono::seconds(1));
			}
			auto zcse(make_shared<CZamClient>("IEDSE", add_se));
			while (true)
			{
				zcse->m_con = zcse->getcon();
				if (zcse->m_con) break;
				this_thread::sleep_for(chrono::seconds(1));
			}
			m_zl->trace("Connexion d'un client à la borne " + add_se);
			sidevse = zcse->getstring("IEDSELDEVSE/DESE1.EVSEId.setVal", "SP");
			vecon->m_id_evse = sidevse;
			m_zl->trace("Ecriture sur la borne " + add_se + " de l'adresse du vehicule " + add_ve);
			if (m_sens == "unload") zcse->setfloat("IEDSELDEVSE/DESE1.ChaPwrTgt.setMag.f", "SP", -1000.);
			if (m_sens == "load") zcse->setfloat("IEDSELDEVSE/DESE1.ChaPwrTgt.setMag.f", "SP", 1000.);
			if (typ == "AC") zcse->setstring("IEDSELDEVSE/DEAO1.EVRef.intAddr", "SP", add_ve);
			if (typ == "DC") zcse->setstring("IEDSELDEVSE/DEDO1.EVRef.intAddr", "SP", add_ve);
			zcse->decon();
			m_zl->trace("Deconnexion du client de la borne");
			vecon->m_EMAId = zcve->getstring("IEDEVLDEV1/DEEV1.EMAId.setVal", "SP");
			m_zl->trace("Lecture de l'identifiant du vehicule: " + vecon->m_EMAId);
			zcve->setstring("IEDEVLDEV1/DEEV1.EVSERef.intAddr", "SP", add_se);
			m_zl->trace("Ecriture sur le vehicule de l'adresse de la borne connectee: " + add_se);
			string sld = "LDSE" + vecon->m_id_evse.substr(4, 1);
			auto ldse = static_pointer_cast<CZamLDSE>(m_zp->m_du[sld]);
			ldse->m_ldse = ldse;
			vecon->m_ldse = ldse;
			vecon->m_ldse->m_adresse_ve = add_ve;
			vecon->m_ldse->m_adresse_se = add_se;
			vecon->m_ldse->m_zs = m_zs;
			vecon->m_ldse->m_zmp = m_zmp;
			vecon->m_ldse->m_inst = sld;
			vecon->m_ldse->m_cal = m_cal;
			vecon->m_ldse->m_zl = m_zl;
			vecon->m_ldse->m_ldcs = m_ldcs;
			vecon->m_ldse->m_energie = m_energie;
			if (vecon->m_ldse->Demarrage()) throw string("Pb demarrage urcb vehicule");
			m_zl->trace("Demarrage du LDSE" + vecon->m_id_evse.substr(4, 1));
			m_borne = vecon->m_id_evse;
			envoi = vecon->m_id_evse + "," + vecon->m_EMAId + "," + m_zp->Sheure();
			m_zmp->Ecrit("int_Connect", envoi);
			m_zl->info("Ecriture sur int_Connect de " + envoi);
			m_zl->info(fmt::format("{} is beeing written onto int_Connect", envoi));
			zcve->decon();
			m_zl->trace("Deconnexion du client du vehicule");
			m_zmp->Ecrit(sibc, 1);
			m_evse_dispo--;
			m_bloc_wifi = false;
			m_zl->info(fmt::format("{:01d} borne(s) disponibles, wifi debloque", m_evse_dispo));
			m_zl->info(fmt::format("{:01d} terminal(s) are available , wifi is  unblocked", m_evse_dispo));
		}
// Deconnexion
		else
		{
			if (m_bad_connect) return -1;
			for (auto ive = m_map_conve.begin(); ive != m_map_conve.end(); ++ive)
			{
				auto vecon = ive->second;
				if (vecon->m_adresse_evse == add_se)
				{
// Depart du vehicule
					vecon->m_bc = false;
					m_zl->trace("Deconnexion d'un vehicule de la borne " + add_se);
					sidevse = vecon->m_id_evse;
					if (sidevse == "EVSE1") m_zmp->Ecrit("bc1", 0);
					if (sidevse == "EVSE2") m_zmp->Ecrit("bc2", 0);
					if (sidevse == "EVSE3") m_zmp->Ecrit("bc3",0);
					auto zcv(make_shared<CZamClient>("IEDEV", vecon->m_adresse_ve));
					zcve = zcv;
					while (true)
					{
						zcve->m_con = zcve->getcons();
						if (zcve->m_con)
							break;
						this_thread::sleep_for(chrono::seconds(1));
					}
					m_zl->trace("Connexion d'un client sur le vehicule " + vecon->m_adresse_ve);
					vecon->m_adresse_evse.clear();
					m_zl->trace("Effacement de l'adresse de la borne dans le vehicule");
					zcve->setstring("IEDEVLDEV1/DEEV1.EVSERef.intAddr", "SP", vecon->m_adresse_evse);
					envoi = vecon->m_id_evse + "," + vecon->m_EMAId + "," + m_zp->Sheure();
					m_zmp->Ecrit("int_Disconnect", envoi);
					this_thread::sleep_for(chrono::milliseconds(500));
					m_erreur = "Merci de deconnecter la WIFI";
					CAlm();
					zcve->setstring("IEDEVLDEV1/DEEV1.AlmMsg.setVal", "SP", m_alm);
					m_zl->trace("Envoi de l'alarme de deconnexion au vehicule");
					m_zl->info(fmt::format("{}", envoi));
					while (true)
					{
						if (!zcve->etacon()) break;
						this_thread::sleep_for(chrono::seconds(1));
					}
					m_zl->info(fmt::format("VE {} deconnecte Wifi", add_ve ));
					m_zl->info(fmt::format("VE {} is deconnected from Wifi", add_ve ));
					vecon->m_typcon.clear(); 
					m_zl->info("Deconnexion du vehicule");
					m_zl->info("the  vehicle is beeing disconnected");
					m_evse_dispo += 1;
					m_zl->info(fmt::format("{} borne(s) disponibles", to_string(m_evse_dispo)));
					m_zl->info(fmt::format("{} available EVSE(s)", to_string(m_evse_dispo)));
				}
			}
		} // deconnect
	} // try
	catch (string exc)
	{
		m_zl->info(fmt::format("LCDS: Conve {}", exc));
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("LCDS: Conve ", string(e.what())));
	}
	catch (...)
	{
		m_zl->info("LCDS Conve: Exception inattendue");
	}
	return err;
}

int CZamLDCS::LED(shared_ptr<CZamevse> pevse)
{
	int err = 0;
	try
	{
		// TODO Un client de la borne doit allumer ou éteindre des LED sur celle-ci
		// ordres SPCS01 et SPCS02 de GGIO1
		if (!pevse->m_dispo) return -1;
		if (pevse->m_couleur == "vert")
		{
			CdeSEBool(pevse->m_adresse, "IEDSELDEVSE/GGIO1.SPCSO2", false);
			CdeSEBool(pevse->m_adresse, "IEDSELDEVSE/GGIO1.SPCSO1", true);
		}
		if (pevse->m_couleur == "rouge")
		{
			CdeSEBool(pevse->m_adresse, "IEDSELDEVSE/GGIO1.SPCSO1", false);
			CdeSEBool(pevse->m_adresse, "IEDSELDEVSE/GGIO1.SPCSO2", true);
		}
	}
	catch (string exc)
	{
		m_zl->info(fmt::format("LCDS: LED {}", exc));
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("LDCS LED : Exception : {}", string(e.what())));
	}
	catch (...)
	{
		m_zl->info("LCDS LED Exception inattendue");
	}
	return err;
}

void CZamLDCS::CAlm()
{
	int err = 0;
	m_alm.clear();
	if (!m_erreur.empty())
	{
		m_alm = "!error:" + m_erreur;
		return;
	}
	if (!m_borne.empty())
	{
		m_alm = "!evse:" + m_borne;
	}
	if (!m_sens.empty())
	{
		m_alm += "!action:" + m_sens;
	}
	if (!m_hdeb.empty())
	{
		m_alm += "!hbegin:" + m_hdeb;
	}
	if (!m_hfin.empty())
	{
		m_alm += "!hend:" + m_hfin;
	}
	if (!m_energie.empty())
	{
		m_alm += "!Wh:" + m_energie;
	}
	return;
}

