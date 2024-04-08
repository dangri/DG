/*

ZamProcess.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include <signal.h>
#include <string>
#include <fmt/format.h>
#include "ZamProcess.hpp"
#include <thread>
#include <chrono>
#include <pugixml.hpp>
#include <ZamLog.h>
#include "ZamSubstation.h"
#include "ZamVL.h"
#include "ZamBay.h"
#include "ZamCondeq.h"
#include "ZamEqf.h"
#include "ZamFunc.h"
#include "ZamTerm.h"
#include "ZamLNode.h"
#include "ZamCondnode.h"
#include "ZamSubnet.h"
#include "ZamConAP.h"
#include "ZamAddress.h"
#include "ZamTypad.h"
#include "ZamModel.hpp"
#include "ZamServeur.hpp"
#include "ZamLDCS.hpp"
#include "ZamLDSE.hpp"
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/permissions.hpp>
#include <boost/lexical_cast.hpp>
#include "ZamCod.hpp"
#include "Zamevse.h"
#include "ZamIed.hpp"

using namespace std;

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
		auto zlog = make_shared<CZamLog>("C++_report_Station", true);	
		m_zl = zlog;
		m_zl->niveau("trace");
		m_zl->trace("Démarrage du Log");
		m_zl->info(fmt::format("PID Process: {}", m_pidevb));
// Semaphores
		m_sem = semaphore("/sem.ZAMSEMAL");
		if (!m_sem) throw string("Pb ouverture semaphore ZAMSEMAL");
		m_semev = semaphore("/sem.ZAMSEMAR");
		if (!m_semev) throw string("Pb ouverture semaphore ZAMSEMAR");
		m_semdg = semaphore("ZAMDG");
		if (!m_semdg) throw string("Pb ouverture semaphore ZAMDG");
		m_zl->trace("Demarrage ZamProcess ...");
		m_signalHandler.setupSignalHandlers();
		auto zmodel(make_shared<CZamModel>(m_nomfichier));
		m_zm = zmodel;
		auto zserver(make_shared<CZamServer>());
		m_zl->trace("Serveur OK");
		m_zs = zserver;
		m_zs->m_zl = m_zl;
		system("./kp");
		err = start();
	}
	catch (exception &e)
	{
		m_zl->info("Exception " + string(e.what()));
	}
	return err;
}

int CZamProcess::start()
{
	m_zl->trace("process start");
	int err = 0;
	try
	{
// Process
		if (LitSCD()) throw string("Pb .scd");
		m_zl->trace("Lecture du fichier .scd");
//Model
		if (m_zm->Demarrage()) 	throw string("Probleme demarrage ZamModel");
		m_zl->trace("Demarrage du model");
		m_zs->m_nomied = m_zm->m_nomied;
//Serveur
		m_zl->trace("Lancement Serveur");
		if (m_zs->Demarrage()) throw string("Erreur lancement serveur");
		m_zl->trace("demarrage du serveur IEC61850");
// Attente serveur OK
		while (!m_zs->m_start)
		{
			this_thread::sleep_for(chrono::seconds(1));
		}
		m_zl->trace("Serveur OK");
//Memoire partagee
		auto zmp(make_shared<CZamMemp>(m_nomvar, m_taille_var));
		m_zmp = zmp;
		m_zl->trace("demarrage memoire partagee");
// Adresses IP
		m_adresseIP_eth0 = AdresseIP("eth0");
		m_zl->debug("adresse IP eth0: " + m_adresseIP_eth0);
		m_adresseMAC_eth0 = AdresseMac("eth0");
		m_adresseIP_wlan0 = AdresseIP("wlan0");
		m_zl->debug("adresse IP wlan0: " + m_adresseIP_wlan0);
		m_adresseMAC_wlan0 = AdresseMac("wlan0");
		if (m_adresseIP_wlan0 == "0.0.0.0")
		{
			m_zl->critical("pas de wifi disponible !!!");
			if (m_adresseIP_eth0 == "0.0.0.0")
			{
				throw string("Pas d'adresse IP disponible");
			}
		}
// Adresse MAC
		m_adresseMAC_eth0 = AdresseMac("eth0");
		m_adresseMAC_wlan0 = AdresseMac("wlan0");

// python ou non
		system("sudo python /home/pi/zamam/ZamRestServer/ZamRestServer.py mode=Station bddrep=/home/pi/zamam&");

// creation unit et equip
		m_ied = m_zm->m_ied;
		m_nomied = m_ied->name;
		for (auto ldi = m_ied->mapLD.begin(); ldi != m_ied->mapLD.end(); ++ldi)
		{
			auto pld = ldi->second;
			CreUnit(pld);
			m_zl->trace("Integration de : " + ldi->first);
			auto zu = m_du[pld->inst];
			for (auto lni = pld->mapln.begin(); lni != pld->mapln.end(); ++lni)
			{
				auto pln = lni->second;
				zu->CreEquip(pln);
				m_zl->trace("Integration de : " + lni->first);
			}
		}
		Init();
		EcritMp();
		InitStation();
		m_zl->trace("Initialisations");
		m_zmp->Ecrit("int_adresseCS", m_adresseIP_wlan0);
		m_zl->trace("Ecriture sur memoire partagee de l'adresse (wlan) de la Station : " + m_adresseIP_wlan0);
// EVSEs
		m_ldcs = static_pointer_cast<CZamLDCS>(m_du["LDCS"]);
		m_ldcs->m_ldcs = m_ldcs;
		m_ldcs->m_zs = m_zs;
		m_ldcs->m_zp = m_zp;
		m_ldcs->m_zmp = m_zmp;
		m_ldcs->m_zl = m_zl;
// Exploration de la liste de subnets
		m_zl->trace("Exploration des EVSE");
		for (auto sb = m_substation->m_map_sbn.begin(); sb != m_substation->m_map_sbn.end(); sb++)
		{
			shared_ptr<CZamSubnet> sbn = sb->second;
			auto conap = sbn->m_conap;
			auto nom = conap->m_iedname;
			auto nomied = conap->m_iedname;
			int id = 1;
			if (nom.substr(0, 5) == "IEDSE")
			{
				auto address = conap->m_address;
				auto typad = address->m_maptyp["IP"];
				auto evse(make_shared<CZamevse>(nom, typad->m_address));
				m_ldcs->m_map_evse[nom] = evse;
				m_ldcs->m_map_evse_ad[evse->m_adresse] = evse;
				m_zl->trace("Intégration de la borne : " + nom + " adresse " + typad->m_address);
			}
		}
// LDCS
		if (m_ldcs->Demarrage()) throw string("Pb lancement de LDCS");
		m_zl->trace("Demarrage de LCDS");
// Serveur de temps
		auto servtemps(make_shared<CZamStemps>());
		servtemps->m_zp = m_zp;
		m_zt = servtemps;
		m_zt->m_zmp = m_zmp;
		if (servtemps->Demarre(0)) throw string("Pb lancement generateur de temps");
		m_zl->trace("Demarrage du serveur de temps");
// Lancements python
		system("sudo python /home/pi/zamam/Zamxmpp/./zamxmpp.py &");
		this_thread::sleep_for(chrono::seconds(1));
		m_zl->trace("Lancement des codes Python");
		thread thr(&CZamProcess::ZamProc, this, 1);
		thr.join();
		thr.detach();
		m_zl->trace("Fin du thread de Process");
	}
	catch (string exc)
	{
		m_zl->critical(fmt::format("Process Start: Exception {}", exc));
		if (err == 0) err = 3;
	}
	catch (exception e)
	{
		m_zl->critical(fmt::format("Process Start: Exception {}", e.what()));
		err = 1;
	}
	catch (...)
	{
		m_zl->critical("Process Start: Exception");
		err = 2;
	}
	return err;
} // start

sem_t* CZamProcess::semaphore(const string& nom)
{
	int semval;
	sem_t* sem = sem_open(nom.data(), O_CREAT | O_RDWR, S_IRWXO, 0); // O_CREAT  O_RDWR
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
		m_zl->info("Ouverture semaphore base impossible");
		return nullptr;
	}
	// regule les acces au serveur du VE
	return sem;
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
		string nom1, nom2, nom3;
// substation
		xml_node xscl = doc.child("SCL");
		xml_node xsub = xscl.child("process").child("substation");
		auto sub(make_shared<CZamSubstation>());
		sub->m_name = xsub.attribute("name").value();
		sub->m_desc = xsub.attribute("desc").value();
		m_substation = sub;
		for (auto ivl = xsub.child("voltageLevel"); ivl; ivl = ivl.next_sibling("voltageLevel"))
		{
			auto vl(make_shared<CZamVL>());
			vl->m_name = ivl.attribute("name").value();
			sub->m_vec_VL.push_back(vl);
			for (auto ibay = ivl.child("bay"); ibay; ibay = ibay.next_sibling("bay"))
			{
				auto bay(make_shared<CZamBay>());
				bay->m_name = ibay.attribute("name").value();
				vl->m_vec_Bay.push_back(bay);
				LitBay(ibay, bay);
			}
		} // voltageLevel
		xml_node xcc = xscl.child("Communication");
		for (auto isubnet = xcc.child("SubNetwork"); isubnet; isubnet = isubnet.next_sibling("SubNetwork"))
		{
			auto subnet(make_shared<CZamSubnet>());
			subnet->m_name = isubnet.attribute("name").value();
			sub->m_map_sbn[subnet->m_name] = subnet;
			for (auto iconap = isubnet.child("ConnectedAP"); iconap; iconap = iconap.next_sibling("ConnectedAP"))
			{
				auto conap(make_shared<CZamConAP>());
				conap->m_iedname = iconap.attribute("iedName").value();
				conap->m_name = iconap.attribute("ApName").value();
				subnet->m_conap = conap;
				auto iadd = iconap.child("Address");
				auto add(make_shared<CZamAddress>());
				conap->m_address = add;
				for (auto ip = iadd.child("P"); ip; ip = ip.next_sibling("P"))
				{
					auto typad(make_shared<CZamTypad>());
					typad->m_address = ip.child_value();
					typad->m_type = ip.attribute("type").value();
					add->m_maptyp[typad->m_type] = typad;
				}
			}
		}
	} // try
	catch (string exc)
	{
		m_zl->critical("LitSCD: Exception: " + exc);
		if (err == 0)
			err = 3;
	}
	catch (exception e)
	{
		m_zl->critical(fmt::format("LitSCD: Exception: {}", e.what()));
		err = 1;
	}
	catch (...)
	{
		m_zl->critical("LitSCD: Exception: ");
		err = 2;
	}
	return err;
}

int CZamProcess::LitBay(pugi::xml_node ibay, shared_ptr<CZamBay> bay)
{
	int err = 0;
	try
	{
		for (auto icondeq = ibay.child("ConductingEquipment"); icondeq; icondeq = icondeq.next_sibling("ConductingEquipment"))
		{
			auto condeq(make_shared<CZamCondeq>());
			condeq->m_name = icondeq.attribute("name").value();
			condeq->m_type = icondeq.attribute("type").value();
			m_map_Condeq[condeq->m_name] = condeq;
			str2 t("Condeq", condeq->m_name);
			bay->m_vec_contenu.push_back(t);
			for (auto ieqf = icondeq.child("eqFunction"); ieqf; ieqf = ieqf.next_sibling("eqFunction"))
			{
				auto eqf(make_shared<CZamEqf>());
				eqf->m_name = ieqf.attribute("name").value();
				eqf->m_desc = icondeq.attribute("desc").value();
				m_map_Eqf[eqf->m_name] = eqf;
				str2 t("Eqf", eqf->m_name);
				bay->m_vec_contenu.push_back(t);
				for (auto iln = ieqf.child("LNode"); iln; iln = iln.next_sibling("LNode"))
				{
					auto ln(make_shared<CZamLNode>());
					ln->m_iedname = iln.attribute("iedName").value();
					ln->m_ldinst = iln.attribute("ldInst").value();
					ln->m_lnclass = iln.attribute("lnClass").value();
					ln->m_lntype = iln.attribute("lnType").value();
					ln->m_lninst = iln.attribute("lnInst").value();
					ln->m_name = ln->m_iedname + ln->m_ldinst + "/" + ln->m_lnclass + ln->m_lninst;
					m_map_LNode[ln->m_name] = ln;
					str2 t("LNode", ln->m_name);
					bay->m_vec_contenu.push_back(t);
				}
				for (auto iterm = ieqf.child("Terminal"); iterm; iterm = iterm.next_sibling("Terminal"))
				{
					auto term(make_shared<CZamTerm>());
					term->m_name = iterm.attribute("name").value();
					term->m_connectivitynode = iterm.attribute("connectivityNode").value();
					term->m_cNodename = iterm.attribute("cNodeName").value();
					m_map_Term[term->m_name] = term;
					str2 t("Term", term->m_name);
					bay->m_vec_contenu.push_back(t);
				}
			} // eqf
			for (auto ifun = icondeq.child("Function"); ifun; ifun = ifun.next_sibling("eqFunction"))
			{
				auto fun(make_shared<CZamFunc>());
				fun->m_name = ifun.attribute("name").value();
				m_map_Func[fun->m_name] = fun;
				str2 t("Func", fun->m_name);
				bay->m_vec_contenu.push_back(t);
				for (auto iln = ifun.child("LNode"); iln; iln = iln.next_sibling("LNode"))
				{
					auto ln(make_shared<CZamLNode>());
					ln->m_iedname = iln.attribute("iedName").value();
					ln->m_ldinst = iln.attribute("ldInst").value();
					ln->m_lnclass = iln.attribute("lnClass").value();
					ln->m_lntype = iln.attribute("lnType").value();
					ln->m_lninst = iln.attribute("lnInst").value();
					ln->m_name = ln->m_iedname + ln->m_ldinst + "/" + ln->m_lnclass + ln->m_lninst;
					m_map_LNode[ln->m_name] = ln;
					str2 t("LNode", ln->m_name);
					bay->m_vec_contenu.push_back(t);
				} // ln
			}	 // function
			for (auto iterm = icondeq.child("Terminal"); iterm; iterm = iterm.next_sibling("Terminal"))
			{
				auto term(make_shared<CZamTerm>());
				term->m_name = iterm.attribute("name").value();
				term->m_connectivitynode = iterm.attribute("connectivityNode").value();
				term->m_cNodename = iterm.attribute("cNodeName").value();
				m_map_Term[term->m_name] = term;
				str2 t("Term", term->m_name);
				bay->m_vec_contenu.push_back(t);
			}
		} // Condeq
		for (auto iln = ibay.child("LNode"); iln; iln = iln.next_sibling("LNode"))
		{
			auto ln(make_shared<CZamLNode>());
			ln->m_iedname = iln.attribute("iedName").value();
			ln->m_ldinst = iln.attribute("ldInst").value();
			ln->m_lnclass = iln.attribute("lnClass").value();
			ln->m_lntype = iln.attribute("lnType").value();
			ln->m_lninst = iln.attribute("lnInst").value();
			ln->m_name = ln->m_iedname + ln->m_ldinst + "/" + ln->m_lnclass + ln->m_lninst;
			m_map_LNode[ln->m_name] = ln;
			str2 t("LNode", ln->m_name);
			bay->m_vec_contenu.push_back(t);
		} // ln
		for (auto icn = ibay.child("ConnectivityNode"); icn; icn = icn.next_sibling("ConnectivityNode"))
		{
			auto cn(make_shared<CZamCondnode>());
			cn->m_name = icn.attribute("name").value();
			cn->m_pathname = icn.attribute("pathName").value();
			m_map_Condnode[cn->m_name] = cn;
			str2 t("Condnode", cn->m_name);
			bay->m_vec_contenu.push_back(t);
		} // conductingequipment
		for (auto xbay = ibay.child("bay"); xbay; xbay = xbay.next_sibling("bay"))
		{
			auto bay(make_shared<CZamBay>());
			bay->m_name = ibay.attribute("name").value();
			bay->m_vec_Bay.push_back(bay);
		}
	} // try
	catch (string exc)
	{
		m_zl->critical("LitBay: Exception: " + exc);
		if (err == 0)
			err = 3;
	}
	catch (exception e)
	{
		m_zl->critical(fmt::format("LitBay: Exception: {}", e.what()));
		err = 1;
	}
	catch (...)
	{
		m_zl->critical("LitBay: Exception: ");
		cout << "Exception inattendue dans LitBay" << endl;
		err = 2;
	}
	return err;
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
			this_thread::sleep_for(chrono::seconds(3));
		}
	}	// try
	catch (SignalException &e)
	{
		m_zl->critical(fmt::format("Process ZamProc: Signal Exception: {}", e.what()));
	}
	catch(string exc)
	{
		m_zl->critical("Process ZamProc: Exception: ");
	}
	catch(exception e)
	{
		m_zl->critical(fmt::format("Process ZamProc: Exception: {}", e.what()));
	}
	catch(...)
	{
		m_zl->critical("Process ZamProc: Exception: ");
	}
	m_zl->info(fmt::format("Arret du programme {}", m_nom));
} // Proc

int CZamProcess::CreUnit(shared_ptr<CZamLD> ld)
{
	shared_ptr<CZamUnit> zu;
	
	if (ld->inst == "LDCS")
	{
		auto zuc(make_shared<CZamLDCS>());
		zu = zuc;
	}
	if (ld->inst == "LDSE1")
	{
		auto zuc(make_shared<CZamLDSE>());
		zu = zuc;
	}
	if (ld->inst == "LDSE2")
	{
		auto zuc(make_shared<CZamLDSE>());
		zu = zuc;
	}
	if (ld->inst == "LDSE3")
	{
		auto zuc(make_shared<CZamLDSE>());
		zu = zuc;
	}
	zu->m_ld = ld;
	zu->m_nom = ld->inst;
	zu->m_zp = m_zp;
	zu->m_zm = m_zm;
	zu->m_zs = m_zs;
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
		m_zl->critical(fmt::format("Process ZamProc: Signal Exception: {}",exc));
		if (err == 0)
			err = 3;
	}
	catch(exception e)
	{
		m_zl->critical(fmt::format("Process Arret: Exception: {}", e.what()));
		err = 1;
	}
	catch(...)
	{
		m_zl->critical("Process ZamProc: Exception");
		err = 2;
	}
	return err;
}

void CZamProcess::Init()
{
	m_zmp->AddVar("temps", "ull");
	m_zmp->AddVar("int_adresseCS", "ip");
	m_zmp->AddVar("mod_MA", "int");
	m_zmp->AddVar("IEDCSDLDCS/MMXU1.TotW.mag.f", "float");
	m_zmp->AddVar("IEDCSDLDCS/XSWI1.Pos.stVal", "int");
	m_zmp->AddVar("int_PID", "int");
	m_zmp->AddVar("int_RQSTConnect", "string");
	m_zmp->AddVar("int_RSPConnect", "string");
	m_zmp->AddVar("int_Connect", "string");
	m_zmp->AddVar("int_ChangeChargeStatus", "string");
	m_zmp->AddVar("int_End", "string");
	m_zmp->AddVar("int_Disconnect", "string");
	m_zmp->AddVar("int_HeureMax", "string");
	m_zmp->AddVars("Vder_module", "double");
	m_zmp->AddVars("Vder_angle", "double");
	m_zmp->AddVars("mdr", "int");
	m_zmp->AddVars("Sder_module", "double");
	m_zmp->AddVars("Sder_angle", "double");
	m_zmp->AddVars("int_sens", "int");
	m_zmp->AddVars("Pder", "double");
	m_zmp->AddVars("bc1", "int");
	m_zmp->AddVars("bc2", "int");
	m_zmp->AddVars("bc3", "int");
}

void CZamProcess::InitStation()
{
	InitCS();
}

void CZamProcess::InitCS()
{
	int err = 0;
	string s = "??????";
	m_zmp->Ecrit("int_RQSTConnect", s);
	m_zmp->Ecrit("int_RSPConnect", s);
	m_zmp->Ecrit("int_Connect", s);
	m_zmp->Ecrit("int_ChangeChargeStatus", s);
	m_zmp->Ecrit("int_End", s);
	m_zmp->Ecrit("int_Disconnect", s);
	m_zmp->Ecrit("Vder_module", 0);
	m_zmp->Ecrit("Vder_angle", 0);
	m_zmp->Ecrit("Sder_module", 0);
	m_zmp->Ecrit("Sder_angle", 0);
	m_zmp->Ecrit("mdr", 1);
	m_zmp->Ecrit("bc1", 0);
	m_zmp->Ecrit("bc2", 0);
	m_zmp->Ecrit("bc3", 0);
}

string CZamProcess::AdresseIP(const string& interface) 
{
	int erreur = 0;
	string adresse;
	if(fs::exists("adip")) remove("adip");
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
	if (fs::exists("adip"))
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
			throw (string("Echec Ecriture dans ZamProcess EcritMp"));
		}
	}
	catch (string exc)
	{
		m_zl->critical(fmt::format("Process EcritMp: Exception: {}", exc));
	}
	catch (exception e)
	{
		m_zl->critical(fmt::format("Process EcritMp: Exception: {}", e.what()));
	}
	catch (...)
	{
		m_zl->critical("Process EcritMp: Exception");
	}
}

string CZamProcess::Sheure()
{
	string rep = "";
	unsigned long long seconde;
	unsigned int minute,heure;
	seconde = m_temps / 1000;
	seconde = seconde % 86400;
	minute = seconde / 60;
	heure = minute / 60;
	minute = minute % 60;
	rep = fmt::format("{:02d}h{:02d}", heure, minute);
	return rep;
}

void CZamProcess::stop()
{
	if (m_signalHandler.gotExitSignal())
	{
		m_stop = true;
		m_zl->trace("kill en vue");
		m_zl->flush();
//		system("sudo ./kp");
		m_pidzam = m_zp->m_zmp->Liti("int_pid");
		kill(m_pidzam, 9);
		kill(m_pidevb, 9);
		return;
	}
}
