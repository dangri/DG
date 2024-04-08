/*

ZamConVE.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include <string>
#include "ZamLDSE.hpp"
#include "Zamevse.h"

using namespace std;

class CZamConVE
{
public:
	CZamConVE();
	~CZamConVE();
	CZamConVE(const string&);
  public:
  string m_adresse_ve = "";
  string m_EVId;
  string m_EMAId;
  string m_adresse_evse = "";
  string m_typcon;
  shared_ptr<CZamLDSE> m_ldse;
  string m_id_evse;
  bool m_bc = false;
};

