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
	auto zc(make_shared<CZamClient>("IEDSE", m_adresse));
	m_zc = zc;
	m_zc->m_con = m_zc->getcons();
	if (!m_zc->m_con)	throw string("Probleme connexion report");
	err = Lance();
	return err;


}

int CZamReport::Lance()
{
	m_stop = false;
	thread thr(&CZamReport::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamReport::Initialisation ()
{
	if (m_inst == "LDSE1")
	{
		m_ref = "IEDCSDLDSE1/";
	}
	if (m_inst == "LDSE2")
	{
		m_ref = "IEDCSDLDSE2/";
	}
	if (m_inst == "LDSE3")
	{
		m_ref = "IEDCSDLDSE3/";
	}
	return true;
} // Initialisation

void CZamReport::ZamProc(int i)
{
	if (Initialisation())
	{
		m_zc->m_ds.clear();
		string cible, sdo, sda;
		int pos;
		float wc;
		shared_ptr<CZamVarc> pv;
		int rep;
		int val;
		rep = m_zc->install_report(m_zc->m_ds, "IEDSELDEV/LLN0.RP.urcbev", "IEDSELDEV/LLN0.DSEV", false);
		if (rep) m_zl->info("not recognized rcb / rcb non reconnu");
		while (true)
		{
			if(m_stop)
			{
				break;
			}
			if (!m_zc->m_con)
			{
				m_zl->info(fmt::format("Report: deconnexion WIFI du vehicule {}", m_inst));
				m_zl->info(fmt::format("Report: WIFI is beeing disconnected from the vehicle {}", m_inst));
				return;
			}
			string setat;
			for (auto ipv = m_zc->m_ds.begin(); ipv != m_zc->m_ds.end(); ++ipv)
			{
				pv = *ipv;
				if (pv->m_change)
				{
					pos = pv->m_nom.find('/');
					cible = pv->m_nom.substr(pos + 1, string::npos);
					pos = cible.find('.');
					cible = cible.substr(pos + 1, string::npos);
					pos = cible.find('.');
					sdo = cible.substr(0, pos);
					if (sdo == "WhTgt")
					{
						if (pv->m_valfloat > 0.)
						{
							m_zs->Ecrit(m_ref + "DEEV1.WhTgt", "setMag.f", pv->m_valfloat);
							m_ldse->m_energie = fmt::format("{:03.0f}",pv->m_valfloat);
						}
					}
					if (sdo == "ConnTypSel")
					{
						m_zs->Ecrit(m_ref + "DEEV1.ConnTypSel", "stVal", pv->m_valint);
						goto end;
					}
					if (sdo == "WCurrent")
					{
						wc = pv->m_valfloat;
						m_zs->Ecrit(m_ref + "DEEV1.WCurrent", "mag.f", wc);
						goto end;
					}
					if (sdo == "EVSt")
					{
						m_zs->Ecrit(m_ref + "DEEV1.EVSt", "stVal", pv->m_valint);
						goto end;
					} // EVSt
					if (sdo == "Soc")
					{
						m_zs->Ecrit(m_ref + "DEEV1.Soc", "mag.f", pv->m_valfloat);
						goto end;
					}
					if (sdo == "EVId")
					{
						m_zs->Ecrit(m_ref + "DEEV1.EVId", "setVal", pv->m_valstring);
						goto end;
					}
					if (sdo == "EMAId")
					{
						m_zs->Ecrit(m_ref + "DEEV1.EMAId", "setVal", pv->m_valstring);
						goto end;
					}
					if (sdo == "DpTm")
					{
						m_zs->Ecrit(m_ref + "DEEV1.DpTm", "setTm", pv->m_valull);
						goto end;
					}
					if (sdo == "WChaTgt")
					{
						m_zs->Ecrit(m_ref + "DEEV1.WChaTgt", "setMag.f", pv->m_valfloat);
						goto end;
					}
					if (sdo == "SocWh")
					{
						m_zs->Ecrit(m_ref + "DSTO1.SocWh", "mag.f", pv->m_valfloat);
						goto end;
					}
					if (sdo == "WSpt")
					{
						m_zs->Ecrit(m_ref + "DSTO1.WSpt", "mxVal.f", pv->m_valfloat);
					}
				end:
					pv->m_change = false;
				}
				this_thread::sleep_for(chrono::milliseconds(500));
			}
			if (!m_zc->m_con)
			{
				m_zl->info(fmt::format("Report: deconnexion du client proxy {}",m_inst));
				return;
			}
			this_thread::sleep_for(chrono::seconds(1));
		}	// boucle
	} // if init
	else
	{
		m_zl->info("Report: Pb Initialisation du client proxy");
	}
//	m_c->decon();
} // ZamProc

