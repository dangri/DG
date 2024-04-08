/*

ZamCalcul.h

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamEquip.hpp"
#include "ZamProcess.hpp"
#include <memory>
#include <string>
#include <filesystem>
#include <complex>
#include <cmath>
#include <pugixml.hpp>
#include "ZamLDCS.hpp"

using namespace std;
using namespace std::complex_literals;
using namespace pugi;
namespace fs = filesystem;

typedef complex<double> comp;

class CZamCalcul : public CZamEquip 
{
public:
	CZamCalcul();
	~CZamCalcul();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int);
	int Arret();
	int start();
	int LitReseau(string&);
	comp DeltaV(comp, comp, comp);
	comp Pertes(comp, comp, comp);
	void CalculVder(comp);
	void calcul();
  private:
	int Lancement();
public:
  shared_ptr<CZamLDCS> m_ldcs;
  string m_nom;
  bool m_bt;
  bool m_stop = false;
  string m_nomvar;
  string m_semap;
  unsigned int m_taille_var;
  unsigned int m_pidevr = 0;
  string m_reseau, m_source, m_autre, m_amont, m_aval, m_cond_amont, m_cond_aval;
  double m_long_amont, m_long_aval;
  comp m_Sn;   //puissance nominale de la source
  float m_w1;	// puissance EVSE1
  float m_w2;  // puissance EVSE2
  float m_w3;  // puissance EVSE3
  comp m_sse1;
  comp m_sse2;
  comp m_sse3;
  comp m_ucc;  //tension de court-circuit de la source
  comp m_Spf;  // pertes fer de la ource
  comp m_Spv;  // pertes variables de la source
  comp m_zsource;   // Impédance source
  comp m_Sext; // puissance hors depart fournie par la source
  comp m_Sdep; // puissance totale hp sur le départ
  comp m_Sam;  // puissance répartie hp sur le tronçon amont
  comp m_Sav;  // puissance répartie hp sur le tronçon aval
  comp m_Sder; // puissance DER
  comp m_zlam; // impédance linéïque du tronçon amont
  comp m_zam;   // Impédance du tronçon amont
  comp m_zlav; // impédance linéïque du tronçon aval
  comp m_zav;  // impédance du tronçon aval
  comp m_Vs;   // tension nominale de la source
  comp m_Vd; // tension au point de raccordement DER
  comp m_Vc;   // tension au point C
  comp m_Vr;   // tension régulée de la source
  comp m_Ider; // courant dans le DER
  comp m_Is;   // courant total source
  comp m_Id;   // courant total départ
  comp m_I3;   // courant départ
  double m_w10, m_w20, m_w30,m_vd0;
  int m_ibc;
  bool m_first;
};


