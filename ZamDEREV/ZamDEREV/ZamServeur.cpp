/*

ZamServeur.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamServeur.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include "ParamCmd.h"
#include "pugixml.hpp"

using namespace std;

using namespace pugi;

CZamServer::CZamServer()
{
}

CZamServer::~CZamServer()
{
}

int CZamServer::Demarrage()
{
	int err = 0;
	m_start = false;
	Conf();
	err = Lance();
	if (err != 0)
	{
		m_zl->info("Probleme au lancement du Serveur");
		return -2;
	}
	while (!m_start)
	{
		this_thread::sleep_for(chrono::milliseconds(100));;
	}
	return 0;
}

int CZamServer::Conf()
{
	int err = 0;
	m_zl->info(fmt::format("Using libIEC61850 version {}", LibIEC61850_getVersionString()));
	/* Create new server configuration object */
	m_config = IedServerConfig_create();
	/* Set buffer size for buffered report control blocks to 200000 bytes */
	IedServerConfig_setReportBufferSize(m_config, 200000);
	/* Set stack compliance to a specific edition of the standard (WARNING: data model has also to be checked for compliance) */
	IedServerConfig_setEdition(m_config, IEC_61850_EDITION_2_1);
	/* Set the base path for the MMS file services */
	IedServerConfig_setFileServiceBasePath(m_config, "./vmd-filestore/");
	/* disable MMS file service */
	IedServerConfig_enableFileService(m_config, false);
	/* enable dynamic data set service */
	IedServerConfig_enableDynamicDataSetService(m_config, true);
	/* disable log service */
	IedServerConfig_enableLogService(m_config, false);
	/* set maximum number of clients */
	IedServerConfig_setMaxMmsConnections(m_config, 15);
	return err;
}

int CZamServer::Lance()
{
	int err = 0;
	thread thr(&CZamServer::ZamProc, this, 1);
	thr.detach();
	return err;
}

static ControlHandlerResult controlHandler(ControlAction ctlact, void* parameter, MmsValue* value, bool test)
{
	CParamCmd *cpc = static_cast<CParamCmd*>(parameter);
	CZamServer *ps = (CZamServer*) cpc->pthis;
	int nc = cpc->nc;
	if (ps->HandlerCommande(nc, value, test))
	{
		return CONTROL_RESULT_FAILED;
	} 
	return CONTROL_RESULT_OK; 
}

static MmsDataAccessError writeAccessHandler(DataAttribute* dataAttribute, MmsValue* value, ClientConnection connection, void* parameter)
{
	CZamServer *ps = static_cast<CZamServer*>(parameter);
	if(ps->WriteAccess(dataAttribute, value))
	{
		return DATA_ACCESS_ERROR_SUCCESS;
	}
	else
	{
		return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;
	}
}

bool CZamServer::WriteAccess(DataAttribute* attr, MmsValue* value)
{
	IedServer_updateAttributeValue(m_server, attr, value);	
	return true;
}


void CZamServer::ZamProc(int i)
{
	try
	{
		m_nomcfg = m_nomied + ".cfg";
		FileHandle configFile = FileSystem_openFile((char*) m_nomcfg.data(), false);
		if (configFile == nullptr)
		{
			throw string("Erreur ouverture du fichier .cfg");
		}
		m_model = ConfigFileParser_createModelFromConfigFile(configFile);
		FileSystem_closeFile(configFile);
		if (m_model == nullptr)
		{
			throw string("Erreur creation du modele");
		}
		m_server = IedServer_create(m_model);
// Access to data attributes by object reference
		IedServerConfig_destroy(m_config);
		IedServer_setGooseInterfaceId(m_server, "eth0");
		IedServer_start(m_server, m_tcpPort);
		if (!IedServer_isRunning(m_server))
		{
			IedServer_destroy(m_server);
			throw string("Probleme au demarrage du serveur");
		}
// Commandes
		CParamCmd pcmd01, pcmd02, pcmd03, pcmd04;

		m_ev_Mod = (DataObject*) IedModel_getModelNodeByShortObjectReference(m_model, "LDEV1/LLN0.Mod");
		pcmd01.pthis = this;
		pcmd01.nc = 1;
		IedServer_setControlHandler(m_server, m_ev_Mod, (ControlHandler) controlHandler, (void*) &pcmd01);

		m_fsch1_DsaReq = (DataObject*) IedModel_getModelNodeByShortObjectReference(m_model, "LDEV1/FSCH1.DsaReq");
		pcmd02.pthis = this;
		pcmd02.nc = 2;
		IedServer_setControlHandler(m_server, m_fsch1_DsaReq, (ControlHandler) controlHandler, (void*) &pcmd02);

		m_dpmc_WLimSpt1 = (DataObject *)IedModel_getModelNodeByShortObjectReference(m_model, "LDEV1/DPMC1.WLimSpt1");
		pcmd03.pthis = this;
		pcmd03.nc = 3;
		IedServer_setControlHandler(m_server, m_dpmc_WLimSpt1, (ControlHandler)controlHandler, (void *)&pcmd03);

		IedServer_setConnectionIndicationHandler(m_server, (IedConnectionIndicationHandler) connectionHandler, nullptr);
// Boucle
		while (true)
		{
			if (!m_start) m_start = true;
			if (m_stop) break;
			this_thread::sleep_for(chrono::milliseconds(500));
		}

// Boucle		
		m_zl->info("Arret du serveur");
		IedServer_stop(m_server);
		IedServer_destroy(m_server);
		IedModel_destroy(m_model);
	}	// try
	catch(string exc)
	{
		m_zl->info(fmt::format( "Exception {}", exc));
	}
	catch(exception e)
	{
		m_zl->info(fmt::format("Exception {}", string(e.what())));
	}
	catch(...)
	{
		m_zl->info(fmt::format("Exception ..."));
	}
}

static void connectionHandler(IedServer m_server, ClientConnection connection, bool connected, void* parameter)
{
//	if (connected)
//		cout << "Connexion" << endl;
//	else
//		cout << "Deconnexion" << endl;
}

int CZamServer::HandlerCommande(int nc, MmsValue* value, bool test)
{
	int err = 0;
	uint64_t timestamp = Hal_getTimeInMs();
	DataAttribute *pat, *pav, *pato, *pavo;
	bool b;
	switch(nc)
	{
	case 1:		
		pat = (DataAttribute*) IedModel_getModelNodeByShortObjectReference(m_model, "LDEV1/LLN0.Mod.t");
		pav = (DataAttribute*) IedModel_getModelNodeByShortObjectReference(m_model, "LDEV1/LLN0.Mod.stVal");
		IedServer_updateUTCTimeAttributeValue(m_server, pat, timestamp);
		IedServer_updateAttributeValue(m_server, pav, value);
		break;
	case 2:
		pat = (DataAttribute*) IedModel_getModelNodeByShortObjectReference(m_model, "LDEV1/FSCH1.DsaReq.t");
		pav = (DataAttribute*) IedModel_getModelNodeByShortObjectReference(m_model, "LDEV1/FSCH1.DsaReq.stVal");
		IedServer_updateUTCTimeAttributeValue(m_server, pat, timestamp);
		b = MmsValue_getBoolean(value);
		IedServer_updateBooleanAttributeValue(m_server, pav, b);
		break;
	case 3:
		pat = (DataAttribute *)IedModel_getModelNodeByShortObjectReference(m_model, "LDEV1/DPMC1.WLimSpt1.t");
		pav = (DataAttribute *)IedModel_getModelNodeByShortObjectReference(m_model, "LDEV1/DPMC1.WLimSpt1.mxVal");
		if (!test)
		{
			IedServer_updateUTCTimeAttributeValue(m_server, pat, timestamp);
			IedServer_updateAttributeValue(m_server, pav, value);
		}
		break;
	default:
		err = 1;
	}
	return err;
}

bool CZamServer::Litb(const string& sref)
{
	DataAttribute* dav = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Ecrit {}", sref));
	return IedServer_getBooleanAttributeValue(m_server, dav);		
}

int CZamServer::Liti(const string& sref)
{
	DataAttribute* dav = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Liti {}", sref));
	int val = IedServer_getInt32AttributeValue(m_server, dav);
	return val;
}

unsigned int CZamServer::Litui(const string& sref)
{
	DataAttribute* dav = (DataAttribute*)IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Litui {}", sref));
	int val = IedServer_getUInt32AttributeValue(m_server, dav);
	return val;
}

float CZamServer::Litf(const string& sref)
{
	DataAttribute* dav = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Litf {}", sref));
	return IedServer_getFloatAttributeValue(m_server, dav);		
}

double CZamServer::Litd(const string& sref)
{
	DataAttribute* dav = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Litd {}", sref));
	return IedServer_getFloatAttributeValue(m_server, dav);		
}

string CZamServer::Lits(const string& sref)
{
	DataAttribute* dav = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Lits {}", sref));
	return string(IedServer_getStringAttributeValue(m_server, dav));		
}

unsigned long long CZamServer::Litull(const string &sref)
{
	DataAttribute *dav = (DataAttribute *)IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Litull {}", sref));
	return IedServer_getUTCTimeAttributeValue(m_server, dav);
}

void CZamServer::Ecrit(const string &doref, const string& daref, bool b)
{
	string sref = doref + "." + daref;
	string tref = doref + ".t";
	DataAttribute* dav = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Ecrit {}", sref));
	DataAttribute *dat = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, tref.data());
	IedServer_lockDataModel(m_server);
	IedServer_updateBooleanAttributeValue(m_server, dav, b);
	if (dat) MmsValue_setUtcTimeMs(dat->mmsValue, Hal_getTimeInMs());
	IedServer_unlockDataModel(m_server);
}

void CZamServer::Ecrit(const string &doref, const string& daref, int i)
{
	string sref = doref + "." + daref;
	string tref = doref + ".t";
	DataAttribute* dav = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Ecrit {}", sref));
	DataAttribute *dat = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, tref.data());
	IedServer_lockDataModel(m_server);
	IedServer_updateInt32AttributeValue(m_server, dav, i);
	if (dat) MmsValue_setUtcTimeMs(dat->mmsValue, Hal_getTimeInMs());
	IedServer_unlockDataModel(m_server);
}

void CZamServer::Ecrit(const string &doref, const string& daref, float f)
{
	string sref = doref + "." + daref;
	string tref = doref + ".t";
	DataAttribute* dav = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Ecrit {}", sref));
	DataAttribute* dat = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, tref.data());
	IedServer_lockDataModel(m_server);
	IedServer_updateFloatAttributeValue(m_server, dav, f);
	if (dat) MmsValue_setUtcTimeMs(dat->mmsValue, Hal_getTimeInMs());
	IedServer_unlockDataModel(m_server);
}

void CZamServer::Ecrit(const string &doref, const string& daref, double d)
{
	string sref = doref + "." + daref;
	string tref = doref + ".t";
	DataAttribute* dav = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Ecrit {}", sref));
	DataAttribute *dat = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, tref.data());
	IedServer_lockDataModel(m_server);
	IedServer_updateFloatAttributeValue(m_server, dav, d);
	if (dat) MmsValue_setUtcTimeMs(dat->mmsValue, Hal_getTimeInMs());
	IedServer_unlockDataModel(m_server);
}

void CZamServer::Ecrit(const string &doref, const string& daref, string& s)
{
	string sref = doref + "." + daref;
	string tref = doref + ".t";
	DataAttribute* dav = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Ecrit {}", sref));
	DataAttribute *dat = (DataAttribute*) IedModel_getModelNodeByObjectReference(m_model, tref.data());
	IedServer_lockDataModel(m_server);
	IedServer_updateVisibleStringAttributeValue(m_server, dav, (char*)s.data());			
	if(dat) MmsValue_setUtcTimeMs(dat->mmsValue, Hal_getTimeInMs());
	IedServer_unlockDataModel(m_server);
}

void CZamServer::Ecrit(const string &doref, const string &daref, unsigned long long u)
{
	string sref = doref + "." + daref;
	string tref = doref + ".t";
	DataAttribute *dav = (DataAttribute *)IedModel_getModelNodeByObjectReference(m_model, sref.data());
	if (!dav) m_zl->info(fmt::format("pb Ecrit {}", sref));
	DataAttribute *dat = (DataAttribute *)IedModel_getModelNodeByObjectReference(m_model, tref.data());
	IedServer_lockDataModel(m_server);
	IedServer_updateUTCTimeAttributeValue(m_server, dav, u);
	if (dat) MmsValue_setUtcTimeMs(dat->mmsValue, Hal_getTimeInMs());
	IedServer_unlockDataModel(m_server);
}

