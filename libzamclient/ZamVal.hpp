/*

ZamVal.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "iec61850_client.h"
#include <string>

using namespace std;

class CZamVal
{
public:
	CZamVal()
	{
	}
	virtual ~CZamVal()
	{
	}
public:
	IedClientError erreur;
	void setok(IedClientError er) {erreur = er;}
	bool ok() {return (erreur == IED_ERROR_OK) ? true : false;}
};

class CZamfloat : public CZamVal
{
public:
	float val;
public:
	void set(float f, IedClientError e) {erreur = e; val = f; }
	float get() { return val;}
};

class CZamdouble : public CZamVal
{
public:
	double val;
public:
	void set(double d, IedClientError e) {erreur = e; val = d; }
	double get() { return val;}
};

class CZambool : public CZamVal
{
public:
	bool val;
public:
	void set(bool b, IedClientError e) {erreur = e; val = b; }
	bool get() { return val;}
};

class CZamint : public CZamVal
{
public:
	int val;
public:
	void set(int i, IedClientError e) {erreur = e; val = i; }
	int get()  { return val;}
};

class CZamstring : public CZamVal
{
public:
	string val;
public:
	void set(string s, IedClientError e) {erreur = e; val = s; }
	string get()  { return val;}
};
