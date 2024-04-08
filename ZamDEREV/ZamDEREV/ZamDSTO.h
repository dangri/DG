/*

ZamDSTO.h

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
#include "ZamDEEV.hpp"

class CZamLDEV;
class CZamMemp;

class CZamDSTO : public CZamEquip
{
  public:
	CZamDSTO();
	~CZamDSTO();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int i);
	int Arret();

  public:
	int m_Beh_stVal;
//	float m_ChaWMax_setMag_f;
//	float m_ChaWMaxRtg_setMag_f;
	string m_DERUnit_setSrcRef;
//	float m_DschWMax_setMag_f;
//	float m_DschWMaxRtg_setMag_f;
	string m_EqSto_setSrcRef;
	string m_InclSto1_setSrcRef;
	string m_InclSto2_setSrcRef;
	string m_InclSto3_setSrcRef;
	string m_NamPlt_swRev;
	string m_NamPlt_vendor;
	float m_SocWh_mag_f;
	float m_WSpt_mxVal_f;
	float m_WhMaxRtg_setMag_f;
	float m_WhMinRtg_setMag_f;
	float m_WhRtg_setMag_f;
	shared_ptr<CZamLDEV> m_ldev;
	shared_ptr<CZamMemp> m_zmp;
	shared_ptr<CZamDEEV> m_deev;
};
