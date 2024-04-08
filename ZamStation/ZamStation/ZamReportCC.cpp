/*

ZamReportCC.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamReportCC.hpp"
#include "ZamVal.hpp"
#include "ZamServeur.hpp"
#include "ZamLDCS.hpp"

CZamReportCC::CZamReportCC()
{
}

CZamReportCC::~CZamReportCC()
{
}

CZamReportCC::CZamReportCC(const string& name, const string& address)
{
	m_nom = name;
	m_adresse = address;
}

int CZamReportCC::Demarrage()
{
	int err = 0;
	m_zc = make_shared<CZamClient>(m_nom, m_adresse);
	m_zc->m_port = 102;
	err = Lance();
	return err;
}

int CZamReportCC::Lance()
{
	m_stop = false;
	thread thr(&CZamReportCC::Zamurcbcc, this, 1);
	thr.detach();
	return 0;
}

bool CZamReportCC::Initialisation ()
{
	return true;
} // Initialisation

void CZamReportCC::Zamurcbcc(int i)
{
	if (Initialisation())
	{
		m_zc->m_ds.clear();
		string cible, sdo, sda, typ;
		int pos, val;
		bool b;
		m_zc->getcons();
		sleep(1);
		if (!m_zc->etacon())
		{
			m_zl->info("ReportCC: mauvaise connexion du client à la borne");
		}
//		m_zc->getDataset(m_zc->m_ds, "IEDSELDEVSE/LLN0.DSCC");
		int rep = m_zc->install_report(m_zc->m_ds, m_nom + "LDEVSE/LLN0.RP.urcbcon", m_nom + "LDEVSE/LLN0.DSCC", false);
		if (rep) m_zl->info("not regonnized rcb / rcb non reconnu");
		while (true)
		{
			if(m_stop)
			{
				break;
			}
			if (!m_zc->m_con)
			{
				m_zl->info(fmt::format("ReportCC: deconnexion WIFI du vehicule {}", m_inst));
				m_zl->info(fmt::format("ReportCC: WIFI has been diconnected fom the  vehicle {}", m_inst));
//				m_ldcs->m_map_ve[m_zc->m_adresse] = nullptr;
				return;
			}
////			ClientReportControlBlock_setRptEna(m_zc->m_rcb, true);
			for (auto ipv = m_zc->m_ds.begin(); ipv != m_zc->m_ds.end(); ++ipv)
			{
//				shared_ptr<CZamVarc> pv;
				auto pv = *ipv;
				if (pv->m_change)
				{
					pos = pv->m_nom.find('/');
					cible = pv->m_nom.substr(pos + 1, string::npos);
					pos = cible.find('.');
					cible = cible.substr(pos + 1, string::npos);
					pos = cible.find('.');
					sdo = cible.substr(0, pos);
					val = pv->m_valint;
					typ = "AC";
					if (sdo == "PlgStDC") typ = "DC";
					b = false;
					if (pv->m_valint == 4) b = true;
					m_ldcs->Conve(m_adresse, typ, b);
					pv->m_change = false;
				}
			}
			if (!m_zc->m_con)
			{
				m_zl->info(fmt::format("ReportCC: deconnexion du client proxy {}", m_inst));
				return;
			}
			this_thread::sleep_for(chrono::seconds(1));
		}	// boucle
	} // if init
	else
	{
		m_zl->info("Pb Initialisation report");
	}
//	m_c->decon();
} // ZamProc

