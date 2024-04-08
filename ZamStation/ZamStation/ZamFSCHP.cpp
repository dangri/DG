/*

ZamFSCH.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamFSCHP.hpp"
#include <fmt/format.h>

CZamFSCHP::CZamFSCHP()
{
}

CZamFSCHP::~CZamFSCHP()
{
}

int CZamFSCHP::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamFSCHP::Lance()
{
	thread thr(&CZamFSCHP::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamFSCHP::Initialisation()
{
	m_Beh_stVal = Liti(m_LN + "Beh.stVal");
	m_change = false;
	m_DsaReq_stVal = false;
	// version DEV
	if (m_nom == "FSCH1")
	{
		
		m_NumEntr_setVal = 96;
		m_SchdPrio_setVal = 1;
		m_SchdReuse_setVal = false;
		m_SchdIntv_setVal = 15;
		m_StrTm1_setCal_occPer = 1;
		m_DsaReq_stVal = true;
		m_EnaReq_stVal = true;
		m_StrTm1_setCal_occPer = 1;
		for (int k = 1; k <= 24; k++)
		{
			m_mapval[k] = 140000;
			Ecrit(m_LN + fmt::format("ValASG{:02d}", k), "setMag.f", m_mapval[k]);
		}
		for (int k = 25; k <= 40; k++)
		{
			m_mapval[k] = 90000;
			Ecrit(m_LN + fmt::format("ValASG{:02d}", k), "setMag.f", m_mapval[k]);
		}
		for (int k = 41; k <= 72; k++)
		{
			m_mapval[k] = 140000;
			Ecrit(m_LN + fmt::format("ValASG{:02d}", k), "setMag.f", m_mapval[k]);
		}
		for (int k = 73; k <= 88; k++)
		{
			m_mapval[k] = 90000;
			Ecrit(m_LN + fmt::format("ValASG{:02d}", k), "setMag.f", m_mapval[k]);
		}
		for (int k = 89; k <= 96; k++)
		{
			m_mapval[k] = 140000;
			Ecrit(m_LN + fmt::format("ValASG{:02d}", k), "setMag.f", m_mapval[k]);
		}
	}
	if (m_nom == "FSCH2")
	{
		m_NumEntr_setVal = 73;
		m_SchdPrio_setVal = 2;
		m_SchdReuse_setVal = false;
		m_SchdIntv_setVal = 15;
		m_EnaReq_stVal = true;
		for (int k = 1; k <= 16; k++)
		{
			m_mapval[k] = 105000;
			Ecrit(m_LN + fmt::format("ValASG{:02d}", k), "setMag.f", m_mapval[k]);
		}
		for (int k = 17; k <= 24; k++)
		{
			m_mapval[k] = 70000;
			Ecrit(m_LN + fmt::format("ValASG{:02d}", k), "setMag.f", m_mapval[k]);
		}
		for (int k = 25; k <= 32; k++)
		{
			m_mapval[k] = 70000;
			Ecrit(m_LN + fmt::format("ValASG{:02d}", k), "setMag.f", m_mapval[k]);
		}
		for (int k = 33; k <= 73; k++)
		{
			m_mapval[k] = 105000;
			Ecrit(m_LN + fmt::format("ValASG{:02d}", k), "setMag.f", m_mapval[k]);
		}
	}
	m_change = true;
	return true;
} // Initialisation

void CZamFSCHP::ZamProc(int i)
{
	if(Initialisation())
	{
		bool cde_bdd, cde_mem;  // commande depuis la bdd ou depuis la memoire
		bool calcul; 	// 1 = oui, 0 = non
		float ValMV;
		int SchdSt;
		int SchdEntr;
		bool bper;
		int periode;
		int journee;
		while(true)
		{
// OFF fonction inactive
		if (m_Beh_stVal == 5) goto finboucle;
// Fonction active
// Lecture
		m_DsaReq_stVal = Litb(m_LN + "DsaReq.stVal");
		if (m_DsaReq_stVal)
		{
			m_EvtTrg_setVal = Litb(m_LN + "EvtTrg.setVal");
			m_NumEntr_setVal = Liti(m_LN + "NumEntr.setVal");
			m_SchdIntv_setVal = Liti(m_LN + "SchdIntv.setVal");
			m_SchdPrio_setVal = Liti(m_LN + "SchdPrio.setVal");
			m_SchdReuse_setVal = Litb(m_LN + "SchdReuse.setVal");
			m_StrTm0_setCal_day = Liti(m_LN + "StrTm0.setCal.day");
			m_StrTm0_setCal_hr = Liti(m_LN + "StrTm0.setCal.hr");
			m_StrTm0_setCal_mn = Liti(m_LN + "StrTm0.setCal.mn");
			m_StrTm0_setCal_month = Liti(m_LN + "StrTm0.setCal.month");
			m_StrTm0_setCal_occ = Liti(m_LN + "StrTm0.setCal.occ");
			m_StrTm0_setCal_occPer = Liti(m_LN + "StrTm0.setCal.occPer");
			m_StrTm0_setCal_occType = Liti(m_LN + "StrTm0.setCal.occType");
			m_StrTm0_setCal_weekDay = Liti(m_LN + "StrTm0.setCal.weekDay");
			m_StrTm0_setTm = Litull(m_LN + "StrTm0.setTm");
			m_StrTm1_setCal_day = Liti(m_LN + "StrTm1.setCal.day");
			m_StrTm1_setCal_hr = Liti(m_LN + "StrTm1.setCal.hr");
			m_StrTm1_setCal_mn = Liti(m_LN + "StrTm1.setCal.mn");
			m_StrTm1_setCal_month = Liti(m_LN + "StrTm1.setCal.month");
			m_StrTm1_setCal_occ = Liti(m_LN + "StrTm1.setCal.occ");
			m_StrTm1_setCal_occPer = Liti(m_LN + "StrTm1.setCal.occPer");
			m_StrTm1_setCal_occType = Liti(m_LN + "StrTm1.setCal.occType");
			m_StrTm1_setCal_weekDay = Liti(m_LN + "StrTm1.setCal.weekDay");
//			m_StrTm1_setTm = Litull(m_LN + "StrTm1.setTm");
			for (int k = 1; k <= m_NumEntr_setVal; k++)
			{
				m_mapval[k] = Litf(m_LN + fmt::format("ValASG{:02d}.setMag.f",k));
			}
			Ecrit(m_LN + "DsaReq", "stVal", false);
			m_DsaReq_stVal = false;
			m_change = true;
			m_zl->info(fmt::format("FSCHP: Mise a jour de {}", m_LN));
			m_zl->info(fmt::format("FSCHP: {} has been updated", m_LN));
			
		}
		else
		{
			m_EnaReq_stVal = Litb(m_LN + "EnaReq.stVal");
		}
// Delai
		finboucle:
			this_thread::sleep_for(chrono::seconds(1));
		}   // boucle
	} // if init
	else
	{
		m_zl->info(fmt::format("FSCHP, Init impossible ", m_nom));
	}
	m_zl->info(fmt::format("Arret du programme {}", m_nom));
} // ZamProcess

int CZamFSCHP::Arret()
{
	int err = 0;
	return err;
}
