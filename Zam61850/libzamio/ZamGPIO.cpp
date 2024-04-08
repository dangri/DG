/*

ZamGPIO.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamGPIO.h"

#include <iostream>


CZamGPIO::CZamGPIO()
{
}

CZamGPIO::~CZamGPIO()
{
}

CZamGPIO::CZamGPIO(const string& name)
{
	m_chip = gpiod_chip_open_lookup(name.data());
	m_nom = gpiod_chip_name(m_chip);
	m_control = gpiod_chip_label(m_chip);
	m_nblines = gpiod_chip_num_lines(m_chip);
}

int CZamGPIO::Liste()
{
	struct gpiod_chip_iter *iter;
	struct gpiod_chip *chip;

	iter = gpiod_chip_iter_new();
	if (iter == NULL)
	{
		perror("gpiod_chip_iter");
		return -1;
	}
	while ((chip = gpiod_chip_iter_next(iter)) != NULL)
	{
		string nom = gpiod_chip_name(chip);
		string control = gpiod_chip_label(chip);
		int nblines = gpiod_chip_num_lines(chip);
		cout << "nom: " << nom << " control: " << control << " nombre de lignes: " << nblines << endl;
	}
	gpiod_chip_iter_free(iter);
	return 0;
}

int CZamGPIO::Listelines(const string& nom)
{
	int erreur = 0;
	struct gpiod_chip* chip = gpiod_chip_open_lookup(nom.data());
	unsigned int nb = gpiod_chip_num_lines(chip);
	const char* str;
	string name = "";
	string consumer = "";
	cout << "gpio" << nom << endl;
	for (unsigned int k = 0; k < nb; k++)
	{
		struct gpiod_line* line = gpiod_chip_get_line(chip, k);
		unsigned int offset = gpiod_line_offset(line);
		str = gpiod_line_name(line);
		if (str != NULL) name = string(str);
		str = gpiod_line_consumer(line);
		if (str != NULL) consumer = string(str);
		int direction = gpiod_line_direction(line);
		int etat = gpiod_line_active_state(line);
		bool used = gpiod_line_is_used(line);
		bool drain = gpiod_line_is_open_drain(line);
		bool source = gpiod_line_is_open_source(line); 
		cout << "offset: " << offset << " nom: " << name << " consumer: " << consumer << " direction: " << direction << " etat: " << etat << " used: " << used << " drain: " << drain << " source: " << source << endl;
	}
	return erreur;
}

string CZamGPIO::consumer(unsigned int l)
{
	string ret;
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	const char* s = gpiod_line_consumer(line);
	if (s) ret = string(s);
	else ret = "-";
	return ret;
}
string CZamGPIO::nom(unsigned int l)
{
	string ret;
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	const char* s = gpiod_line_name(line);
	if (s) ret = string(s);
	else ret = "-";
	return ret;
}
int CZamGPIO::direction(unsigned int l)
{
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	return gpiod_line_direction(line);
} // 1 = IN 2 = OUT
int CZamGPIO::actif(unsigned int l)
{
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	return gpiod_line_active_state(line);
} // 1 = active high, 2 = active low
bool CZamGPIO::used(unsigned int l)
{
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	return gpiod_line_is_used(line);
}
bool CZamGPIO::open_drain(unsigned int l)
{
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	return gpiod_line_is_open_drain(line);
}
bool CZamGPIO::open_source(unsigned int l)
{
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	return gpiod_line_is_open_source(line);
}

int CZamGPIO::Reserv_in(unsigned int l)
{
	int erreur = 0;
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	erreur = gpiod_line_request_input(line, nullptr);
	return erreur;
}

int CZamGPIO::Reserv_out(unsigned int l)
{
	int erreur = 0;	
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	erreur = gpiod_line_request_output(line, "zam", 0);
	return erreur;
}
void CZamGPIO::Release(unsigned int l)
{
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	gpiod_line_release(line);
}
int CZamGPIO::Get(unsigned int l)
{
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	int val;
	val = gpiod_line_get_value(line);
	return val;
}
int CZamGPIO::Set(unsigned int l, int val)
{	
	int erreur;
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	erreur = gpiod_line_set_value(line, val);
	return erreur;
}

int CZamGPIO::Attend0(unsigned int l, const struct timespec* timeout)
{
	int ret;
	struct gpiod_line_event event;
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	ret = gpiod_line_request_falling_edge_events(line, (gpiod_line_consumer(line)));
	while (true)
	{
		ret = gpiod_line_event_wait(line, timeout);
		if (gpiod_line_event_read(line, &event) == 0)
		{
			break;
		}
	}
	return 0;
}

int CZamGPIO::Attend1(unsigned int l, const struct timespec* timeout)
{
	int ret;
	struct gpiod_line_event event;
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	ret = gpiod_line_request_rising_edge_events(line, (gpiod_line_consumer(line)));
	while (true)
	{
		ret = gpiod_line_event_wait(line, timeout);
		if (gpiod_line_event_read(line, &event) == 0)
		{
			break;
		}
	}
	return 0;
}

int CZamGPIO::Attend(unsigned int l, const struct timespec* timeout)
{
	int ret;
	struct gpiod_line_event event;
	struct gpiod_line* line = gpiod_chip_get_line(m_chip, l);
	ret = gpiod_line_request_both_edges_events(line, (gpiod_line_consumer(line)));
	while (true)
	{
		gpiod_line_event_wait(line, timeout);
		if (gpiod_line_event_read(line, &event) == 0)
		{
			if (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE) ret = 1;
			else ret = 2;
			break;
		}
	}
	return ret;
}
