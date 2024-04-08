/*

ZamVarc.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include <string>
//#include<boost/variant.hpp>

using namespace std;

class CZamVarc
{
public:
	CZamVarc();
	virtual ~CZamVarc();
	
public:
	string m_nom;  	// nom de la variable
	string m_type; 	// Type (bool, int, float, double, time)
	string m_fc; 	// Fonctional Constraint 61850void setval(bool b);
	bool m_change = false;
	unsigned long ts;
	string m_ref;
	void setval(bool);
	void setval(int);
	void setval(unsigned int);
	void setval(float);
	void setval(double);
	void setval(string);
	void setval (unsigned long long);
	bool getvalb();
	int getvali();
	float getvalf();
	double getvald();
	string getvals();
	unsigned long long getvalull();
	bool m_valbool; 	// valeur bool
	int m_valint; // valeur int
	unsigned long long m_valull; // valeur unsigned long lond (Timestamp en ms)
	float m_valfloat; 	// valeur float
	double m_valdouble;
	string m_valstring;
	string getsval();

};
