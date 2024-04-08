/*

ZamProcess.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include <memory>
#include <list>
#include <string>
#include <map>
#include <filesystem>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include "semaphore.h"
#include <boost/interprocess/shared_memory_object.hpp>
#include "ZamUnit.hpp"
#include "ZamVar.hpp"
#include "ZamStemps.hpp"
#include "ZamServeur.hpp"
#include "ZamMemp.hpp"
#include "ZamSignal.hpp"
#include "goose_subscriber.h"
#include <pugixml.hpp>
#include <ZamLog.h>

using namespace std;
namespace fs = filesystem;

class CZamUnit;
class CZamVar;
class CZamStemps;
class CZamCinit;
class CZamModel;
class CZamServer;
class CZamIed;
class CZamLD;
class CZamLDCS;
class CZamLDSE;
class CZamSubstation;
class CZamCondeq;
class CZamEqf;
class CZamFunc;
class CZamLNode;
class CZamTerm;
class CZamCondnode;
class CZamBay;

typedef map<string, shared_ptr<CZamVar>> dicvarun;
typedef map<string, shared_ptr<CZamUnit>> dicunit;

class CZamProcess
{
public:
	CZamProcess();
	~CZamProcess();
	int start();
	void stop();
	sem_t* semaphore(const string& );
	int LitSCD();
	int LitBay(pugi::xml_node, shared_ptr<CZamBay>);
	void Init();
	void EcritMp();
	void InitStation();
	void InitCS();
	int Demarrage();
	int Arret();
	int CreUnit(shared_ptr<CZamLD>);
	void EcouteGoose(GooseSubscriber subscriber);
	void ZamProc(int);
	string AdresseIP(const string&);
	string AdresseMac(const string &);
	string Sheure();
	bool Litb(const string& name) { return m_zs->Litb(name);};
	int Liti(const string& name) { return m_zs->Liti(name);};
	unsigned int Litui(const string& name) { return m_zs->Litui(name);};
	float Litf(const string& name) { return m_zs->Litf(name);};
	double Litd(const string& name) { return m_zs->Litd(name);};
	string Lits(const string& name) { return m_zs->Lits(name);};
	unsigned long long Litull(const string& name) { return m_zs->Litull(name);};
	void Ecrit(const string& ndo, const string& nda, bool b) { m_zs->Ecrit(ndo, nda, b);};
	void Ecrit(const string& ndo, const string& nda, int i) { m_zs->Ecrit(ndo, nda, i);};
	void Ecrit(const string& ndo, const string& nda, float f) { m_zs->Ecrit(ndo, nda, f);}	;
	void Ecrit(const string& ndo, const string& nda, double d) { m_zs->Ecrit(ndo, nda, d);};
	void Ecrit(const string& ndo, const string& nda, string s) { m_zs->Ecrit(ndo, nda, s);};
	void Ecrit(const string& ndo, const string& nda, unsigned long long ull) { m_zs->Ecrit(ndo, nda, ull);};
private:
	int Lancement();
	map<int, string> m_params;
	map<string, int> m_mapd;
	SignalHandler m_signalHandler;
protected:
	boost::array<unsigned char, 10> m_rec_buf;
	boost::array<unsigned char, 20> m_env_buf;
public:
	shared_ptr<CZamLDCS> m_ldcs;
	string m_nomfichier;
	shared_ptr<CZamIed> m_ied;
	string m_nom;
	string m_desc;
	string m_nomied;
	string m_adresseIP_wlan0;
	string m_adresseIP_eth0;
	string m_adresseMAC_eth0;
	string m_adresseMAC_wlan0;
	unsigned long m_ladresse;
	string m_adresse;
	unsigned long long m_temps;
	shared_ptr<CZamModel> m_zm;
	shared_ptr<CZamProcess> m_zp;
	shared_ptr<CZamServer> m_zs;
	shared_ptr<CZamStemps> m_zt;
	shared_ptr<CZamMemp> m_zmp;
	shared_ptr<CZamLog> m_zl;
	dicunit m_du;
	dicvarun m_dv;
	string m_msg;
	bool m_stop = false;
	string m_nomvar;
	string m_semap;
	unsigned int m_taille_var;
	sem_t* m_sem;
	sem_t *m_semdg;
	sem_t* m_semev;
	unsigned int m_port = 4000;
	unsigned int m_pidevb = 0;
	unsigned int m_pidzam = 0;
	bool m_change;
	bool m_reinit = false;
	bool m_bcon = false;  // true si client connect
	boost::interprocess::shared_memory_object m_mempar;
	shared_ptr<CZamSubstation> m_substation;
	map<string, shared_ptr<CZamCondeq>> m_map_Condeq;
	map<string, shared_ptr<CZamEqf>> m_map_Eqf;
	map<string, shared_ptr<CZamLNode>> m_map_LNode;
	map<string, shared_ptr<CZamTerm>> m_map_Term;
	map<string, shared_ptr<CZamFunc>> m_map_Func;
	map<string, shared_ptr<CZamCondnode>> m_map_Condnode;
};

void gooseListener(GooseSubscriber subscriber, void *parameter);
