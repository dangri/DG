/*

ZamProcess.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamProcess.hpp"
#include <thread>
#include <fmt/format.h>
#include <pugixml.hpp>
#include "ZamIed.hpp"
#include "ZamModel.hpp"
#include "ZamMemp.hpp"
#include "ZamServeur.hpp"
#include "ZamEVSE.h"
#include "ZamLDEV.hpp"
#include "ZamCtemps.hpp"
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/permissions.hpp>
#include "ZamCod.hpp"
#include <signal.h>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;
using namespace pugi;

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
		auto zlog = make_shared<CZamLog>("C++_report_Borne");
		m_zl = zlog;
		m_zl->niveau("trace");
		m_zl->trace("Démarrage du Log");
// Sémaphores
		m_sem = semaphore("/sem.ZAMSEMAL");
		if (!m_sem) throw string("Pb ouverture semaphore ZAMSEMAL");
		m_semev = semaphore("/sem.ZAMSEMAR");
		if (!m_semev) throw string("Pb ouverture semaphore ZAMSEMAR");
		m_semdg = semaphore("ZAMDG");
		if (!m_semdg) throw string("Pb ouverture semaphore ZAMSEMAR");
		m_signalHandler.setupSignalHandlers();
		auto zmodel(make_shared<CZamModel>(m_nomfichier));
		m_zm = zmodel;
		auto zserver(make_shared<CZamServer>());
		m_zs = zserver;
		m_zs->m_zl = m_zl;
		system("./kp");
		err = start();
	}
	catch (exception &e)
	{
		cout << e.what();
	}
	return err;
}

int CZamProcess::start()
{
	// Lancement du process
	int err = 0;
	try
	{
// SCD
		if (LitSCD()) throw string("Pb .scd");
//Model
		if (m_zm->Demarrage()) 	throw string("Probleme demarrage ZamModel");
		m_zs->m_nomied = m_zm->m_nomied;
//Serveur
		if (m_zs->Demarrage()) throw string("Erreur lancement serveur");
// Attente serveur OK
		while (!m_zs->m_start)
		{
			this_thread::sleep_for(chrono::seconds(1));
		}
//Memoire partagee
		auto zmp(make_shared<CZamMemp>(m_nomvar, m_taille_var));
		m_zmp = zmp;
// Adresses IP
		m_adresseIP_eth0 = AdresseIP("eth0");
		m_adresseMAC_eth0 = AdresseMac("eth0");
		m_adresseIP_wlan0 = AdresseIP("wlan0");
		m_adresseMAC_wlan0 = AdresseMac("wlan0");
		if (m_adresseIP_wlan0 == "0.0.0.0")
		{
			m_zl->critical("Pas de wifi !!");
			throw string("Pas d'adresse IP disponible");
		}
		m_adresse = m_adresseIP_wlan0;
		m_zl->info(fmt::format("Adresse IP wlan0: {}", m_adresse));
// Demarrage python
		if (m_adresse == "10.3.141.121") system("sudo python /home/pi/zamam/ZamRestServer/ZamRestServer.py mode=EVSE1&");
		if (m_adresse == "10.3.141.122") system("sudo python /home/pi/zamam/ZamRestServer/ZamRestServer.py mode=EVSE2&");
		if (m_adresse == "10.3.141.123") system("sudo python /home/pi/zamam/ZamRestServer/ZamRestServer.py mode=EVSE3&");

// creation unit et equip
		m_ied = m_zm->m_ied;
		m_nomied = m_ied->name;
		for (auto ldi = m_ied->mapLD.begin(); ldi != m_ied->mapLD.end(); ++ldi)
		{
			auto pld = ldi->second;
			CreUnit(pld);
			auto zu = m_du[pld->inst];
			for (auto lni = pld->mapln.begin(); lni != pld->mapln.end(); ++lni)
			{
				auto pln = lni->second;
				zu->CreEquip(pln);
			}
		}
// Initialisations
// Interface IHM
		Init();
		EcritMp();
		Initborne();
// Mise en marche Borne
// EVSEs
		m_evse = static_pointer_cast<CZamEVSE>(m_du["LDEVSE"]);
		m_evse->m_evse = m_evse;
		m_evse->m_adresse = m_adresse;
		m_evse->m_adresse_cs = m_adresse_cs;
		m_evse->m_zp = m_zp;
		m_evse->m_zl = m_zl;
		if (m_evse->Demarrage()) throw string("Pb demarrage de LDEVSE");
		m_zl->trace("Demarrage EVSE");
		m_ldev = static_pointer_cast<CZamLDEV>(m_du["LDEV"]);
		m_ldev->m_zs = m_zs;
		m_ldev->m_zp = m_zp;
		m_ldev->m_zl = m_zl;
		m_evse->m_ldev = m_ldev;
		this_thread::sleep_for(chrono::seconds(2));
// Client de temps
		auto ctemps(make_shared<CZamCtemps>());
		ctemps->m_zp = m_zp;
		m_zt = ctemps;
		if (ctemps->Demarre()) throw string("Pb lancement recepteur de temps");
		m_zl->trace("Demarrage du client de temps");
		system("sudo python /home/pi/zamam/ZamCab/moncable.py &");
		m_zl->trace("Demarrage des codes Python");
		this_thread::sleep_for(chrono::seconds(1));
		thread thr(&CZamProcess::ZamProc, this, 1);
		thr.join();
	}
	catch (string exc)
	{
		m_zl->critical(fmt::format("Process Start Exception: {}", exc));
		if (err == 0) err = 3;
	}
	catch (exception e)
	{
		m_zl->critical(fmt::format("Process Start Exception: {}", string(e.what())));
		err = 1;
	}
	catch (...)
	{
		m_zl->critical("Process, Start, Exception inattendue");
		err = 2;
	}
	return err;
} // start

sem_t *CZamProcess::semaphore(const string &nom)
{
	int semval;
	sem_t *sem = sem_open(nom.data(), O_CREAT | O_RDWR, S_IRWXO, 0); // O_CREAT  O_RDWR
	if (sem != SEM_FAILED)
	{
		sem_getvalue(sem, &semval);
		//	cout << "valeur du semaphore: " << semval << endl;
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
		cout << "ouverture semaphore base impossible" << endl;
		return nullptr;
	}
	// regule les acces au serveur du VE
	return sem;
}

void gooseListener(GooseSubscriber subscriber, void *parameter)
{
	CZamProcess *pp = static_cast<CZamProcess *>(parameter);
	pp->EcouteGoose(subscriber);
}

void CZamProcess::EcouteGoose(GooseSubscriber subscriber) 
{
//	cout << "GOOSE event" << endl;
//	cout << "  stNum: " << GooseSubscriber_getStNum(subscriber) << "sqNum: " << GooseSubscriber_getSqNum(subscriber) << endl;
//	printf("  timeToLive: %u\n", GooseSubscriber_getTimeAllowedToLive(subscriber));
//
//	uint64_t timestamp = GooseSubscriber_getTimestamp(subscriber);
//
//	printf("  timestamp: %u.%u\n", (uint32_t)(timestamp / 1000), (uint32_t)(timestamp % 1000));
//
//	MmsValue *values = GooseSubscriber_getDataSetValues(subscriber);
//
//	char buffer[1024];
//
//	MmsValue_printToBuffer(values, buffer, 1024);
//
//	printf("%s\n", buffer);
}

void CZamProcess::ZamProc(int i)
{
	try
	{
		while (true)
		{
			stop();
// Delai
			this_thread::sleep_for(chrono::seconds(2));
		}
	}	// try
	catch (SignalException &e)
	{
		m_zl->critical(fmt::format("Process Proc Signal Exception: {}", string(e.what())));
	}
	catch(string exc)
	{
		m_zl->critical(fmt::format("Process Proc Exception: {}", exc));
	}
	catch(exception e)
	{
		m_zl->critical(fmt::format("Process Proc Exception: {}", string(e.what())));
	}
	catch(...)
	{
		m_zl->critical("Process, Proc, Exception inattendue");
	}
	m_zl->critical("Process, Proc, Arret de Process");
} // Proc

int CZamProcess::CreUnit(shared_ptr<CZamLD> ld)
{
	shared_ptr<CZamUnit> zu;
	if (ld->inst == "LDEVSE")
	{
		auto zuc(make_shared<CZamEVSE>());
		zu = zuc;
	}
	if (ld->inst == "LDEV")
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

int CZamProcess::Arret()
{
	int err = 0;
	try
	{
		for (auto iu = m_zp->m_du.begin(); iu != m_zp->m_du.end(); ++iu)
		{
			shared_ptr<CZamUnit> zu = iu->second;
			for (auto ie = zu->m_de.begin(); ie != zu->m_de.end(); ++ie)
			{
				shared_ptr<CZamEquip> ze = ie->second;
				ze->m_stop = true;
			}
			zu->m_stop = true;
		}
		m_zs->m_stop = true;
//		m_stop = true;
	} // try
	catch(string exc)
	{
		m_zl->critical(fmt::format("Process Arret Exception: {}", exc));
		if (err == 0)
			err = 3;
	}
	catch(exception e)
	{
		m_zl->critical(fmt::format("Process Arret Exception: {}", string(e.what())));
		err = 1;
	}
	catch(...)
	{
		m_zl->critical("Process Arret Exception inattendue");
		err = 2;
	}
	return err;
}

void CZamProcess::Init()
{
	m_zmp->AddVar("temps", "ull");
	m_zmp->AddVar("int_adresseCS", "ip");
	m_zmp->AddVar("int_adresseEV", "ip");
	m_zmp->AddVar("int_adresseSE", "ip");
	m_zmp->AddVar("mod_MA", "int");
	m_zmp->AddVar("IEDSELDEVSE/DEAO1.CabRtgAC.stVal", "int");
	m_zmp->AddVar("IEDSELDEVSE/DEAO1.ChaARtg.setVal", "int");
	m_zmp->AddVar("IEDSELDEVSE/DEAO1.ConnSt.stVal", "int");
	m_zmp->AddVar("IEDSELDEVSE/DEAO1.ConnTypDC.setVal", "bool");
	m_zmp->AddVar("IEDSELDEVSE/DEAO1.ConnTypPhs1.setVal", "bool");
	m_zmp->AddVar("IEDSELDEVSE/DEAO1.ConnTypPhs3.setVal", "bool");
	m_zmp->AddVar("IEDSELDEVSE/DEAO1.PlgStAC.stVal", "int");
	m_zmp->AddVar("IEDSELDEVSE/DEAO1.ChaAMaxg.setVal", "int");
	m_zmp->AddVar("IEDSELDEVSE/DESE1.ChaA.mag.f", "float");
	m_zmp->AddVar("IEDSELDEVSE/DESE1.ChaV.mag.f", "float");
	m_zmp->AddVar("int_PID", "int");
	m_zmp->AddVar("IEDSELDEVSE/DEDO1.CabRtgDC.stVal", "int");
	m_zmp->AddVar("IEDSELDEVSE/DEDO1.ChaARtg.setVal", "int");
	m_zmp->AddVar("IEDSELDEVSE/DEDO1.ConnStA.stVal", "int");
	m_zmp->AddVar("IEDSELDEVSE/DEDO1.ConnStC.stVal", "int");
	m_zmp->AddVar("IEDSELDEVSE/DEDO1.PlgStDC.stVal", "int");
	m_zmp->AddVar("int_LDEVSE_VCurrent", "float");
	m_zmp->AddVar("int_LDEVSE_AmpCurrent", "float");
}

void CZamProcess::Initborne()
{
	InitVE();
}

void CZamProcess::InitVE()
{
	m_adresse_ve.clear();
	m_zmp->Ecrit("int_adresseEV", string(""));
//FSCH1
// DEAO
	m_adresse_ve.clear();
	m_zs->Ecrit("IEDSELDEVSE/DEAO1.PlgStAC", "stVal", 98);
	m_zs->Ecrit("IEDSELDEVSE/DEAO1.EVRef", "intAddr", m_adresse_ve);
	m_zs->Ecrit("IEDSELDEVSE/DEDO1.PlgStDC", "stVal", 98);
	m_zs->Ecrit("IEDSELDEVSE/DEDO1.EVRef", "intAddr", m_adresse_ve);
// DEEV
	m_zs->Ecrit("IEDSELDEV/DEEV1.ConnTypSel", "stVal", 98);
	m_zs->Ecrit("IEDSELDEV/DEEV1.DptTm", "setTm", 0);
	m_zs->Ecrit("IEDSELDEV/DEEV1.EVSt", "stVal",98);
	m_zs->Ecrit("IEDSELDEV/DEEV1.Soc", "mag.f", 0);
	m_zs->Ecrit("IEDSELDEV/DEEV1.WChaTgt", "setMag.f", 0);
	m_zs->Ecrit("IEDSELDEV/DEEV1.WCurrent", "mag.f", 0);
	m_zs->Ecrit("IEDSELDEV/DEEV1.WhTgt", "setMag.f", 0);
	string s = "??????";
	m_zs->Ecrit("IEDSELDEV/DEEV1.EMAId", "setVal", s);
	m_zs->Ecrit("IEDSELDEV/DEEV1.EVId", "setVal", s);
}


string CZamProcess::AdresseIP(const string& interface) 
{
	int erreur = 0;
	string adresse;
	remove("adip");
	string st = "ifconfig " + interface + " >> adip";
	system(st.data());
	ifstream fic("adip");
	string sligne;
	getline(fic, sligne);
	getline(fic, sligne);
	sligne = sligne.substr(13);
	size_t s = sligne.find_first_of('.');
	if(s == string::npos)
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
	if(interface == "wlan0")
	{
		unsigned int iadd4 = boost::lexical_cast<unsigned int>(sadd);
		m_ladresse = iadd1 + iadd2 + iadd3 + iadd4;
		for(int i = 0; i < 12; i++) m_env_buf[i] = 0x00;
		m_env_buf[12] = ((m_ladresse >> 24) & 0xFF);
		m_env_buf[13] = ((m_ladresse >> 16) & 0xFF);
		m_env_buf[14] = ((m_ladresse >> 8) & 0xFF);
		m_env_buf[15] = (m_ladresse & 0xFF);
		m_env_buf[16] = ((m_port >> 8) & 0xFF);
		m_env_buf[17] = (m_port & 0xFF);
		m_zp->m_ladresse = m_ladresse;
	}
	return adresse;
}

string CZamProcess::AdresseMac(const string &interface)
{
	int erreur = 0;
	remove("adip");
	string st = "ifconfig " + interface + " >> adip";
	system(st.data());
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
		shared_ptr<CZamVarp> v;
		for(auto it = m_zmp->m_mapvara.begin(); it != m_zmp->m_mapvara.end(); ++it)
		{
			v = it->second;
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
			throw (string("Echec Ecriture"));
		}
	}
	catch (string exc)
	{
		m_zl->info(fmt::format("ZamProcess: Echec ecriture variables", exc));
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("ZamProcess: Echec ecriture variables", string(e.what())));
	}
	catch (...)
	{
		m_zl->info(fmt::format("ZamProcess: Echec ecriture variables"));
	}
}

int CZamProcess::LitSCD()
{
	int err = 0;
	try
	{
		xml_document doc;
		xml_parse_result res = doc.load_file("DER.scd");
		if (!res)
		{
			throw string("Erreur ouverture du fichier .scd");
		}
		xml_node xscl = doc.child("SCL");
		xml_node xcc = xscl.child("Communication");
		for (auto isubnet = xcc.child("SubNetwork"); isubnet; isubnet = isubnet.next_sibling("SubNetwork"))
		{
			for (auto iconap = isubnet.child("ConnectedAP"); iconap; iconap = iconap.next_sibling("ConnectedAP"))
			{
				string iedname = iconap.attribute("iedName").value();
				if (iedname == "IEDCSD")
				{
					auto iadd = iconap.child("Address");
					auto ip = iadd.child("P");
					{
						m_adresse_cs = ip.child_value();
					}
				}
			}
		}
	}
	catch (string exc)
	{
		m_zl->info(fmt::format("Exception ", exc));
		if (err == 0)
			err = 3;
	}
	catch (exception e)
	{
		m_zl->info(fmt::format("Exception dans LitSCD : {}", string(e.what())));
		err = 1;
	}
	catch (...)
	{
		m_zl->info(fmt::format("Exception inattendue dans LitSCD"));
		err = 2;
	}
	return err;
}

void CZamProcess::stop()
{
	if (m_signalHandler.gotExitSignal())
	{
		m_stop = true;
		m_zl->trace("kill en vue");
		m_zl->flush();
		system("sudo ./kp");
		kill(m_pidzam, 9);
		kill(m_pidevb, 9);
		return;
	}
}
