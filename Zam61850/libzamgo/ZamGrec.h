/*

ZamGrec.h

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/
#pragma once

#include <string>
#include "goose_receiver.h"
#include "goose_subscriber.h"

using namespace std;

class CZamGrec
{
public:
	CZamGrec();
	~CZamGrec();
	void Capte();
	void Grecrec(const string &refi, const string, ushort appid);
	bool start();
	MmsValue* Lit();
	void stop();

public:
	bool m_capte = false;
	MmsValue* m_val;
	GooseReceiver m_grec;
	GooseSubscriber m_gsub;
};

void capte(GooseSubscriber subscriber, void* parameter);