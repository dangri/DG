/*

ZamDSTO.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus
 industriels developpes en C++ standard et en Python par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements reposent sur les normes IEC et en particulier la norme IEC 61850
ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en oeuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850"
developpee et geree par MZ Automation GmbH (http://libiec61850.com).
La liste complete des sources est fournie dans le fichier "sources_zamiren.txt"
*/

#include "ZamDSTO.h"

CZamDSTO::CZamDSTO()
{
}

CZamDSTO::~CZamDSTO()
{
}

int CZamDSTO::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamDSTO::Lance()
{
	thread thr = thread(&CZamDSTO::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDSTO::Initialisation()
{
	m_Beh_stVal = m_zs->Liti(m_LN + "Beh.stVal");
	m_DERUnit_setSrcRef = m_zs->Lits(m_LN + "DERUnit.setSrcRef");
	m_EqSto_setSrcRef = m_zs->Lits(m_LN + "EqSto.setSrcRef");
	m_NamPlt_swRev = m_zs->Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = m_zs->Lits(m_LN + "NamPlt.vendor");
	m_SocWh_mag_f = m_deev->m_Soc;
	m_WSpt_mxVal_f = m_deev->m_WCurrent;
	m_WhMaxRtg_setMag_f = m_zmp->Litf("IEDEVLDEV1/DSTO1.WhMaxRtg.setMag.f");
	m_WhMinRtg_setMag_f = m_zmp->Litf("IEDEVLDEV1/DSTO1.WhMinRtg.setMag.f");;
	return true;
}

void CZamDSTO::ZamProc(int i)
{
	int err = 0;
	if (Initialisation())
	{
		float socwh, wspt, whmaxrtg, whminrtg, whrtg;
		while (true)
		{
			if (m_zp->m_stop)
				break;
// Lecture nouvelles valeurs
//			chawmax = m_zs->Litf(m_LN + "ChaWMax.setMag.f");
//			chawmaxrtg = m_zs->Litf(m_LN + "ChaWMaxRtg.setMag.f");
//			dschawmax = m_zs->Litf(m_LN + "DschWMax.setMag.f");
//			dschawmaxrtg = m_zs->Litf(m_LN + "DschWMaxRtg.setMag.f");
			socwh = m_deev->m_Soc;
			wspt = m_zs->Litf(m_LN + "WSpt.mxVal.f");
			whmaxrtg = m_zmp->Litf(m_LN + "WhMaxRtg.setMag.f");
			whminrtg = m_zmp->Litf(m_LN + "WhMinRtg.setMag.f");
			whrtg = whmaxrtg - whminrtg;
// Boucle de traitement
			if (m_SocWh_mag_f != socwh)
			{
				m_SocWh_mag_f = socwh;
				Ecrit(m_LN + "SocWh", "mag.f", socwh);
			}
			if (m_WSpt_mxVal_f != wspt)
			{
				m_WSpt_mxVal_f = wspt;
				Ecrit(m_LN + "WSpt", "mxVal.f", wspt);
			}
			if (m_WhMaxRtg_setMag_f != whmaxrtg)
			{
				m_WhMaxRtg_setMag_f = whmaxrtg;
				Ecrit(m_LN + "WhMaxRtg", "setMag.f", socwh);
			}
			if (m_WhMinRtg_setMag_f != whminrtg)
			{
				m_WhMinRtg_setMag_f = whminrtg;
				Ecrit(m_LN + "WhMinRtg", "setMag.f", whminrtg);
			}
// Delai
			sleep(1);
		} // boucle
	}	 // if Initialisation
	else
	{
		m_zl->info(fmt::format("DSTO: Init impossible {}", m_nom));;
	}
	m_zl->info(fmt::format("DSTO: Arret du serveur {}", m_nom));
}

int CZamDSTO::Arret()
{
	int err = 0;

	return err;
}
