/*

ZamLLN0.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once
#include "ZamEquip.hpp"

class CZamLLN0 :
	public CZamEquip
{
public:
	CZamLLN0();
	~CZamLLN0();
	int Demarrage();
	bool Initialisation();
	int Lance();
	int Arret();
	void ZamProc(int);

private:
	int m_Beh_q;
	unsigned long long m_Beh_t;
	int m_Health_q;
	int m_Health_stVal;
	unsigned long long m_Health_t;
	int m_Mod_ctlModel;
	int m_Mod_q;
	unsigned long long m_Mod_t;
	string m_NamPlt_ldNs;
	string m_NamPlt_swRev;
	string m_NamPlt_vendor;
  public:
	int m_Beh_stVal;
	int m_Mod_stVal;
};

