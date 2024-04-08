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
	m_zs->Ecrit(m_LN + "Hz", "mag.f", 0);
	m_Beh_stVal = m_zs->Liti(m_LN + "Beh.stVal");
	m_Hz_mag_f = m_zs->Litf(m_LN + "Hz.mag.f");
	m_NamPlt_swRev = m_zs->Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = m_zs->Lits(m_LN + "NamPlt.vendor");
	float f = m_zs->Litf(m_LN + "Hz.mag.f");
	m_zs->Litmag(m_LN + "PNV.phsA");
//	m_zs->Litf(m_LN + "PNV.phsA.cval.mag.f");
	return true;
} // Initialisation

void CZamMMXU::ZamProc(int i)
{

	if (Initialisation())
	{
		double Vder_module;
		double Vder_angle;
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
//			Vder_module = m_zmp->Litf("Vder_module");
//			Vder_angle = m_zmp->Litf("Vder_angle");
//			m_zs->Litmag(m_LN + "PNV.phsA");
//			m_zs->Litf(m_LN + "PNV.phsB.cVal.mag.f");
//			m_zs->Litf(m_LN + "PNV.phsC.cVal.mag.f");
//			m_zs->Litf(m_LN + "PNV.phsA.cVal.ang.f");
//			m_zs->Litf(m_LN + "PNV.phsB.cVal.ang.f");
//			m_zs->Litf(m_LN + "PNV.phsC.cVal.ang.f");

			// Traitement

			// Ecriture
			//			if (m_PNV_phsA_cVal_mag_f != Vder_module)
			//			{
			//				m_PNV_phsA_cVal_mag_f = Vder_module;
			//				m_zs->Ecrit(m_LN + "PNV.phsA", "cVal.mag.f",Vder_module);
			//			}
			//			if (m_PNV_phsA_cVal_ang_f != Vder_angle)
			//			{
			//				m_PNV_phsA_cVal_ang_f = Vder_angle;
			//				m_zs->Ecrit(m_LN + "PNV.phsA", "cVal.ang.f",Vder_angle);
			//			}
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

int CZamMMXU::Arret()
{
	int err = 0;
	m_stop = true;
	return err;
}
