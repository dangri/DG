/*

ZamMemp.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once
#include "ZamVarp.hpp"
#include <memory>
#include <string>
#include <map>
#include <boost/interprocess/shared_memory_object.hpp>
#include <complex>

using namespace std;

class CZamMemp
{
public:
	CZamMemp();
	~CZamMemp();
	CZamMemp(const string&, const int);
	int AddVar(const string&, const string&);
	int AddVars(const string &, const string &);
	bool Litb(const string&);	
	int Liti(const string&);
	float Litf(const string&);
	double Litd(const string&);
	string Lits(const string&);
	complex<double> Litc(const string&);
	unsigned long long Litull(const string&);
	int Ecrit(const string&, bool);
	int Ecrit(const string&, int);
	int Ecrit(const string&, float);
	int Ecrit(const string&, double);
	int Ecrit(const string&, const string&);
	int Ecrit(const string& nomvar, char* c);
	int Ecrit(const string&, unsigned long long);
	int Ecrit(const string&, complex<double>);

  public:
	int m_prochain = 0;
	int m_prochains = 3000;
	boost::interprocess::shared_memory_object m_mempar;
	map<string, shared_ptr<CZamVarp>> m_mapvar;
	map<int,shared_ptr<CZamVarp>> m_mapvara;
	int m_taille_bool = 4;
	int m_taille_int = 4;
	int m_taille_float = 4;
	int m_taille_double = 8;
	int m_taille_ull = 8;
	int m_taille_string = 256;
	int m_taille_ip = 20;
	int m_taille_comp = 16;
};
