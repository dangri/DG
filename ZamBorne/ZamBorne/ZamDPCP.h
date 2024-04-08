/*

ZamDPCP.h

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

class CZamDPCP : public CZamEquip
{
public:
	CZamDPCP();
	~CZamDPCP();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int i);
	int Arret();

public:
	string m_AreaEpsEcpId_setVal;
	int m_Beh_q;
	int m_Beh_stVal;
	unsigned long long m_Beh_t;
	string m_ElcRefId_setVal;
	int m_GndSys_setVal;
	string m_NamPlt_swRev;
	string m_NamPlt_vendor;
	float m_VRefSet_setMag_f;
	float m_WMaxSet_setMag_f;
};
