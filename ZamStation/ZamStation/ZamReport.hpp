/*

ZamReport.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include <string>
#include "ZamProcess.hpp"
#include "ZamUnit.hpp"
#include "ZamServeur.hpp"
#include "ZamClient.hpp"
#include "ZamVarc.hpp"
#include "ZamDEEVP.hpp"
#include "ZamLDSE.hpp"

using namespace std;

class CZamUnit;

class CZamReport : public CZamEquip
{
public:
	CZamReport();
	~CZamReport();
	int Demarrage();
	int Lance();
	void ZamProc(int);
	int TransSchd(const string& srcref);
	int Arret();

  private:
	bool Initialisation();
public:
	bool m_stop;
	int m_port = 102;
	string m_nom;
	string m_ref;
	string m_inst;
	string m_refs;
	string m_adresse;
	string m_nom_IEDR;
	bool m_bcon;
	ClientReportControlBlock m_rcb;
	shared_ptr<CZamReport> m_cb;
	shared_ptr<CZamClient> m_zc;
	shared_ptr<CZamDEEVP> m_deev;
	shared_ptr<CZamLDSE> m_ldse;
	shared_ptr<CZamLDCS> m_ldcs;
};

