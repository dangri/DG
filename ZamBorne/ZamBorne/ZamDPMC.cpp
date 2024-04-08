/*

ZamDPMC.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus
 industriels developpes en C++ standard et en Python par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements reposent sur les normes IEC et en particulier la norme IEC 61850
ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en oeuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850"
developpee et geree par MZ Automation GmbH (http://libiec61850.com).
La liste complete des sources est fournie dans le fichier "sources_zamiren.txt"
*/

#include "ZamDPMC.h"

CZamDPMC::CZamDPMC()
{
}

CZamDPMC::~CZamDPMC()
{
}

int CZamDPMC::Demarrage()
{
	m_zl->trace("Demarrage DPMC1");
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamDPMC::Lance()
{
	thread thr = thread(&CZamDPMC::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDPMC::Initialisation()
{
	m_Beh_stVal = m_zs->Liti(m_LN + "Beh.stVal");
	m_DERRef_setSrcRef = m_zs->Lits(m_LN + "DERRef.setSrcRef");
	m_EcpRef_setSrcRef = m_zs->Lits(m_LN + "EcpRef.setSrcRef");
	m_FctEna_stVal = m_zs->Litb(m_LN + "FctEna.stVal");
	m_NamPlt_swRev = m_zs->Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = m_zs->Lits(m_LN + "NamPlt.vendor");
	m_max = m_max0 = m_max1 = m_max2 = m_max3 = 0;
	return true;
}

void CZamDPMC::dimax()
{
// Lecture nouvelles valeurs
	m_WLimSpt1_mxVal_f = m_zs->Litf(m_LN + "WLimSpt1.mxVal.f"); // lim sur tension prov Station
	if (m_WLimSpt1_mxVal_f == 0) m_WLimSpt1_mxVal_f = 60000.;
	m_WLimSpt2_mxVal_f = m_zs->Litf(m_LN + "WLimSpt2.mxVal.f"); // lim sur schd prov Station
//	m_zl->trace(fmt::format("DPMC: Limite de Puissance 2 = {:03.0f} W", m_WLimSpt2_mxVal_f));
	m_WLimSpt3_mxVal_f = m_zs->Litf(m_LN + "WLimSpt3.mxVal.f"); // lim sur câble branchement
//	m_zl->trace(fmt::format("DPMC:Limite de Puissance 3 = {:03.0f} W", m_WLimSpt3_mxVal_f));

// Boucle de traitement
	if (abs(m_max1 - m_WLimSpt1_mxVal_f) > 1000.)
	{
		m_max1 = m_WLimSpt1_mxVal_f;
//		m_zl->info(fmt::format("DPMC: Nouvelle limite de Puissance 1 = {:03.0f} W", m_max1));

	}
	if (m_zp->m_sens == 1)
	{
		if (abs(m_max2 - m_WLimSpt2_mxVal_f) > 1000.)
		{
			m_max2 = m_WLimSpt2_mxVal_f;
//			m_zl->info(fmt::format("DPMC: Nouvelle limite de Puissance 2 = {:03.0f} W", m_max2));
		}
	}
	else
	{
		m_max2 = m_max1;
	}
	if (abs(m_max3 - m_WLimSpt3_mxVal_f) > 1000.)
	{
		m_max3 = m_WLimSpt3_mxVal_f;
//		m_zl->info(fmt::format("DPMC: Nouvelle limite de Puissance 3 = {:03.0f} W", m_max3));
	}
	m_max = m_max1;
	if (m_max2 > 0.)
	{
		if (m_max2 < m_max) m_max = m_max2;
	}
	if (m_max3 < m_max) m_max = m_max3;
	// Ecriture changements
	if (m_max != m_max0)
	{
		m_max0 = m_max;
		m_zs->Ecrit(m_IED + m_LD + "/DWMX1.ReqWLim", "mag.f", m_max);
		m_zl->info(fmt::format("DPMC: Nouvelle limite de Puissance = {:03.0f} W", m_max));
		m_zl->info(fmt::format("DPMC: New Power limit  = {:03.0f} W", m_max));
	}
}

void CZamDPMC::ZamProc(int i)
{
	int err = 0;
	if (Initialisation())
	{
		while(true)
		{
			if (m_zp->m_stop) break;
// Delai
			sleep(2);
		} // boucle
	} // if Initialisation
	else
	{
		m_zl->info("DPMC: Init impossible");
	}
	m_zl->trace("DPMC: Arret du programme");
}

int CZamDPMC::Arret()
{
	int err = 0;

	return err;
}
