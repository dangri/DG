/*

Zamerveur.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include <memory>
#include <string>
#include <iostream>
#include "iec61850_server.h"
#include "hal_thread.h"
#include "hal_filesystem.h"
#include "iec61850_config_file_parser.h"
#include <filesystem>
#include <ZamLog.h>

using namespace std;
namespace fs = filesystem;

class CZamModel;

class CZamServer
{
public:
	CZamServer();
	~CZamServer();
	int Demarrage();
	int HandlerCommande(int, MmsValue*, bool);
	bool WriteAccess(DataAttribute* attr, MmsValue* value);

public:
  int Conf();
  void ZamProc(int);
  int Lance();
  bool Litb(const string &sref);
  int Liti(const string &sref);
  unsigned int Litui(const string &sref);
  float Litf(const string &sref);
  float Litmag(const string &sref);
  void Ecritmag(const string sref, float f);
  float Litang(const string &sref);
  void Ecritang(const string sref, float f);

  double Litd(const string &sref);
  string Lits(const string &sref);
  unsigned long long Litull(const string &sref);
  void Ecrit(const string &, const string &, bool);
  void Ecrit(const string &, const string &, int);
  void Ecrit(const string &, const string &, float);
  void Ecrit(const string &, const string &, double);
  void Ecrit(const string &, const string &, string&);
  void Ecrit(const string &, const string &, unsigned long long);
  void Ecritmag(const string &doref, float);
  void Ecritang(const string &doref, float);
 
public:
  IedServerConfig m_config;
  int m_tcpPort = 102;
  string m_nomied;
  string m_nomficied;
  bool m_stop = false;
  bool m_start = false;
  string m_nomcfg;
  shared_ptr<CZamServer> m_zs;
  shared_ptr<CZamModel> m_zm;
  shared_ptr<CZamLog> m_zl;
  IedServer m_server;
  IedModel *m_model;
  DataObject *m_fsch01_EnaReq;
  DataObject *m_fsch01_DsaReq;
  DataObject *m_fsch02_EnaReq;
  DataObject *m_fsch02_DsaReq;
  DataObject *m_cs_Mod;
  DataObject *m_evse1_Mod;
  DataObject *m_evse2_Mod;
};


static ControlHandlerResult controlHandler(void* parameter, MmsValue* value, bool test);
static void connectionHandler(IedServer, ClientConnection connection, bool connected, void* parameter);
static MmsDataAccessError writeAccessHandler(DataAttribute* dataAttribute, MmsValue* value, ClientConnection connection, void* parameter);


