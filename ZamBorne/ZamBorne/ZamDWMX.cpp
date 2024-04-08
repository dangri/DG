/*

ZamDWMX.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus
 industriels developpes en C++ standard et en Python par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements reposent sur les normes IEC et en particulier la norme IEC 61850
ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en oeuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850"
developpee et geree par MZ Automation GmbH (http://libiec61850.com).
La liste complete des sources est fournie dans le fichier "sources_zamiren.txt"
*/

#include "ZamDWMX.h"

CZamDWMX::CZamDWMX()
{
}

CZamDWMX::~CZamDWMX()
{
}

int CZamDWMX::Demarrage()
{
	m_zl->trace("Demarrage DWMX1");
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamDWMX::Lance()
{
	thread thr = thread(&CZamDWMX::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDWMX::Initialisation()
{
	m_Beh_stVal = m_zs->Liti(m_LN + "Beh.stVal");
	m_EcpRef_setSrcRef = m_zs->Lits(m_LN + "EcpRef.setSrcRef");
	m_FctEna_stVal = m_zs->Litb(m_LN + "FctEna.stVal");
	m_NamPlt_swRev = m_zs->Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = m_zs->Lits(m_LN + "NamPlt.vendor");
	m_ReqWLim_mag_f = m_zs->Litf(m_LN + "ReqWLim.mag.f");
	m_WLimSet_setMag_f = m_zs->Litf(m_LN + "WLimSet.setMag.f");
	m_WLimSpt_mxVal_f = m_zs->Litf(m_LN + "WLimSpt.mxVal.f");
	m_reqwlim = 0;
	return true;
}

void CZamDWMX::memax()
{
// Lecture nouvelles valeurs
	m_ReqWLim_mag_f = m_zs->Litf(m_LN + "ReqWLim.mag.f");
//	m_zl->trace(fmt::format("DWMX-lecture ReqWLim: {:03.0f} W", reqwlim));
	if (abs(m_ReqWLim_mag_f - m_reqwlim) > 1000.)
	{
		m_reqwlim = m_ReqWLim_mag_f;
		m_zs->Ecrit(m_LN + "WLimSet", "setMag.f", m_reqwlim);
		m_zs->Ecrit(m_IED + m_LD + "/DESE1.ChaPwrLim", "setMag.f", m_reqwlim);
		m_zl->trace(fmt::format("DWMX: Nouvelle limite globale de Puissance = {:03.0f} W", m_reqwlim));
		auto zc(make_shared<CZamClient>("IEDEV", m_evse->m_adresse_ve));
		zc->getcons();
		zc->cdeFloat("IEDEVLDEV1/DPMC1.WLimSpt1", m_reqwlim, false);
		m_zl->trace(fmt::format("DWMX: Envoi de commande de puissance max au vehicule connecte "));
		zc->decon();
	}
}
void CZamDWMX::ZamProc(int i)
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
		m_zl->info(fmt::format(" {}: Init impossible", m_nom));
	}
	m_zl->info(fmt::format("{}: Arret du programme", m_nom));
}

int CZamDWMX::Arret()
{
	int err = 0;
	return err;
}
