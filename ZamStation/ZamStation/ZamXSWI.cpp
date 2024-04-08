/*

ZamXSWI.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamXSWI.hpp"

CZamXSWI::CZamXSWI()
{
}

CZamXSWI::~CZamXSWI()
{
}

int CZamXSWI::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamXSWI::Lance()
{
	m_stop = false;
	thread thr(&CZamXSWI::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamXSWI::Initialisation()
{
	m_Beh_stVal = Liti(m_LN + "Beh.stVal");
	m_NamPlt_swRev = Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = Lits(m_LN + "NamPlt.vendor");
	m_Pos_ctlModel = Liti(m_LN + "Pos.ctlModel");
	m_Pos_stVal = Liti(m_LN + "Pos.stVal");
	m_SwTyp_stVal = Liti(m_LN + "SwTyp.stVal");
	return true;
} // Initialisation

void CZamXSWI::ZamProc(int i)
{

	if (Initialisation())
	{
		string sv;
		bool Pos_ctlVal;
		int Pos_stVal;
		while (true)
		{
			if (m_stop)
			{
				return;
			}
// OFF fonction inactive
			if (m_Beh_stVal == 2) goto finboucle;
// Fonction active
// Lecture
			m_Pos_stVal = Liti(m_LN + "Pos.stVal");
			m_zmp->Ecrit(m_LN + "Pos.stVal", m_Pos_stVal);
// Traitement
// Ecriture
			Ecrit(m_LN + "Pos", "stVal", m_Pos_stVal);
// Delai
		finboucle:
			this_thread::sleep_for(chrono::seconds(1));
		}   // boucle
	} // if init
	else
	{
		m_zl->info(fmt::format("Init impossible", m_nom));
	}
	m_zl->info(fmt::format("Arret du programme {}", m_nom));
} // ZamProcess


int CZamXSWI::Arret()
{
	int err = 0;
	m_stop = true;
	return err;
}