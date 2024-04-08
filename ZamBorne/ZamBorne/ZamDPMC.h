/*

ZamDPMC.h

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus
 industriels developpes en C++ standard et en Python par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements reposent sur les normes IEC et en particulier la norme IEC 61850
ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en oeuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850"
developpee et geree par MZ Automation GmbH (http://libiec61850.com).
La liste complete des sources est fournie dans le fichier "msources_zamiren.txt" »
*/

#pragma once
#include "ZamEquip.hpp"

class CZamDPMC : public CZamEquip
{
public:
	CZamDPMC();
	~CZamDPMC();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int i);
	int Arret();
	void dimax();

  public:
	int m_Beh_stVal;
	;
	string m_DERRef_setSrcRef;
	string m_EcpRef_setSrcRef;
	bool m_FctEna_stVal;
	string m_NamPlt_swRev;
	string m_NamPlt_vendor;
	float m_ReqW_mag_f;
	float m_ReqW1_mag_f;
	float m_ReqW2_mag_f;
	float m_ReqW3_mag_f;
	float m_WLimSpt1_mxVal_f;
	float m_WLimSpt2_mxVal_f;
	float m_WLimSpt3_mxVal_f;
	float m_WLimSpt4_mxVal_f;
	float m_WLimSpt5_mxVal_f;
	bool m_branche;
	bool m_charge;
	bool m_decharge;
	float m_max, m_max0, m_max1, m_max2, m_max3;
};
