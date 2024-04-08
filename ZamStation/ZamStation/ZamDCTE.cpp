/*

ZamDCTE.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus
 industriels developpes en C++ standard et en Python par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements reposent sur les normes IEC et en particulier la norme IEC 61850
ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en oeuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850"
developpee et geree par MZ Automation GmbH (http://libiec61850.com).
La liste complete des sources est fournie dans le fichier "sources_zamiren.txt"
*/

#include "ZamDCTE.h"

CZamDCTE::CZamDCTE()
{
}

CZamDCTE::~CZamDCTE()
{
}

int CZamDCTE::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamDCTE::Lance()
{
	thread thr = thread(&CZamDCTE::ZamProc, this, 1);
	return 0;
}

bool CZamDCTE::Initialisation()
{
	m_Beh_stVal = m_zs->Liti(m_LN + "Beh.stVal");
	m_CeaEngzReqSt_stVal = m_zs->Liti(m_LN + "CeaEngzReqSt.stVal");
	m_EcpRef_setSrcRef = m_zs->Lits(m_LN + "EcpRef.setSrcRef");
	m_FctEna_stVal = m_zs->Litb(m_LN + "FctEna.stVal");
	m_HzHiLim_setMag_f = m_zs->Litf(m_LN + "HzHiLim.setMag.f");
	m_HzLoLim_setMag_f = m_zs->Litf(m_LN + "HzLoLim.setMag.f");
	m_NamPlt_swRev = m_zs->Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = m_zs->Lits(m_LN + "NamPlt.vendor");
	m_RtnDlTmms_setVal = m_zs->Liti(m_LN + "RtnDlTmms.setVal");
	m_RtnRmpTmms_setVal = m_zs->Liti(m_LN + "RtnRmpTmms.setVal");
	m_RtnSvcAuto_setVal = m_zs->Litb(m_LN + "RtnSvcAuto.setVal");
	m_VArLimPct_setMag_f = m_zs->Litf(m_LN + "VArLimPct.setMag.f");
	m_VHiLim_setMag_f = m_zs->Litf(m_LN + "VHiLim.setMag.f");
	m_VLoLim_setMag_f = m_zs->Litf(m_LN + "VLoLim.setMag.f");
	return true;
}

void CZamDCTE::ZamProc(int i)
{
	int err = 0;
	if (Initialisation())
	{
		while(true)
		{
			if (m_zp->m_stop) break;
// Lecture nouvelles valeurs

// Boucle de traitement

// Ecriture changements

// Delai
			sleep(1);
		} // boucle
	} // if Initialisation
	else
	{
		m_zl->info(fmt::format("Init impossible", m_nom));
	}
	m_zl->info(fmt::format("Arret du programme {}", m_nom));
}

int CZamDCTE::Arret()
{
	int err = 0;

	return err;
}
