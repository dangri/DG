/*

ZamDINV.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus
 industriels developpes en C++ standard et en Python par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements reposent sur les normes IEC et en particulier la norme IEC 61850
ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en oeuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850"
developpee et geree par MZ Automation GmbH (http://libiec61850.com).
La liste complete des sources est fournie dans le fichier "sources_zamiren.txt"
*/

#include "ZamDINV.h"

CZamDINV::CZamDINV()
{
}

CZamDINV::~CZamDINV()
{
}

int CZamDINV::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamDINV::Lance()
{
	thread thr(&CZamDINV::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDINV::Initialisation()
{
	m_ACTyp_setVal = m_zs->Liti(m_LN + "ACTyp.setVal");
	m_Beh_stVal = m_zs->Liti(m_LN + "Beh.stVal");
	m_InvDCLosAlm_stVal = m_zs->Litb(m_LN + "InvDCLosAlm.stVal");
	m_InvGriLosAlm_stVal = m_zs->Litb(m_LN + "InvGriLosAlm.stVal");
	m_NamPlt_swRev = m_zs->Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = m_zs->Lits(m_LN + "NamPlt.vendor");
	m_OutWSet_setMag_f = m_zs->Litf(m_LN + "OutWSet.setMag_f");
	m_VARtg_setMag_f = m_zs->Litf(m_LN + "VARtg.setMag_f");
	m_WRtg_setMag_f = m_zs->Litf(m_LN + "WRtg.setMag_f");
	return true;
}

void CZamDINV::ZamProc(int i)
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
			this_thread::sleep_for(chrono::seconds(1));
		} // boucle
	} // if Initialisation
	else
	{
		m_zl->info(fmt::format("Init impossible", m_nom));
	}
	 m_zl->info(fmt::format("Arret du programme {}", m_nom));
}

int CZamDINV::Arret()
{
	int err = 0;

	return err;
}
