#pragma once
#include "ZamProcess.hpp"
#include <boost/asio.hpp>
#include "sqlite3.h"
#include "ZamSQL.hpp"


class CZamProcess;
class CZamIEDEV;
class CZamLDEV;

using boost::asio::ip::udp;


class CZamC118
{
public:
	CZamC118();
	virtual ~CZamC118();
	void ZamProc(int i);
	int Lance();
	int Demarrage();
private:
	bool Initialisation();
protected:
//	void handler(const boost::system::error_code& error, std::size_t bytes_transferred);
	void Detwifi();
	shared_ptr<CZamC118> m_zc;
	shared_ptr<CZamIEDEV> m_ze;
	bool m_timout;
	bool m_blu;
	int m_lrecbuf;
	boost::asio::ip::udp::endpoint m_remote_endpoint;
	boost::asio::ip::udp::endpoint m_local_endpoint;
	int m_lus;
	bool m_err;
	string m_adresse_station;
	string m_adresse;
	string m_adressew;
	unsigned int m_port;
	sqlite3 *m_pdbEV;
  public:
	shared_ptr<CZamProcess> m_zp;
	shared_ptr<CZamLDEV> m_ldev;
};
