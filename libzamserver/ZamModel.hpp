/*

ZamModel.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include <memory>
#include <map>
#include <string>
#include "ZamVar.hpp"
#include "pugixml.hpp"
#include <filesystem>
#include <fstream>

using namespace std;
namespace fs = filesystem;
using namespace pugi;

class CZamLNType;
class CZamDOType;
class CZamDAType;
class CZamEnumType;
class CZamIed;
class CZamLD;
class CZamLN;

//class CZamVar;
class CZamDO;

class CZamModel
{
public:
	CZamModel();
	~CZamModel();
	CZamModel(const string&);

public:
	int Demarrage();
	int LitICD();

  protected:
	void Constantes();
	string Valitype(const string&);
	string UnderscoreToPoint(const string&);
	string PointToUnderscore(const string& ref);
	void Dod1Ln(shared_ptr<CZamLN> pln);
	void Dad1Do(shared_ptr<CZamDO>);
  public:
	xml_document m_doc;
	string m_pext;
	string m_nomficied;
	string m_nomfichier;
	string m_nomied;
	shared_ptr<CZamIed> m_ied;
	map<string, int> m_mapfc;
	map<string, int> m_maptyp;
	map<int, string> m_maptyps;
	map<string, shared_ptr<CZamLNType>> maplntype;
	map<string, shared_ptr<CZamDOType>> mapdotype;
	map<string, shared_ptr<CZamDAType>> mapdatype;
	map<string, shared_ptr<CZamEnumType>> mapenumtype;
	map<string, shared_ptr<CZamVar>> m_mapvar;
	bool m_stop = false;
	map<string, shared_ptr<CZamIed>> m_map_IED;
	shared_ptr<CZamLD> m_pld;
	shared_ptr<CZamLN> m_pln;
	shared_ptr<CZamDO> m_pdo;
};

