/*

ZamEVSE.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once
#include "ZamUnit.hpp"

class CZamLLN0;
class CZamLPHD;
class CZamS118;
class CZamLDCS;
class CZamDEAO;
class CZamDEDO;
class CZamDESE;
class CZamLDEV;
class CZamFSCH;
class CZamFSCC;
class CZamGGIO;
class CZamCl;
class CZamClient;
class CZamClientEVB;
class CZamDPMC;
class CZamDWMX;

class CZamEVSE : public CZamUnit
{
public:
	CZamEVSE ();
	virtual ~CZamEVSE ();
	void ZamProc (int i);
	int Demarrage ();
	int Lance ();
	int FschControl();
	int TransSchd(const string&);
	int CdeSEBool(const string &, const string &, bool);

  private:
	bool Initialisation ();
public:
  string m_name;
  int m_id;
  string m_sid;
  shared_ptr<CZamEVSE> m_evse;
  shared_ptr<CZamLLN0> m_lln0;
  shared_ptr<CZamLPHD> m_lphd;
  shared_ptr<CZamS118> m_serv118;
  shared_ptr<CZamLDCS> m_ldcs;
  shared_ptr<CZamFSCH> m_fsch1, m_fsch2;
  shared_ptr<CZamDEAO> m_deao;
  shared_ptr<CZamDEDO> m_dedo;
  shared_ptr<CZamDESE> m_dese;
  shared_ptr<CZamFSCC> m_fscc;
  shared_ptr<CZamGGIO> m_ggio;
  shared_ptr<CZamClient> m_zc;
  shared_ptr<CZamClient> m_zc1;
  shared_ptr<CZamClientEVB> m_zcb;
  shared_ptr<CZamCl> m_zcl;
  shared_ptr<CZamLDEV> m_ldev;
  shared_ptr<CZamDPMC> m_dpmc;
  shared_ptr<CZamDWMX> m_dwmx;
  string m_adresse_cs;
  string m_adresse_ve;
  bool m_bcon;
  bool m_connwifi = false;
  int m_ac;
  int m_dc;
  string m_ref = "";
  int m_ibeh;
  float m_wh;
  bool m_chaac;
  string m_adresse;
};

