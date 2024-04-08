/*

ZamDEEV.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamDEEV.hpp"

CZamDEEV::CZamDEEV()
{
}

CZamDEEV::~CZamDEEV()
{
}

int CZamDEEV::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamDEEV::Lance()
{
	thread thr (&CZamDEEV::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDEEV::Initialisation()
{
// Lire le fichier 
	m_Beh = Liti(m_LN + "Beh.stVal");
	m_ConnTypSel = Liti(m_LN + "ConnTypSel.stVal"); 
	m_DptTm = Litull(m_LN + "DptTm.setTm"); 
	m_EVId = Lits(m_LN + "EVId.setVal");
	m_EMAId = Lits(m_LN + "EMAId.setVal");
	m_EVSt = Liti(m_LN + "EVSt.stVal");
	m_Soc = Litd(m_LN + "Soc.mag.f"); 
	m_WhTgt = Litd(m_LN + "WhTgt.setMag.f");
	m_EVSERef = Litd(m_LN + "EVSERef.intAddr");
	m_WChaTgt = 0.;
 	m_zp->m_zmp->Ecrit(m_LN + "Soc.mag.f", m_Soc);
	m_zp->m_zmp->Ecrit(m_LN + "ConnTypSel.stVal", m_ConnTypSel);
	m_zp->m_zmp->Ecrit(m_LN + "EVId.setVal", m_EVId);
	m_zp->m_zmp->Ecrit(m_LN + "EMAId.setVal", m_EMAId);
	m_zp->m_zmp->Ecrit(m_LN + "EVSt.stVal", m_EVSt); 
	m_zp->m_zmp->Ecrit(m_LN + "DptTm.setTm", m_DptTm);
	m_zp->m_zmp->Ecrit(m_LN + "WhTgt.setMag.f", m_WhTgt);
	m_zp->m_zmp->Ecrit("int_adresseSE", m_EVSERef);
	m_WCurrent = 0.;
	m_zp->m_zmp->Ecrit(m_LN + "WChaTgt.setMag.f", m_WChaTgt);
	return true;
} // Initialisation

void CZamDEEV::ZamProc(int i)
{
	int err = 0;
	string ref;
	if (Initialisation())
	{
		float Soc;
		int ConnTypSel;
		string EMAId;
		string EVId;
		int EVSt;
		unsigned long long DptTm;
		unsigned long long StartTm;
		float WhTgt;
		float WCurrent;
		float WChaTgt;
		string AlmMsg;
		string evse_ref;
		float Wmax;
		while (true)
		{
			if (m_zp->m_stop) break;
//LDEV1_DEEV1_
// Lecture nouvelles valeurs
			WCurrent = m_zmp->Litf(m_LN + "WCurrent.mag.f");
			if (m_WCurrent != WCurrent)
			{
				m_WCurrent = WCurrent;
				sem_post(m_zp->m_sem);
				Ecrit(m_LN + "WCurrent", "mag.f", m_WCurrent);
				m_zl->trace(fmt::format("DEEVP: WCurrent={:03.0f}", WCurrent));
			}
			Soc = m_zmp->Litf(m_LN + "Soc.mag.f");
			ConnTypSel = m_zmp->Liti(m_LN + "ConnTypSel.stVal");
			EVSt = m_zmp->Liti(m_LN + "EVSt.stVal");
			DptTm = m_zmp->Litull(m_LN + "DptTm.setTm");
			StartTm = m_zmp->Litull(m_LN + "StartTm.setTm");
			WhTgt = m_zmp->Litf(m_LN + "WhTgt.setMag.f");
			Wmax = m_zmp->Litf("int_Wmax");
			WChaTgt = Litf(m_LN + "WChaTgt.setMag.f");
			AlmMsg = Lits(m_LN + "AlmMsg.setVal");
			evse_ref = Lits(m_LN + "EVSERef.intAddr");
			EMAId = Lits(m_LN + "EMAId.setVal");
			EVId = Lits(m_LN + "EVId.setVal");
// gerer les modifications
			if (m_WChaTgt != WChaTgt)
			{
				m_WChaTgt = WChaTgt;
				m_zmp->Ecrit(m_LN + "WChaTgt.setMag.f", WChaTgt);
//				m_zl->trace(fmt::format("DEEVP: WChaTgt={}", WChaTgt));
			}
			if (m_Soc != Soc)
			{
				m_Soc = Soc;
				Ecrit(m_LN + "Soc", "mag.f", Soc);
				m_zs->Ecrit("IEDEVLDEV1/DSTO1", "SocWh.mag.f", Soc);
			}
			if (m_ConnTypSel != ConnTypSel)
			{
				m_ConnTypSel = ConnTypSel;
				Ecrit(m_LN + "ConnTypSel", "stVal", ConnTypSel);
			}
			if (m_EVSt != EVSt)
			{
				m_EVSt = EVSt;
				Ecrit(m_LN + "EVSt", "stVal", EVSt);
			}
			if (m_DptTm != DptTm)
			{
				m_DptTm = DptTm;
				Ecrit(m_LN + "DptTm", "setTm", DptTm);
			}
			if (m_StartTm != StartTm)
			{
				m_StartTm = StartTm;
				Ecrit(m_LN + "DptTm", "setTm", StartTm);
			}
			if (m_WhTgt != WhTgt)
			{
				m_WhTgt = WhTgt;
				Ecrit(m_LN + "WhTgt", "setMag.f", WhTgt);
			}
			if (m_EVSERef != evse_ref)
			{
				m_EVSERef = evse_ref;
				Ecrit(m_LN + "EVSERef","intAddr",evse_ref);
				m_zp->m_zmp->Ecrit("int_adresseSE", evse_ref);
			}
			if (m_AlmMsg_setVal != AlmMsg)
			{
				m_AlmMsg_setVal = AlmMsg;
				m_zp->m_zmp->Ecrit("IEDEVLDEV1/DEEV1.AlmMsg.setVal", AlmMsg);
			}
			if (m_EMAId != EMAId)
			{
				m_EMAId = EMAId;
				Ecrit(m_LN + "EMAId", "setVal", EMAId);
			}
			if (m_EVId != EVId)
			{
				m_EVId = EVId;
				Ecrit(m_LN + "EVId", "setVal", EVId);
			}
// Delai
			this_thread::sleep_for(chrono::milliseconds(1000));
		}   // boucle
	} // if init
	else
	{
		m_zl->info(fmt::format("DEEV: Arret du serveur {}", m_nom));
	}
	m_zl->info(fmt::format("DEEV: Arret du serveur {}", m_nom));
} // ZamProcess
