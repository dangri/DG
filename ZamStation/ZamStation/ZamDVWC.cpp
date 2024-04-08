/*

ZamDVWC.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus
 industriels developpes en C++ standard et en Python par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements reposent sur les normes IEC et en particulier la norme IEC 61850
ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en oeuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850"
developpee et geree par MZ Automation GmbH (http://libiec61850.com).
La liste complete des sources est fournie dans le fichier "sources_zamiren.txt"

Réglage du niveau de puissance active en fonction de la tension Nom: DVWC
La fonction opérationnelle de tension-puissance active (LN DVWC) définit ou limite la puissance active
en fonction de la tension basse ou haute. Des paramètres ou des courbes peuvent être utilisés.
Pour la Californie, la puissance active est réduite à un taux de 25 % des caractéristiques assignées
réelles par 1 % de la tension nominale et la puissance active produite par l'onduleur intelligent est réduite
à 0 watt lorsque la tension mesurée est supérieure à 110 % de la tension nominale. L'utilisation initiale
de la fonction est de 106 % de la tension nominale.
Egalement appelé P(U).
*/

#include "ZamDVWC.h"
#include "ZamLDCS.hpp"
#include "ZamDPMC.h"
#include <fmt/format.h>

CZamDVWC::CZamDVWC()
{
}

CZamDVWC::~CZamDVWC()
{
}

int CZamDVWC::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = "LDCS";
	m_nom = "DVWC1";
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

int CZamDVWC::Lance()
{
	thread thr = thread(&CZamDVWC::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDVWC::Initialisation()
{
	m_Beh_stVal = m_zs->Liti(m_LN + "Beh.stVal");
	m_EcpRef_setSrcRef = m_zs->Lits(m_LN + "EcpRef.setSrcRef");
	m_FctEna_stVal = m_zs->Litb(m_LN + "FctEna.stVal");
	m_ReqW_mag_f = m_zs->Litf(m_LN + "ReqW.mag.f");
	m_RmpRteUse_setVal = m_zs->Litb(m_LN + "RmpRteUse.setVal");
	m_SnptBarEna_setVal = m_zs->Litb(m_LN + "SnptBarEna.setVal");
	m_VRefSet_setMag_f = m_zs->Litf(m_IED + m_LD + "/DECP1.VRefSet.setMag.f");
	m_Vmax = m_zs->Litf(m_IED + m_LD + "/DECP1.VMax.setMag.f");
	m_Vmin = m_zs->Litf(m_IED + m_LD + "/DECP1.VMin.setMag.f");
	m_Vmin = 210;
	m_ReqW_mag_f = 120000.;
	m_zs->Ecrit(m_LN + "ReqW", "mag.f", 120000.);
	m_change = false;
	m_start = true;
	return true;
}

void CZamDVWC::calmax()
{
// Lecture nouvelles valeurs
// puissance
	float req = m_zs->Litf(m_LN + "ReqW.mag.f");
	m_dim = false;
	m_p = m_zmp->Litd("Pder");
//	m_zl->trace(fmt::format("DVWC: Puissance= {:03.2f} W", m_p));
// tension
	m_V = m_zmp->Litd("Vder_module");
	if (abs(m_V - m_v) > 1.)
	{
		m_zl->trace(fmt::format("DVWC: Tension= {:03.2f} Volt", m_V));
		if ((m_V > m_Vmax) || (m_V < m_Vmin))
		{
			m_zl->info(fmt::format("DVWC:{:03.1f} V -> Diminution de la puissance de 5%", m_V));
			m_zl->info(fmt::format("DVWC:{:03.1f} V -> power is 5% decreased ", m_V));
			m_ReqW_mag_f = m_ReqW_mag_f * 0.95;
			m_change = true;
			m_dim = true;
		}
		else
		{
			if ((m_V <= (m_Vmax - 1.)) && (m_V >= (m_Vmin + 1.)) )
			{
				if (m_ReqW_mag_f < 120000)
				{
					m_zl->info(fmt::format("DVWC:{:03.1f} V -> Augmentation de la puissance de 5%", m_V));
					m_zl->info(fmt::format("DVWC:{:03.1f} V -> power is 5% increased", m_V));
					m_ReqW_mag_f = abs(m_ReqW_mag_f / .95);
					m_change = true;
				}
			}
		}
		// Ecriture changements
		if (m_change || m_start)
		{
			m_zs->Ecrit(m_IED + m_LD + "/DPMC1.WLimSpt1", "mxVal.f", m_ReqW_mag_f);
			m_zl->info(fmt::format("DVWC: Consigne de puissance : {:03.0f} W", m_ReqW_mag_f));
			m_zl->info(fmt::format("DVWC: Power setpoint : {:03.0f} W", m_ReqW_mag_f));
			m_zs->Ecrit(m_LN + "ReqW", "mag.f", m_ReqW_mag_f);
			m_zl->info("DVWC: Consigne de puissance envoyee a DPMC");
			m_zl->info("DVWC: power setpoint beeing sent to DPMC");
			m_change = false;
			m_start = false;
		}
		m_v = m_V;
	}
}

void CZamDVWC::ZamProc(int i)
{
	int err = 0;
	bool bchange = false;
	int sens = 1;
	double p;
	double v = 0.;
	if (Initialisation())
	{
		m_ReqW_mag_f = 120000.;
		m_zs->Ecrit(m_LN + "ReqW", "mag.f", 120000.);
		m_zs->Ecrit(m_IED + m_LD + "/DPMC1.WLimSpt1", "mxVal.f", 120000.);
		while(true)
		{
			if (m_zp->m_stop) break;
			// Delai
			sleep(2);
		} // boucle
	} // if Initialisation
	else
	{
		m_zl->info(fmt::format("Init impossible", m_nom));
	}
	m_zl->info(fmt::format("Arret du programme {}", m_nom));
}

int CZamDVWC::Arret()
{
	int err = 0;

	return err;
}
