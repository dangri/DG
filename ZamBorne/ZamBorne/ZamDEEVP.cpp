/*

ZamDEEVP.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamDEEVP.hpp"

CZamDEEVP::CZamDEEVP()
{
}

CZamDEEVP::~CZamDEEVP()
{
}

int CZamDEEVP::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;	
}

int CZamDEEVP::Lance()
{
//	m_zu->m_zp->Log(trace, "sys", "DEEVP", "Lancement du serveur " + m_nomc);
	thread thr(&CZamDEEVP::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDEEVP::Initialisation()
{
	//	entete des noms et identification du LD
	//	m_LD = m_nomp + m_nomc;
	//	m_sne = m_nomc + "_";
	m_EVSt = 98;
	return true;
} // Initialisation

void CZamDEEVP::ZamProc(int i)
{
	int err = 0;
	if (Initialisation())
	{
		int evst;
		float w, wh;
//		bool bcd, bd = false;
//		m_wh = 0;
//		m_w = 0;
//		unsigned long long t, t0;
//		t0 = 0;
//		string envoi;
		while (true)
		{
			if (m_stop)
			{
				return;
			}
			// Lecture
			evst = m_zs->Liti(m_LN + "EVSt.stVal");
			w = m_zs->Litf(m_LN + "WCurrent.mag.f");
			wh = m_zs->Litf(m_LN + "WhTgt.setMag.f");
// gerer les modifications
			if (m_EVSt != evst)
			{
				m_EVSt = evst;
//				if (!bcd)
//				{
//					bcd = true;
//					m_EVSt = evst;
//					t = m_zp->m_temps;
//					if (m_WCurrent != m_w)
//					{
//						m_wh = m_WCurrent * (t - t0);
//						m_wh /= 3600.;
//						m_wht += m_wh;
//						m_WCurrent = m_w;
//					}
//				}
			}
			if (m_WCurrent != w)
			{
				m_WCurrent = w;
			}
			if (m_WhTgt != wh)
			{
				m_WhTgt = wh;
//				m_zl->info(fmt::format("WhTgt = {:03.0f}", wh));
			}
			// Delai
			this_thread::sleep_for(chrono::seconds(1));
		}   // boucle
	} // if init
	else
	{
		m_zl->info("DEEVP: Init impossible");
	}
	m_zl->info("DEEVP : Arret du serveur ");
} // ZamProcess


