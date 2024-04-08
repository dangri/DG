/*

ZamLLN0.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamLLN0.hpp"

CZamLLN0::CZamLLN0()
{
}


CZamLLN0::~CZamLLN0()
{
}

int CZamLLN0::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}


int CZamLLN0::Lance()
{
	thread thr(&CZamLLN0::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamLLN0::Initialisation()
{
// Lire les valeurs de configuration
	m_Beh_stVal = Liti(m_LN + "Beh.stVal");
	m_Health_stVal = Liti(m_LN + "Health.stVal");
	m_Mod_ctlModel = Liti(m_LN + "Mod.ctlModel");
	m_Mod_stVal = Liti(m_LN + "Mod.stVal");
	m_NamPlt_ldNs = Lits(m_LN + "NamPlt.ldNs");
	m_NamPlt_swRev = Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = Lits(m_LN + "NamPlt.vendor");
	return true;
} // Initialisation

void CZamLLN0::ZamProc(int i)
{
	int err = 0;
	int ibeh, ihealth, imod;
	if (Initialisation())
	{
		while (true)
		{
// Lecture nouvelles valeurs
			ibeh = Liti(m_LN + "Beh.stVal");
			if (m_Beh_stVal != ibeh)
			{
				m_Beh_stVal = ibeh;
			}
			ihealth = Liti(m_LN + "Health.stVal");
			if (m_Health_stVal != ihealth)
			{
				m_Health_stVal = ihealth;
			}
			imod = Liti(m_LN + "Mod.stVal");
			if (m_Mod_stVal != imod)
			{
				Ecrit(m_LN + "Mod", "stVal", imod);
				ibeh = imod;
				Ecrit(m_LN + "Beh", "stVal", ibeh);
				m_Mod_stVal = imod;
				m_Beh_stVal = ibeh;
			}
// Traitements

// Delai
			this_thread::sleep_for(chrono::seconds(1));
		} // boucle
	}	 // if init
	else
	{
		m_zl->info(fmt::format(" {}: Init impossible", m_nom));
	}
	m_zl->info(fmt::format("{}: Arret du programme", m_nom));
} // ZamProcess

int CZamLLN0::Arret()
{
	int err = 0;

	return err;
} // Initialisation
