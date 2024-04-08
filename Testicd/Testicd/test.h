#pragma once

#include <memory>
#include <map>
#include <string>
#include "pugixml.hpp"
#include <filesystem>
#include <fstream>
#include "ZamVar.hpp"

using namespace std;
namespace fs = filesystem;
using namespace pugi;

class CZamLNType;
class CZamDOType;
class CZamDAType;
class CZamEnumType;
class CZamIed;
class CZamLN;

//class CZamVar;
class CZamDO;


class Ctest
{
  public:
	Ctest();
	~Ctest();
	Ctest(const string &);

  public:
	int Demarrage();
	int LitICD();
	void EcritEntete_H();
	void EcritEntete_CPP();
	void EcritInit();
	void EcritProc();
	void EcritArret();
	void EcritInfoH();
	void EcritInfoC();

  protected:
	void Constantes();
	string Valitype(const string&);
	int EcritVariables();
	string UnderscoreToPoint(const string &);
	string PointToUnderscore(const string &ref);
	void Dad1Do(shared_ptr<CZamDO>);

  public:
	xml_document m_doc;
	string m_pext;
	string m_nomficied;
	string m_nomfichier;
	string m_nom;
	string m_nomied;
	shared_ptr<CZamIed> m_ied;
	shared_ptr<CZamLN> m_pln;
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
	ofstream m_osfich, m_osficc;
};

