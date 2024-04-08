/*

ZamCod.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once
#include <string>

using namespace std;
bool decodb(const unsigned char *p);
void encodb(bool b, unsigned char *p);
short decods(const unsigned char *p);
void encods(const short s, unsigned char *p);
int decodi(const unsigned char *p);
void encodi(const int i, unsigned char *p);
void encodd(const double f, unsigned char *p);
double decodd(const unsigned char *p);
void encodu (const unsigned long long, unsigned char *p);
unsigned long long decodu (const unsigned char *p);
float decodf(const unsigned char* p);
void encodf(const float f, unsigned char *p);
string UtoPc(const string& ref);
string UtoP(const string& ref);
string PtoU(const string& ref);