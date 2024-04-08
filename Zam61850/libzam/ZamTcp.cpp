/*

ZamTcp.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamTcp.hpp"

CZamTcp::CZamTcp()
{
}

CZamTcp::~CZamTcp()
{
}

int CZamTcp::EnvoiS (tcp::socket& sock, const string mess)
{
	unsigned int len;
	boost::system::error_code ec;
	try
	{
		len= (int)boost::asio::write (sock, boost::asio::buffer (mess, mess.length ()), ec);
		if (ec)
		{
			// cout << "erreur connect" << endl;
			return -2;
		}
		if (len != (unsigned int)mess.length ()) { return -1; }
	}
	catch (...)
	{
		return 0;
	}
	return len;
}

int CZamTcp::EnvoiS (tcp::socket& sock, const int i)
{
	string toi= to_string (i);
	return EnvoiS (sock, toi);
}


string CZamTcp::ReceptS (tcp::socket& sock)
{
	string recep (1024, '_');
	unsigned int len;
	array<char, 1024> buf;
	boost::system::error_code ec;
	try
	{
		len= (int)sock.read_some (boost::asio::buffer (buf, 1024), ec);
		if (ec)
		{
			// cout << "erreur connect" << endl;
			return "";
		}
		if (len > 1024) { return ""; }
		if (len == 0) { return ""; }
		std::copy (buf.begin (), buf.begin () + len, recep.begin ());
		recep= recep.substr (0, len);
	}
	catch (...)
	{
		return "";
	}
	return recep;
}

string CZamTcp::ReceptF (tcp::socket& sock, int l)
{
	array<char, 80> buf;
	boost::system::error_code ec;
	string bloc (l, ' ');
	try
	{
		int len= (int)boost::asio::read (sock, boost::asio::buffer (buf, l), ec);
		if (ec)
		{
			// cout << "erreur connect" << endl;
			return "";
		}
		if (len != l) { return ""; }
		copy (buf.begin (), buf.begin () + l, bloc.begin ());
	}
	catch (...)
	{
		return "";
	}
	return bloc;
}

void CZamTcp::ReceptT (tcp::socket& s, boost::asio::mutable_buffers_1 const& buffers, const boost::asio::deadline_timer::duration_type& expiry_time)
{
	boost::optional<boost::system::error_code> timer_result;
	boost::asio::deadline_timer timer (s.get_io_service ());
	timer.expires_from_now (expiry_time);
	timer.async_wait ([&timer_result](const boost::system::error_code& error) { timer_result.reset (error); });

	boost::optional<boost::system::error_code> read_result;
	boost::asio::async_read (
		s, buffers, [&read_result](const boost::system::error_code& error, size_t) { read_result.reset (error); });

	s.get_io_service ().reset ();
	while (s.get_io_service ().run_one ())
	{
		if (read_result)
			timer.cancel ();
		else if (timer_result)
			s.cancel ();
	}
	if (*read_result) throw boost::system::system_error (*read_result);
}

int CZamTcp::ReceptFichier (const string& adresse, const int port, const string& sfichier)
{
	boost::system::error_code ec;
	boost::asio::io_service ios;
	tcp::endpoint endp (address::from_string (adresse), port);
	tcp::socket sock (ios);
	bool battente= false;
	bool bconnect= false;
	string reponse;
	string sko= "ko";
	string sok= "ok";
	while (!bconnect)
	{
		try
		{
			// attente du serveur
			if (!battente)
			{
				//				cout << "Chargement en attente ..." << to_string(time(0)) << endl;
				battente= true;
			}
			sock.connect (endp, ec);
			if (ec)
			{
				sock.close ();
				boost::this_thread::sleep (boost::posix_time::milliseconds (1000));
				continue;
			}
			bconnect= true;
			battente= false;
			cout << "Chargement en cours ..." << to_string (time (0)) << endl;
			break;
		}
		catch (exception e)
		{
			if (battente)
			{
				sock.close ();
				boost::this_thread::sleep (boost::posix_time::milliseconds (1000));
				continue;
			}
		}
	} // while non connecté
	try
	{
		reponse= ReceptS (sock);
		EnvoiS (sock, "charger_b;" + sfichier + ";");
		reponse= ReceptS (sock);
		if (reponse != sok)
		{
			sock.close ();
			return -1;
		}
		size_t s= sfichier.find_last_of ('/');
		string sfic= sfichier.substr (s + 1);
		ofstream fic (sfic, ios::binary | ios::trunc);
		if (!fic) { EnvoiS (sock, sko); }
		EnvoiS (sock, sok);
		int l= 0;
		int bloc= 0;
		char buf[256];
		while (true)
		{
			reponse= ReceptS (sock);
			int alire= boost::lexical_cast<int> (reponse);
			EnvoiS (sock, sok);
			if (alire == 0) break;
			l= (int)read (sock, boost::asio::buffer (buf, alire));
			fic.write (buf, l);
			bloc++;
			EnvoiS (sock, sok);
			l++;
		}
		reponse= ReceptS (sock);
		int bonbloc= boost::lexical_cast<int> (reponse);
		if (bloc != bonbloc)
			EnvoiS (sock, sko);
		else
			EnvoiS (sock, sok);
		fic.flush ();
		fic.close ();
		sock.close ();
	} // try
	catch (exception e)
	{
		if (battente)
		{
			sock.close ();
			boost::this_thread::sleep (boost::posix_time::milliseconds (1000));
		}
		else
		{
			return -4;
		}
	}
	return 0;
}

int CZamTcp::EnvoiFichier (const string& adresse, const int port, const string& snom, const string& spatho, const string& spathd, const bool br)
{
	boost::system::error_code ec;
	boost::asio::io_service ios;
	tcp::endpoint endp (address::from_string (adresse), port);
	tcp::socket sock (ios);
	bool battente= false;
	bool bconnect= false;
	string demande;
	string reponse;
	string sko= "ko";
	string sok= "ok";
	while (!bconnect)
	{
		try
		{
			// attente du serveur
			if (!battente)
			{
				//				cout << "Envoi en attente ..." << to_string(time(0)) << endl;
				battente= true;
			}
			sock.connect (endp);
			ReceptS (sock);
			bconnect= true;
			battente= false;
			cout << "Envoi en cours ..." << to_string (time (0)) << endl;
			break;
		}
		catch (exception e)
		{
			if (battente)
			{
				sock.close ();
				boost::this_thread::sleep (boost::posix_time::milliseconds (1000));
				continue;
			}
		}
	} // while non connecté
	try
	{
		size_t s= snom.find_last_of ('/');
		string sfic;
		sfic= snom.substr (s + 1);
		string snomfic= spatho + snom;
		ifstream fic (snomfic, ios::binary);
		if (!fic)
		{
			EnvoiS (sock, sko);
			sock.close ();
			return 0;
		}
		else
		{
			EnvoiS (sock, "envoyer_b;" + spathd + sfic + ";" + (br ? "1" : "0") + ";");
			size_t lu= 0;
			int blocs= 0;
			bool b= true;
			char buf[256];
			reponse= ReceptS (sock);
			while (true)
			{
				fic.read (&buf[lu], 1);
				if (fic.eof ())
				{
					if (lu > 0)
					{
						EnvoiS (sock, to_string (lu));
						reponse= ReceptS (sock);
						if (reponse != sok)
						{
							b= false;
							break;
						}
						boost::asio::write (sock, boost::asio::buffer (buf, lu));
						reponse= ReceptS (sock);
						if (reponse != sok)
						{
							b= false;
							break;
						}
						blocs++;
					}
					break;
				}
				lu++;
				if (lu == 256)
				{
					EnvoiS (sock, to_string (lu));
					reponse= ReceptS (sock);
					if (reponse != sok)
					{
						b= false;
						break;
					}
					boost::asio::write (sock, boost::asio::buffer (buf, lu));
					reponse= ReceptS (sock);
					if (reponse != sok)
					{
						b= false;
						break;
					}
					blocs++;
					lu= 0;
				}
			} // while true
			if (b)
			{
				EnvoiS (sock, "0");
				reponse= ReceptS (sock);
				EnvoiS (sock, to_string (blocs));
				demande= ReceptS (sock);
			}
			fic.close ();
		} //if fic
		sock.close ();
	} // try
	catch (exception e)
	{
		if (battente)
		{
			sock.close ();
			boost::this_thread::sleep (boost::posix_time::milliseconds (1000));
		}
		else
		{
			return -4;
		}
	}
	return 0;
}

bool CZamTcp::Initialisation ()
{
	//	init_logging("tcp");
	m_msg = "Tcp";
	m_mapd["envoi"] = 0;
	m_mapd["reception"] = 1;
	return true;
}

void CZamTcp::Server(int i)
{
	if (Initialisation ())
	{
		m_port= i;
		boost::asio::io_service ios;
		tcp::acceptor acceptor (ios, tcp::endpoint (tcp::v4 (), m_port));
		tcp::socket sock (ios);
		string demande;
		string renvoi;
		string recep;
		size_t lparam;
		size_t ldemande;
		string reponse;
		string sfin= "finfinfin";
		string sok= "ok";
		string sko= "ko";
		int nparam;
		int commande;
		
		while (true)
		{
			if (m_stop)
			{
				sock.close ();
				return;
			}
//			Log (info, "Tcp", "Tcp en ecoute");
			acceptor.accept (sock);
//			m_zk->Log (info, "Tcp", "Tcp connecte");
			m_adresse = sock.remote_endpoint ().address().to_string();
			m_port = sock.remote_endpoint().port();
//			m_zk->Log (info, "tcp", "Connexion de " + m_spa_sup->getAdresse () + ":" + to_string (m_spa_sup->getPort ()));
			EnvoiS (sock, m_msg);
//			m_zk->Log (trace, "tcp", "Réponse envoyée");
			while (1) // traitement des demandes
			{
				try
				{
					m_params.clear ();
					demande= ReceptS (sock);
//					m_zk->Log (trace, "tcp", "reception: " + demande);
					ldemande= demande.length ();
					// demande vide
					if (ldemande == 0) throw string ("serveur tcp demande vide");
					nparam = 1;
					cout << demande << endl;
					// les paramètres sont délimités par ";"
					// le premier paramètre est la commande
					// la commande doit être présente dans m_mapd (Initialisation)
					while (ldemande > 0)
					{
						lparam= demande.find (';');
						if (lparam == string::npos) throw string ("serveur tcp Erreur delimiteur commande");
						m_params[nparam] = demande.substr (0, lparam);
						demande= demande.substr (lparam + 1, demande.length () - lparam);
						ldemande= demande.length ();
						nparam++;
					}
					nparam--;
					commande = m_mapd[m_params[0]];
					cout << m_params[0] << " " << m_params[1] << " " << m_params[2] << " " << m_params[3] << " " << m_params[4] << endl;
					//	m_zk->Log (trace, "tcp", "commande " + m_params[1] + " " + m_params[2] + " " + m_params[3] + " " + m_params[4]);
					switch (commande)
					{
						case 0:
						{
							ifstream fic (m_params[1]);
							if (!fic)
							{
								EnvoiS (sock, sko);
							}
							else
							{
								EnvoiS (sock, sok);
								size_t lu;
								int blocs= 0;
								bool b= true;
								//array<char, 512> buf;
								char buf[512];
								reponse= ReceptS (sock);
								while (!fic.eof ())
								{
									fic.read (buf, 256);
									lu= fic.gcount ();
									EnvoiS (sock, to_string (lu));
									reponse= ReceptS (sock);
									if (reponse != sok)
									{
										b= false;
										break;
									}
									boost::asio::write (sock, boost::asio::buffer (buf, lu));
									reponse= ReceptS (sock);
									if (reponse != sok)
										break;
									blocs++;
								}
								if (!b)
								{
									cout << "erreur de transfert" << endl;
									break;
								}
								EnvoiS (sock, "0");
								reponse= ReceptS (sock);
								EnvoiS (sock, to_string (blocs));
								demande= ReceptS (sock);
								fic.close ();
							}
							break;
						}
						case 1:
						{
							ofstream fic (m_params[1], ios_base::trunc | ios_base::binary);
							if (!fic)
							{
								EnvoiS (sock, sko);
							}
							EnvoiS (sock, sok);
							int l= 0;
							int bloc= 0;
							char buf[512];
							while (true)
							{
								reponse= ReceptS (sock);
								int alire= boost::lexical_cast<int> (reponse);
								EnvoiS (sock, sok);
								if (alire == 0)
								{
									break;
								}
								l= (int)read (sock, boost::asio::buffer (buf, alire));
								fic.write (buf, l);
								bloc++;
								EnvoiS (sock, sok);
								l++;
							}
							demande= ReceptS (sock);
							int bonbloc= boost::lexical_cast<int> (demande);
							if (bloc != bonbloc)
							{
								EnvoiS (sock, sko);
							}
							else
							{
								EnvoiS (sock, sok);
							}
							fic.flush ();
							fic.close ();
							break;
						}
						default:
						{
							EnvoiS (sock, sko);
							break;
						}
					} // switch demande
				} // try
				catch (string exc)
				{
					cout << "exception" << exc << " deconnexion" << endl;
					sock.close ();
					break;
				}
				catch (boost::system::system_error e)
				{
					cout << "exception boost " << string (e.what ()) << " deconnexion" << endl;
					sock.close ();
					break;
				}
				catch (exception e)
				{
					cout << "exception" << string (e.what ()) << " deconnexion" << endl;
					sock.close ();
					break;
				}
				catch (...)
				{
					cout << "exception inconnue deconnexion" << endl;
					sock.close ();
					break;
				}
			} // while 1 boucle sur nouvelle demande
			sock.close ();
		} // while true boucle infinie
	}
	else // Initialisation en faute
	{
		cout << "tcp Serveur : Initialisation impossible" << endl;
	}
	cout << "tcp Serveur : Arret du serveur" << endl;
} // ZamProc

