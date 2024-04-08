/*

ZamRC.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include <string>
#include <map>


using namespace std;

class CZamRC
{
public:
	CZamRC();
	~CZamRC();
public:
	string name;
	string datSet;
	string intgPd;
	string confRev;
	string bufTime;
	string buffered;
	string rptID;
	string indexed;
	string trg_dchg;
	string trg_qchg;
	string trg_dupd;
	string trg_period;
	string trg_gi;
	string opt_seqNum;
	string opt_timeStamp;
	string opt_reasonCode;
	string opt_dataSet;
	string opt_dataRef;
	string opt_bufOvfl;
	string opt_entryID;
	string opt_configRef;
	string opt_segmentation;
	string Rpt_max;
	int numtrg;
	int numopt;
};


