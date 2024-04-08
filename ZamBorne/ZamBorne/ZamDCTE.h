/*

ZamDCTE.h

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

class CZamDCTE : public CZamEquip
{
public:
	CZamDCTE();
	~CZamDCTE();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int i);
	int Arret();

public:
  int m_Beh_stVal;
  int m_CeaEngzReqSt_stVal;
  string m_EcpRef_setSrcRef;
  bool m_FctEna_stVal;
  float m_HzHiLim_setMag_f;
  float m_HzLoLim_setMag_f;
  string m_NamPlt_swRev;
  string m_NamPlt_vendor;
  int m_RtnDlTmms_setVal;
  int m_RtnRmpTmms_setVal;
  bool m_RtnSvcAuto_setVal;
  float m_VArLimPct_setMag_f;
  float m_VHiLim_setMag_f;
  float m_VLoLim_setMag_f;
};
