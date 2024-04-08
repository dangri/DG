/*

ZamUnit.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamProcess.hpp"
#include <thread>
#include "ZamFSCHP.hpp"
#include "ZamDEEVP.hpp"
#include "ZamMMXU.hpp"
#include "ZamXSWI.hpp"
#include "ZamDCTE.h"
#include "ZamDECP.h"
#include "ZamDSTO.h"
#include "ZamLPHD.hpp"
#include "ZamLLN0.hpp"
#include "ZamDINV.h"
#include "ZamDRTF.h"
#include "ZamDVWC.h"
#include "ZamDPMC.h"
#include "ZamDWMX.h"

CZamUnit::CZamUnit ()
{
}

CZamUnit::~CZamUnit ()
{
}

int CZamUnit::CreEquip(shared_ptr<CZamLN> ln)
{
	shared_ptr<CZamEquip> ze= nullptr;
	string st= ln->lnType;
	if (st == "DEEVP")
	{
		auto zt (make_shared<CZamDEEVP> ());
		ze= zt;
	}

	if (st == "FSCHP")
	{
		auto zt (make_shared<CZamFSCHP> ());
		ze= zt;
	}

	if (st == "LPHDZ")
	{
		auto zt (make_shared<CZamLPHD> ());
		ze= zt;
	}
	if (st == "LLN0Z")
	{
		auto zt (make_shared<CZamLLN0> ());
		ze= zt;
	}

	if (st == "XSWIZ")
	{
		auto zt (make_shared<CZamXSWI> ());
		ze= zt;
	}
	if (st == "MMXUZ")
	{
		auto zt (make_shared<CZamMMXU> ());
		ze= zt;
	}

	if (st == "DCTEZ")
	{
		auto zt(make_shared<CZamDCTE>());
		ze = zt;
	}

	if (st == "DECPZ")
	{
		auto zt(make_shared<CZamDECP>());
		ze = zt;
	}

	if ((st == "DSTOCS") || (st == "DSTOEVSE") || (st == "DSTOEV") )
	{
		auto zt(make_shared<CZamDSTO>());
		ze = zt;
	}
	if (st == "DINVZ")
	{
		auto zt(make_shared<CZamDINV>());
		ze = zt;
	}
	if (st == "DRTFZ")
	{
		auto zt(make_shared<CZamDRTF>());
		ze = zt;
	}
	if (st == "DVWCZ")
	{
		auto zt(make_shared<CZamDVWC>());
		ze = zt;
	}
	if (st == "DPMCZ")
	{
		auto zt(make_shared<CZamDPMC>());
		ze = zt;
	}
	if (st == "DWMXZ")
	{
		auto zt(make_shared<CZamDWMX>());
		ze = zt;
	}
	if (ze == nullptr)
	{
		m_zl->info(fmt::format("Unit: LN inconnu {}", st));
	}
	else
	{
		ze->m_ln= ln;
		ln->m_equip= ze;
		ze->m_zu= shared_ptr<CZamUnit> (this);
		ze->m_zp= m_zp;
		ze->m_zm= m_zm;
		ze->m_zmp = m_zp->m_zmp;
		ze->m_zl = m_zp->m_zl;
		ze->m_zs= m_zs;
		ze->m_nom= ln->lnClass + ln->inst;
		m_de[ze->m_nom]= ze;
	}
	//	ze->ze = ze;
	return 0;
}

int CZamUnit::Arret()
{
	int err= 0;
	m_stop= true;
	return err;
}