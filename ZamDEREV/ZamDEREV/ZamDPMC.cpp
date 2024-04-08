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
#include "ZamDWMX.h"

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
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	m_zl->trace("DPMC: Demarrage");
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
	m_ReqW_mag_f = m_zs->Litf(m_LN + "ReqW.mag.f");
	m_ReqW1_mag_f = m_zs->Litf(m_LN + "ReqW1.mag.f");
	m_ReqW2_mag_f = m_zs->Litf(m_LN + "ReqW2.mag.f");
	m_ReqW3_mag_f = m_zs->Litf(m_LN + "ReqW3.mag.f");
	m_WLimSpt1_mxVal_f = m_zs->Litf(m_LN + "WLimSpt1.mxVal.f");
	m_WLimSpt2_mxVal_f = m_zs->Litf(m_LN + "WLimSpt2.mxVal.f");
	m_WLimSpt3_mxVal_f = m_zs->Litf(m_LN + "WLimSpt3.mxVal.f");
	m_max = m_max0 = m_max1 = m_max2 = 0;

	return true;
}

void CZamDPMC::dimax()
{
	// Lecture nouvelles valeurs
	m_WLimSpt1_mxVal_f = m_zs->Litf(m_LN + "WLimSpt1.mxVal.f"); // limite externe issue de la borne
	if (m_zp->m_sens == 1)
	{
		m_WLimSpt2_mxVal_f = m_zs->Litf(m_LN + "WLimSpt2.mxVal.f"); // limite interne due au schedule
	}
	else
	{
		m_WLimSpt2_mxVal_f = m_WLimSpt1_mxVal_f;
	}
	if (abs(m_WLimSpt1_mxVal_f - m_max1) > 1000.)
	{
		m_max1 = m_WLimSpt1_mxVal_f;
		m_zl->trace(fmt::format("DPMC:Nouvelle limite de Puissance 1 = {:03.0f} W", m_max1));
	}
	if (abs(m_WLimSpt2_mxVal_f - m_max2) > 1000.)
	{
		m_max2 = m_WLimSpt2_mxVal_f;
		m_zl->trace(fmt::format("DPMC:Nouvelle limite de Puissance 2 = {:03.0f} W", m_max2));
	}
	// traitement
	m_max = m_max1;
	if (m_zp->m_sens == 1)
	{
		if (m_max2 < m_max)	m_max = m_max2;
	}
	// Ecriture changements
	if (abs(m_max0 - m_max) > 1000)
	{
		m_zs->Ecrit(m_IED + m_LD + "/DWMX1.ReqWLim", "mag.f", m_max);
		m_zl->info(fmt::format("DPMC: Nouvelle limite de Puissance = {:03.0f} W", m_max));
		m_zl->info(fmt::format("DPMC: New Power limit   = {:03.0f} W", m_max));
		m_max0 = m_max;
	}
}

void CZamDPMC::ZamProc(int i)
{
	int err = 0;
	if (Initialisation())
	{
		// Delai
		while (true)
		{
			if (m_zp->m_stop)
				break;
			this_thread::sleep_for(chrono::seconds(2));
		}
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
