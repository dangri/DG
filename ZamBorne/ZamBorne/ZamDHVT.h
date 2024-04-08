/*

ZamDHVT.h

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

class CZamDHVT : public CZamEquip
{
public:
	CZamDHVT();
	~CZamDHVT();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int i);
	int Arret();

public:
	int m_Beh_q;
	int m_Beh_stVal;
	unsigned long long m_Beh_t;
	string m_EcpRef_setSrcRef;
	bool m_FctEna_Cancel_Test;
	unsigned int m_FctEna_Oper_Check;
	int m_FctEna_ctlModel;
	unsigned short m_FctEna_ctlNum;
	string m_FctEna_origin_orIdent;
	int m_FctEna_q;
	bool m_FctEna_stVal;
	unsigned long long m_FctEna_t;
	string m_NamPlt_swRev;
	string m_NamPlt_vendor;
};
