/*

ZamCtemps.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamCtemps.hpp"
#include <boost/asio.hpp>
#include "boost/format.hpp"
#include <iostream>
#include <boost/array.hpp>
#include <thread>
#include <boost/lexical_cast.hpp>
#include "ZamCod.hpp"
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/permissions.hpp>
#include "semaphore.h"

using namespace std;
using namespace boost::interprocess;

using boost::asio::ip::udp;
using boost::format;

CZamCtemps::CZamCtemps()
{
}


CZamCtemps::~CZamCtemps()
{
}

int CZamCtemps::Demarre()
{
	thread thr(&CZamCtemps::Ecoute, this, 1);
	thr.detach();
	return 0;
}

void CZamCtemps::Ecoute(int i)
{
	boost::system::error_code erreur;
	boost::asio::io_service ios;
	udp::socket sock(ios, udp::endpoint(udp::v4(), 1313));
	sock.set_option(boost::asio::ip::udp::socket::reuse_address(true));
	size_t len;
	unsigned long long temps;
	udp::endpoint serveur;
	unsigned char recv_buf[8];
	while (1)
	{
		try
		{
			if (m_zp->m_stop) break;
			size_t len = sock.receive_from(boost::asio::buffer(recv_buf), serveur, 0, erreur);
			if (erreur)
			{
				m_zp->m_zl->info(fmt::format("Ctemps: {}", erreur.message()));
				throw string("Erreur boost");
			}
			temps = decodu(recv_buf);
			m_zp->m_zmp->Ecrit("temps", temps);
			m_zp->m_temps = temps;
			if (!m_zp->m_bt)
			{
				m_zp->m_bt = true;
			}
		}
		catch (string exc)
		{
			m_zp->m_zl->info(fmt::format("Ctemps: sortie sur exception {}", exc));
			sock.close();
			break;
		}
		catch (boost::system::system_error e)
		{
			m_zp->m_zl->info("Ctemps: sortie sur exception boost");
			sock.close();
			break;
		}
		catch (exception e)
		{
			m_zp->m_zl->info(fmt::format("Ctemps sortie exception {}", e.what()));
			sock.close();
			break;
		}
		catch (...)
		{
			m_zp->m_zl->info("Ctemps : exception");
			sock.close();
			break;
		}
	} // ecoute temps
	sock.close();
	m_zp->m_zl->info("Ctemps :: sortie des couloirs du temps");
}

