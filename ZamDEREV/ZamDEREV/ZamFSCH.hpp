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

class CZamFSCH : public CZamEquip
{
public:
	CZamFSCH();
	virtual ~CZamFSCH();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int);
	int Arret();
public:
	int m_status;
	unsigned long long m_td; 	// instant depart
	unsigned int m_per;   // periode actuelle
	unsigned long long m_t; 	// instant courant
	unsigned long long m_ti; 	// interval
	unsigned long long m_tp; 	// instant prochain changement
	unsigned long long m_tf; 	// instant fin
	bool m_change = false;
	int m_Beh_q;
	int m_Beh_stVal;
	unsigned long long m_Beh_t;
	unsigned int m_DsaReq_Oper_Check;
	unsigned long long m_DsaReq_Oper_T;
	bool m_DsaReq_Oper_Test;
	unsigned int m_DsaReq_Oper_ctlNum;
	bool m_DsaReq_Oper_ctlVal;
	int m_DsaReq_Oper_origin_orCat;
	string m_DsaReq_Oper_origin_orIdent;
	int m_DsaReq_ctlModel;
	int m_DsaReq_q;
	bool m_DsaReq_stVal;
	unsigned long long m_DsaReq_t;
	unsigned int m_EnaReq_Oper_Check;
	unsigned long long m_EnaReq_Oper_T;
	bool m_EnaReq_Oper_Test;
	unsigned int m_EnaReq_Oper_ctlNum;
	bool m_EnaReq_Oper_ctlVal;
	int m_EnaReq_Oper_origin_orCat;
	string m_EnaReq_Oper_origin_orIdent;
	int m_EnaReq_ctlModel;
	int m_EnaReq_q;
	bool m_EnaReq_stVal;
	unsigned long long m_EnaReq_t;
	bool m_EvtTrg_setVal;
	string m_InSyn_intAddr;
	string m_InSyn_setSrcRef;
	string m_InSyn_setTstRef;
	bool m_InSyn_tstEna;
	int m_IntvPer_setVal;
	int m_IntvPer_units_SIUnit;
	int m_IntvPer_units_multiplier;
	int m_IntvTyp_setVal;
	string m_NamPlt_swRev;
	string m_NamPlt_vendor;
	int m_NumEntr_setVal;
	int m_NumEntr_units_SIUnit;
	int m_NumEntr_units_multiplier;
	int m_SchdEntr_q;
	int m_SchdEntr_stVal;
	unsigned long long m_SchdEntr_t;
	int m_SchdIntv_setVal;
	int m_SchdPrio_setVal;
	bool m_SchdReuse_setVal;
	int m_SchdSt_q;
	int m_SchdSt_stVal;
	unsigned long long m_SchdSt_t;
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
	map<int, float> m_mapval;
	float m_ValMV_mag_f;
	int m_ValMV_q;
	unsigned long long m_ValMV_t;
	int m_SIUnit = 84;
};

