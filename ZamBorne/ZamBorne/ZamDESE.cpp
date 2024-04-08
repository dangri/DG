/*

ZamDESE.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamDESE.hpp"
#include <thread>

CZamDESE::CZamDESE()
{
}


CZamDESE::~CZamDESE()
{
}

int CZamDESE::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamDESE::Lance()
{
	m_stop = false;
	thread thr(&CZamDESE::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDESE::Initialisation()
{
	m_Beh_stVal = m_zs->Liti(m_LN + "Beh.stVal");
	m_ChaA_mag_f = m_zs->Litf(m_LN + "ChaA.mag.f");
	m_ChaPwrLim_setMag_f = m_zs->Litf(m_LN + "ChaPwrLim.setMag.f");
	m_ChaPwrRtg_setMag_f = m_zs->Litf(m_LN + "ChaPwrRtg.setMag.f");
	m_ChaPwrTgt_setMag_f = m_zs->Litf(m_LN + "ChaPwrTgt.setMag.f");
	m_ChaV_mag_f = m_zs->Litf(m_LN + "ChaV.mag.f");
	m_ConnACRef_intAddr = m_zs->Lits(m_LN + "ConnACRef.intAddr");
	m_ConnACRef_setSrcRef = m_zs->Lits(m_LN + "ConnACRef.setSrcRef");
	m_ConnDCRef_intAddr = m_zs->Lits(m_LN + "ConnDCRef.intAddr");
	m_ConnDCRef_setSrcRef = m_zs->Lits(m_LN + "ConnDCRef.setSrcRef");
	m_EVSEId_setVal = m_zs->Lits(m_LN + "EVSEId.setVal");
	m_EVSENam_vendor = m_zs->Lits(m_LN + "EVSENam.vendor");
	m_Mod_stVal = m_zs->Liti(m_LN + "Mod.stVal");
	m_NamPlt_swRev = m_zs->Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = m_zs->Lits(m_LN + "NamPlt.vendor");
	return true;
} // Initialisation

void CZamDESE::ZamProc(int i)
{

	if (Initialisation())
	{
		float ChaA, ChaV, ChaPwrLim, ChaPwrTgt;
		while (true)
		{
			if (m_stop)
			{
				return;
			}
			ChaA = Litf(m_LN + "ChaA.mag.f");
			ChaV = Litf(m_LN + "ChaV.mag.f");
			ChaPwrLim = Litf(m_LN + "ChaPwrLim.setMag.f");
			ChaPwrTgt = Litf(m_LN + "ChaPwrTgt.setMag.f");

			// Ecriture
			if (m_ChaPwrLim_setMag_f != ChaPwrLim)
			{
				m_ChaPwrLim_setMag_f = ChaPwrLim;
			}
			if (m_ChaPwrTgt_setMag_f != ChaPwrTgt)
			{
				m_ChaPwrTgt_setMag_f = ChaPwrTgt;
			}
			if (m_ChaA_mag_f != ChaA)
			{
				m_ChaA_mag_f = ChaA;
			}
			if (m_ChaV_mag_f != m_ChaV)
			{
				m_ChaV_mag_f = ChaV;
			}
			
// Delai
			this_thread::sleep_for(chrono::seconds(1));
		}   // boucle
	} // if init
	else
	{
		m_zl->info(fmt::format(" {}: Init impossible", m_nom));
	}
	 m_zl->info(fmt::format("{}: Arret du programme", m_nom));
}
