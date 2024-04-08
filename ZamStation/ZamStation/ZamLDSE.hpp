/*

ZamLDSE.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once
#include "ZamUnit.hpp"
#include "ZamCalcul.h"

class CZamLLN0;
class CZamLPHD;
class CZamClient;
class CZamReport;
class CZamDEEVP;
class CZamLDCS;
class CZamCalcul;

class CZamLDSE : public CZamUnit
{
public:
	CZamLDSE ();
	virtual ~CZamLDSE ();
	int Demarrage ();
	void ZamProc (int i);
	int Lance ();

private:
	bool Initialisation ();

public:
	shared_ptr<CZamLLN0> m_lln0;
	shared_ptr<CZamLPHD> m_lphd;
	string m_adresse_ve;
	string m_adresse_se;
	shared_ptr<CZamServer> m_zs;
	shared_ptr<CZamReport> m_zcb;
	shared_ptr<CZamClient> m_zc1;
	shared_ptr<CZamLDSE> m_ldse;
	shared_ptr<CZamDEEVP> m_deev;
	shared_ptr<CZamLDCS> m_ldcs;
	shared_ptr<CZamCalcul> m_cal;
	bool m_bcon;
	int m_ibeh;
	string m_inst;
	string m_energie;
};
