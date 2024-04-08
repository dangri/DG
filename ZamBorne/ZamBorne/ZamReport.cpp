/*

ZamReport.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamReport.hpp"
#include "ZamVal.hpp"
#include "ZamServeur.hpp"

CZamReport::CZamReport()
{
}

CZamReport::~CZamReport()
{
}

int CZamReport::Demarrage()
{
	int err = 0;
	auto zc(make_shared<CZamClient>("IEDEV", m_adresse));
	m_zc = zc;
	m_zc->m_con = m_zc->getcon();
	if (!m_zc->m_con)	throw string("Probleme connexion report");
	err = Lance();
	return err;
}

int CZamReport::Lance()
{
	m_stop = false;
	thread thr(&CZamReport::Zamurcb01, this, 1);
	thr.detach();
	return 0;
}

bool CZamReport::Initialisation ()
{
	m_zs->Ecrit("IEDSELDEV/DEEV1.ConnTypSel", "stVal", m_zc->getint("IEDEVLDEV1/DEEV1.ConnTypSel.stVal", "ST"));
	m_zs->Ecrit("IEDSELDEV/DEEV1.DptTm", "setTm", m_zc->getutct("IEDEVLDEV1/DEEV1.DptTm.setTm", "SP"));
	string sc = m_zc->getstring("IEDEVLDEV1/DEEV1.EVId.setVal", "SP");
	m_zs->Ecrit("IEDSELDEV/DEEV1.EVId", "setVal", sc);
	string sc1 = m_zc->getstring("IEDEVLDEV1/DEEV1.EMAId.setVal", "SP");
	m_zs->Ecrit("IEDSELDEV/DEEV1.EMAId", "setVal", sc1);
	m_zs->Ecrit("IEDSELDEV/DEEV1.EVSt", "stVal", m_zc->getint("IEDEVLDEV1/DEEV1.EVSt.stVal", "ST"));
	m_zs->Ecrit("IEDSELDEV/DEEV1.Soc", "mag.f", m_zc->getfloat("IEDEVLDEV1/DEEV1.Soc.mag.f", "MX"));
	m_zs->Ecrit("IEDSELDEV/DEEV1.WChaTgt", "setMag.f", m_zc->getfloat("IEDEVLDEV1/DEEV1.WChaTgt.setMag.f", "SP"));
	m_zs->Ecrit("IEDSELDEV/DEEV1.WCurrent", "mag.f", m_zc->getfloat("IEDEVLDEV1/DEEV1.WCurrent.mag.f", "MX"));
	m_zs->Ecrit("IEDSELDEV/DEEV1.WhTgt", "setMag.f", m_zc->getfloat("IEDEVLDEV1/DEEV1.WhTgt.setMag.f", "SP"));

	m_zs->Ecrit("IEDSELDEV/DSTO1.SocWh","mag.f", m_zc->getfloat("IEDEVLDEV1/DSTO1.SocWh.mag.f", "MX"));
	m_zs->Ecrit("IEDSELDEV/DSTO1.WSpt", "mxVal.f", m_zc->getfloat("IEDEVLDEV1/DSTO1.WSpt.mxVal.f", "MX"));
	m_zs->Ecrit("IEDSELDEV/DSTO1.WhMaxRtg", "setMag.f", m_zc->getfloat("IEDEVLDEV1/DSTO1.WhMaxRtg.setMag.f", "SP"));
	m_zs->Ecrit("IEDSELDEV/DSTO1.WhMinRtg", "setMag.f", m_zc->getfloat("IEDEVLDEV1/DSTO1.WhMinRtg.setMag.f", "SP"));

	return true;
} // Initialisation

void CZamReport::Zamurcb01(int i)
{
	if (Initialisation())
	{
		m_zc->m_ds.clear();
		string cible, sdo, sda;
		int pos;
//		m_zc->getDataset(m_zc->m_ds, "IEDEVLDEV1/LLN0.DSCS");
		shared_ptr<CZamVarc> pv;
		int	rep = m_zc->install_report(m_zc->m_ds, "IEDEVLDEV1/LLN0.RP.urcb01", "IEDEVLDEV1/LLN0.DSEV", false);
		if (rep) m_zl->info("rcb non reconnu");
		while (true)
		{
			if(m_stop)
			{
				break;
			}
			if (!m_zc->m_con)
			{
				m_zl->info(fmt::format("vehicule is disconnecting from WIFI {}", m_inst));
				return;
			}
			if (m_stop)
			{
				break;
			}
			unsigned long long t0, t1;
			for (auto ipv = m_zc->m_ds.begin(); ipv != m_zc->m_ds.end(); ++ipv)
			{
				pv = *ipv;
				if (pv->m_change)
				{
					pos = pv->m_nom.find('/');
					cible = pv->m_nom.substr(pos+1, string::npos);
					pos = cible.find('.');
					cible = cible.substr(pos+1, string::npos);
					pos = cible.find('.');
					sdo = cible.substr(0, pos);
					if (sdo == "ConnTypSel")
					{
						m_zs->Ecrit("IEDSELDEV/DEEV1.ConnTypSel", "stVal", pv->m_valint);
					}
					if (sdo == "EVSt")
					{
						m_zs->Ecrit("IEDSELDEV/DEEV1.EVSt", "stVal", pv->m_valint);
					}
					if (sdo == "Soc")
					{
						m_zs->Ecrit("IEDSELDEV/DEEV1.Soc", "mag.f", pv->m_valfloat);
					}
					if (sdo == "WCurrent")
					{
						m_zs->Ecrit("IEDSELDEV/DEEV1.WCurrent", "mag.f", pv->m_valfloat);
					}
					if (sdo == "EVId")
					{
						m_zs->Ecrit("IEDSELDEV/DEEV1.EVId", "setVal", pv->m_valint);
					}
					if (sdo == "EMAId")
					{
						m_zs->Ecrit("IEDSELDEV/DEEV1.EMAId", "setVal", pv->m_valint);
					}
					if (sdo == "DpTm")
					{
						m_zs->Ecrit("IEDSELDEV/DEEV1.DpTm", "setTm", pv->m_valull);
					}
					if (sdo == "WhTgt")
					{
						m_zs->Ecrit("IEDSELDEV/DEEV1.WhTgt", "setMag.f", pv->m_valfloat);
					}
					if (sdo == "WChaTgt")
					{
						m_zs->Ecrit("IEDSELDEV/DEEV1.WChaTgt", "setMag.f", pv->m_valfloat);
					}
					if (sdo == "SocWh")
					{
						m_zs->Ecrit("IEDSELDEV/DSTO1.SocWh", "mag.f", pv->m_valfloat);
					}
					if (sdo == "WSpt")
					{
						m_zs->Ecrit("IEDSELDEV/DSTO1.WSpt", "mxVal.f", pv->m_valfloat);
					}
				end:
					pv->m_change = false;
				}
				this_thread::sleep_for(chrono::milliseconds(500));
			}
			if (!m_zc->m_con)
			{
				m_zl->info(fmt::format("Zamreport deconnexion du client proxy {}",m_inst));
				return;
			}
			this_thread::sleep_for(chrono::seconds(1));
		}	// boucle
	} // if init
	else
	{
		m_zl->info("ZamReport: Pb Initialisation du client proxy");
	}
//	m_c->decon();
} // ZamProc

