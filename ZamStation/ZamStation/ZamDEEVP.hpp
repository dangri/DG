/*

ZamDEEVP.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once
#include "ZamEquip.hpp"
#include <string>
#include "ZamLDCS.hpp"
#include "ZamLDSE.hpp"
#include "ZamCalcul.h"

class CZamDEEVP : public CZamEquip
{
public:
	CZamDEEVP();
	~CZamDEEVP();
	int Demarrage();
	int Lance();
	void ZamProc(int i);
	int CreVar();

private:
	bool Initialisation();

public:
	string m_LD;
	string m_iLD;
	string m_sne;
	int m_ise;
	string m_inst;
	string m_ref;
	int m_ConnTypSel_stVal = 0;
	float m_Soc_ = 0;
	string m_EMAId;
	string m_EVId;
	int m_EVSt = 98;
	float m_WCurrent = 0;
	float m_WChaTgt = 0;
	unsigned long long m_DptTm = 0;
	unsigned long long m_StartTm = 0;
	float m_WhTgt;
	bool m_binit = false;
	float m_wh;
	float m_wht = 0;
	float m_w = 0.;
	float m_w0 = 0.;
	unsigned long long m_t;
	unsigned long long m_t0;
	string m_evse;
	shared_ptr<CZamLDCS> m_ldcs;
	shared_ptr<CZamLDSE> m_ldse;
	shared_ptr<CZamCalcul> m_cal;
};
