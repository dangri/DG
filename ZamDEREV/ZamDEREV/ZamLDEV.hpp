/*

ZamLDEV.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamUnit.hpp"
#include <memory>

class CZamLLN0;
class CZamLPHD;
class CZamDEEV;
class CZamFSCC;
class CZamFSCH;
class CZamC118;
class CZamLDEV;
class CZamXSWI;
class CZamReport;
class CZamMemp;
class CZamDSTO;
class CZamDPMC;
class CZamDWMX;

using namespace std;

class CZamLDEV : public CZamUnit
{
public:
	CZamLDEV();
	~CZamLDEV();
	int Demarrage();
	void ZamProc(int i);
	int Lance();
	int Arret();
	int Program();
	void DAlm();
	int ProgramD();
	int ProgramC();
	int Pmoyenne();
	string Sheure();

  private:
	bool Initialisation();
public:
	shared_ptr <CZamLLN0> m_lln0;
	shared_ptr <CZamLPHD> m_lphd;
	shared_ptr <CZamDEEV> m_deev;
	shared_ptr <CZamFSCC> m_fscc;
	shared_ptr <CZamFSCH> m_fsch1;
	shared_ptr <CZamFSCH> m_fsch2;
	shared_ptr <CZamC118> m_c118;
	shared_ptr <CZamLDEV> m_ldev;
	shared_ptr<CZamXSWI> m_xswi;
	shared_ptr<CZamMemp> m_zmp;
	shared_ptr<CZamReport> m_zcb;
	shared_ptr<CZamDSTO> m_dsto;
	shared_ptr<CZamDPMC> m_dpmc;
	shared_ptr<CZamDWMX> m_dwmx;

	bool m_wifi;
	string m_adresse_borne;
	unsigned long long m_DptTm;
	unsigned long long m_StartTm;
	int m_fsch;
	int m_ibeh;
	float m_Wcha;
	float m_wcha;
	float m_Wmax;
	float m_Wmax0 = 0.;
	float m_pmoy;
	string m_erreur;
	string m_borne;
	string m_sens;
	string m_hdeb;
	string m_hfin;
	string m_energie;
	float m_energy;
	string m_alm;
	bool m_bide;
};
