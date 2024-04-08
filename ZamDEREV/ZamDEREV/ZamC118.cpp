/*

ZamC118.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamC118.hpp"
#include <boost/thread.hpp>
#include <thread>
#include "ZamLDEV.hpp"

using namespace std;

using boost::asio::ip::udp;

namespace asio = boost::asio;

CZamC118::CZamC118()
{
}

CZamC118::~CZamC118()
{
}

void CZamC118::Detwifi()
{
	bool bretour = false;
	int err = 0;
	int b;
	while(true)
	{
		b = m_zp->m_zmp->Liti("int_wifi");
		if(b)
		{
			bretour = true;
		}
		if(bretour)
		{
			if(m_zp->m_bt)
			{
				break;
			}
		}
		this_thread::sleep_for(chrono::seconds(1));
	}
	return;
}


int CZamC118::Lance()
{
	thread thr(&CZamC118::ZamProc, this, 1);
	thr.detach();
	return 0;
}

void CZamC118::ZamProc(int i)
{
	if (Initialisation())
	{
		boost::system::error_code erreur;
		boost::asio::io_service ios;
		boost::array<unsigned char, 10> env_buf;
		boost::array<unsigned char, 20> rec_buf;
		// header
		env_buf[0] = 0x01;           	// V2G : version du protocole
		env_buf[1] = 0xFE;           	// inverse de V2G
		env_buf[2] = 0x90;        		// 
		env_buf[3] = 0x00;           	// 9000 SDP Request Message
		env_buf[4] = 0x02;           	// l
		env_buf[5] = 0x00;           	// l
		env_buf[6] = 0x00;           	// l
		env_buf[7] = 0x00;           	// l
		// msg
		env_buf[8] = 0x10;            	// no TLS
		env_buf[9] = 0x00;            	// TCP
		boost::asio::ip::udp::endpoint remote_endpoint;   
		// initialisation buf de reception
		for(int i = 0 ; i < 20 ; i++)
		{
			rec_buf[i] = 0;
		}
		boost::asio::ip::udp::socket sock(ios);
		sock.open(boost::asio::ip::udp::v4(), erreur);
		if (!erreur)
		{
			sock.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(m_zp->m_adresseIP_wlan0), 15118));
			sock.set_option(boost::asio::ip::udp::socket::reuse_address(true));
			sock.set_option(boost::asio::socket_base::broadcast(true));
			boost::asio::ip::udp::endpoint senderEndpoint(boost::asio::ip::address_v4::broadcast(), 15118);
			// reponse attendue 50 fois au moins 250ms
			bool bc = false;
			for (int i = 0; i < 50; i++)
			{
// Envoi
				sock.send_to(asio::buffer(env_buf, 10), senderEndpoint);
				this_thread::sleep_for(chrono::seconds(1));
				sock.receive_from(boost::asio::buffer(rec_buf, 20), remote_endpoint, 0, erreur);
				if (!erreur)
				{
					this_thread::sleep_for(chrono::milliseconds(100));
					if (rec_buf[19] == env_buf[1])
					{
						break;
					}
				}	// if !erreur
				this_thread::sleep_for(chrono::seconds(5));
			} // for
			if(i < 50)
			{
				boost::array<unsigned char, 16> adr_buf;
				unsigned char b[16];
				boost::array<unsigned char, 2> port_buf;
				boost::array<unsigned char, 2> prot_buf;
				copy(rec_buf.begin(), rec_buf.begin() + 16, adr_buf.begin());
				copy(rec_buf.begin() + 16, rec_buf.begin() + 18, port_buf.begin());
				copy(rec_buf.begin() + 18, rec_buf.end(), prot_buf.begin());
				m_port = (unsigned int) ((rec_buf[16] * 256) + (unsigned int) rec_buf[17]);
				unsigned long long int lladresse = 0;
				for (int i = 0; i < 16; i++)
				{
					lladresse += (unsigned long long int) rec_buf[15 - i] * pow(256, i);
				}
				unsigned long adresse = (unsigned long) lladresse;
				unsigned int ad1 = adresse / (256 * 256 * 256);
				adresse -= ad1 * 256 * 256 * 256;
				unsigned int ad2 = adresse / (256 * 256);
				adresse -= ad2 * 256 * 256;
				unsigned int ad3 = adresse / 256;
				adresse -= ad3 * 256;
				m_adresse_station = to_string(ad1) + "." + to_string(ad2) + "." + to_string(ad3) + "." + to_string(adresse);
				m_zp->m_zmp->Ecrit("int_adresseCS", m_adresse_station);
				m_zp->m_adresse_station = m_adresse_station;
				m_zp->m_adresse = m_adresse;
				m_ldev->m_wifi = true;
				sock.close();
				m_zp->m_zl->info("Arret client UDP Broadcast");
				return;
			}
			else
			{
				m_zp->m_zl->info("Echec contact WIFI");
			}
			sock.close();
			m_zp->m_zl->info("Arret client UDP");
		}
		// erreur ouverture SOC
		else
		{
			m_zp->m_zl->info("Erreur ouverture socket UDP");
		}
	} //  if initialisation
	else
	{
		m_zp->m_zl->info("Pb Initialisation du client 15118");
	}	
}	// Proc

int CZamC118::Demarrage()
{
	int err = 0;
	Detwifi();
	Lance();
	return err;
}

bool CZamC118::Initialisation()
{
	return true;
} // Initialisation

