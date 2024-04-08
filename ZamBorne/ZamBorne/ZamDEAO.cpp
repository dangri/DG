/*

ZamDEAO.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamDEAO.hpp"

CZamDEAO::CZamDEAO()
{
	
}

CZamDEAO::~CZamDEAO()
{
}

int CZamDEAO::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	Lance();
	return err;
}

int CZamDEAO::Lance()
{
	m_stop = false;
	thread thr(&CZamDEAO::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDEAO::Initialisation()
{
	m_Beh_stVal = Liti(m_LN + "Beh.stVal");
	m_CabRtgAC_stVal = Liti(m_LN + "CabRtgAC.stVal");
	m_ConnSt_stVal = Liti(m_LN + "ConnSt.stVal");
	m_ConnTypDC_setVal = Litb(m_LN + "ConnTypDC.setVal");
	m_ConnTypPhs1_setVal = Litb(m_LN + "ConnTypPhs1.setVal");
	m_ConnTypPhs3_setVal = Litb(m_LN + "ConnTypPhs3.setVal");
	m_PlgStAC_stVal = Liti(m_LN + "PlgStAC.stVal");
	m_EVRef_intAddr = Liti(m_LN + "EVRef.intAddr");
	return true;
} // Initialisation

void CZamDEAO::ZamProc(int i)
{
	if (Initialisation())
	{
		int CabRtgAC;
		float imax, pmax = 0;
		int ConnSt;
		bool ConnTypDC;
		bool ConnTypPhs1;
		bool ConnTypPhs3;
		int PlgStAC;
		int essai = 0;
		bool btrouve = false;
		string EVRef;
		try
		{
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
				CabRtgAC = m_zp->m_zmp->Liti(m_LN + "CabRtgAC.stVal");
				ConnSt = m_zp->m_zmp->Liti(m_LN + "ConnSt.stVal");
				ConnTypDC = m_zp->m_zmp->Litb(m_LN + "ConnTypDC.setVal");
				ConnTypPhs1 = m_zp->m_zmp->Litb(m_LN + "ConnTypPhs1.setVal");
				ConnTypPhs3 = m_zp->m_zmp->Litb(m_LN + "ConnTypPhs3.setVal");
				PlgStAC = m_zp->m_zmp->Liti(m_LN + "PlgStAC.stVal");
				EVRef = Lits(m_LN + "EVRef.intAddr");
// Traitement
				if (m_CabRtgAC_stVal != CabRtgAC)			
				{
					m_CabRtgAC_stVal = CabRtgAC;
					m_zs->Ecrit(m_LN + "CabRtgAC", "stVal", CabRtgAC);
					m_zl->trace(fmt::format("DEAO: CabRtgAC= {:01d}", CabRtgAC));
				}
				if (m_PlgStAC_stVal != PlgStAC)
				{
					m_PlgStAC_stVal = PlgStAC;
					m_zs->Ecrit(m_LN + "PlgStAC", "stVal", PlgStAC);
				}
				// Attente branchement vehicule
				if (m_zp->m_connect_ve)
				{
					if (!btrouve)
					{
						while (essai < 5)
						{
							btrouve = false;
							if (CabRtgAC == 1)
							{
								imax = 13.;
								btrouve = true;
								break;
							}
							if (CabRtgAC == 2)
							{
								imax = 20.;
								btrouve = true;
								break;
							}
							if (CabRtgAC == 3)
							{
								imax = 32.;
								btrouve = true;
								break;
							}
							if (CabRtgAC == 4)
							{
								imax = 70.;
								btrouve = true;
								break;
							}
							essai += 1;
						}
						if (btrouve)
						{
							m_zs->Ecrit(m_LN + "CabRtgAC", "stVal", CabRtgAC);
							pmax = 230 * imax * 3;
//							m_zl->trace(fmt::format("DEAO: CabRtgAC= {:03d}", pmax));
							Ecrit(m_IED + m_LD + "/DPMC1.WLimSpt3", "mxVal.f", pmax);
						}
						else
						{
							CabRtgAC = 4;
							imax = 70.;
							pmax = 230 * imax * 3;
							Ecrit(m_IED + m_LD + "/DPMC1.WLimSpt3", "mxVal.f", pmax);
							btrouve = true;
							break;
						}
					}
				}
				if (m_ConnSt_stVal != ConnSt)
				{
					m_ConnSt_stVal = ConnSt;
					Ecrit(m_LN + "ConnSt", "stVal", ConnSt);
				}
				if (m_ConnTypDC_setVal != ConnTypDC)
				{
					m_ConnTypDC_setVal = ConnTypDC;
					Ecrit(m_LN + "ConnTypDC", "setVal", ConnTypDC);
				}
				if (m_ConnTypPhs1_setVal != ConnTypPhs1)
				{
					m_ConnTypPhs1_setVal = ConnTypPhs1;
					Ecrit(m_LN + "ConnTypPhs1", "setVal", ConnTypPhs1);
				}
				if (m_ConnTypPhs3_setVal != ConnTypPhs3)
				{
					m_ConnTypPhs3_setVal = ConnTypPhs3;
					Ecrit(m_LN + "ConnTypPhs3", "setVal", ConnTypPhs3);
				}
				if (m_EVRef_intAddr != EVRef)
				{
					m_EVRef_intAddr = EVRef;
					m_zp->m_zmp->Ecrit("int_adresseEV", EVRef);
				}
				// Delai
finboucle:
				this_thread::sleep_for(chrono::milliseconds(1000));
			} // boucle
		} // try
		catch (string exc)
		{
			m_zl->info(fmt::format("Exception dans DEAO: {}",exc));
		}
	} // if init
	else
	{
		m_zl->info("DEAO : Initialisation impossible");
	}
	m_zl->info("DEAO : Arret de DEAO");
} // ZamProcess

