/*

ZamFSCH.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamEquip.hpp"


using namespace std;

class CZamFSCHP : public CZamEquip
{
public:
	CZamFSCHP();
	virtual ~CZamFSCHP();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int);
	int Arret();
public:
	int m_Beh_q;
	int m_Beh_stVal;
	bool m_DsaReq_stVal;
	bool m_EnaReq_stVal;
	bool m_EvtTrg_setVal;
	int m_IntvPer_setVal;
	int m_IntvPer_units_SIUnit;
	int m_IntvPer_units_multiplier;
	int m_NumEntr_setVal;
	int m_SchdIntv_setVal;
	int m_SchdIntv_units_SIUnit;
	int m_SchdIntv_units_multiplier;
	int m_SchdPrio_setVal;
	bool m_SchdReuse_setVal;
	unsigned int m_StrTm0_setCal_day;
	unsigned int m_StrTm0_setCal_hr;
	unsigned int m_StrTm0_setCal_mn;
	int m_StrTm0_setCal_month;
	unsigned int m_StrTm0_setCal_occ;
	int m_StrTm0_setCal_occPer;
	int m_StrTm0_setCal_occType;
	int m_StrTm0_setCal_weekDay;
	unsigned long long m_StrTm0_setTm;
	unsigned int m_StrTm1_setCal_day;
	unsigned int m_StrTm1_setCal_hr;
	unsigned int m_StrTm1_setCal_mn;
	int m_StrTm1_setCal_month;
	unsigned int m_StrTm1_setCal_occ;
	int m_StrTm1_setCal_occPer;
	int m_StrTm1_setCal_occType;
	int m_StrTm1_setCal_weekDay;
	unsigned long long m_StrTm1_setTm;
	int m_ValMV_units_SIUnit;
	int m_ValMV_units_multiplier;
	map<int, float> m_mapval;
	bool m_ena;
	bool m_change;
};

