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
#include "ZamLDCS.hpp"

CZamDPMC::CZamDPMC()
{
}

CZamDPMC::~CZamDPMC()
{
}

int CZamDPMC::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = "LDCS";
	m_nom = "DPMC1";
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
	m_FctEna_stVal = m_zs->Litb(m_LN + "FctEna.stVal");
	m_NamPlt_swRev = m_zs->Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = m_zs->Lits(m_LN + "NamPlt.vendor");
	m_ReqW_mag_f = m_zs->Litf(m_LN + "ReqW.mag.f");
	m_ReqW1_mag_f = m_zs->Litf(m_LN + "ReqW1.mag.f");
	m_ReqW2_mag_f = m_zs->Litf(m_LN + "ReqW2.mag.f");
	m_ReqW3_mag_f = m_zs->Litf(m_LN + "ReqW3.mag.f");
	m_WLimSpt1_mxVal_f = 120000.;
	m_lim1 = 120000.;
	m_lim0 = 0.;
	return true;
}

void CZamDPMC::dimax()
{
	m_WLimSpt1_mxVal_f = m_zs->Litf(m_LN + "WLimSpt1.mxVal.f");
	if (abs(m_WLimSpt1_mxVal_f - m_lim0) > 100.)
	{
		m_zl->info(fmt::format("DPMC: Nouvelle consigne de puissance recue de DVWC: {:03.0f} W", m_WLimSpt1_mxVal_f));
		m_zl->info(fmt::format("DPMC: DVWC has received a new power setpoint : {:03.0f} W", m_WLimSpt1_mxVal_f));
		m_lim0 = m_WLimSpt1_mxVal_f;
		m_zs->Ecrit(m_IED + m_LD + "/DWMX1.ReqWLim", "mag.f", m_lim0);
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
			// envoyer aux trois bornes
// Delai
			sleep(2);
		} // boucle
	} // if Initialisation
	else
	{
		m_zl->critical("DPMC: Init impossible");
	}
	m_zl->critical("DPMC: Arret du programme");
}

int CZamDPMC::Arret()
{
	int err = 0;

	return err;
}
