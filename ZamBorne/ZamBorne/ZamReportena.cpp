/*

ZamReportena.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamReportena.h"
#include "ZamVal.hpp"
#include "ZamEVSE.h"
#include "ZamFSCH.hpp"

CZamReportena::CZamReportena()
{
}

CZamReportena::~CZamReportena()
{
}

int CZamReportena::Demarrage()
{
	int err = 0;
	auto zc(make_shared<CZamClient>("IEDCSD", m_adresse));
	m_zc = zc;
	while (true)
	{
		m_zc->m_con = m_zc->getcons();
		if (m_zc->m_con) break;
		this_thread::sleep_for(chrono::milliseconds(500));
	}
	err = Lance();
	return err;
}

int CZamReportena::Lance()
{
	m_stop = false;
	thread thr(&CZamReportena::Zamurcbena, this, 1);
	thr.detach();
	return 0;
}

bool CZamReportena::Initialisation ()
{
	return true;
} // Initialisation

void CZamReportena::Zamurcbena(int i)
{
	if (Initialisation())
	{
		m_zc->m_ds.clear();
		string cible, sdo, sda;
		int pos;
//		m_zc->getDataset(m_zc->m_ds, "IEDEVLDEV1/LLN0.DSCS");
		shared_ptr<CZamVarc> pv;
		int	rep = m_zc->install_report(m_zc->m_ds, "IEDCSDLDCS/LLN0.RP.urcbena2", "IEDCSDLDCS/LLN0.DSENA2", false);
		if (rep) m_zl->info("rcb non reconnu");
		while (true)
		{
			if(m_stop)
			{
				break;
			}
			if (!m_zc->m_con)
			{
				m_zl->info(fmt::format("deconnexion WIFI du vehicule {}", m_inst));
				m_zl->info(fmt::format("vehicle is disconnecting{}", m_inst));
				return;
			}
			for (auto ipv = m_zc->m_ds.begin(); ipv != m_zc->m_ds.end(); ++ipv)
			{
				pv = *ipv;
				if (pv->m_change)
				{
//					pos = pv->m_nom.find('/');
//					cible = pv->m_nom.substr(pos+1, string::npos);
//					pos = cible.find('.');
//					cible = cible.substr(pos+1, string::npos);
//					pos = cible.find('.');
//					sdo = cible.substr(0, pos);
//					if (sdo == "EnaReq")
//					{
						m_evse->m_fsch2->m_EnaReq_stVal = pv->m_valbool;
//					}
//				end:
					pv->m_change = false;
				}
			}
			if (!m_zc->m_con)
			{
				m_zl->info(fmt::format("Zamreport deconnexion du client proxy {}", m_inst));
				return;
			}
			this_thread::sleep_for(chrono::seconds(1));
		}	// boucle
	} // if init
	else
	{
		m_zl->info(fmt::format("Zamreport deconnexion du client proxy {}", m_inst));
	}
} // ZamProc

