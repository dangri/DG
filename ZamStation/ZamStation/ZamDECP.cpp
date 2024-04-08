/*

ZamDECP.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamDECP.h"
#include <thread>

CZamDECP::CZamDECP()
{
}

CZamDECP::~CZamDECP()
{
}

int CZamDECP::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamDECP::Lance()
{
	m_stop = false;
	thread thr(&CZamDECP::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDECP::Initialisation()
{
	m_Beh_stVal = m_zs->Liti(m_LN + "Beh.stVal");
	m_ElcMsRef1_setSrcRef = m_zs->Lits(m_LN + "ElcMsRef1.setSrcRef");
	m_ElcRefid_setVal = m_zs->Lits(m_LN + "ElcRefid.setVal");
	m_GndSys_ctlModel = m_zs->Liti(m_LN + "GndSys.ctlModel");
	m_GndSys_stVal = m_zs->Liti(m_LN + "GndSys.stVal");
	m_NamPlt_swRev = m_zs->Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = m_zs->Lits(m_LN + "NamPlt.vendor");
	m_RefFrm_setVal = m_zs->Litb(m_LN + "RefFrm.setVal");
	m_zs->Ecrit(m_LN + "VMax", "setMag.f", 240.);
	m_VMax_setMag_f = m_zs->Litf(m_LN + "VMax.setMag.f");
	m_zs->Ecrit(m_LN + "VMin", "setMag.f", 220.);
	m_VMin_setMag_f = m_zs->Litf(m_LN + "VMin.setMag.f");
	m_zs->Ecrit(m_LN + "VRefSet", "setMag.f", 230.);
	m_VRefSet_setMag_f = m_zs->Litf(m_LN + "VRefSet.setMag.f");
	return true;
} // Initialisation

void CZamDECP::ZamProc(int i)
{

	if (Initialisation())
	{
		float TotW;
		while (true)
		{
			if (m_stop)
			{
				return;
			}
			// OFF fonction inactive
			if (m_Beh_stVal == 2)
				goto finboucle;
			// Fonction active
			// Lecture

			// Traitement

			// Ecriture

			// Delai
		finboucle:
			sleep(1);
		} // boucle
	}	 // if init
	else
	{
		m_zl->info(fmt::format("Init impossible", m_nom));
	}
	m_zl->info(fmt::format("Arret du programme {}", m_nom));
} // ZamProcess

int CZamDECP::Arret()
{
	int err = 0;
	m_stop = true;
	return err;
}
