/*

ZamDEEVP.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamDEEVP.hpp"
#include <complex>

typedef complex<double> comp;

CZamDEEVP::CZamDEEVP()
{
}

CZamDEEVP::~CZamDEEVP()
{
}

int CZamDEEVP::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_iLD = m_LD.substr(4, 1);
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	if (m_LD == "LDSE1")
	{
		m_ref = "IEDCSDLDSE1/";
		m_ise = 1;
	}
	if (m_LD == "LDSE2")
	{
		m_ref = "IEDCSDLDSE2/";
		m_ise = 2;
	}
	if (m_LD == "LDSE3")
	{
		m_ref = "IEDCSDLDSE3/";
		m_ise = 3;
	}
	err = Lance();
	return err;	
}

int CZamDEEVP::Lance()
{
//	m_zu->m_zp->Log(trace, "sys", "DEEVP", "Lancement du serveur " + m_nomc);
	thread thr(&CZamDEEVP::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamDEEVP::Initialisation()
{
	//	entete des noms et identification du LD
	//	m_LD = m_nomp + m_nomc;
	//	m_sne = m_nomc + "_";
	return true;
} // Initialisation

void CZamDEEVP::ZamProc(int i)
{
	if (Initialisation())
	{
		bool deb = false;
		bool fin = false;
		int evst;
		bool bcd= false;
		bool bd = false;
		m_wh = 0;
		m_w = 0;
		string envoi;
		int id = stoi(m_iLD);
		m_t0 = m_zp->m_temps;
		while (true)
		{
			if (m_stop)
			{
				return;
			}
			int err = 0;
// Lecture
			evst = m_zs->Liti(m_LN + "EVSt.stVal");
			if (m_EVSt != evst)
			{
				// gerer les modifications
				m_zl->info(fmt::format("DEEVP {} -> EVSt = {:2d}", m_LD, evst));
				m_EVSt = evst;
				switch (evst)
				{
				case 0:
					err = 1;
					break;
				case 1:
					if (m_ise == 1)
						m_zmp->Ecrit("bc1", 0);
					if (m_ise == 2)
						m_zmp->Ecrit("bc2", 0);
					if (m_ise == 3)
						m_zmp->Ecrit("bc3", 0);
					if (deb == false)
					{
						deb = true;
					}
					else
					{
						fin = true;
					}
					break;
				case 2:
				case 3:
					if (m_ise == 1)
						m_zmp->Ecrit("bc1", 1);
					if (m_ise == 2)
						m_zmp->Ecrit("bc2", 1);
					if (m_ise == 3)
						m_zmp->Ecrit("bc3", 1);
					envoi = m_evse + ", chargestart" + "," + m_zp->Sheure() + "," + m_ldse->m_energie;
					m_zmp->Ecrit("int_ChangeChargeStatus", envoi); // ou WhTot'
					m_zl->info(fmt::format("DEEVP: Message: {}", envoi));
					break;
				case 4:
				case 5:
					if (m_ise == 1)
						m_zmp->Ecrit("bc1", -1);
					if (m_ise == 2)
						m_zmp->Ecrit("bc2", -1);
					if (m_ise == 3)
						m_zmp->Ecrit("bc3", -1);
					envoi = m_evse + ", dischargestart" + "," + m_zp->Sheure() + "," + m_ldse->m_energie;
					m_zmp->Ecrit("int_ChangeChargeStatus", envoi); // ou WhTot'
					m_zl->info(fmt::format("DEEVP: Message: {}", envoi));
					bd = true;
					break;
				case 12:
				case 13:
				case 14:
				case 15:
					if (m_ise == 1)
						m_zmp->Ecrit("bc1", 0);
					if (m_ise == 2)
						m_zmp->Ecrit("bc2", 0);
					if (m_ise == 3)
						m_zmp->Ecrit("bc3", 0);
					break;
				default:
					err = 1;
					break;
				} // switch
			} // evst change
			if (err)
			{
//				m_zl->info(fmt::format("DEEVP: Valeur de EVSt impossible : {:d}", evst));
				m_zl->info(fmt::format("DEEVP: EVSt value not possible : {:d}", evst));
				continue;
			}
			// gerer la charge/decharge
			if (!fin)
			{
				while (m_ldse->m_energie.empty())
				{
					m_WhTgt = Litf(m_LN + "WhTgt.setMag.f");
					if (abs(m_WhTgt) > 0.)
					{
						m_ldse->m_energie = fmt::format("{:03.0f}", m_WhTgt);
					}
					this_thread::sleep_for(chrono::milliseconds(200));
				}
				m_w = m_zs->Litf(m_LN + "WCurrent.mag.f");
				switch (m_ise)
				{
				case 1:
					m_cal->m_w1 = m_w;
					break;
				case 2:
					m_cal->m_w2 = m_w;
					break;
				case 3:
					m_cal->m_w3 = m_w;
					break;
				}
				m_wh = m_w + m_w0;
				m_wh /= 2;
				m_t = m_zp->m_temps;
				m_wht += m_wh * (m_t - m_t0);
				m_w0 = m_w;
				m_t0 = m_t;
				evst = m_zs->Liti(m_LN + "EVSt.stVal");
				this_thread::sleep_for(chrono::milliseconds(500));
			} // sortie : evst = 1 fin de charge
			else
			{
				break;
				// fin = true;
			}
		}	// while true
		m_wht /= 3600000.;
		if (bd)
		{
			envoi = m_evse + ", dischargeend" + "," + m_zp->Sheure() + "," + to_string(m_wht);
			m_zp->m_zmp->Ecrit("int_ChangeChargeStatus", envoi); // WhTot'
			m_zl->info(fmt::format("DEEVP: Message: {}", envoi));
		}
		else
		{
			envoi = m_evse + ", chargeend" + "," + m_zp->Sheure() + "," + to_string(m_wht);
			m_zmp->Ecrit("int_ChangeChargeStatus", envoi); // WhTot'
			m_zl->info(fmt::format("DEEVP: Message: {}", envoi));
		}
	}	 // if init
	else
	{
		m_zl->trace("DEEVP: Initialisation fautive");
	}
	m_zl->trace("DEEVP: Sortie fin de charge");
} // ZamDEEVP


