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
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_nom = "DWMX1";
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
	m_NamPlt_swRev = m_zs->Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = m_zs->Lits(m_LN + "NamPlt.vendor");
	m_ReqWLim_mag_f = m_zs->Litf(m_LN + "ReqWLim.mag.f");
	m_WLimSet_setMag_f = m_zs->Litf(m_LN + "WLimSet.setMag.f");
	return true;
}

void CZamDWMX::ZamProc(int i)
{
	int err = 0;
	if (Initialisation())
	{
		float ReqWLim = 0;
		try
		{
			while (true)
			{
				if (m_zp->m_stop)
					break;
				// Lecture nouvelles valeurs
				ReqWLim = m_zs->Litf(m_LN + "ReqWLim.mag.f");
				if (ReqWLim != m_ReqWLim_mag_f)
				{
					// traitement
					// Répartition sur les bornes de la puissance totale max requise
					// on l'envoie sur les DO WlimSpt1 avec WSlimSpt1
					int err = 0;
					int ibc1, ibc2, ibc3, bcon, ibct;
					ibc1 = m_zmp->Liti("bc1");
					ibc2 = m_zmp->Liti("bc2");
					ibc3 = m_zmp->Liti("bc3");
					ibct = abs(ibc1) + abs(ibc2) + abs(ibc3);
					m_zl->info(fmt::format("LDCS: {:01d} vehicules actifs ", ibct));
					m_zl->info(fmt::format("LDCS: {:01d}  active vehicles  ", ibct));
// puissance disponible pour les ibct véhicules
					float W13 = ReqWLim / ibct;
					if(ibct) 
					{
						m_zl->info(fmt::format("Puissance disponible/acceptable {:03.0f} W pour {:d} EV", ReqWLim, ibct));
						m_zl->info(fmt::format(" {:03.0f} W available/acceptable power for  pour {:d} EV", ReqWLim, ibct));
					}	
					if (ibc1)
					{
						auto pevse = m_ldcs->m_map_evse["IEDSE1"];
						auto zc1 = make_shared<CZamClient>("IEDSE", pevse->m_adresse);
						bcon = (int)zc1->getcons();
						if (bcon)
						{
							zc1->cdeFloat("IEDSELDEVSE/DPMC1.WLimSpt1", W13, false);
							//	m_zl->info(fmt::format("LDCS: Consigne de puissance max pour {}: {:03.0f} W", pevse->m_adresse, W13));
							zc1->decon();
						}
					}
					if (ibc2)
					{
						auto pevse = m_ldcs->m_map_evse["IEDSE2"];
						auto zc2 = make_shared<CZamClient>("IEDSE", pevse->m_adresse);
						bcon = (int)zc2->getcons();
						if (bcon)
						{
							zc2->cdeFloat("IEDSELDEVSE/DPMC1.WLimSpt1", W13, false);
							//	m_zl->info(fmt::format("LDCS: Consigne de puissance max pour {}: {:03.0f} W", pevse->m_adresse, W13));
						}
						zc2->decon();
					}
					if (ibc3)
					{
						auto pevse = m_ldcs->m_map_evse["IEDSE3"];
						auto zc3 = make_shared<CZamClient>("IEDSE", pevse->m_adresse);
						bcon = (int)zc3->getcons();
						if (bcon)
						{
							zc3->cdeFloat("IEDSELDEVSE/DPMC1.WLimSpt1", W13, false);
							//	m_zl->info(fmt::format("LDCS: Consigne de puissance max pour {}: {:03.0f} W", pevse->m_adresse, W13));
							zc3->decon();
						}
					}
// ecriture nouvelles valeurs
					m_ReqWLim_mag_f = ReqWLim;
				} //  if change m_ReqWLim_mag_f
 // Delai
				sleep(1);
			} // boucle
		} // try
		catch (string exc)
		{
			m_zl->critical(fmt::format("DVMX Proc : {}", exc));
		}
		catch (exception e)
		{
			m_zl->critical(fmt::format("DVMX Proc : {}", string(e.what())));
		}
		catch (...)
		{
			m_zl->critical("DVMX Proc : Exception inattendue ");
		}
	} // if Initialisation
	else
	{
		m_zl->info(fmt::format("Init impossible", m_nom));
	}
	m_zl->info(fmt::format("Arret du programme {}", m_nom));
}

int CZamDWMX::Arret()
{
	int err = 0;

	return err;
}
