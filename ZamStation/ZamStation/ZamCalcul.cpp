/*

ZamCalcul.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamCalcul.h"

#include <iostream>
#include <signal.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>
#include <fmt/format.h>


CZamCalcul::CZamCalcul()
{
}

CZamCalcul::~CZamCalcul()
{
}

int CZamCalcul::Demarrage()
{
	int err = 0;
	try
	{
//		init_logging("process");
//		Log(info, "process", "demarrage");
		err = start();
	}
	catch (exception &e)
	{
		m_zl->info(fmt::format("Calcul demarrage; Exception {}", e.what()));
	}
	return err;
}

int CZamCalcul::start()
{
// Lancement du process
//	Log(trace, "process", "start");
	int err = 0;
	m_zl->trace("Lancement CALCUL");
	try
	{
//Memoire partagee
		string nomfic = "mdr.xml";
		LitReseau(nomfic);
		Lance();
		return err;
	}
	catch (string exc)
	{
		m_zl->info(fmt::format("Calcul : Start Exception {}", exc));
		if (err == 0) err = 3;
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("Calcul, Start, Exception {}", string(e.what())));
		err = 1;
	}
	catch (...)
	{
		m_zl->info("Calcul, Start, Exception Exception inattendue");
		err = 2;
	}
	m_zl->info(fmt::format("Arret du programme {}", m_nom));
	return err;
} // start

int CZamCalcul::Lance()
{
	thread thr = thread(&CZamCalcul::ZamProc, this, 1);
	thr.detach();
	return 0;
}

bool CZamCalcul::Initialisation()
{
	//	m_zmp->AddVars("Sse1", "comp");
	//	m_zmp->AddVars("Sse2", "comp");
	//	m_zmp->AddVars("Sse3", "comp");
	//	m_zmp->AddVars("Vder_module", "float");
	//	m_zmp->AddVars("Vder_angle", "float");
	//	m_zmp->AddVars("Shp", "comp");
	//	m_zmp->Ecrit("Sse1", comp(0, 0));
	//	m_zmp->Ecrit("Sse2", comp(0, 0));
	//	m_zmp->Ecrit("Sse3", comp(0, 0));
	return true;
}
void CZamCalcul::calcul() 
{
// m_w1, m_w2, m_w3 sont actualisés par Report
	bool change = false;
	m_ibc = m_zmp->Liti("bc1");
	if (m_ibc)
	{
		if (abs((m_w10 * m_ibc) - (m_w1 * m_ibc)) > 2000.)
		{
			m_zl->trace(fmt::format("Cal: lecture WCurrent EVSE1 = {:03.0f} W", m_w1));
			change = true;
			m_w10 = m_w1;
		}
	}
	m_ibc = m_zmp->Liti("bc2");
	if (m_ibc)
	{
		if (abs((m_w20 * m_ibc) - (m_w2 * m_ibc)) > 2000.)
		{
			m_zl->trace(fmt::format("Cal: lecture WCurrent EVSE2 = {:03.0f} W", m_w2));
			change = true;
			m_w20 = m_w2;
		}
	}
	m_ibc = m_zmp->Liti("bc3");
	if (m_ibc)
	{
		if (abs((m_w30 * m_ibc) - (m_w3 * m_ibc)) > 2000.)
		{
			m_zl->trace(fmt::format("Cal: lecture WCurrent EVSE3 = {:03.0f} W", m_w3));
			change = true;
			m_w30 = m_w3;
		}
	}
	if (change || m_first)
	{
		m_zl->trace(fmt::format("Changement dans les puissances"));
		m_sse1 = comp(m_w1, - 0.4 * m_w1);
		m_zl->trace(fmt::format("Puissance DER 1 = {:03.0f} VA", abs(m_sse1)));
		m_sse2 = comp(m_w2, - 0.4 * m_w2);
		m_zl->trace(fmt::format("Puissance DER 2 = {:03.0f} VA", abs(m_sse2)));
		m_sse3 = comp(m_w3, - 0.4 * m_w3);
		m_zl->trace(fmt::format("Puissance DER 3 = {:03.0f} VA", abs(m_sse3)));
		m_Sder = m_sse1 + m_sse2 + m_sse3;
		m_zl->trace(fmt::format("Puissance DER totale = {:03.0f} VA", abs(m_Sder)));
		double p = m_w1 + m_w2 + m_w3;
		if (p >= 1000)
		{
			m_zmp->Ecrit("int_sens", 1);
			m_zl->trace(fmt::format("Puissance der consommee: {:03.0f} W", p));
		}
		if(p < 1000)
		{
			m_zmp->Ecrit("int_sens", -1);
			m_zl->trace(fmt::format("Puissance der produite: {:03.0f} W", -p));
		}
		m_zmp->Ecrit("Sder_module", abs(m_Sder));
		m_zmp->Ecrit("Sder_angle", arg(m_Sder));
		comp Shp = m_Sext + m_Sam + m_Sav + m_Sder;
		m_zl->trace(fmt::format("Puissance totale hors pertes = {:03.0f} VA", abs(Shp)));
		comp zam = m_zlam * m_long_amont;
		comp zav = m_zlav * m_long_aval;
		CalculVder(Shp);
		double vdc = abs(m_Vc);
		double shp = abs(Shp / (double)1000.);
		double Sder = abs(m_Sder) / 1000.;
		double vd = abs(m_Vd) / sqrt(2);
		double vc = abs(m_Vc) / sqrt(2);
		double vr = abs(m_Vr) / sqrt(2);
		m_zl->info(fmt::format("Vder = {:03.0f} Volt", vd));
		m_zmp->Ecrit("Vder_module", vd);
		m_zmp->Ecrit("Pder", p);
		m_zmp->Ecrit("Vder_angle", 0);
		m_vd0 = vd;
		m_first = false;
		change = false;
	}
}

void CZamCalcul::ZamProc(int i)
{
	if (Initialisation())
	{
		try
		{
			double vd0, vc0;
			vd0 = vc0 = 0.;
			m_zam = m_zlam * m_long_amont;
			m_zav = m_zlav * m_long_aval;
			m_first = true;
			m_w1 = m_w2 = m_w3 = 0.;
			while (true)
			{
				if (m_zp->m_stop)
					break;
				this_thread::sleep_for(chrono::seconds(2));
			}
		}	 // try
		catch (SignalException &e)
		{
			m_zl->info(fmt::format("Calcul Exception {}", string(e.what())));
		}
		catch (string exc)
		{
			m_zl->info(fmt::format("Calcul: Exception {}", exc));
		}
		catch (exception e)
		{
			m_zl->info(fmt::format("Calcul Exception {}", string(e.what())));
		}
		catch (...)
		{
			m_zl->info("Calcul: Exception inattendue");
		}
	} // if Initialisation
	else
	{
		m_zl->info(fmt::format("Init impossible", m_nom));
	}
	m_zl->info(fmt::format("Arret du programme {}", m_nom));
} // Proc

comp CZamCalcul::DeltaV(comp V, comp z, comp S)
{
	comp I = S / (3. * V);
	comp dv = z * I;
	dv /= 2.;
	comp V1 = V - dv;
	return V1;
}

comp CZamCalcul::Pertes(comp V1, comp z, comp S)
{
	comp p = z.real() * (S.real() * S.real() + S.imag() * S.imag());
	p = p / (abs(V1) * abs(V1));
	p /= 2.;
	return 0;
}

void CZamCalcul::CalculVder(comp St)
{
	comp Vd, Vc, pam, pav;
	comp Vder1;
	float v;
	m_Vr = m_Vs * (1. - m_ucc * (St / m_Sn));
	comp S = St - m_Sext;
	Vd = DeltaV(m_Vr, m_zam, S);
	v = abs(Vd) / sqrt(2);
	pam = Pertes(Vd, m_zam, S);
	Vc = DeltaV(Vd, m_zav, m_Sav);
	pav = Pertes(Vc, m_zav, m_Sav);
	// total des pertes
	comp ptf = m_Spf + m_Spv + pam + pav;
	comp Stp = St + ptf;
	m_Vr = m_Vs * (1. - m_ucc * (Stp / m_Sn));
	comp Sp = Stp - m_Sext;
	Vd = DeltaV(m_Vr, m_zam, S);
	Vc = DeltaV(Vd, m_zav, m_Sav + pav);
	m_Vd = Vd;
	m_Vc = Vc;
	return;
}

int CZamCalcul::LitReseau(string& nomfichier)
{
	int err = 0;
	try
	{
		xml_document doc;
		xml_parse_result res = doc.load_file("mdr.xml");
		if (!res)
		{
			throw string("Erreur ouverture du fichier du modèle de Reseau");
		}
		double S, mod, tan, ang, arg, P, Q, R, X;
		xml_node xr = doc.child("reseau");
		m_reseau = xr.attribute("desc").value();
// source
		xml_node xs = xr.child("source");
		m_source = xs.attribute("desc").value();
		m_Vs = xs.attribute("tension").as_double();
		m_Vs *= sqrt(2);
		xml_node xsp = xs.child("puissance");
		m_Sn = xsp.attribute("nominale").as_double() * 1000.;
		m_ucc = xsp.attribute("ucc").as_double();
		xml_node xse = xs.child("pertes");
		m_Spf = xse.attribute("constantes").as_double() * 1000.;
		m_Spv = xse.attribute("variables").as_double() * 1000.;
// autre
		xml_node xau = xr.child("ext");
		m_autre = xau.attribute("desc").value();
		mod = xau.attribute("kVA").as_double() * 1000.;
		tan = xau.attribute("tg").as_double();
		arg = atan(tan);
		m_Sext = polar(mod, arg);
// amont
		xml_node xam = xr.child("amont");
		m_amont = xam.attribute("desc").value();
		mod = xam.attribute("kVA").as_double() * 1000.;
		tan = xam.attribute("tg").as_double();
		arg = atan(tan);
		m_Sam = polar(mod, arg);
		xml_node xamc = xam.child("conducteur");
		m_cond_amont = xamc.attribute("desc").value();
		m_long_amont = xamc.attribute("longueur").as_double();
		R = xamc.attribute("impedance_re").as_double();
		X = xamc.attribute("impedance_im").as_double();
		m_zlam = comp(R, X);
// aval
		xml_node xav = xr.child("aval");
		m_aval = xav.attribute("desc").value();
		mod = xav.attribute("kVA").as_double() * 1000.;
		tan = xav.attribute("tg").as_double();
		arg = atan(tan);
		m_Sav = polar(mod, arg);
		xml_node xavc = xav.child("conducteur");
		m_cond_aval = xavc.attribute("desc").value();
		m_long_aval = xavc.attribute("longueur").as_double();
		R = xavc.attribute("impedance_re").as_double();
		X = xavc.attribute("impedance_im").as_double();
		m_zlav = comp(R, X);
	}
	catch (string exc)
	{
		m_zl->info(fmt::format("Calcul : Litreseau Exception {}",exc));
		if (err == 0)
			err = 3;
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("Calcul, Litreseau: Exception", string(e.what())));
		err = 1;
	}
	catch (...)
	{
		m_zl->info("Calcul, Litreseau, Exception Exception inattendue");
		err = 2;
	}
	return err;
}
