/*

ZamTcp.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <string>
#include <filesystem>
#include <fstream>
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace std;
namespace fs = filesystem;
using namespace boost::asio::ip;

class CZamTcp
{
public:
	CZamTcp();
	~CZamTcp();
	void Server(int);
	void Client(int);
	bool Initialisation ();
	int ReceptFichier (const string& adresse, const int port, const string&);
	int EnvoiFichier (const string& adresse, const int port, const string& snom, const string& spatho= "", const string& spathd= "", const bool br= false);
	int EnvoiS (tcp::socket& sock, const string mess);
	int EnvoiS (tcp::socket& sock, const int i);
	int EnvoiL (tcp::socket& sock, const string mess);
	int EnvoiLine (tcp::socket& sock, const string mess);
	string ReceptS (tcp::socket& sock);
	string ReceptF (tcp::socket& sock, int l);
	string ReceptLine (tcp::socket& sock);
	void ReceptT (tcp::socket&, boost::asio::mutable_buffers_1 const&, const boost::asio::deadline_timer::duration_type&);

public:
	unsigned int m_port;
	bool m_stop;
	string m_adresse;
	string m_msg;
	map<int, string> m_params;
	map<string, int> m_mapd;
};
