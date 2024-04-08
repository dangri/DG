/*

ZamFSCC.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamEquip.hpp"

class CZamFSCC : public CZamEquip
{
public:
	CZamFSCC();
	virtual ~CZamFSCC();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int);
	int Arret();

 private:
	int m_nbs;
	 int m_ActSchdRef_q;
	 unsigned long long m_ActSchdRef_t;
	 int m_Beh_q;
	 unsigned long long m_Beh_t;
	 string m_CtlEnt_intAddr;
	 string m_CtlEnt_setSrcRef;
	 string m_CtlEnt_setTstRef;
	 bool m_CtlEnt_tstEna;
	 string m_InSyn_intAddr;
	 string m_InSyn_setSrcRef;
	 string m_InSyn_setTstRef;
	 bool m_InSyn_tstEna;
	 string m_NamPlt_swRev;
	 string m_NamPlt_vendor;
	 string m_Schd1_intAddr;
	 string m_Schd1_setSrcRef;
	 string m_Schd1_setTstRef;
	 bool m_Schd1_tstEna;
	 string m_Schd2_intAddr;
	 string m_Schd2_setSrcRef;
	 string m_Schd2_setTstRef;
	 bool m_Schd2_tstEna;
	 string m_Schd3_intAddr;
	 string m_Schd3_setSrcRef;
	 string m_Schd3_setTstRef;
	 bool m_Schd3_tstEna;
	 int m_ValMV_q;
	 unsigned long long m_ValMV_t;
	 int m_ValMV_units_SIUnit;
	 int m_ValMV_units_multiplier;

   public:
	 int m_Beh_stVal;
	 string m_ActSchdRef_stVal, m_n_ActSchdRef_stVal;
	 float m_ValMV_mag_f = 0, m_n_ValMV_mag_f;
};

