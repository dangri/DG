/*

ZamMemp.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamMemp.hpp"
#include <iostream>
#include <stdlib.h>
#include <memory>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/permissions.hpp>

using namespace std;

CZamMemp::CZamMemp()
{
}

CZamMemp::~CZamMemp()
{
}

CZamMemp::CZamMemp(const string& nomvar, int taille)
{
	boost::interprocess::permissions perm;
	perm.set_unrestricted();
	m_mempar = boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, nomvar.c_str(), boost::interprocess::read_write, perm);
	m_mempar.truncate(taille);
	m_prochain = 0;
}

int CZamMemp::AddVar(const string& nomvar, const string& type)
{
	auto pvar(make_shared<CZamVarp>());
	pvar->m_nom = nomvar;
	pvar->m_type = type;
	pvar->m_taille = 8; 
	if(type == "bool") pvar->m_taille = m_taille_bool;
	if(type == "int") pvar->m_taille = m_taille_int;
	if(type == "float") pvar->m_taille = m_taille_float;
	if(type == "double") pvar->m_taille = m_taille_double;
	if(type == "ull") pvar->m_taille = m_taille_ull;
	if(type == "string") pvar->m_taille = m_taille_string;
	if(type == "ip") pvar->m_taille = m_taille_ip;
	if (type == "comp") pvar->m_taille = m_taille_comp;
	pvar->m_position = m_prochain;
	m_mapvara[pvar->m_position] = pvar;
	m_mapvar[nomvar] = pvar;
	m_prochain += pvar->m_taille;
	return pvar->m_position;
}

int CZamMemp::AddVars(const string &nomvar, const string &type)
{
	auto pvar(make_shared<CZamVarp>());
	pvar->m_nom = nomvar;
	pvar->m_type = type;
	pvar->m_taille = 8;
	if (type == "bool") pvar->m_taille = m_taille_bool;
	if (type == "int") pvar->m_taille = m_taille_int;
	if (type == "float") pvar->m_taille = m_taille_float;
	if (type == "double") pvar->m_taille = m_taille_double;
	if (type == "ull") pvar->m_taille = m_taille_ull;
	if (type == "string") pvar->m_taille = m_taille_string;
	if (type == "ip") pvar->m_taille = m_taille_ip;
	if(type == "comp") pvar->m_taille = m_taille_comp;
	pvar->m_position = m_prochains;
	m_mapvara[pvar->m_position] = pvar;
	m_mapvar[nomvar] = pvar;
	m_prochains += pvar->m_taille;
	return pvar->m_position;
}

bool CZamMemp::Litb(const string& nomvar)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	bool* pt = static_cast<bool *>(mr.get_address());
	return *pt;
}	

int CZamMemp::Liti(const string& nomvar)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	int* pt = static_cast<int *>(mr.get_address());
	return *pt;
}

float CZamMemp::Litf(const string& nomvar)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	float* pt = static_cast<float *>(mr.get_address());
	return *pt;
}

double CZamMemp::Litd(const string& nomvar)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	double* pt = static_cast<double *>(mr.get_address());
	return *pt;
}

string CZamMemp::Lits(const string& nomvar)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	string s = string((char*) static_cast<char*>(mr.get_address()));
	return s;
}

unsigned long long CZamMemp::Litull(const string& nomvar)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	unsigned long long* pt = static_cast<unsigned long long *>(mr.get_address());
	return *pt;
}

complex<double> CZamMemp::Litc(const string &nomvar)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	complex<double> *pt = static_cast<complex<double> *>(mr.get_address());
	return *pt;
}

int CZamMemp::Ecrit(const string& nomvar, const bool b)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	bool* pt = static_cast<bool *>(mr.get_address());
	*pt = b;
 return 0;
}

int CZamMemp::Ecrit(const string& nomvar, const int i)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	int* pt = static_cast<int *>(mr.get_address());
	*pt = i;
	return 0;
}

int CZamMemp::Ecrit(const string& nomvar, const float f)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	float* pt = static_cast<float *>(mr.get_address());
	*pt = f;
	return 0;
}

int CZamMemp::Ecrit(const string& nomvar, const double d)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	double* pt = static_cast<double *>(mr.get_address());
	*pt = d;
	return 0;
}

int CZamMemp::Ecrit(const string& nomvar, const string& s)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	char* buf = (char*)calloc(pv->m_taille, 1);
	memcpy(buf, s.c_str(), s.length());
	memcpy(mr.get_address(), buf, pv->m_taille);
	free(buf);
	return 0;
}

int CZamMemp::Ecrit(const string& nomvar, char* c)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	char* buf = (char*)calloc(pv->m_taille, 1);
	memcpy(buf, c, pv->m_taille);
	memcpy(mr.get_address(), buf, pv->m_taille);
	free(buf);
	return 0;
}

int CZamMemp::Ecrit(const string& nomvar, const unsigned long long ull)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	unsigned long long* pt = static_cast<unsigned long long *>(mr.get_address());
	*pt = ull;
	return 0;
}

int CZamMemp::Ecrit(const string &nomvar, const complex<double> co)
{
	auto pv = m_mapvar[nomvar];
	boost::interprocess::mapped_region mr(m_mempar, boost::interprocess::read_write, pv->m_position, pv->m_taille);
	complex<double> *pt = static_cast<complex<double>*>(mr.get_address());
	*pt = co;
	return 0;
}