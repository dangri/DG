/*

ZamStemps.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamStemps.hpp"
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp> 
#include "ZamCod.hpp"
#include <thread>

using namespace std;

using boost::asio::ip::udp;

CZamStemps::CZamStemps()
{
}


CZamStemps::~CZamStemps()
{
}

int CZamStemps::Demarre(unsigned long long t)
{
	m_zp->m_temps = t;
	m_zmp->Ecrit("temps", t);
	thread thr(&CZamStemps::Envoi, this, 1);
	thr.detach();
	return 0;
}

void CZamStemps::Envoi(int i)
{
	boost::asio::io_service ios;
	boost::asio::ip::udp::endpoint loc_endpoint(boost::asio::ip::address_v4::from_string(m_zp->m_adresseIP_wlan0), 1313); 
	udp::socket sock(ios);
	sock.open(udp::v4());
	sock.set_option(boost::asio::ip::udp::socket::reuse_address(true));
	sock.set_option(boost::asio::socket_base::broadcast(true));
	sock.bind(loc_endpoint);
	boost::asio::ip::udp::endpoint senderEndpoint(boost::asio::ip::address_v4::broadcast(), 1313);
	unsigned long long temps = m_zp->m_temps;
	boost::array<unsigned char, 128> data;
	boost::system::error_code erreur;
	unsigned char buf_env[8];
	m_cad = 6;
	m_inc = 1000;
	while (true)
	{
		m_zp->m_temps = temps;
		m_zp->m_zmp->Ecrit("temps", temps);
		encodu(temps, buf_env);
		sock.send_to(boost::asio::buffer(buf_env), senderEndpoint,0,erreur);
		this_thread::sleep_for(chrono::seconds(1));
		temps = temps + m_cad * m_inc;
		if (erreur)
		{
			m_zl->critical(fmt::format("Stemps: {}", erreur.message()));
		}
		if (m_stop != 0) break;
	}
	m_zl->critical("arret du serveur de temps");
	sock.close();
} 

