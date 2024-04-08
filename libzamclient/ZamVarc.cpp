/*

ZamVarc.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamVarc.hpp"

CZamVarc::CZamVarc()
{
}

CZamVarc::~CZamVarc()
{
}

void CZamVarc::setval(bool b)
{
	m_valbool = b;
}

void CZamVarc::setval(int i)
{ 
	m_valint = i;
}

void CZamVarc::setval(unsigned int i)
{ 
	m_valint = i;
}

void CZamVarc::setval(float f)
{ 
	m_valfloat = f;
}

void CZamVarc::setval(double d)
{ 
	m_valfloat = (float)d;
}

void CZamVarc::setval(string s)
{ 
	m_valstring = s;
}

void CZamVarc::setval (unsigned long long ull)
{
	m_valull= ull;
}

int CZamVarc::getvali() 
{ 
	return m_valint;
}

unsigned long long CZamVarc::getvalull()
{
	return m_valull;
}

bool CZamVarc::getvalb() 
{ 
	return m_valbool;
}

float CZamVarc::getvalf()
{ 
	return (m_valfloat);
}

double CZamVarc::getvald()
{ 
	return (m_valdouble);
}

string CZamVarc::getvals()
{ 
	return m_valstring;
}

string CZamVarc::getsval()
{
	if (m_type == "bool") return to_string(m_valbool);
	if (m_type == "int") return to_string(m_valint);
	if (m_type == "float") return to_string(m_valfloat);
	if (m_type == "double") return to_string(m_valdouble);
	if (m_type == "string") return m_valstring;
	return "?????";
}