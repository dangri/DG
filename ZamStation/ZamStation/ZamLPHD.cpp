/*

ZamLPHD.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamLPHD.hpp"

CZamLPHD::CZamLPHD()
{
}

CZamLPHD::~CZamLPHD()
{
}

int CZamLPHD::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamLPHD::Lance()
{
	thread thr(&CZamLPHD::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamLPHD::Initialisation()
{
	m_PhyHealth_stVal = Liti(m_LN + "PhyHealth.stVal");
//	m_PhyNam_hwRev = Lits(m_LN + "PhyNam.hwRev");
//	m_PhyNam_name = Lits(m_LN + "PhyNam.name");
//	m_PhyNam_vendor = Lits(m_LN + "PhyNam.vendor");
//	m_Proxy_stVal = Litb(m_LN + "Proxy.stVal");
	return true;
} // Initialisation

void CZamLPHD::ZamProc(int i)
{
	int err = 0;
	int ilec;
	if (Initialisation())
	{
		while (true)
		{
			if (m_stop)
			{
				return;
			}
			// Lecture nouvelles valeurs
			
			ilec = Liti(m_LN + "PhyHealth.stVal");
			if (m_PhyHealth_stVal != ilec)
			{
				m_PhyHealth_stVal = ilec;
			}
// Delai
			this_thread::sleep_for(chrono::seconds(1));
		} // boucle
	}	 // if init
	else
	{
		m_zl->info(fmt::format("Init impossible", m_nom));
	}
	m_zl->info(fmt::format("Arret du programme {}", m_nom));
} // ZamProcess

int CZamLPHD::Arret()
{
	int err = 0;

	return err;
}