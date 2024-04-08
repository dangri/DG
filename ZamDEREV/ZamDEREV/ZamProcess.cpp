/*

ZamProcess.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamProcess.hpp"
#include "ZamLDEV.hpp"
#include "ZamCtemps.hpp"
#include "ZamIed.hpp"
#include "ZamModel.hpp"
#include "ZamServeur.hpp"
#include <csignal>
#include <fmt/core.h>
#include <thread>
#include <chrono>
#include <filesystem>

using namespace std;

CZamProcess::CZamProcess()
{
}

CZamProcess::~CZamProcess()
{
}

int CZamProcess::Demarrage()
{
	int err = 0;
	try
	{
// LOGS
		auto zlog = make_shared<CZamLog>("C++_report_Vehicule");
		m_zl = zlog;
		m_zl->niveau("trace");
		m_zl->trace("Demarrage du log");
		m_sem = semaphore("/sem.ZAMSEMAL");
// Sémaphores
		m_semdg = semaphore("ZAMDG");
		if (!m_semdg) throw string("Pb ouverture semaphore ZAMDG");
		m_zl->trace("semaphore m_semdg OK");
		m_sem = semaphore("/sem.ZAMSEMAL");
		if (!m_sem) throw string("Pb ouverture semaphore ZAMSEMAL");
		m_zl->trace("semaphore ZAMSEMAL OK");
		m_zl->info(fmt::format("Process: demarrage"));
		m_signalHandler.setupSignalHandlers();
		system("./kp");
		m_nomied = "IEDEV";
		auto zmodel(make_shared<CZamModel>(m_nomied + ".icd"));
		zmodel->m_nomfichier = m_nomied + ".icd";
		m_zm = zmodel;
		m_nomld = m_nomied + "LDEV1";
		auto zserver(make_shared<CZamServer>());
		m_zs = zserver;
		m_zs->m_zl = m_zl;
		m_zs->m_nomied = m_nomied;
		err = start();
	}
	catch (exception &e)
	{
		m_zl->info(fmt::format("Process: {]", e.what()));
	}
	return err;
}

int CZamProcess::start()
{
	int err = 0;
	try
	{
// Adresses IP et MAC
//		m_adresseIP_eth0 = AdresseIP("eth0");
//		m_adresseMac_eth0 = AdresseMac("eth0");
		m_adresseIP_wlan0 = AdresseIP("wlan0");
		m_zl->trace(fmt::format("Adresse IP wlan0 {}", m_adresseIP_wlan0));
//		m_adresseMac_wlan0 = AdresseMac("wlan0");
//Model
		if (m_zm->Demarrage()) throw string("Problème de construction du modèle");
// Serveur
		if(m_zs->Demarrage()) throw string("Erreur lancement serveur");
		m_zl->trace("Lancement du serveur IEC61850");
// Attente serveur OK
		while (! m_zs->m_start)
		{
			this_thread::sleep_for(chrono::seconds(1));
		}
		m_zl->trace("Serveur IEC61850 OK");
//Memoire partagee
		auto zmp(make_shared<CZamMemp>(m_nomvar, m_taille_var));
		m_zmp = zmp;
		Init();
		m_zl->trace("Creation de la memoire partagee");
		EcritMp();
		Initvehicule();
//		m_zp->m_zmp->Ecrit("int_adresseEV", m_zp->m_adresse);
// temps
		auto ctemps(make_shared<CZamCtemps>());
		ctemps->m_zp = m_zp;
		m_zt = ctemps;
		if (ctemps->Demarre()) throw string("Erreur lancement récepteur de temps");
		this_thread::sleep_for(chrono::seconds(5));
		m_zl->trace("Lancement du client de Temps");
// IHM
//		system("sudo python /home/pi/zamam/restihmEV/ZamihmEV.py mode=filaire bddrep=/home/pi/zamam &");
// ou
		system("sudo python /home/pi/zamam/restihmEV/ZamihmEV.py mode=autonome bddrep=/home/pi/zamam &");
		this_thread::sleep_for(chrono::milliseconds(900));
		m_zl->trace("Lancement des services Python");
		m_pidzam = m_zmp->Liti("int_PID");
// unit et equip
		m_ied = m_zm->m_ied;
		m_nomied = m_ied->name;
		for (auto ldi = m_ied->mapLD.begin(); ldi != m_ied->mapLD.end(); ++ldi)
		{
			auto pld = ldi->second;
			CreUnit(pld);
			auto zu = m_du[pld->inst];
			m_zl->trace(fmt::format("Integration de {} ", pld->inst));
			for (auto lni = pld->mapln.begin(); lni != pld->mapln.end(); ++lni)
			{
				auto pln = lni->second;
				zu->CreEquip(pln);
				m_zl->trace(fmt::format("Intgration de {} ", lni->first));
			}
		}
//LDEV1
		m_ldev = static_pointer_cast<CZamLDEV>(m_du["LDEV1"]);
		m_ldev->m_ldev = m_ldev;
		m_ldev->m_zmp = m_zmp;
		m_ldev->m_zl = m_zl;
		if (m_ldev->Demarrage()) throw string("Erreur Lancement ldev");
		m_zl->trace("Demarrage ldev");
//
//		CommParameters gooseCommParameters;
//		gooseCommParameters.appId = 1000;
//		gooseCommParameters.dstAddress[0] = 0x01;
//		gooseCommParameters.dstAddress[1] = 0x0c;
//		gooseCommParameters.dstAddress[2] = 0xcd;
//		gooseCommParameters.dstAddress[3] = 0x01;
//		gooseCommParameters.dstAddress[4] = 0x00;
//		gooseCommParameters.dstAddress[5] = 0x01;
//		gooseCommParameters.vlanId = 0;
//		gooseCommParameters.vlanPriority = 4;
//		GoosePublisher publisher = GoosePublisher_create(&gooseCommParameters, "eth0");
//		string sgo = "IEDEVLDEV1/LLN0$GO$gcb01";
//		GoosePublisher_setGoCbRef(publisher, sgo.data());
//		GoosePublisher_setConfRev(publisher, 2);
//		Ecrit("IEDEVLDEV1/DEEV1.EVId", "setVal", string("?"));
// attente wifi
		m_zl->trace("Attente WIFI");
		while(!m_ldev->m_wifi)
		{
			stop();
			this_thread::sleep_for(chrono::seconds(2));
		}
// Boucle
		thread thr(&CZamProcess::ZamProc, this, 1);
		thr.join();
	}
	catch (string exc)
	{
		m_zl->info(fmt::format("process, start, Exception {}", exc));
		if (err == 0) err = 3;
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("process, Start, Exception {}", string(e.what())));
		err = 1;
	}
	catch (...)
	{
		m_zl->info(fmt::format("process, Start, Exception inattendue"));
		err = 2;
	}
	m_zl->info(fmt::format("process, Start, Arrêt du process"));
	return err;
}


int CZamProcess::CreUnit(shared_ptr<CZamLD> ld)
{
	shared_ptr<CZamUnit> zu;
	if (ld->inst == "LDEV1")
	{
		auto zuc(make_shared<CZamLDEV>());
		zu = zuc;
	}
	zu->m_ld = ld;
	zu->m_nom = ld->inst;
	zu->m_zp = m_zp;
	zu->m_zm = m_zm;
	zu->m_zs = m_zs;
	zu->m_zl = m_zl;
	m_du[ld->inst] = zu;
	return 0;
}

void CZamProcess::ZamProc(int i)
{
	int erreur = 0;
	unsigned long long t, t0;
	try
	{
		t0 = m_temps;
		while (true)
		{
			stop();
// Delai
		this_thread::sleep_for(chrono::seconds(3));
//			t = m_temps;
//			if (t == t0)
//			{
//				m_zl->info(fmt::format("Process : temps figé ..."));
//				break;
//			}
//			t0 = t;
		}	// while true boucle infinie
	}	// try		
	catch(string exc)
	{
		m_zl->info(fmt::format("Process Proc Exception {}", exc));
	}
	catch(exception e)
	{
		m_zl->info(fmt::format("Process Proc Exception {}", string(e.what())));
	}
	catch(...)
	{
		m_zl->info(fmt::format("Process Proc Exception ..."));
	}
	m_zl->info(fmt::format("sortie de boucle de process"));
} // Proc

int CZamProcess::Arret()
{
	int err = 0;
	m_stop = true;
	this_thread::sleep_for(chrono::seconds(2));
	return err;
}

int CZamProcess::Init()
{
	m_zmp->AddVar("temps", "ull");
	m_zmp->AddVar("can_ConnSt", "int");
	m_zmp->AddVar("int_chaMax", "float");
	m_zmp->AddVar("int_chaMin", "float");
	m_zmp->AddVar("int_ChargeMarche", "int");
	m_zmp->AddVar("int_adresseCS", "ip"); 
	m_zmp->AddVar("int_adresseEV", "ip");
	m_zmp->AddVar("int_adresseSE", "ip");
	m_zmp->AddVar("int_wifi", "int");
	m_zmp->AddVar(m_nomld + "/DEEV1.ConnTypSel.stVal", "int");
	m_zmp->AddVar(m_nomld + "/DEEV1.DptTm.setTm", "ull");
	m_zmp->AddVar(m_nomld + "/DEEV1.EVId.setVal", "string");
	m_zmp->AddVar(m_nomld + "/DEEV1.EVSt.stVal", "int");
	m_zmp->AddVar(m_nomld + "/DEEV1.Soc.mag.f", "float");
	m_zmp->AddVar(m_nomld + "/DEEV1.WChaTgt.setMag.f", "float");
	m_zmp->AddVar(m_nomld + "/DEEV1.WCurrent.mag.f", "float");
	m_zmp->AddVar(m_nomld + "/DEEV1.WhTgt.setMag.f", "float");
	m_zmp->AddVar(m_nomld + "/DEEV1.EMAId.setVal", "string");
	m_zmp->AddVar(m_nomld + "/DEEV1.AlmMsg.setVal", "string");
	m_zmp->AddVar("int_PID", "int");
	m_zmp->AddVar("int_Wmax", "float");
	m_zmp->AddVar(m_nomld + "/DEEV1.StartTm.setTm", "ull");
	m_zmp->AddVar("int_connected", "bool");
	m_zmp->AddVar(m_nomld + "/DSTO1.WhMaxRtg.setMag.f", "float");
	m_zmp->AddVar(m_nomld + "/DSTO1.WhMinRtg.setMag.f", "float");
	return 0;
}

int CZamProcess::Initvehicule()
{
	int err = 0;
//	sem_wait(m_sem);
	m_zmp->Ecrit("int_wifi", 0);
	m_zmp->Ecrit("int_adresseEV", string(""));
	m_zmp->Ecrit("int_adresseCS", string(""));
	m_zmp->Ecrit("int_adresseSE", string(""));
	m_zmp->Ecrit(m_nomld + "/DEEV1.ConnTypSel.stVal", 1);
	m_zmp->Ecrit(m_nomld + "/DEEV1.Soc.mag.f", 40.);
	m_zmp->Ecrit(m_nomld + "/DEEV1.DptTm.setTm", 0);
	m_zmp->Ecrit(m_nomld + "/DEEV1.StartTm.setTm", 0);
	m_zmp->Ecrit("int_connected", 2);
// DEEV1
	Ecrit(m_nomld + "/DEEV1.Soc", "mag.f", 40.);
	Ecrit(m_nomld + "/DEEV1.DptTm", "setTm", 0.);
	Ecrit(m_nomld + "/DEEV1.EVSt", "stVal", 98);
	Ecrit(m_nomld + "/DEEV1.Soc", "mag.f", 40.);
	Ecrit(m_nomld + "/DEEV1.WChaTgt", "setMag.f", 0.);
	Ecrit(m_nomld + "/DEEV1.WCurrent", "mag.f", 0.);

	// FSCH1
	Ecrit(m_nomld + "/FSCH1.EnaReq", "stVal", true);
	Ecrit(m_nomld + "/FSCH1.DsaReq", "stVal", false);
	Ecrit(m_nomld + "/FSCH1.NumEntr", "setVal", 96);
	Ecrit(m_nomld + "/FSCH1.SchdEntr", "stVal", 1);
	Ecrit(m_nomld + "/FSCH1.SchdPrio", "setVal", 1);
	Ecrit(m_nomld + "/FSCH1.SchdIntv", "setVal", 0);
	Ecrit(m_nomld + "/FSCH1.StrTm1", "setCal.occPer", 0);
	Ecrit(m_nomld + "/FSCH1.ValMV", "mag.f", 0.);
	Ecrit(m_nomld + "/FSCH1.StrTm0", "setTm", 0);
	Ecrit(m_nomld + "/DEEV1.DptTm", "setTm", 0);
	Ecrit(m_nomld + "/DEEV1.StartTm", "setTm", 0);
	// FSCH2
	Ecrit(m_nomld + "/FSCH2.EnaReq", "stVal", false);
	Ecrit(m_nomld + "/FSCH2.DsaReq", "stVal", false);
	Ecrit(m_nomld + "/FSCH2.NumEntr", "setVal", 96);
	Ecrit(m_nomld + "/FSCH2.SchdSt", "stVal", 1);
	Ecrit(m_nomld + "/FSCH2.SchdEntr", "stVal", 1);
	Ecrit(m_nomld + "/FSCH2.SchdPrio", "setVal", 2);
	Ecrit(m_nomld + "/FSCH2.SchdIntv", "setVal", 15);
	Ecrit(m_nomld + "/FSCH2.StrTm1", "setCal.occPer", 0);
	Ecrit(m_nomld + "/FSCH2.ValMV", "mag.f", 0.);
	Ecrit(m_nomld + "/FSCH2.StrTm0", "setTm", 0);
	for (int k = 1; k <= 96; k++)
	{
		Ecrit(m_nomld + fmt::format("/FSCH1.ValASG{:02d}", k), "setMag.f", 0.);
		Ecrit(m_nomld + fmt::format("/FSCH2.ValASG{:02d}", k), "setMag.f", 0.);
	}
	// FSCC 
	Ecrit(m_nomld + "/FSCC1.ActSchdRef","stVal", string("LDEV1/FSCH1"));
	Ecrit(m_nomld + "/FSCC1.ValMV", "mag.f", 0);
	return err;
}

string CZamProcess::AdresseIP(const string& interface) 
{
	int erreur = 0;
	int rep;
	string adresse;
	if (fs::exists("adip")) remove("adip");
	string st = "ifconfig " + interface + " >> adip";
	rep = system(st.data());
	if(rep) return string("0.0.0.0");
	ifstream fic("adip");
	string sligne;
	getline(fic, sligne);
	getline(fic, sligne);
	sligne = sligne.substr(13);
	size_t s = sligne.find_first_of('.');
	if (s == string::npos)
	{
		return "0.0.0.0";
	}
	string sadd = sligne.substr(0, s);
	adresse = sadd + ".";
	unsigned long iadd1 = boost::lexical_cast<unsigned long>(sadd) * 256 * 256 * 256;
	sligne = sligne.substr(s + 1);
	s = sligne.find_first_of('.');
	sadd = sligne.substr(0, s);
	adresse += sadd + ".";
	unsigned long iadd2 = boost::lexical_cast<unsigned long>(sadd) * 256 * 256;
	sligne = sligne.substr(s + 1);
	s = sligne.find_first_of('.');
	sadd = sligne.substr(0, s);
	adresse += sadd + ".";
	unsigned int iadd3 = boost::lexical_cast<unsigned int>(sadd) * 256;
	sligne = sligne.substr(s + 1);
	s = sligne.find_first_of(' ');
	sadd = sligne.substr(0, s);
	adresse += sadd;
	return adresse;
}

string CZamProcess::AdresseMac(const string &interface)
{
	int erreur = 0;
	int rep;
	if (fs::exists("adip")) remove("adip");
	string st = "ifconfig " + interface + " >> adip";

	if (rep = system(st.data())) return string("0.0.0.0");
	ifstream fic("adip");
	string sligne;
	getline(fic, sligne);
	getline(fic, sligne);
	sligne = sligne.substr(13);
	size_t s = sligne.find_first_of('.');
	if (s == string::npos)
	{
		return "0.0.0.0";
	}
	st = "xxxxx";
	while (st != "ether")
	{
		getline(fic, sligne);
		st = sligne.substr(8, 5);
	}
	sligne = sligne.substr(14);
	sligne = sligne.substr(0, 17);
	return sligne;
}

void CZamProcess::EcritMp()
{
	try
	{
		xml_document doc;
		auto decl = doc.prepend_child(pugi::node_declaration);
		decl.append_attribute("version") = "1.0";
		decl.append_attribute("encoding") = "iso-8859-1";
		decl.append_attribute("standalone") = "yes";
		auto xmaj = doc.append_child("Variables");
		for(auto it = m_zmp->m_mapvara.begin(); it != m_zmp->m_mapvara.end(); ++it)
		{
			shared_ptr<CZamVarp> v = it->second;
			auto xvar = xmaj.append_child("ZamVarp");
			xvar.append_attribute("nom");
			xvar.attribute("nom").set_value(v->m_nom.data());
			xvar.append_attribute("type");
			xvar.attribute("type").set_value(v->m_type.data());
			xvar.append_attribute("taille");
			xvar.attribute("taille").set_value(v->m_taille);
			xvar.append_attribute("adresse");
			xvar.attribute("adresse").set_value(v->m_position);
		}
		if(!doc.save_file("zamvarp.xml"))
		{
			throw(string("Echec Ecriture dans ZamProcess EcritMp"));
		}
	}
	catch(string exc)
	{
		m_zl->info(fmt::format("Process: Echec ecriture variables dans ZamProcess EcritMp"));
	}
	catch(exception e)
	{
		m_zl->info(fmt::format("{} dans ZamProcess EcritMp", string(e.what())));
	}
	catch(...)
	{
		m_zl->info(fmt::format("Exception inattendue dans ZamProcess"));
	}
}

void CZamProcess::stop()
{
	if (m_signalHandler.gotExitSignal() || m_stop)
	{
		m_stop = true;
		m_zl->trace("kill en vue");
		m_zl->flush();
		system("sudo ./kp");
		kill(m_pidzam, 9);
		kill(m_pidevv, 9);
		return;
	}
}

sem_t *CZamProcess::semaphore(const string &nom)
{
	int semval;
	sem_t *sem = sem_open(nom.data(), O_CREAT | O_RDWR, S_IRWXO, 0); // O_CREAT  O_RDWR
	if (sem != SEM_FAILED)
	{
		sem_getvalue(sem, &semval);
		while (semval < 1)
		{
			sem_post(sem);
			sem_getvalue(sem, &semval);
		}
		while (semval > 1)
		{
			sem_wait(sem);
			sem_getvalue(sem, &semval);
		}
		sem_getvalue(sem, &semval);
	}
	else
	{
		m_zl->info("ouverture semaphore base impossible");
		return nullptr;
	}
	// regule les acces au serveur du VE
	return sem;
}
