/*

ZamDESE.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamEquip.hpp"

class CZamDESE : public CZamEquip
{
public:
	CZamDESE();
	virtual ~CZamDESE();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int);

public:
	int m_Beh_stVal;
	float m_ChaA_mag_f;
	float m_ChaPwrLim_setMag_f;
	float m_ChaPwrRtg_setMag_f;
	float m_ChaPwrTgt_setMag_f;
	float m_ChaV_mag_f;
	string m_ConnACRef_intAddr;
	string m_ConnACRef_setSrcRef;
	string m_ConnDCRef_intAddr;
	string m_ConnDCRef_setSrcRef;
	string m_EVSEId_setVal;
	string m_EVSENam_vendor;
	int m_Mod_stVal;
	string m_NamPlt_swRev;
	string m_NamPlt_vendor;
	float m_ChaA = 0.;
	float m_ChaV = 0.;
	float m_ChaPwrLim = 0.;
	float m_ChaPwrTgt = 0.;
	float m_ChaPwrRtg = 0.;
};

