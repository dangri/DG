/*

ZamLog.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamLog.h"
#include <memory>
#include <filesystem>

using namespace std;
using namespace spdlog;

namespace fs = filesystem;
CZamLog::CZamLog()
{
}

CZamLog::CZamLog(const string &nom, bool del)
{
	m_del = del;
	string nomfic = "/home/pi/logs/" + nom + ".txt";
	if (m_del)
	{
		if (fs::exists(nomfic))
			fs::remove(nomfic);
	}
	auto console_sink = make_shared<sinks::stdout_sink_mt>();
	console_sink->set_level(level::info);
	console_sink->set_pattern("%v");
	m_console_sink = console_sink;

//	auto file_sink = make_shared<sinks::rotating_file_sink_mt>(nomfic, 1024 * 1024 * 5, 10);
	auto file_sink = make_shared<sinks::basic_file_sink_mt>(nomfic);

	file_sink->set_level(level::trace);
	m_file_sink = file_sink;

	spdlog::sinks_init_list sink_list = {console_sink, file_sink};

	auto logger(make_shared<spdlog::logger>(nom, sink_list.begin(), sink_list.end()));
	logger->set_level(level::trace);
	m_logger = logger;
	register_logger(m_logger);
	flush_every(std::chrono::seconds(1));
}

CZamLog::~CZamLog()
{
}

void CZamLog::trace(const string &mess)
{
	m_logger->trace(mess);
}
void CZamLog::debug(const string &mess)
{
	m_logger->debug(mess);
}

void CZamLog::info(const string &mess)
{
	m_logger->info(mess);
}

void CZamLog::warn(const string &mess)
{
	m_logger->warn(mess);
}

void CZamLog::err(const string &mess)
{
	m_logger->error(mess);
}

void CZamLog::critical(const string &mess)
{
	m_logger->critical(mess);
}

void CZamLog::flush()
{
	m_logger->flush();
}

void CZamLog::flush(int d)
{
	flush_every(std::chrono::seconds(d));
}

void CZamLog::niveau(int l)
{
	switch (l)
	{
	case 0:
		set_level(level::trace);
		break;
	case 1:
		set_level(level::debug);
		break;
	case 2:
		set_level(level::info);
		break;
	case 3:
		set_level(level::warn);
		break;
	case 4:
		set_level(level::err);
		break;
	case 5:
		set_level(level::critical);
		break;
	case 6:
		set_level(level::off);
		break;
	}
}

void CZamLog::niveauC(int l)
{
	switch (l)
	{
	case 0:
		m_console_sink->set_level(level::trace);
		break;
	case 1:
		m_console_sink->set_level(level::debug);
		break;
	case 2:
		m_console_sink->set_level(level::info);
		break;
	case 3:
		m_console_sink->set_level(level::warn);
		break;
	case 4:
		m_console_sink->set_level(level::err);
		break;
	case 5:
		m_console_sink->set_level(level::critical);
		break;
	case 6:
		m_console_sink->set_level(level::off);
		break;
	}
}

void CZamLog::niveauF(int l)
{
	switch (l)
	{
	case 0:
		m_file_sink->set_level(level::trace);
		break;
	case 1:
		m_file_sink->set_level(level::debug);
		break;
	case 2:
		m_file_sink->set_level(level::info);
		break;
	case 3:
		m_file_sink->set_level(level::warn);
		break;
	case 4:
		m_file_sink->set_level(level::err);
		break;
	case 5:
		m_file_sink->set_level(level::critical);
		break;
	case 6:
		m_file_sink->set_level(level::off);
		break;
	}
}

void CZamLog::niveau(const string& sniv)
{
	set_level(level::trace);
	if (sniv == "debug")
		set_level(level::debug);
	if (sniv == "info")
		set_level(level::info);
	if (sniv == "warning")
		set_level(level::warn);
	if (sniv == "erreur")
		set_level(level::err);
	if (sniv == "critique")
		set_level(level::critical);
	if (sniv == "off")
		set_level(level::off);
}

void CZamLog::niveauC(const string &sniv)
{
	m_console_sink->set_level(level::trace);
	if (sniv == "debug")
		m_console_sink->set_level(level::debug);
	if (sniv == "info")
		m_console_sink->set_level(level::info);
	if (sniv == "warning")
		m_console_sink->set_level(level::warn);
	if (sniv == "erreur")
		m_console_sink->set_level(level::err);
	if (sniv == "critique")
		m_console_sink->set_level(level::critical);
	if (sniv == "off")
		m_console_sink->set_level(level::off);
}

void CZamLog::niveauF(const string &sniv)
{
	m_file_sink->set_level(level::trace);
	if (sniv == "debug")
		m_file_sink->set_level(level::debug);
	if (sniv == "info")
		m_file_sink->set_level(level::info);
	if (sniv == "warning")
		m_file_sink->set_level(level::warn);
	if (sniv == "erreur")
		m_file_sink->set_level(level::err);
	if (sniv == "critique")
		m_file_sink->set_level(level::critical);
	if (sniv == "off")
		m_file_sink->set_level(level::off);
}