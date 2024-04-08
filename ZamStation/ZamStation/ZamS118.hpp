/*

ZamS118.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamLDCS.hpp"

class CZamS118
{
public:
	CZamS118();
	virtual ~CZamS118();
	int MonadresseIP(const string&);
	bool Ecoute();
	int Demarrage();
	void Attente();
	bool Initialisation();
protected:
	boost::array<unsigned char, 10> m_rec_buf;
	boost::array<unsigned char, 20> m_env_buf;

public:
  shared_ptr<CZamProcess> m_zp;
  shared_ptr<CZamLDCS> m_ldcs;
  boost::array<char, 16> m_buf_add;
  unsigned int m_port = 4000;
  bool m_stop = false;
  bool m_becoute = false;
  int m_evenecoute = 0;
};

