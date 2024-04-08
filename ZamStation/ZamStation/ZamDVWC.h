/*

ZamDVWC.h

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus
 industriels developpes en C++ standard et en Python par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements reposent sur les normes IEC et en particulier la norme IEC 61850
ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en oeuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850"
developpee et geree par MZ Automation GmbH (http://libiec61850.com).
La liste complete des sources est fournie dans le fichier "msources_zamiren.txt" »
*/

#pragma once
#include "ZamEquip.hpp"

class CZamLCDS;

class CZamDVWC : public CZamEquip
{
public:
	CZamDVWC();
	~CZamDVWC();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int i);
	int Arret();
	void calmax();

  public:
	int m_Beh_stVal;
	string m_EcpRef_setSrcRef;
	bool m_FctEna_stVal;
	string m_NamPlt_swRev;
	string m_NamPlt_vendor;
	float m_ReqW_mag_f;
	bool m_RmpRteUse_setVal;
	bool m_SnptBarEna_setVal;
	float m_VRefSet_setMag_f;
	shared_ptr<CZamLDCS> m_ldcs;
	double m_V; // tension courante lue en mémoire partagée
	double m_S; // puissance totale lue en mémoire partagée
	float m_Vmax;
	float m_Vmin;
	float m_p;
	float m_v;
	bool m_change;
	bool m_start;
	bool m_dim = false;
};
