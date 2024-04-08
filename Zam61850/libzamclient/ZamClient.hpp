/*

ZamClient.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamVarc.hpp"
#include "iec61850_client.h"
#include "iec61850_model.h"
#include <memory>
#include <map>
#include <vector>
#include "ZamVarc.hpp"
#include "ZamVal.hpp"
#include <iostream>

using namespace std;

class CZamClient
{
public:
	CZamClient();
	CZamClient(const string&, const string&);
	CZamClient(const string&);
	~CZamClient();
	IedConnection getcon();
	IedConnection getcons();
	void decon();
	bool etacon();
	string getNom();
	float getfloat(const string&, const string&);
	double getdouble(const string&, const string&);
	bool getbool(const string&, const string&); 
	int getint(const string&, const string&);
	double getutct(const string& sref, const string& sfc);
	string getstring(const string&, const string&);
	int setbool(const string&, const string&, const bool);
	int setint(const string&, const string&, const int);
	int setuint(const string&, const string&, const int);
	int setfloat(const string&, const string&, const float);
	int setdouble(const string&, const string&, const double);
	int setutct(const string&, const string&, const unsigned long long);
	int setstring(const string&, const string&, const string&);
	void getval(MmsValue*, shared_ptr<CZamVarc>);
	void getDataset(vector<shared_ptr<CZamVarc>>&, const string&);
	int install_report(vector<shared_ptr<CZamVarc>>&, const string&, const string&, bool);
	void InstallConClose();
	void recrcb(ClientReport rep);
	ClientReportControlBlock get_rcb(const string& srcb);

  public:
	IedClientError cdeBool(const string& cRef, bool b, bool btest=false);
	IedClientError cdeInt(const string &sref, int i, bool btest=false);
	IedClientError cdeFloat(const string& cRef, float f, bool btest=false);
	void  OutTermination(ControlObjectClient connection);
	IedConnection m_con;
	string m_nom;
	string m_adresse;
	int m_port = 102;
	shared_ptr<CZamClient> m_m;
	bool m_surv;
	vector<shared_ptr<CZamVarc>> m_ds;
	ClientReportControlBlock m_rcb;
	bool m_stop_rcb;
	string m_rpt;
	bool m_conperdu;
};

static void closecon(void *parameter, IedConnection con);

void reportCallbackFunction(void* parameter, ClientReport report);	
static void comHandler(void *r, ControlObjectClient);
static void commandTerminationHandler(void *parameter, ControlObjectClient connection);
//char* StringUtils_copyString(const char* string);
static void rec_report(void *);
