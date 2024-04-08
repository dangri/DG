/*

ZamGrec.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »

usage = shared_ptr<CZamGrec> m_zg; m_zg->Grecrec("eth0", "IEDEVLDEV1/LLN0$GO$gcbDSCS", 1000); MmsValue* val=m_zg->Ecoute();

*/

#include "ZamGrec.h"
#include <iostream>

using namespace std;

CZamGrec::CZamGrec()
{

}

CZamGrec::~CZamGrec()
{

}

void capte(GooseSubscriber subscriber, void* parameter)
{
	CZamGrec *pc = static_cast<CZamGrec*>(parameter);
	pc->Capte();
}

void CZamGrec::Capte()
{
	if (!m_capte)
	{
		m_capte = true;
		cout << "GOOSE event";
		cout << "  stNum: " << GooseSubscriber_getStNum(m_gsub) << "  sqNum: " << GooseSubscriber_getSqNum(m_gsub) << endl;
		cout << "time: " << GooseSubscriber_getTimestamp(m_gsub) << endl;
		m_val = GooseSubscriber_getDataSetValues(m_gsub);
	}
//	char buffer[1024];
//	MmsValue_printToBuffer(val, buffer, 1024);
//	cout << buffer << endl;
}

MmsValue* CZamGrec::Lit()
{
	if (!m_capte)
	{
		return nullptr;
	}
	else
	{
		m_capte = false;
	}
	return m_val;
}

void CZamGrec::Grecrec(const string& refi, string refg, ushort appid)
{
	//  exemples
	//  refi = "eth0"
	//	"IEDEVLDEV1/LLN0$GO$gcbDSCS"
	m_grec = GooseReceiver_create();
	GooseReceiver_setInterfaceId(m_grec, refi.c_str());
	m_gsub = GooseSubscriber_create((char*) refg.c_str(), NULL);
	GooseSubscriber_setAppId(m_gsub, appid);
	GooseSubscriber_setListener(m_gsub, capte, (void*) this);
	GooseReceiver_addSubscriber(m_grec, m_gsub);
}

bool CZamGrec::start()
{
	GooseReceiver_start(m_grec);
	if (!GooseReceiver_isRunning(m_grec))
	{
		cout << "Goose Receiver not running" << endl;
		return false;
	}
	return true;
}

void CZamGrec::stop()
{
	GooseReceiver_stop(m_grec);
	GooseReceiver_destroy(m_grec);
}
