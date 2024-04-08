/*

ZamS118.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamS118.hpp"
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <thread>

using boost::asio::ip::udp;

//
// Zam serveur 118 
// Ecoute le port UDP 15118 
// Si un message est conforme à une demande de véhicule, envoie la réponse standard.
//

CZamS118::CZamS118()
{
}

CZamS118::~CZamS118()
{
}

int CZamS118::Demarrage()
{
	Initialisation();
	thread attente(&CZamS118::Attente, this);
	attente.detach();
	return 0;
}

void CZamS118::Attente()
{
	while (true)
	{
		while (true)
		{
			if (!m_ldcs->m_bloc_wifi)
			{
				if (m_ldcs->m_evse_dispo)
				{
					m_ldcs->m_bloc_wifi = true;
					break;
				}
			}
			this_thread::sleep_for(chrono::seconds(1));
		}
		while (true)
		{
			if (m_stop)
			{
				m_zp->m_zl->info("Arret de S118");
				break;
			}
			if (!m_becoute)
			{
				m_becoute = true;
				if (Ecoute())
				{
					m_becoute = false;
					break;
				}
			}
			this_thread::sleep_for(chrono::seconds(2));
		} // while true
	}
}

bool CZamS118::Ecoute()
{
	boost::system::error_code erreur;
	boost::asio::io_service ios;
	udp::socket sock(ios);
	boost::asio::ip::udp::endpoint remote_endpoint;
	boost::asio::ip::udp::endpoint local_endpoint;
	sock.open(boost::asio::ip::udp::v4());
	boost::asio::socket_base::broadcast option(true);
	sock.set_option(boost::asio::socket_base::broadcast(true));
	sock.set_option(boost::asio::ip::udp::socket::reuse_address(true));
	local_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(m_zp->m_adresseIP_wlan0), 15118);
	sock.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::any(), 15118));
	int len;
	string adresse;
	while (true)
	{
		len = sock.receive_from(boost::asio::buffer(m_rec_buf, 10), remote_endpoint, 0, erreur);
		if (!erreur && (len == 10))
		{
			m_env_buf[18] = m_rec_buf[0];
			m_env_buf[19] = m_rec_buf[1];
			sock.send_to(boost::asio::buffer(m_env_buf, 20), remote_endpoint, 0, erreur);
			if (!erreur)
			{
				m_zp->m_zl->info("Serveur 15118 Envoi du message de connexion");
				boost::asio::ip::address address = remote_endpoint.address();
				adresse = address.to_string();
				m_ldcs->Newve(adresse);
				this_thread::sleep_for(chrono::seconds(1));
				break;
			} // if pas d'erreur
		}
		this_thread::sleep_for(chrono::seconds(1));
	} // while (true)
	return true;
} // ZamEcoute

bool CZamS118::Initialisation()
{
	for (int i = 0; i < 16; i++)
	{
		m_env_buf[i] = 0;
	}
	m_env_buf[16] = 0x04;
	m_env_buf[17] = 0xD2;
	m_env_buf[18] = 0xFE;
	m_env_buf[19] = 0x01;
	unsigned long iadd = m_zp->m_ladresse;
	for (int i = 0; i < 16; i++)
	{
		m_env_buf[15 - i] = iadd & 0xFF;
		iadd = iadd >> 8;
	}
	return true;
}   // Initialisation

