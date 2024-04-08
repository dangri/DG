/*

ZamDHVT.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus
 industriels developpes en C++ standard et en Python par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements reposent sur les normes IEC et en particulier la norme IEC 61850
ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en oeuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850"
developpee et geree par MZ Automation GmbH (http://libiec61850.com).
La liste complete des sources est fournie dans le fichier "sources_zamiren.txt"
*/

#include "ZamDHVT.h"
#include <thread>

CZamDHVT::CZamDHVT()
{
}

CZamDHVT::~CZamDHVT()
{
}

int CZamDHVT::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamDHVT::Lance()
{
	boost::thread thr = boost::thread(&CZamDHVT::ZamProc, this, 1);
	return 0;
}

bool CZamDHVT::Initialisation()
{
	m_Beh_stVal = m_zs->Liti(m_LN + "Beh.stVal");
	m_EcpRef_setSrcRef = m_zs->Lits(m_LN + "EcpRef.setSrcRef");
	m_FctEna_Cancel_Test = m_zs->Litb(m_LN + "FctEna.Cancel_Test");
	m_FctEna_Oper_Check = m_zs->Litui(m_LN + "FctEna.Oper_Check");
	m_FctEna_ctlModel = m_zs->Liti(m_LN + "FctEna.ctlModel");
	m_FctEna_ctlNum = m_zs->Liti(m_LN + "FctEna.ctlNum");
	m_FctEna_origin_orIdent = m_zs->Lits(m_LN + "FctEna.origin_orIdent");
	m_FctEna_stVal = m_zs->Litb(m_LN + "FctEna.stVal");
	m_NamPlt_swRev = m_zs->Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = m_zs->Lits(m_LN + "NamPlt.vendor");
	return true;
}

void CZamDHVT::ZamProc(int i)
{
	int err = 0;
	if (Initialisation())
	{
		while(true)
		{
			if (m_zp->m_stop) break;
// Lecture nouvelles valeurs

// Boucle de traitement

// Ecriture changements

// Delai
			sleep(1);
		} // boucle
	} // if Initialisation
	else
	{
		cout << "Init impossible" << m_nom << endl;
	}
	 cout << m_nom << "Arret du programme" << endl;
}

int CZamDHVT::Arret()
{
	int err = 0;

	return err;
}
