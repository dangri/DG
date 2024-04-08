/*

ZamGPIO.h
Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/
#pragma once

#include <gpiod.h>
#include <string>
#include <map>

using namespace std;

class CZamGPIO
{
public:
	CZamGPIO();
	CZamGPIO(const string&);
	~CZamGPIO();
	int Liste();
	int Listelines(const string&);
	string consumer(unsigned int);
	string nom(unsigned int);
	int direction(unsigned int); // 1 = IN 2 = OUT
	int actif(unsigned int); // 1 = active high, 2 = active low
	bool used(unsigned int);
	bool open_drain(unsigned int);
	bool open_source(unsigned int);
	int Reserv_in(unsigned int);
	int Reserv_out(unsigned int);
	void Release(unsigned int);
	int Get(unsigned int);
	int Set(unsigned int, int val);
	int Attend0(unsigned int, const struct timespec* timeout);
	int Attend1(unsigned int, const struct timespec* timeout);
	int Attend(unsigned int, const struct timespec* timeout);
public:
	string m_rpi_name = "gpiochip0";
	struct gpiod_chip* m_chip;
	string m_nom;
	string m_control;
	int m_nblines;
};

