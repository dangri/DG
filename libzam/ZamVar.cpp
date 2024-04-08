/*

ZamVar.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamVar.hpp"

using namespace std;

CZamVar::CZamVar()
{
}

CZamVar::~CZamVar()
{
}

void CZamVar::setval(bool b)
{
	m_valbool = b;
}

void CZamVar::setval(int i)
{ 
	m_valint = i;
}

void CZamVar::setval(float f)
{ 
	m_valfloat = f;
}

void CZamVar::setval(double d)
{ 
	m_valdouble = d;
}

void CZamVar::setval(string& s)
{ 
	m_valstring = s;
}

void CZamVar::setval(unsigned long long t)
{ 
	m_valt = t;
}

bool CZamVar::getvalb() 
{ 
	
	return m_valbool;
}

int CZamVar::getvali() 
{ 
	return m_valint;
}

float CZamVar::getvalf()
{ 
	return m_valfloat;
}

double CZamVar::getvald()
{ 
	return m_valdouble;
}

string CZamVar::getvals()
{ 
	return m_valstring; 
}

unsigned long long CZamVar::getvalt()
{
	return m_valt;
}

string CZamVar::getsval()
{
	if (m_type == "bool")
	{
		bool b = getvalb();
		if (b) return "1";
		else return "0";
	}
	if (m_type == "int")
	{
		return to_string(getvali());
	}
	if (m_type == "float")
	{
		return to_string(getvalf());
	}
	if (m_type == "double")
	{
		return to_string(getvald());
	}
	if (m_type == "uint64")
	{
		return to_string(getvalt());
	}
	return "";
}
