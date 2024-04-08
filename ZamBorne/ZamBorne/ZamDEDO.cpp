/*

ZamDEDO.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamDEDO.hpp"


CZamDEDO::CZamDEDO()
{
	
}

CZamDEDO::~CZamDEDO()
{
}

int CZamDEDO::Demarrage()
{
	int err= 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance ();
	return err;
}

int CZamDEDO::Lance()
{
	m_stop = false;
	thread thr(&CZamDEDO::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDEDO::Initialisation ()
{
	m_Beh_stVal = Liti(m_LN + "Beh.stVal");
	m_CabRtgDC_stVal = Liti(m_LN + "CabRtgDC.stVal");
//	m_ChaARtg_setVal = Liti(m_LN + "ChaARtg.setVal");
	m_ConnStA_stVal = Liti(m_LN + "ConnStA.stVal");
	m_ConnStC_stVal = Liti(m_LN + "ConnStC.stVal");
	m_PlgStDC_stVal = Liti(m_LN + "PlgStDC.stVal");
	m_EVRef_intAddr = Liti(m_LN + "EVRef.intAddr");
	return true;
} // Initialisation

void CZamDEDO::ZamProc(int i)
{
	if(Initialisation())
	{
		int CabRtgDC;
		int ChaARtg;
		float imax, pmax;
		bool bmax = false;
		int ConnStA;
		int ConnStC;
		int PlgStDC;
		string EVRef;
		while(true)
		{
			if(m_stop)
			{
				return;
			}
// OFF fonction inactive
		if (m_Beh_stVal == 2) goto finboucle;
// Fonction active
// Lecture
			CabRtgDC = m_zp->m_zmp->Liti(m_LN + "CabRtgDC.stVal");
			ConnStA = m_zp->m_zmp->Liti(m_LN + "ConnStA.stVal");
			ConnStC = m_zp->m_zmp->Liti(m_LN + "ConnStC.stVal");
			PlgStDC = m_zp->m_zmp->Liti(m_LN + "PlgStDC.stVal");
			EVRef = Lits(m_LN + "EVRef.intAddr");
// Traitement
			if (CabRtgDC != m_CabRtgDC_stVal)
			{
				Ecrit(m_LN + "CabRtgDC", "stVal", CabRtgDC);
				m_CabRtgDC_stVal = CabRtgDC;
			}
			if(m_PlgStDC_stVal != PlgStDC)
			{
				m_PlgStDC_stVal = PlgStDC;
				Ecrit(m_LN + "PlgStDC", "stVal", PlgStDC);
			}
// Attente branchement vehicule
			if (m_zp->m_connect_ve)
			{
				if (!bmax)
				{
					if (CabRtgDC != 98)
					{
						if (CabRtgDC == 1)
							imax = 13.;
						if (CabRtgDC == 2)
							imax = 16.;
						if (CabRtgDC == 3)
							imax = 32.;
						if (CabRtgDC == 4)
							imax = 63.;
						if (CabRtgDC == 5)
							imax = 70.;
						if (CabRtgDC == 6)
							imax = 80.;
						if (CabRtgDC == 7)
							imax = 125.;
						if (CabRtgDC == 8)
							imax = 200.;
						if (CabRtgDC == 9)
							imax = 250.;
						if (CabRtgDC == 10)
							imax = 400.;
						pmax = 400 * imax;
						Ecrit(m_IED + m_LD + "/DPMC1.WLimSpt3", "mxVal.f", pmax);
						bmax = true;
					}
				}
			}
//			if(m_ChaARtg_setVal != ChaARtg)
//			{
//				m_ChaARtg_setVal = ChaARtg;
//				Ecrit(m_LN + "ChaARtg", "setVal", ChaARtg);
//			}
			if(m_ConnStA_stVal != ConnStA)
			{
				m_ConnStA_stVal = ConnStA;
				Ecrit(m_LN + "ConnStA", "stVal", ConnStA);
			}
			if(m_ConnStC_stVal != ConnStC)
			{
				m_ConnStC_stVal = ConnStC;
				Ecrit(m_LN + "ConnStC", "stVal", ConnStC);
			}
			if (m_EVRef_intAddr != EVRef)
			{
				m_EVRef_intAddr = EVRef;
				m_zp->m_zmp->Ecrit("int_adresseEV", EVRef);
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


