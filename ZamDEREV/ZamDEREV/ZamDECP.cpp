/*

ZamDECP.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamDECP.h"

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
	m_ElcRefid_setVal = Lits(m_LN + "ElcRefid.setVal");
	m_GndSys_Oper_Check = Litui(m_LN + "GndSys.Oper.Check");
	m_GndSys_stVal = Liti(m_LN + "GndSys.stVal");
	m_InclEclRef_setSrcRef = Lits(m_LN + "InclEclRef.setSrcRef");
	m_InclEclRef_setTstRef = Lits(m_LN + "InclEclRef.setTstRef");
	m_InclEclRef_tstEna = Litb(m_LN + "InclEclRef.tstEna");
	m_RefFrm_setVal = Litb(m_LN + "RefFrm.setVal");
	m_VRefSet_setMag_f = Litf(m_LN + "VRefSet.setMag.f");
	return true;
} // Initialisation

void CZamDECP::ZamProc(int i)
{

	if (Initialisation())
	{
		float TotW;
		while (true)
		{
			if (m_zp->m_stop)
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
			this_thread::sleep_for(chrono::seconds(1));
		} // boucle
	}	 // if init
	else
	{
		m_zl->info(fmt::format("DECP: Init impossible {}", m_nom));
	}
	m_zl->info(fmt::format("DECP: Arret du serveur {}", m_nom));
} // ZamProcess

int CZamDECP::Arret()
{
	int err = 0;
	m_stop = true;
	return err;
}
