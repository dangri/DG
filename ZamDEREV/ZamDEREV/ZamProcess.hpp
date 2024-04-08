/*

ZamProcess.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamUnit.hpp"
#include "ZamVar.hpp"
#include <memory>
#include <list>
#include <string>
#include <filesystem>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include "pugixml.hpp"
#include "ZamCtemps.hpp"
#include "semaphore.h"
#include "ZamServeur.hpp"
#include <boost/interprocess/shared_memory_object.hpp>
#include "ZamLDEV.hpp"
#include "ZamMemp.hpp"
#include "ZamSignal.hpp"
#include <ZamLog.h>

using namespace std;
namespace fs = filesystem;
using namespace pugi;

class CZamUnit;
class CZamVar;
class CZamCtemps;
class CZamModel;
class CZamServer;
class CZamIed;
class CZamLD;
class CZamLDEV;

typedef map<string, shared_ptr<CZamVar>> dicvarun;
typedef map<string, shared_ptr<CZamUnit>> dicunit;

class CZamProcess
{
public:
	CZamProcess();
	CZamProcess(const string &);
	~CZamProcess();
	sem_t *semaphore(const string &nom);
	int start();
	void stop();
	int Demarrage();
	int CreUnit(shared_ptr<CZamLD>);
	int Arret();
	int CrevarT(const string&);
	void ZamProc(int i);
	int Init();
	void EcritMp();
	int Initvehicule();
	string AdresseIP(const string&);
	string AdresseMac(const string&);
	bool Litb (const string& name) { return m_zs->Litb (name); };
	int Liti (const string& name) { return m_zs->Liti (name); };
	unsigned int Litui (const string& name) { return m_zs->Litui (name); };
	float Litf (const string& name) { return m_zs->Litf (name); };
	double Litd (const string& name) { return m_zs->Litd (name); };
	string Lits (const string& name) { return m_zs->Lits (name); };
	unsigned long long Litull (const string& name) { return m_zs->Litull (name); };
	void Ecrit (const string& ndo, const string& nda, bool b) { m_zs->Ecrit (ndo, nda, b); };
	void Ecrit (const string& ndo, const string& nda, int i) { m_zs->Ecrit (ndo, nda, i); };
	void Ecrit (const string& ndo, const string& nda, float f) { m_zs->Ecrit (ndo, nda, f); };
	void Ecrit (const string& ndo, const string& nda, double d) { m_zs->Ecrit (ndo, nda, d); };
	void Ecrit (const string& ndo, const string& nda, string s) { m_zs->Ecrit (ndo, nda, s); };
	void Ecrit (const string& ndo, const string& nda, unsigned long long ull) { m_zs->Ecrit (ndo, nda, ull); };
private:
	int Lancement();
	SignalHandler m_signalHandler;
	map<int, string> m_params;
	map<string, int> m_mapd;
public:
	shared_ptr<CZamIed> m_ied;
	string m_nomld;
	string m_desc;
	string m_nomied;
	string m_adresseIP_wlan0;
	string m_adresseIP_eth0;
	string m_adresseMac_wlan0;
	string m_adresseMac_eth0;
	unsigned long m_ladresse;
	unsigned int m_port = 4000;
protected:
	boost::array<unsigned char, 10> m_rec_buf;
	boost::array<unsigned char, 20> m_env_buf;
public:
	double m_temps;
	string m_stemps;
	bool m_bt = false;
	bool m_arret = false;
	bool m_stop = false;
	shared_ptr<CZamProcess> m_zp;
	shared_ptr<CZamLDEV> m_ldev;
	shared_ptr<CZamCtemps> m_zt;
	shared_ptr<CZamModel> m_zm;
	shared_ptr<CZamServer> m_zs;
	shared_ptr<CZamMemp> m_zmp;
	shared_ptr<CZamLog> m_zl;
	dicunit m_du;
	dicvarun m_dv;
	string m_msg;
	bool m_wifi = false;
	string m_adresse_station;
	string m_adresse;
	string m_nomvar;
//	string m_nombase;
	int m_taille_var;
	shared_ptr<CZamVar> m_pt;
	sem_t *m_sem;
	sem_t *m_semdg;
	boost::interprocess::shared_memory_object m_mempar;
	pid_t m_pidevv;
	pid_t m_pidzam;
	int m_nv;
	int m_sens; // +1 charge  -1 decharge
};
