/*

ZamLNType.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s�appuient sur les normes de l�IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en �uvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier � sources_zamiren.txt �
*/

#pragma once

#include <string>
#include <map>
#include "ZamDO.hpp"

using namespace std;


class CZamLNType
{
public:
	CZamLNType();
	virtual ~CZamLNType();
public:
	string id;
	string lnClass;
	map<string, shared_ptr<CZamDO>> mapdo;
};



CZamLNType::CZamLNType()
{
}


CZamLNType::~CZamLNType()
{
}
