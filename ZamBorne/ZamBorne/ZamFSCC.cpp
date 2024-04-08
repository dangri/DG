/*

ZamFSCC.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamFSCC.hpp"
#include "ZamCod.hpp"

CZamFSCC::CZamFSCC()
{
}

CZamFSCC::~CZamFSCC()
{
}

int CZamFSCC::Lance()
{
	thread thr(&CZamFSCC::ZamProc, this, 1);
	thr.detach();
	return 0;
}

int CZamFSCC::Demarrage()
{
	int err = 0;
	m_IED = m_zp->m_nomied;
	m_LD = m_zu->m_nom;
	m_LN = m_IED + m_LD + "/" + m_nom + ".";
	m_sne = m_LD + "_" + m_nom + "_";
	err = Lance();
	return err;
}

bool CZamFSCC::Initialisation()
{
	m_ActSchdRef_stVal = Lits(m_LN + "ActSchdRef.stVal");
	m_Beh_stVal = Liti(m_LN + "Beh.stVal");
	m_CtlEnt_intAddr = Lits(m_LN + "CtlEnt.intAddr");
	m_CtlEnt_setSrcRef = Lits(m_LN + "CtlEnt.setSrcRef");
	m_NamPlt_swRev = Lits(m_LN + "NamPlt.swRev");
	m_NamPlt_vendor = Lits(m_LN + "NamPlt.vendor");
	m_Schd1_intAddr = Lits(m_LN + "Schd1.intAddr");
	m_Schd1_setSrcRef = Lits(m_LN + "Schd1.setSrcRef");
	m_Schd2_intAddr = Lits(m_LN + "Schd2.intAddr");
	m_Schd2_setSrcRef = Lits(m_LN + "Schd2.setSrcRef");
	m_Schd3_intAddr = Lits(m_LN + "Schd3.intAddr");
	m_Schd3_setSrcRef = Lits(m_LN + "Schd3.setSrcRef");
	m_ValMV_mag_f = Litf(m_LN + "ValMV.mag.f");
	m_nbs = 1;
	m_CtlEnt_setSrcRef = Lits(m_LN + "CtlEnt.setSrcRef");
	if (! m_Schd2_setSrcRef.empty())
	{
		m_nbs = 2;
	}
	return true;
} // Initialisation

void CZamFSCC::ZamProc(int i)
{
	int st, st1, st2, st3;
	int pr, pr1, pr2, pr3;
	int run;
	float ValMV;
	string outref;
	string ActSchdRef;
	if (Initialisation())
	{
		while (true)
		{
			if (m_stop)
			{
				return;
			}
// OFF fonction inactive
			if (m_Beh_stVal == 2) goto finboucle;
// Fonction active
// Lecture variables
			ActSchdRef = Lits(m_LN + "ActSchdRef.stVal");
			if (ActSchdRef.empty()) ActSchdRef = m_Schd1_setSrcRef;
			// schedule status, priority
			st1 = Liti(m_IED + m_Schd1_setSrcRef + ".SchdSt.stVal");
			pr1 = Liti(m_IED + m_Schd1_setSrcRef + ".SchdPrio.setVal");
			if (m_nbs > 1)
			{
				st2 = Liti(m_IED + m_Schd2_setSrcRef + ".SchdSt.stVal");
				pr2 = Liti(m_IED + m_Schd2_setSrcRef + ".SchdPrio.setVal");
				if (m_nbs > 2)
				{
					st3 = Liti(m_IED + m_Schd3_setSrcRef + ".SchdSt.stVal");
					pr3 = Liti(m_IED + m_Schd3_setSrcRef + ".SchdPrio.setVal");
				}
			}
			// Traitements
			if (ActSchdRef == m_Schd1_setSrcRef)
			{
				run = 1;
				pr = pr1;
			}
			if (m_nbs > 1)
			{
				if (ActSchdRef == m_Schd2_setSrcRef)
				{
					run = 2;
					pr = pr2;
				}
				if (m_nbs > 2)
				{
					if (ActSchdRef == m_Schd3_setSrcRef)
					{
						run = 3;
						pr = pr3;
					}
				}
			}
			if (m_nbs > 1)
			{
				switch (run)
				{
				case 0: // rien n'est en cours ? on y met le 1
					run = 1;
					pr = pr1;
					break;
				case 1: // sch 1 est en cours
					pr = pr1;
					if ((st2 == 4) && (pr2 > pr1))
					{
						run = 2;
						pr = pr2;
						ActSchdRef = m_Schd2_setSrcRef;
					}
					if ((st3 == 4) && (pr3 > pr))
					{
						run = 2;
						pr = pr3;
						ActSchdRef = m_Schd3_setSrcRef;
					}
					break;
				case 2: // sch2 est en cours
					if(st2 == 1)		// 2 est disable
					{
						run = 1;
						pr = pr1;
						ActSchdRef = m_Schd1_setSrcRef;
					}
					pr = pr2;
					if ((st1 == 4) && (pr1 > pr))
					{
						run = 1;
						pr = pr1;
						ActSchdRef = m_Schd1_setSrcRef;
					}
					if ((st3 == 4) && (pr3 > pr))
					{
						run = 3;
						pr = pr3;
						ActSchdRef = m_Schd3_setSrcRef;
					}
					break;
				case 3: // sch3 est en cours
					pr = pr3;
					if ((st1 == 4) && (pr1 > pr))
					{
						run = 1;
						pr = pr1;
						ActSchdRef = m_Schd1_setSrcRef;
					}
					if ((st2 == 4) && (pr2 > pr))
					{
						run = 2;
						pr = pr2;
						ActSchdRef = m_Schd2_setSrcRef;
					}
					break;
				}
			}
			if (run == 0) run = 1;
			if (m_ActSchdRef_stVal != ActSchdRef)
			{
				m_ActSchdRef_stVal = ActSchdRef;
				Ecrit(m_LN + "ActSchdRef", "stVal", ActSchdRef); 
			}
// Ecrire la valeur sur la cible
			st = Liti(m_IED + m_ActSchdRef_stVal + ".SchdSt.stVal");
			if (st == 4)
			{
				ValMV = Litf(m_IED + m_ActSchdRef_stVal + ".ValMV.mag.f");
				if (m_ValMV_mag_f != ValMV)
				{
					m_ValMV_mag_f = ValMV;
					m_zs->Ecrit(m_LN + "ValMV", "mag.f", ValMV);
					outref = m_CtlEnt_setSrcRef;
					if ( ! outref.empty())
					{
						m_zs->Ecrit(m_IED + outref, "mxVal.f", ValMV);
					}
				}
			}
//	 Delai
		finboucle:
			this_thread::sleep_for(chrono::seconds(1));
		} // boucle
	} // if init
	else
	{
		m_zl->info(fmt::format(" {}: Init impossible", m_nom));
	}
	m_zl->info(fmt::format("{}: Arret du programme", m_nom));
} // ZamProcess

int CZamFSCC::Arret()
{
	int err = 0;

	return err;
}

