/*

ZamMMXU.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamEquip.hpp"

class CZamMMXU : public CZamEquip
{
public:
	CZamMMXU();
	virtual ~CZamMMXU();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int);
	int Arret();
public:
   int m_Beh_stVal;
  float m_Hz_mag_f;
  string m_NamPlt_swRev;
  string m_NamPlt_vendor;
  float m_PNV_phsA_cVal_ang_f;
  float m_PNV_phsA_cVal_mag_f;
  float m_PNV_phsB_cVal_ang_f;
  float m_PNV_phsB_cVal_mag_f;
  float m_PNV_phsC_cVal_ang_f;
  float m_PNV_phsC_cVal_mag_f;
  float m_TotW_mag_f;
  bool m_PNV_phs_To_Neut;
 };

