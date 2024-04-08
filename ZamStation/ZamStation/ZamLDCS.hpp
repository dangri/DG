/*

ZamLDCS.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamUnit.hpp"
#include "ZamS118.hpp"
#include "Zamevse.h"
#include "ZamConVE.h"
#include "ZamServeur.hpp"
#include "ZamCalcul.h"

class CZamLLN0;
class CZamLPHD;
class CZamMMXU;
class CZamXSWI;
class CZamFSCHP;
class CZamS118;
class CZamGGIO;
class CZamevse;
class CZamCalcul;
class CZamConVE;
class CZamDPMC;
class CZamDVWC;
class CZamDECP;
class CZamDWMX;

class CZamLDCS : public CZamUnit
{
public:
	CZamLDCS();
	virtual ~CZamLDCS();
	int Demarrage();
	void ZamProc(int);
	int Lance();
	int TransSchd(shared_ptr<CZamFSCHP>, const string&, float);
	float Repart(int);
	int CdeSEBool(const string&, const string&, bool );
	int Newve(const string&);
	int Conve(const string&, const string&, bool);
	int LED(shared_ptr<CZamevse>);
	void CAlm();

  private:
	bool Initialisation();

public:
  shared_ptr<CZamLDCS> m_ldcs;
//  shared_ptr<CZamEVSE> m_evse;
  shared_ptr<CZamLLN0> m_lln0;
  shared_ptr<CZamLPHD> m_lphd;
  shared_ptr<CZamFSCHP> m_fsch1, m_fsch2;
  shared_ptr<CZamMMXU> m_mmxu;
  shared_ptr<CZamXSWI> m_xswi;
  shared_ptr<CZamS118> m_serv118;
  shared_ptr<CZamGGIO> m_ggio;
  shared_ptr<CZamCalcul> m_cal;
  shared_ptr<CZamDPMC> m_dpmc;
  shared_ptr<CZamDVWC> m_dvwc;
  shared_ptr<CZamDECP> m_decp;
  shared_ptr<CZamDWMX> m_dwmx;

  int m_ibeh;
  bool m_change;
  bool m_reinit = false;
  bool m_bcon = false; // true si client connect
  bool m_ena1 = false;
  bool m_ena2 = false;
  bool m_dsa1 = false;
  bool m_dsa2 = false;
  bool m_bfs;
  map<string, shared_ptr<CZamevse>> m_map_evse;
  map<string, shared_ptr<CZamevse>> m_map_evse_ad;
  map<string, shared_ptr<CZamConVE>> m_map_conve;
  bool m_bloc_wifi = false;
  int m_evse_dispo = 0;
  string m_adcurrent;
  string m_borne_reserv;
  bool m_bad_connect;
  string m_rspcon;
  string m_borne;
  string m_sens;
  string m_hdeb;
  string m_hfin;
  string m_energie;
  string m_erreur;
  string m_alm;
  float m_S1;	// puissance borne 1
  float m_S2;   // puissance borne 2
  float m_S3;   // puissance borne 3
  float m_S;	// puissance totale
  float m_soc1, m_soc2, m_soc3;
};
