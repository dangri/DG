/*

ZamGpub.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »

usage : 

*/

# include "ZamGpub.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include "goose_publisher.h"

using namespace std;

CZamGpub::CZamGpub()
{

}

CZamGpub::~CZamGpub()
{

}

bool CZamGpub::Prepare(const string &interface, const string &mac, string refg, string refd, int appid)
{
	CommParameters param;
	for (int k = 0; k < 6; k++)
	{
		uint i1 = CtoI(mac[3 * k]);
		uint i2 = CtoI(mac[3 * k + 1]);
		uint i12 = i1 * 16 + i2;
		param.dstAddress[k] = i12;
	}
	param.vlanId = 0;
	param.vlanPriority = 4;
	param.appId = appid;
	m_pub = GoosePublisher_create(&param, interface.c_str());
	if (!m_pub)
	{
		cout << "Impossible de creer le goose" << endl;
		return false;
	}
	GoosePublisher_setGoCbRef(m_pub, (char *)refg.c_str());
	GoosePublisher_setConfRev(m_pub, 1);
	GoosePublisher_setDataSetRef(m_pub, (char *)refd.c_str());
	return true;
}

bool CZamGpub::Pub(LinkedList dsval)
{
	int i = 0;
	if (GoosePublisher_publish(m_pub, dsval) == -1)
	{
		return false;
	}
	return true;
}

void CZamGpub::Del()
{
	GoosePublisher_destroy(m_pub);
}

int CZamGpub::CtoI(char c)
{
	switch (c)
	{
		case '0':
			return 0;
			break;
		case '1':
			return 1;
			break;
		case '2':
			return 2;
			break;
		case '3':
			return 3;
			break;
		case '4':
			return 4;
			break;
		case '5':
			return 5;
			break;
		case '6':
			return 6;
			break;
		case '7':
			return 7;
			break;
		case '8':
			return 8;
			break;
		case '9':
			return 9;
			break;
		case 'a':
			return 10;
			break;
		case 'b':
			return 11;
			break;
		case 'c':
			return 12;
			break;
		case 'd':
			return 13;
			break;
		case 'e':
			return 14;
			break;
		case 'f':
			return 15;
			break;
		default:
			return -1;
	}
}