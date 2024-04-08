/*

ZamUnit.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include <string>
#include <map>
#include <memory>
#include <thread>
#include <chrono>
#include "ZamProcess.hpp"
#include "ZamVar.hpp"
#include "ZamEquip.hpp"
#include "ZamLD.hpp"
#include "ZamModel.hpp"
#include "ZamServeur.hpp"
#include "ZamLN.hpp"
#include "ZamMemp.hpp"
#include <ZamLog.h>

using namespace std;

class CZamProcess;
class CZamEquip;
class CZamLD;

typedef map<string, shared_ptr<CZamVar>> dicvarun;
typedef map<string, shared_ptr<CZamEquip>> dicequip;

class CZamUnit
{
public:
	CZamUnit ();
	~CZamUnit ();
	virtual int Demarrage ()= 0;
	virtual bool Initialisation ()= 0;
	virtual int Lance ()= 0;
	int Arret ();
	virtual void ZamProc (int)= 0;
	int CreEquip (shared_ptr<CZamLN>);
	bool Litb(const string& name) { return m_zs->Litb(name);};
	int Liti(const string& name) { return m_zs->Liti(name);};
	unsigned int Litui(const string& name) { return m_zs->Litui(name);};
	float Litf(const string& name) { return m_zs->Litf(name);};
	double Litd(const string& name) { return m_zs->Litd(name);};
	string Lits(const string& name) { return m_zs->Lits(name);};
	unsigned long long Litull(const string& name) { return m_zs->Litull(name);};
	void Ecrit(const string& ndo, const string& nda, bool b) { m_zs->Ecrit(ndo, nda, b);};
	void Ecrit(const string& ndo, const string& nda, int i) { m_zs->Ecrit(ndo, nda, i);};
	void Ecrit(const string& ndo, const string& nda, float f) { m_zs->Ecrit(ndo, nda, f);};
	void Ecrit(const string& ndo, const string& nda, double d) { m_zs->Ecrit(ndo, nda, d);};
	void Ecrit(const string& ndo, const string& nda, string& s) { m_zs->Ecrit(ndo, nda, s);};
	void Ecrit(const string& ndo, const string& nda, unsigned long long ull) { m_zs->Ecrit(ndo, nda, ull);};
public:
	string m_IED;
	string m_LD;
	string m_inst;
	string m_nom;
	bool m_stop= false;
	shared_ptr<CZamModel> m_zm;
	shared_ptr<CZamMemp> m_zmp;
	shared_ptr<CZamServer> m_zs;
	shared_ptr<CZamProcess> m_zp;
	shared_ptr<CZamLog> m_zl;
	shared_ptr<CZamUnit> m_zu;
	shared_ptr<CZamLD> m_ld;
	dicvarun m_dv;
	dicequip m_de;
};