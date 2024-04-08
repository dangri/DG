/*

ZamMMXU.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamEquip.hpp"

class CZamMMXU : public CZamEquip
{
public:
	CZamMMXU();
	virtual ~CZamMMXU();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int);
	int Arret();
private:
	int m_Beh_q;
	unsigned long long m_Beh_t;
	float m_Hz_mag_f;
	int m_Hz_q;
	unsigned long long m_Hz_t;
	int m_Hz_units_SIUnit;
	int m_Hz_units_multiplier;
	string m_NamPlt_swRev;
	string m_NamPlt_vendor;
	float m_TotW_mag_f;
	int m_TotW_q;
	unsigned long long m_TotW_t;
	int m_TotW_units_SIUnit;
	int m_TotW_units_multiplier;
//	float m_A_phsA_cVal_mag_f, m_n_A_phsA_cVal_mag_f;
//	float m_A_phsA_cVal_ang_f, m_n_A_phsA_cVal_ang_f;
//	float m_A_phsB_cVal_mag_f, m_n_A_phsB_cVal_mag_f;
//	float m_A_phsB_cVal_ang_f, m_n_A_phsB_cVal_ang_f;
//	float m_A_phsC_cVal_mag_f, m_n_A_phsC_cVal_mag_f;
//	float m_A_phsC_cVal_ang_f, m_n_A_phsC_cVal_ang_f;
//	float m_A_net_cVal_mag_f, m_n_A_net_cVal_mag_f;
//	float m_A_net_cVal_ang_f, m_n_A_net_cVal_ang_f;
//	float m_PhV_phsA_cVal_mag_f, m_n_PhV_phsA_cVal_mag_f;
//	float m_PhV_phsA_cVal_ang_f, m_n_PhV_phsA_cVal_ang_f;
//	float m_PhV_phsB_cVal_mag_f, m_n_PhV_phsB_cVal_mag_f;
//	float m_PhV_phsB_cVal_ang_f, m_n_PhV_phsB_cVal_ang_f;
//	float m_PhV_phsC_cVal_mag_f, m_n_PhV_phsC_cVal_mag_f;
//	float m_PhV_phsC_cVal_ang_f, m_n_PhV_phsC_cVal_ang_f;
//	float m_PhV_net_cVal_mag_f, m_n_PhV_net_cVal_mag_f;
//	float m_PhV_net_cVal_ang_f, m_n_PhV_net_cVal_ang_f;
//	float m_W_phsA_cVal_mag_f, m_n_W_phsA_cVal_mag_f;
//	float m_W_phsA_cVal_ang_f, m_n_W_phsA_cVal_ang_f;
//	float m_W_phsB_cVal_mag_f, m_n_W_phsB_cVal_mag_f;
//	float m_W_phsB_cVal_ang_f, m_n_W_phsB_cVal_ang_f;
//	float m_W_phsC_cVal_mag_f, m_n_W_phsC_cVal_mag_f;
//	float m_W_phsC_cVal_ang_f, m_n_W_phsC_cVal_ang_f;
//	float m_W_net_cVal_mag_f, m_n_W_net_cVal_mag_f;
//	float m_W_net_cVal_ang_f, m_n_W_net_cVal_ang_f;
//	float m_TotW_mag_f, m_n_TotW_mag_f;
public:
	int m_Beh_stVal;
};

