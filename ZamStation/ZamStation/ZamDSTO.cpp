/*

ZamDSTO.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
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
	m_stop = false;
	thread thr(&CZamDSTO::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDSTO::Initialisation()
{
	m_Beh_stVal = m_zs->Liti(m_LN + "Beh.stVal");

	m_DERUnit_setSrcRef = m_zs->Lits(m_LN + "DERUnit.setSrcRef");
	m_EqSto_setSrcRef = m_zs->Lits(m_LN + "EqSto.setSrcRef");
	m_InclSto1_setSrcRef = m_zs->Lits(m_LN + "InclSto1.setSrcRef");
	m_InclSto2_setSrcRef = m_zs->Lits(m_LN + "InclSto2.setSrcRef");
	m_InclSto3_setSrcRef = m_zs->Lits(m_LN + "InclSto3.setSrcRef");
	m_NamPlt_swRev = m_zs->Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = m_zs->Lits(m_LN + "NamPlt.vendor");
	m_SocWh_mag_f = 0;
	m_WSpt_mxVal_f = 0;
	m_WhMaxRtg_setMag_f = 0;
	m_WhMinRtg_setMag_f = 0;
	return true;
}

void CZamDSTO::ZamProc(int i)
{
	int err = 0;
	if (Initialisation())
	{
		float soc, spt, max, min;
		while (true)
		{
			if (m_zp->m_stop)
				break;
// Lecture nouvelles valeurs
			soc =  Litf("IEDCSDLDSE1/DSTO1.SocWh.mag.f");
			soc += Litf("IEDCSDLDSE2/DSTO1.SocWh.mag.f");
			soc += Litf("IEDCSDLDSE3/DSTO1.SocWh.mag.f");
			spt =  Litf("IEDCSDLDSE1/DSTO1.WSpt.mxVal.f");
			spt += Litf("IEDCSDLDSE2/DSTO1.WSpt.mxVal.f");
			spt += Litf("IEDCSDLDSE3/DSTO1.WSpt.mxVal.f");
			max =  Litf("IEDCSDLDSE1/DSTO1.WhMaxRtg.setMag.f");
			max += Litf("IEDCSDLDSE2/DSTO1.WhMaxRtg.setMag.f");
			max += Litf("IEDCSDLDSE3/DSTO1.WhMaxRtg.setMag.f");
			min =  Litf("IEDCSDLDSE1/DSTO1.WhMinRtg.setMag.f");
			min += Litf("IEDCSDLDSE1/DSTO1.WhMinRtg.setMag.f");
			min += Litf("IEDCSDLDSE1/DSTO1.WhMinRtg.setMag.f");
// Boucle de traitement
			if (m_SocWh_mag_f != soc)
			{
				m_SocWh_mag_f = soc;
				m_zs->Ecrit(m_LN + "SocWh", "mag.f", soc);
			}
			if (m_WSpt_mxVal_f != spt)
			{
				m_WSpt_mxVal_f = spt;
				m_zs->Ecrit(m_LN + "m_WSpt", "mxVal.f", spt);
			}
			if (m_WhMaxRtg_setMag_f != max)
			{
				m_WhMaxRtg_setMag_f = max;
				m_zs->Ecrit(m_LN + "WhMaxRtg", "setMag.f", max);
			}
			if (m_WhMinRtg_setMag_f != min)
			{
				m_WhMinRtg_setMag_f = min;
				m_zs->Ecrit(m_LN + "WhMinRtg", "setMag.f", min);
			}
// Delai
			sleep(1);
		} // boucle
	}	 // if Initialisation
	else
	{
		m_zl->info(fmt::format("Init impossible", m_nom));
	}
	m_zl->info(fmt::format("Arret du programme {}", m_nom));
}

int CZamDSTO::Arret()
{
	int err = 0;

	return err;
}
