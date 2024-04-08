/*

ZamDINV.h

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

class CZamDINV : public CZamEquip
{
public:
	CZamDINV();
	~CZamDINV();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int i);
	int Arret();

public:
	int m_ACTyp_setVal;
	int m_Beh_q;
	int m_Beh_stVal;
	unsigned long long m_Beh_t;
	int m_InvDCLosAlm_q;
	bool m_InvDCLosAlm_stVal;
	unsigned long long m_InvDCLosAlm_t;
	int m_InvGriLosAlm_q;
	bool m_InvGriLosAlm_stVal;
	unsigned long long m_InvGriLosAlm_t;
	string m_NamPlt_swRev;
	string m_NamPlt_vendor;
	float m_OutWSet_setMag_f;
	float m_VARtg_setMag_f;
	float m_WRtg_setMag_f;
};
