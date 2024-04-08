/*

ZamDEAO.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamEquip.hpp"


class CZamDEAO : public CZamEquip
{
public:
	CZamDEAO();
	virtual ~CZamDEAO();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int);
private:
	int m_Beh_q;
	int m_CabRtgAC_q;
	int m_CabRtgAC_stVal;
	int m_ChaAMaxg_setVal;
	int m_ChaARtg_setVal;
	int m_ChaARtg_units_SIUnit;
	int m_ChaARtg_units_multiplier;
	int m_ConnSt_q;
	int m_ConnSt_stVal;
	bool m_ConnTypDC_setVal;
	bool m_ConnTypPhs1_setVal;
	bool m_ConnTypPhs3_setVal;
	string m_EEName_vendor;
	string m_EVRef_intAddr;
	string m_EVRef_setSrcRef;
	string m_NamPlt_swRev;
	string m_NamPlt_vendor;
	int m_PlgStAC;

//	int m_CabRtgAC_stVal, m_n_CabRtgAC_stVal;
//	int m_ChaAMaxg_setVal, m_n_ChaAMaxg_setVal;		
//	int m_ChaARtg_setVal, m_n_ChaARtg_setVal;
//	int m_ConnSt_stVal, m_n_ConnSt_stVal;			
//	bool m_ConnTypDC_setVal, m_n_ConnTypDC_setVal;
//	bool m_ConnTypPhs1_setVal, m_n_ConnTypPhs1_setVal;
//	bool m_ConnTypPhs3_setVal, m_n_ConnTypPhs3_setVal;
//	bool m_EVRef_tstEna, m_n_EVRef_tstEna;
//	int m_PlgStAC_stVal, m_n_PlgStAC_stVal;
	string m_LD;
	string m_sne;
	bool m_binit = false;
public:
	int m_Beh_stVal;
	int m_PlgStAC_stVal;
	shared_ptr<CZamEVSE> m_evse;
};

