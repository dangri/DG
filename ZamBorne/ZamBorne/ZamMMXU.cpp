/*

ZamMMXU.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamMMXU.hpp"

CZamMMXU::CZamMMXU()
{
}

CZamMMXU::~CZamMMXU()
{
}

int CZamMMXU::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamMMXU::Lance()
{
	m_stop = false;
	thread thr(&CZamMMXU::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamMMXU::Initialisation()
{
//	m_Beh_stVal = Liti(m_LN + "Beh.stVal");
//	m_Hz_mag_f = Litf(m_LN + "Hz.mag.f");
//	m_Hz_units_SIUnit = Liti(m_LN + "Hz.units.SIUnit");
//	m_Hz_units_multiplier = Liti(m_LN + "Hz.units.multiplier");
//	m_NamPlt_swRev = Lits(m_LN + "NamPlt.swRev");
//	m_NamPlt_vendor = Lits(m_LN + "NamPlt.vendor");
	m_TotW_mag_f = Litf(m_LN + "TotW.mag.f");
//	m_TotW_units_SIUnit = Liti(m_LN + "TotW.units.SIUnit");
//	m_TotW_units_multiplier = Liti(m_LN + "TotW.units.multiplier");
	return true;
} // Initialisation

void CZamMMXU::ZamProc(int i)
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
			TotW = Litf(m_LN + "TotW.mag.f");
			
// Traitement
			
// Ecriture
			if (m_TotW_mag_f != TotW)
			{
				m_TotW_mag_f = TotW;
//				sem_wait(m_zp->m_sem);
				m_zp->m_zmp->Ecrit(m_LN + "TotW.mag.f", m_TotW_mag_f);
//				Update(m_zp->m_nombase, "TotW_mag_f", to_string(m_TotW_mag_f), to_string(m_zp->m_temps));
//				sem_post(m_zp->m_sem);
			}
// Delai
		finboucle:
			this_thread::sleep_for(chrono::seconds(1));
		}   // boucle
	} // if init
	else
	{
		m_zl->info(fmt::format(" {}: Init impossible", m_nom));
	}
	m_zl->info(fmt::format("{}: Arret du programme", m_nom));
} // ZamProcess

int CZamMMXU::Arret()
{
	int err = 0;
	m_stop = true;
	return err;
}
