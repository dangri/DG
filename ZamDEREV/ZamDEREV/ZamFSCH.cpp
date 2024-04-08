/*

ZamFSCH.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamFSCH.hpp"
#include <fmt/format.h>

CZamFSCH::CZamFSCH()
{
}

CZamFSCH::~CZamFSCH()
{
}

int CZamFSCH::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamFSCH::Lance()
{
	thread thr(&CZamFSCH::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamFSCH::Initialisation()
{
	return true;
} // Initialisation

void CZamFSCH::ZamProc(int i)
{
	if (Initialisation())
	{
		bool cde_bdd, cde_mem; // commande depuis la bdd ou depuis la memoire
		bool calcul;		   // 1 = oui, 0 = non
		float ValMV;
		int SchdSt;
		int SchdEntr;
		bool bper;
		int periode;
		int journee;
		bool att = false;
		while (true)
		{
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
				m_StrTm1_setTm = Litull(m_LN + "StrTm1.setTm");
				for (int k = 1; k <= m_NumEntr_setVal; k++)
				{
					m_mapval[k] = Litf(m_LN + fmt::format("ValASG{:02d}.setMag.f",k));
				}
				Ecrit(m_LN + "DsaReq", "stVal", false);
				m_DsaReq_stVal = false;
				m_change = true;
				m_zl->info(fmt::format("Mise a jour de {} prise en compte", m_LN));
				m_zl->info(fmt::format(" {} update is taken into account ", m_LN));
			}
			bool b_end;   // Fin du programme
			// OFF fonction inactive
			if (m_Beh_stVal == 5) goto finboucle;
// Fonction active
// Lecture
			ValMV = Litf(m_LN + "ValMV.mag.f");
			SchdSt = Liti(m_LN + "SchdSt.stVal");
			SchdEntr = Liti(m_LN + "SchdEntr.stVal");
			m_status = SchdSt;
			m_EnaReq_stVal = Litb(m_LN + "EnaReq.stVal");
// Status	
			if(m_EnaReq_stVal)
			{
				if(m_status < 4) m_status = 3;
			}
			else
			{
				m_status = 1;
				Ecrit(m_LN + "SchdSt", "stVal", m_status);
				this_thread::sleep_for(chrono::seconds(1));
				continue;
			}
// Trg
			m_EvtTrg_setVal = Liti(m_LN + "EvtTrg.setVal");
			if(m_EvtTrg_setVal && m_status == 3)
			{
				m_status = 4;
			}
			m_SchdIntv_setVal = Liti(m_LN + "SchdIntv.setVal");
			m_StrTm1_setCal_occPer = Liti(m_LN + "StrTm1.setCal.occPer");
			bper = false;
			unsigned long long td, tf;
			switch (m_SIUnit)
			{
			case 84: 
				journee = 60 * 24 * 60000; // la journee en millisecondes
				break;
			case 85:
				journee = 24 * 3600000; // la journee en millisecondes
				break;
			default:
				journee = 60 * 24 * 60000; // la journee en millisecondes
				break;
			}
			if(m_StrTm1_setCal_occPer)  // periodique
			{
				bper = true;
				switch (m_StrTm1_setCal_occType)
				{
				case 0:
					if(m_SIUnit == 84) periode = 60; // heure si SIUnit=84 (mn)
					if (m_SIUnit == 85) periode = 1; // heure si SIUnit=84 (mn)
					break;
				case 1:
					if (m_SIUnit == 84) periode = 60 * 24; // jour
					if (m_SIUnit == 85) periode = 24; //jour
					break;
				case 2:
					if (m_SIUnit == 84) periode = 60 * 24 * 7; // semaine
					if (m_SIUnit == 85) periode = 24 * 7;
					break;
				default:
					m_zl->info(fmt::format("Periode {} non prise en compte", m_StrTm1_setCal_occType));
					m_zl->info(fmt::format(" {} period not taken into account", m_StrTm1_setCal_occType));
					return;
					break;
				}
				periode *= 60000; // tout en ms
			}
			else	// calcul debut et fin si non periodique
			{
//				td = m_StrTm0_setTm;
				td = 0;
				tf = td + (m_NumEntr_setVal * m_SchdIntv_setVal * 60000);
			}
			// Traitement du status
			switch(m_status)
			{
			case 1:	// Not ready
			break;   // 1
			case 2 :  	//	Start Time required
				if(bper) m_status = 3;
				else
				{
					
				}
				break; // 2
			case 3 :  	//	ready
				if (bper)
					m_status = 4;
				else
				{
					if (m_zp->m_temps >= td)
					{
						if (m_zp->m_temps < tf)
						{
							m_status = 4;
						}
						else
						{
							m_status = 5;
						}
					}
					else
					{
						if (!att)
						{
							att = true;
						}
					}
				}
				break;
			case 4: // running
				if(!bper)
				{
					if(m_zp->m_temps < td)
					{
						m_status = 3;
					}
					if(m_zp->m_temps > tf)
					{
						if(!m_SchdReuse_setVal)
						{
							m_status = 1;
						}
						else
						{
							m_status = 3;
						}
					}
				}
				break;
				// Fin de l'execution
			case 5 :   //	fin d'execution 
				// not periodique
				if(!bper)
				{
					// no reuse
					if(!m_SchdReuse_setVal)
					{
						m_status = 1;
					}
					// reuse
					else
					{
						m_status = 2;
					}
				}
				// periodique
				else
				{
					m_status = 2;
				}
				// Manque reuse et ext ou strtm non tous consommes
				if((m_SchdReuse_setVal && m_EvtTrg_setVal != 0) || (m_StrTm0_setTm != 0))
				{
					m_status = 2;
				}
				break;
			}	// switch status schdst
			
			m_SchdSt_stVal = m_status;
			
// Action si running
			if(m_status == 4)
			{
				if(m_StrTm1_setCal_occPer == 0)
				{
					// non periodique
					if (m_SIUnit == 84) // units minut
					{
						m_ti = m_SchdIntv_setVal * 60000;
						m_t = ((unsigned long)m_zp->m_temps) % journee;
					}
					if (m_SIUnit == 85) // units heure
					{
						m_ti = m_SchdIntv_setVal * 3600000;
						m_t = ((unsigned long)m_zp->m_temps) % journee;
					}
//					m_td = m_StrTm0_setTm; // instant de depart 1 559 800
					m_td = 0;
					m_tf = m_td + m_NumEntr_setVal * m_ti;
					if(m_t > m_tf)
					{
						if(!m_SchdReuse_setVal)
						{
							m_status = 1;
							Ecrit(m_LN + "SchdSt", "stVal", 1);
							continue;
						}
						m_status = 5;
					}
					else
					{
						if(m_t > m_td)
						{
							m_per = 1 + int((m_t - m_td) / m_ti);
							m_tp = (m_per + 1) * m_ti;
							m_ValMV_mag_f = m_mapval[m_per];
							m_SchdEntr_stVal = m_per;
							m_SchdSt_stVal = 4;
						}
					}
				} // non periodique
				else
				{
					// periodique
					m_td = m_StrTm0_setTm;    	// instant de depart
					if(m_StrTm1_setCal_occType == 0)	// depart a un instant donne (Time, WeekDay, WeekofYear, DayOfMonth, DayOfYear)
					{
						if(m_StrTm1_setCal_occPer == 1)	// periode day ( Hour, Day, Week, Month, Year)
						{
							if(m_SIUnit == 84)		// units minut
							{
								m_ti = m_SchdIntv_setVal * 60000;  // .001s
							}
							if (m_SIUnit == 85) // units heure
							{
								m_ti = m_SchdIntv_setVal * 3600000; // .001s
							}
// Le temps virtuel est exprimé en 60eme de seconde 
							m_t = ((unsigned long) m_zp->m_temps) % journee;
							m_per = 1 + int((m_t - m_td) / m_ti);
							m_tp = (m_per + 1) * m_ti;
							m_ValMV_mag_f = m_mapval[m_per];
							m_SchdEntr_stVal = m_per;
							m_SchdSt_stVal = 4;
						}
					}
				}  // periodique
				if (m_ValMV_mag_f != ValMV) Ecrit(m_LN + "ValMV", "mag.f", m_ValMV_mag_f);
			}	// fin action si running
// Updates
// maj des commandes
			if(m_status != SchdSt) Ecrit(m_LN + "SchdSt", "stVal", m_status);
			if(SchdEntr <= 0) SchdEntr = 1;
			if (m_SchdEntr_stVal != SchdEntr)
			{
				Ecrit(m_LN + "SchdEntr", "stVal", SchdEntr);
				m_SchdEntr_stVal = SchdEntr;
			}
// Delai
		finboucle:
			this_thread::sleep_for(chrono::seconds(1));
		}   // boucle
	} // if init
	else
	{
		m_zl->info(fmt::format("FSCH: Init impossible {}", m_nom));;
	}
	m_zl->info(fmt::format("FSCH: Arret du serveur {}", m_nom));
} // ZamProcess

int CZamFSCH::Arret()
{
	int err = 0;

	return err;
}
