/*

ZamLD.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include <string>
#include <map>
#include "ZamLN0.hpp"

using namespace std;

class CZamUnit;

class CZamLD
{
public:
	CZamLD();
	~CZamLD();
	void CreUnit();
	int Demarrage();
	bool Initialisation();
	int Lance();
	int Arret();

public:
	shared_ptr<CZamUnit> m_unit;
	string inst;
	shared_ptr<CZamLN0> pln0;
	int nbln;
	map<string, shared_ptr<CZamLN>> mapln;
};


