/*

ZamLog.h

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/
#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>


#include "spdlog/sinks/stdout_sinks.h"

#define SPDLOG_CLOCK_COARSE
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#define SPDLOG_NO_ATOMIC_LEVELS


using namespace std;

class CZamLog
{
public:
	CZamLog();
	CZamLog(const string&, bool del=true);
	~CZamLog();
	void trace(const string &mess);
	void debug(const string &mess);
	void info(const string &mess);
	void warn(const string &mess);
	void err(const string &mess);
	void critical(const string &mess);
	void niveau(int);
	void niveauC(int);
	void niveauF(int);
	void niveau(const string &);
	void niveauC(const string &);
	void niveauF(const string &);
	void flush(int);
	void flush();

  public:
	shared_ptr<spdlog::logger> m_logger;
	shared_ptr<spdlog::sinks::basic_file_sink_mt> m_file_sink;
//	shared_ptr<spdlog::sinks::rotating_file_sink_mt> m_file_sink;
	shared_ptr<spdlog::sinks::stdout_sink_mt> m_console_sink;
	bool m_flush;
	int m_fperiod;
	string m_level; // trace debug info warn error critical off
	bool m_del = true; // Effacer le fichier existant
};
