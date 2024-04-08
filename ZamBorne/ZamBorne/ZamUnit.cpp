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
#include "ZamLPHD.hpp"
#include "ZamLLN0.hpp"
#include "ZamFSCC.hpp"
#include "ZamFSCH.hpp"
#include "ZamDEAO.hpp"
#include "ZamDEDO.hpp"
#include "ZamDESE.hpp"
#include "ZamDEEVP.hpp"
#include "ZamMMXU.hpp"
#include "ZamXSWI.hpp"
#include "ZamDECP.h"
#include "ZamDPMC.h"
#include "ZamDSTO.h"
#include "ZamDPCP.h"
#include "ZamDPMC.h"
#include "ZamDCTE.h"
#include "ZamDWMX.h"

CZamUnit::CZamUnit ()
{
}

CZamUnit::~CZamUnit ()
{
}

int CZamUnit::CreEquip (shared_ptr<CZamLN> ln)
{
	shared_ptr<CZamEquip> ze= nullptr;
	string st= ln->lnType;
	if (st == "DEEVP")
	{
		auto zt (make_shared<CZamDEEVP> ());
		ze= zt;
	}
	if (st == "FSCCZ")
	{
		auto zt (make_shared<CZamFSCC> ());
		ze= zt;
	}

	if (st == "FSCHZ")
	{
		auto zt (make_shared<CZamFSCH> ());
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
	if (st == "DEAOZ")
	{
		auto zt (make_shared<CZamDEAO> ());
		ze= zt;
	}
	if (st == "DEDOZ")
	{
		auto zt (make_shared<CZamDEDO> ());
		ze= zt;
	}
	if (st == "DESEZ")
	{
		auto zt (make_shared<CZamDESE> ());
		ze= zt;
	}
	if (st == "DEEVP")
	{
		auto zt(make_shared<CZamDEEVP>());
		ze = zt;
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

	if (st == "DPCPZ")
	{
		auto zt(make_shared<CZamDPCP>());
		ze = zt;
	}

	if (st == "DSTOZ")
	{
		auto zt(make_shared<CZamDSTO>());
		ze = zt;
	}

	if (ze == nullptr)
	{
		m_zl->info(fmt::format("LN non reconnu: {}", st));
		m_zl->info(fmt::format("LN {} not recognized", st));
		return -1;
	}
	else
	{
		ze->m_ln= ln;
		ln->m_equip= ze;
		ze->m_zu= shared_ptr<CZamUnit> (this);
		ze->m_zp= m_zp;
		ze->m_zm= m_zm;
		ze->m_zs= m_zs;
		ze->m_zl = m_zl;
		ze->m_nom= ln->lnClass + ln->inst;
		m_de[ze->m_nom]= ze;
	}
	//	ze->ze = ze;
	return 0;
}

int CZamUnit::Arret ()
{
	int err= 0;
	m_stop= true;
	return err;
}