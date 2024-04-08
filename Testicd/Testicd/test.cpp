#include "test.h"
#include "ZamModel.hpp"
#include <iostream>
#include <filesystem>
#include <thread>
#include <boost/lexical_cast.hpp>
#include "pugixml.hpp"
#include <boost/algorithm/string.hpp>
#include "ZamIed.hpp"
#include "ZamDOType.hpp"
#include "ZamCod.hpp"
#include "ZamSDOType.hpp"
#include "ZamDAType.hpp"
#include "ZamEnumType.hpp"
#include "ZamLNType.hpp"
#include "ZamLD.hpp"
#include "ZamLN0.hpp"
#include "ZamDS.hpp"
#include "ZamFCDA.hpp"
#include "ZamRC.hpp"
#include "ZamBDA.hpp"

typedef tuple<string, string> str2;

Ctest::Ctest()
{
}

Ctest::Ctest(const string &fichier)
{
	m_nomfichier = fichier;
}

Ctest::~Ctest()
{
}

int Ctest::Demarrage()
{
	int err = 0;
	try
	{
		Constantes();
		err = LitICD();
	}
	catch (string exc)
	{
		cout << "Exception dans le demarrage du modele : " << exc << endl;
		err = -1;
	}
	catch (std::exception e)
	{
		err = -2;
	}
	catch (...)
	{
		err = -3;
	}
	return err;
}

int Ctest::LitICD()
{
	int err = 0;
	try
	{
		xml_parse_result res = m_doc.load_file(m_nomfichier.c_str());
		if (!res)
		{
			throw string("Erreur ouverture du fichier .icd");
		}
		// origine
		xml_node xscl = m_doc.child("SCL");
		// Lecture des datatypes
		xml_node xdt = xscl.child("DataTypeTemplates");
		// LNTypes
		for (auto xln = xdt.child("LNodeType"); xln; xln = xln.next_sibling("LNodeType"))
		{
			auto ln(make_shared<CZamLNType>());
			ln->id = xln.attribute("id").value();
			ln->lnClass = xln.attribute("lnClass").value();
			maplntype[ln->id] = ln;
			for (auto xdo = xln.child("DO"); xdo; xdo = xdo.next_sibling("DO"))
			{
				auto d(make_shared<CZamDO>());
				d->name = xdo.attribute("name").value();
				d->type = xdo.attribute("type").value();
				ln->mapdo[d->name] = d;
			}
		}
		// DOTypes
		for (auto xdo = xdt.child("DOType"); xdo; xdo = xdo.next_sibling("DOType"))
		{
			auto dot(make_shared<CZamDOType>());
			dot->id = xdo.attribute("id").value();
			dot->cdc = xdo.attribute("cdc").value();
			dot->type = xdo.attribute("type").value();
			mapdotype[dot->id] = dot;
			for (auto xsdo = xdo.child("SDO"); xsdo; xsdo = xsdo.next_sibling("SDO"))
			{
				auto sdo(make_shared<CZamSDOType>());
				sdo->name = xsdo.attribute("name").value();
				sdo->type = xsdo.attribute("type").value();
				dot->mapsdo[sdo->name] = sdo;
			}
			for (auto xda = xdo.child("DA"); xda; xda = xda.next_sibling("DA"))
			{
				auto pda(make_shared<CZamDA>());
				pda->name = xda.attribute("name").value();
				pda->fc = xda.attribute("fc").value();
				pda->type = xda.attribute("type").value();
				pda->bType = xda.attribute("bType").value();
				pda->typval = m_maptyp[pda->bType];
				pda->itype = Valitype(pda->bType);
				string dchg = xda.attribute("dchg").value();
				if (dchg == "true") pda->dchg = 1;
				string dupd = xda.attribute("dupd").value();
				if (dupd == "true") pda->dupd = 4;
				string qchg = xda.attribute("qchg").value();
				if (qchg == "true") pda->qchg = 2;
				dot->mapda[pda->name] = pda;
			}
		}
		// DATypes
		for (auto xda = xdt.child("DAType"); xda; xda = xda.next_sibling("DAType"))
		{
			auto dat(make_shared<CZamDAType>());
			dat->id = xda.attribute("id").value();
			mapdatype[dat->id] = dat;
			for (auto xb = xda.child("BDA"); xb; xb = xb.next_sibling("BDA"))
			{
				auto bda(make_shared<CZamBDA>());
				bda->name = xb.attribute("name").value();
				bda->bType = xb.attribute("bType").value();
				bda->typval = m_maptyp[bda->bType];
				bda->typval = m_maptyp[bda->bType];
				bda->itype = Valitype(bda->bType);
				bda->type = xb.attribute("type").value();
				bda->desc = xb.attribute("desc").value();
				dat->vecbda.push_back(bda);
				dat->mapbda[bda->name] = bda;
			}
		}
		// EnumTypes
		for (auto xbe = xdt.child("EnumType"); xbe; xbe = xbe.next_sibling("EnumType"))
		{
			auto en(make_shared<CZamEnumType>());
			en->id = xbe.attribute("id").value();
			mapenumtype[en->id] = en;
			for (auto xe = xbe.child("EnumVal"); xe; xe = xe.next_sibling("EnumVal"))
			{
				auto enu(make_shared<CZamEnumVal>());
				enu->ord = xe.attribute("ord").value();
				enu->val = xe.child_value();
				en->mapenumval[enu->val] = enu;
			}
		}
// Monter le Modele complet
// parcours des LnTypes
		for (auto lnt = maplntype.begin(); lnt != maplntype.end(); ++lnt)
		{
			auto plnt = lnt->second;
			for (auto ido = plnt->mapdo.begin(); ido != plnt->mapdo.end(); ++ido)
			{
				auto pdo = ido->second;
				auto pdot = mapdotype[pdo->type];
				if (!pdot->mapsdo.empty())
				{
					for (auto isdo = pdot->mapsdo.begin(); isdo != pdot->mapsdo.end(); ++isdo)
					{
						auto psdo = isdo->second;
						auto pdos(make_shared<CZamDO>());
						pdos->name = psdo->name;
						pdos->type = psdo->type;
						pdo->mapdo[pdos->name] = pdos;
						Dad1Do(pdos);
					} // SDO
				}
				else
				{
					Dad1Do(pdo);
				}
			} // DO
		} // LN
// Fin des Datatypes
// Lecture fichier
		xml_node xied, xld, xln, xel, xdo, xdos, xda, xdf, xln0, xds, xfp, xrc, xt, xo, xoo, xv, xrp, xr;
		string sc;
		xied = xscl.child("IED");
		m_nomied = xied.attribute("name").value();
		auto pied(make_shared<CZamIed>());
		m_ied = pied;
		m_ied->name = m_nomied;
		xel = xied.child("AccessPoint").child("Server");
		// server
		for (xld = xel.child("LDevice"); xld; xld = xld.next_sibling("LDevice"))
		{
			auto pld(make_shared<CZamLD>());
			pld->inst = xld.attribute("inst").value();
			m_ied->mapLD[pld->inst] = pld;
// LLN0
			auto pln0(make_shared<CZamLN0>());
			xln0 = xld.child("LN0");
			pln0->lnClass = xln0.attribute("lnClass").value();
			pln0->lnType = xln0.attribute("lnType").value();
			pln0->inst = xln0.attribute("inst").value();
			pld->pln0 = pln0;
			pld->mapln["LLN0"] = pln0;
			for (xds = xln0.child("DataSet"); xds; xds = xds.next_sibling("DataSet"))
			{
				auto pds(make_shared<CZamDS>());
				pds->name = xds.attribute("name").value();
				pds->desc = xds.attribute("desc").value();
				pln0->mapds[pds->name] = pds;
				int i = 0;
				for (xdf = xds.child("FCDA"); xdf; xdf = xdf.next_sibling("FCDA"))
				{
					auto pf(make_shared<CZamFCDA>());
					pf->ldInst = xdf.attribute("ldInst").value();
					pf->lnClass = xdf.attribute("lnClass").value();
					pf->fc = xdf.attribute("fc").value();
					pf->lnInst = xdf.attribute("lnInst").value();
					pf->doName = xdf.attribute("doName").value();
					pf->daName = xdf.attribute("daName").value();
					pds->listfcda.push_back(pf);
					i++;
				} // FCDA
			}
			for (xrc = xln0.child("ReportControl"); xrc; xrc = xrc.next_sibling("ReportControl"))
			{
				auto prc(make_shared<CZamRC>());
				prc->name = xrc.attribute("name").value();
				prc->datSet = xrc.attribute("datSet").value();
				prc->intgPd = xrc.attribute("intgPd").value();
				prc->confRev = xrc.attribute("confRev").value();
				prc->bufTime = xrc.attribute("bufTime").value();
				prc->buffered = xrc.attribute("buffered").value();
				prc->rptID = xrc.attribute("rptID").value();
				prc->indexed = xrc.attribute("indexed").value();
				xt = xrc.child("TrgOps");
				prc->trg_dchg = xt.attribute("dchg").value();
				prc->trg_qchg = xt.attribute("qchg").value();
				prc->trg_dupd = xt.attribute("dupd").value();
				prc->trg_period = xt.attribute("period").value();
				prc->trg_gi = xt.attribute("gi").value();
				xo = xrc.child("OptFields");
				prc->opt_seqNum = xo.attribute("seqNum").value();
				prc->opt_timeStamp = xo.attribute("timeStamp").value();
				prc->opt_reasonCode = xo.attribute("reasonCode").value();
				prc->opt_dataSet = xo.attribute("dataSet").value();
				prc->opt_dataRef = xo.attribute("dataRef").value();
				prc->opt_bufOvfl = xo.attribute("bufOvfl").value();
				prc->opt_entryID = xo.attribute("entryID").value();
				prc->opt_configRef = xo.attribute("configRef").value();
				prc->opt_segmentation = xo.attribute("segmentation").value();
				xoo = xrc.child("RptEnabled");
				prc->Rpt_max = xoo.attribute("max").value();
				prc->numtrg = 0;
				if (prc->trg_dchg == "true")
					prc->numtrg = 1;
				if (prc->trg_qchg == "true")
					prc->numtrg += 2;
				if (prc->trg_dupd == "true")
					prc->numtrg += 4;
				if (prc->trg_period == "true")
					prc->numtrg += 8;
				if (prc->trg_gi == "true")
					prc->numtrg += 8;
				// seqid timestamp reason dataset dataref bufovf rptid confrev
				prc->numopt = 0;
				if (prc->opt_seqNum == "true")
					prc->numopt = 1;
				if (prc->opt_timeStamp == "true")
					prc->numopt += 2;
				if (prc->opt_reasonCode == "true")
					prc->numopt += 4;
				if (prc->opt_dataSet == "true")
					prc->numopt += 8;
				if (prc->opt_bufOvfl == "true")
					prc->numopt += 16;
				if (prc->opt_entryID == "true")
					prc->numopt += 32;
				if (prc->opt_configRef == "true")
					prc->numopt += 64;
				if (prc->opt_segmentation == "true")
					prc->numopt += 128;
				pln0->maprc[prc->name] = prc;
			}
// Les DO de LN0 (depuis LN type)
			auto plnt = maplntype.at(pln0->lnType);
			for (auto ido = plnt->mapdo.begin(); ido != plnt->mapdo.end(); ++ido)
			{
				auto pd = ido->second;
				auto pdo(make_shared<CZamDO>());
				pdo->name = pd->name;
				pdo->type = pd->type;
				pln0->mapdo[pdo->name] = pdo;
// Les DA
				Dad1Do(pdo);
				for (auto ida = pdo->mapda.begin(); ida != pdo->mapda.end(); ++ida)
				{
					auto da = ida->second;
					if (da->mapda.empty())
					{
						auto zv(make_shared<CZamVar>());
						zv->m_nied = m_ied->name;
						zv->m_nld = pld->inst;
						zv->m_ln = pln0->lnClass + pln0->inst;
						zv->m_do = pdo->name;
						zv->m_da = da->name;
						zv->m_nom = zv->m_nied + zv->m_nld + "/" + pln0->lnClass + pln0->inst + "." + pdo->name + "." + da->name;
						zv->m_fc = da->fc;
						zv->m_type_61850 = da->bType;
						zv->m_type = da->itype;
						pln0->m_mapvar[zv->m_nom] = zv;
						m_mapvar[zv->m_nom] = zv;
					}
					for (auto ida1 = da->mapda.begin(); ida1 != da->mapda.end(); ++ida1)
					{
						auto da1 = ida1->second;
						if (da1->mapda.empty())
						{
							auto zv(make_shared<CZamVar>());
							zv->m_nied = m_ied->name;
							zv->m_nld = pld->inst;
							zv->m_ln = pln0->lnClass + pln0->inst;
							zv->m_do = pdo->name;
							zv->m_da = da->name + "." + da1->name;
							zv->m_nom = zv->m_nied + zv->m_nld + "/" + pln0->lnClass + pln0->inst + "." + pdo->name + "." + zv->m_da;
							zv->m_fc = da1->fc;
							zv->m_type_61850 = da1->bType;
							zv->m_type = da1->itype;
							pln0->m_mapvar[zv->m_nom] = zv;
							m_mapvar[zv->m_nom] = zv;
						}
						for (auto ida2 = da1->mapda.begin(); ida2 != da1->mapda.end(); ++ida2)
						{
							auto da2 = ida2->second;
							if (da2->mapda.empty())
							{
								auto zv(make_shared<CZamVar>());
								zv->m_nied = m_ied->name;
								zv->m_nld = pld->inst;
								zv->m_ln = pln0->lnClass + pln0->inst;
								zv->m_do = pdo->name;
								zv->m_da = da->name + "." + da1->name + "." + da2->name;
								zv->m_nom = zv->m_nied + zv->m_nld + "/" + pln0->lnClass + pln0->inst + "." + pdo->name + "." + zv->m_da;
								zv->m_fc = da2->fc;
								zv->m_type_61850 = da2->bType;
								zv->m_type = da2->itype;
								pln0->m_mapvar[zv->m_nom] = zv;
								m_mapvar[zv->m_nom] = zv;
							}
							for (auto ida3 = da2->mapda.begin(); ida3 != da2->mapda.end(); ++ida3)
							{
								auto da3 = ida3->second;
								auto zv(make_shared<CZamVar>());
								zv->m_nied = m_ied->name;
								zv->m_nld = pld->inst;
								zv->m_ln = pln0->lnClass + pln0->inst;
								zv->m_do = pdo->name;
								zv->m_da = da->name + "." + da1->name + "." + da2->name + "." + da3->name;
								zv->m_nom = zv->m_nied + zv->m_nld + "/" + pln0->lnClass + pln0->inst + "." + pdo->name + "." + zv->m_da;
								zv->m_fc = da3->fc;
								zv->m_type_61850 = da3->bType;
								zv->m_type = da3->itype;
								pln0->m_mapvar[zv->m_nom] = zv;
								m_mapvar[zv->m_nom] = zv;
							}
						}
					}
				}
			} // DO de LN0
// les DOI de LN0
			for (xdo = xln0.child("DOI"); xdo; xdo = xdo.next_sibling("DOI"))
			{
				string doname = xdo.attribute("name").value();
				shared_ptr<CZamDO> pdo = pln0->mapdo[doname];
				if (pdo == nullptr)
					throw string("DOI de nom inconnu");
				auto plnt = maplntype.at(pln0->lnType);
				auto pdot = plnt->mapdo[doname];
				for (xda = xdo.child("DAI"); xda; xda = xda.next_sibling("DAI"))
				{
					string daname = xda.attribute("name").value();
					shared_ptr<CZamDA> pda = pdo->mapda[daname];
					if (pda == nullptr)
						throw string("DAI de nom inconnu " + daname + " dans " + pld->inst + "/LLN0." + doname);
					xv = xda.child("Val");
					pda->valinit = xv.child_value();
					if (pda->bType == "Enum")
					{
						shared_ptr<CZamEnumType> pet = mapenumtype[pda->type];
						shared_ptr<CZamEnumVal> pev = pet->mapenumval[pda->valinit];
						if (pev == nullptr)
							cout << "LD " << pld->inst << "_LLN0 DOI " << doname << " DA " << daname << " Enum " << pda->valinit << " absent de " << pda->type << endl;
						else
							pda->valinit = pev->ord;
					}
				} // DAI
			}	 // DOI

 // LES LN
			for (xln = xld.child("LN"); xln; xln = xln.next_sibling("LN"))
			{
				auto pln(make_shared<CZamLN>());
				pln->lnClass = xln.attribute("lnClass").value();
				pln->lnType = xln.attribute("lnType").value();
				pln->inst = xln.attribute("inst").value();
				pln->prefix = xln.attribute("prefix").value();
				pld->mapln[pln->lnClass + pln->inst] = pln;
				// les DO (depuis LnType)
				auto plnt = maplntype[pln->lnType];
				if (!plnt)
				{
					cout << "Manque " << pln->lnType << endl;
					continue;
				}
				//	auto plnt = maplntype.at(pln->lnType);
				for (auto ido = plnt->mapdo.begin(); ido != plnt->mapdo.end(); ++ido)
				{
					auto pd = ido->second;
					auto pdo(make_shared<CZamDO>());
					pdo->name = pd->name;
					pdo->type = pd->type;
					pln->mapdo[pdo->name] = pdo;
					auto pdot = mapdotype[pd->type];
					if (!pdot->mapsdo.empty())
					{
						for (auto isdo = pdot->mapsdo.begin(); isdo != pdot->mapsdo.end(); ++isdo)
						{
							auto sdo = isdo->second;
							auto pdos(make_shared<CZamDO>());
							pdos->name = sdo->name;
							pdos->type = sdo->type;
							pdo->mapdo[pdos->name] = pdos;
							Dad1Do(pdos);
							for (auto ida = pdos->mapda.begin(); ida != pdos->mapda.end(); ++ida)
							{
								auto da = ida->second;
								if (da->mapda.empty())
								{
									auto zv(make_shared<CZamVar>());
									zv->m_nied = m_ied->name;
									zv->m_nld = pld->inst;
									zv->m_ln = pln->lnClass + pln->inst;
									zv->m_do = pdo->name;
									zv->m_da = da->name;
									zv->m_nom = zv->m_nied + zv->m_nld + "/" + pln->lnClass + pln->inst + "." + pdo->name + "." + pdos->name + "." + zv->m_da;
									zv->m_fc = da->fc;
									zv->m_type_61850 = da->bType;
									zv->m_type = da->itype;
									pln->m_mapvar[zv->m_nom] = zv;
									m_mapvar[zv->m_nom] = zv;
								}
								for (auto ida1 = da->mapda.begin(); ida1 != da->mapda.end(); ++ida1)
								{
									auto da1 = ida1->second;
									if (da1->mapda.empty())
									{
										auto zv(make_shared<CZamVar>());
										zv->m_nied = m_ied->name;
										zv->m_nld = pld->inst;
										zv->m_ln = pln->lnClass + pln->inst;
										zv->m_do = pdo->name;
										zv->m_da = da->name + "." + da1->name;
										zv->m_nom = zv->m_nied + zv->m_nld + "/" + pln->lnClass + pln->inst + "." + pdo->name + "." + pdos->name + "." + zv->m_da;
										zv->m_fc = da1->fc;
										zv->m_type_61850 = da1->bType;
										zv->m_type = da1->itype;
										pln->m_mapvar[zv->m_nom] = zv;
										m_mapvar[zv->m_nom] = zv;
									}
									for (auto ida2 = da1->mapda.begin(); ida2 != da1->mapda.end(); ++ida2)
									{
										auto da2 = ida2->second;
										if (da2->mapda.empty())
										{
											auto zv(make_shared<CZamVar>());
											zv->m_nied = m_ied->name;
											zv->m_nld = pld->inst;
											zv->m_ln = pln0->lnClass + pln0->inst;
											zv->m_do = pdo->name;
											zv->m_da = da->name + "." + da1->name + "." + da2->name;
											zv->m_nom = zv->m_nied + zv->m_nld + "/" + pln->lnClass + pln->inst + "." + pdo->name + "." + pdos->name + "." + zv->m_da;
											zv->m_fc = da2->fc;
											zv->m_type_61850 = da2->bType;
											zv->m_type = da2->itype;
											pln->m_mapvar[zv->m_nom] = zv;
											m_mapvar[zv->m_nom] = zv;
										}
										for (auto ida3 = da2->mapda.begin(); ida3 != da2->mapda.end(); ++ida3)
										{
											auto da3 = ida3->second;
											auto zv(make_shared<CZamVar>());
											zv->m_nied = m_ied->name;
											zv->m_nld = pld->inst;
											zv->m_ln = pln0->lnClass + pln0->inst;
											zv->m_do = pdo->name;
											zv->m_da = da->name + "." + da1->name + "." + da2->name + "." + da3->name;
											zv->m_nom = zv->m_nied + zv->m_nld + "/" + pln->lnClass + pln->inst + "." + pdo->name + "." + pdos->name + "." + zv->m_da;
											zv->m_fc = da3->fc;
											zv->m_type_61850 = da3->bType;
											zv->m_type = da3->itype;
											pln->m_mapvar[zv->m_nom] = zv;
											m_mapvar[zv->m_nom] = zv;
										}
									}
								} // si array
							}
						} //SDO
					}	 // si SDO
					else
					{
						// Les DA
						Dad1Do(pdo);
						for (auto ida = pdo->mapda.begin(); ida != pdo->mapda.end(); ++ida)
						{
							auto da = ida->second;
							if (da->mapda.empty())
							{
								auto zv(make_shared<CZamVar>());
								zv->m_nied = m_ied->name;
								zv->m_nld = pld->inst;
								zv->m_ln = pln->lnClass + pln->inst;
								zv->m_do = pdo->name;
								zv->m_da = da->name;
								zv->m_nom = zv->m_nied + zv->m_nld + "/" + pln->lnClass + pln->inst + "." + pdo->name + "." + da->name;
								zv->m_fc = da->fc;
								zv->m_type_61850 = da->bType;
								zv->m_type = da->itype;
								pln->m_mapvar[zv->m_nom] = zv;
								m_mapvar[zv->m_nom] = zv;
							}
							for (auto ida1 = da->vecda.begin(); ida1 != da->vecda.end(); ++ida1)
							//							for (auto ida1 = da->mapda.begin(); ida1 != da->mapda.end(); ++ida1)
							{
								auto da1 = *ida1;
								if (da1->vecda.empty())
								{
									auto zv(make_shared<CZamVar>());
									zv->m_nied = m_ied->name;
									zv->m_nld = pld->inst;
									zv->m_ln = pln->lnClass + pln->inst;
									zv->m_do = pdo->name;
									zv->m_da = da->name + "." + da1->name;
									zv->m_nom = zv->m_nied + zv->m_nld + "/" + pln->lnClass + pln->inst + "." + pdo->name + "." + zv->m_da;
									zv->m_fc = da1->fc;
									zv->m_type_61850 = da1->bType;
									zv->m_type = da1->itype;
									pln->m_mapvar[zv->m_nom] = zv;
									m_mapvar[zv->m_nom] = zv;
								}
								for (auto ida2 = da1->mapda.begin(); ida2 != da1->mapda.end(); ++ida2)
								{
									auto da2 = ida2->second;
									if (da2->mapda.empty())
									{
										auto zv(make_shared<CZamVar>());
										zv->m_nied = m_ied->name;
										zv->m_nld = pld->inst;
										zv->m_ln = pln->lnClass + pln->inst;
										zv->m_do = pdo->name;
										zv->m_da = da->name + "." + da1->name + "." + da2->name;
										zv->m_nom = zv->m_nied + zv->m_nld + "/" + pln->lnClass + pln->inst + "." + pdo->name + "." + da->name;
										zv->m_fc = da2->fc;
										zv->m_type_61850 = da2->bType;
										zv->m_type = da2->itype;
										if (da2->mapda.empty())
											pln->m_mapvar[zv->m_nom] = zv;
										m_mapvar[zv->m_nom] = zv;
									}
									for (auto ida3 = da2->mapda.begin(); ida3 != da2->mapda.end(); ++ida3)
									{
										auto da3 = ida3->second;
										auto zv(make_shared<CZamVar>());
										zv->m_nied = m_ied->name;
										zv->m_nld = pld->inst;
										zv->m_ln = pln->lnClass + pln->inst;
										zv->m_do = pdo->name;
										zv->m_da = da->name + "." + da1->name + "." + da2->name + "." + da3->name;
										zv->m_nom = zv->m_nied + zv->m_nld + "/" + pln->lnClass + pln->inst + "." + pdo->name + "." + da->name;
										zv->m_fc = da3->fc;
										zv->m_type_61850 = da3->bType;
										zv->m_type = da3->itype;
										pln->m_mapvar[zv->m_nom] = zv;
										m_mapvar[zv->m_nom] = zv;
									}
								}
							} // var
						}	 // fin si pas SDO
					}		  // fin si pas SDO
				}			  // DO
				for (xdo = xln.child("DOI"); xdo; xdo = xdo.next_sibling("DOI"))
				{
					string doname = xdo.attribute("name").value();
					shared_ptr<CZamDO> pdo = pln->mapdo[doname];
					if (pdo == nullptr)
						throw string("DOI de nom inconnu " + doname);
					auto plnt = maplntype.at(pln->lnType);
					auto pdot = plnt->mapdo[doname];
					for (xda = xdo.child("DAI"); xda; xda = xda.next_sibling("DAI"))
					{
						string daname = xda.attribute("name").value();
						shared_ptr<CZamDA> pda = pdo->mapda[daname];
						if (pda == nullptr)
							throw string("DAI de nom inconnu " + daname + " dans " + pld->inst + "/" + pln->lnClass + pln->inst + "." + doname);
						xv = xda.child("Val");
						pda->valinit = xv.child_value();
						if (pda->bType == "Enum")
						{
							shared_ptr<CZamEnumType> pet = mapenumtype[pda->type];
							shared_ptr<CZamEnumVal> pev = pet->mapenumval[pda->valinit];
							if (pev == nullptr)
								cout << "ln " << pld->inst << "_" << pln->lnClass + pln->inst << " DOI " << doname << " DA " << daname << " Enum " << pda->valinit << " absent de " << pda->type << endl;
							else
								pda->valinit = pev->ord;
						} // Enum
					}	 // DAI
				}		  // DOI
			}			  //LN
		}				  // LD
// Le modele est complet
// Construction du fichier modele
		EcritVariables();
	} // try
	catch (string exc)
	{
		cout << "Exception dans lecture icd " << exc << endl;
		err = 3;
	}
	catch (exception e)
	{
		cout << string(e.what()) + " dans lecture icd" << endl;
		err = 1;
	}
	catch (...)
	{
		cout << "Exception inattendue dans lecture icd" << endl;
		err = 2;
	}
	return err;
}

void Ctest::EcritEntete_H()
{
	m_osfich << "/*" << endl;
	m_osfich << endl;
	m_osfich << m_nom << ".h" << endl;
	m_osfich << endl;
	EcritInfoH();
	m_osfich << "*/" << endl;
	m_osfich << endl;
	m_osfich << "#pragma once" << endl;
	m_osfich << "#include \"ZamEquip.hpp\"" << endl;
	m_osfich << endl;
	m_osfich << "class C" << m_nom << " : public CZamEquip" << endl;
	m_osfich << "{" << endl;
	m_osfich << "public:" << endl;
	m_osfich << "\tC" << m_nom << "();" << endl;
	m_osfich << "\t~C" << m_nom << "();" << endl;
	m_osfich << "\tint Demarrage();" << endl;
	m_osfich << "\tbool Initialisation();" << endl;
	m_osfich << "\tint Lance();" << endl;
	m_osfich << "\tvoid ZamProc(int i);" << endl;
	m_osfich << "\tint Arret();" << endl;
	m_osfich << endl;
	m_osfich << "public:" << endl;
}

void Ctest::EcritEntete_CPP()
{
	m_osficc << "/*" << endl;
	m_osficc << endl;
	m_osficc << m_nom << ".cpp" << endl;
	m_osficc << endl;
	EcritInfoC();
	m_osficc << "*/" << endl;
	m_osficc << endl;
	m_osficc << "#include \"" << m_nom << ".h\"" << endl;
	m_osficc << endl;
	m_osficc << "C" << m_nom << "::C" << m_nom << "()" << endl;
	m_osficc << "{" << endl;
	m_osficc << "}" << endl;
	m_osficc << endl;
	m_osficc << "C" << m_nom << "::~C" << m_nom << "()" << endl;
	m_osficc << "{" << endl;
	m_osficc << "}" << endl;
	m_osficc << endl;
	m_osficc << "int C" << m_nom << "::Demarrage()" << endl;
	m_osficc << "{" << endl;
	m_osficc << "\tint err = 0;" << endl;
	m_osficc << "\tm_IED = m_zp->m_nomied;" << endl;
	m_osficc << "\tm_LD = m_zu->m_nom;" << endl;
	m_osficc << "\tm_LN = m_IED + m_LD + \"/\" + m_nom + \".\";" << endl;
	m_osficc << "\tm_sne = m_LD + \"_\" + m_nom + \"_\";" << endl;
	m_osficc << "\terr = Lance();" << endl;
	m_osficc << "\treturn err;" << endl;
	m_osficc << "}" << endl;
	m_osficc << endl;
	m_osficc << "int C" << m_nom << "::Lance()" << endl;
	m_osficc << "{" << endl;
	m_osficc << "\tthread thr = thread(&C" << m_nom <<"::ZamProc, this, 1);" << endl;
	m_osficc << "\tthr.detach();" << endl;
	m_osficc << "\treturn 0;" << endl;
	m_osficc << "}" << endl;
	m_osficc << endl;
	m_osficc << "bool C" << m_nom << "::Initialisation()" << endl;
	m_osficc << "{" << endl;
}

void Ctest::EcritInit()
{
	string styp;
	string sda;

	for (auto iv = m_pln->m_mapvar.begin(); iv != m_pln->m_mapvar.end(); ++iv)
	{
		auto v = iv->second;
		sda = PtoU(v->m_da);
		if ((sda != "q") && (sda != "t"))
		{
			if (v->m_type == "bool") styp = "Litb";
			if (v->m_type == "int")  styp = "Liti";
			if (v->m_type == "unsigned int") styp = "Litui";
			if (v->m_type == "float") styp = "Litf";
			if (v->m_type == "double") styp = "Litf";
			if (v->m_type == "string") styp = "Lits";
			if (v->m_type == "unsigned long long") styp = "Litull";
			int pos = v->m_nom.find_first_of('.');
			string s = v->m_nom.substr(pos + 1);
			m_osficc << "\t" << "m_" << PtoU(s) << " = m_zs->" + styp + "(m_LN + " << "\"" << s << "\");" << endl;
		}
	}
	m_osficc << "\treturn true;" << endl;
	m_osficc << "}" << endl;
	m_osficc << endl;
}

void Ctest::EcritProc()
{
	m_osficc << "void C" << m_nom << "::ZamProc(int i)" << endl;
	m_osficc << "{" << endl;
	m_osficc << "\tint err = 0;" << endl;
	m_osficc << "\tif (Initialisation())" << endl;
	m_osficc << "\t{" << endl;
	m_osficc << "\t\twhile(true)" << endl;
	m_osficc << "\t\t{" << endl;
	m_osficc << "\t\t\tif (m_zp->m_stop) break;" << endl;
	m_osficc << "// Lecture nouvelles valeurs" << endl;
	m_osficc << endl;
	m_osficc << "// Boucle de traitement" << endl;
	m_osficc << endl;
	m_osficc << "// Ecriture changements" << endl;
	m_osficc << endl;
	m_osficc << "// Delai" << endl;
	m_osficc << "\t\t\tsleep(1);" << endl;
	m_osficc << "\t\t} // boucle" << endl;
	m_osficc << "\t} // if Initialisation" << endl;
	m_osficc << "\telse" << endl;
	m_osficc << "\t{" << endl;
	m_osficc << "\t\tcout << \"Init impossible\" << m_nom << endl;" << endl;
	m_osficc << "\t}" << endl;
	m_osficc << "\t cout << m_nom << \"Arret du programme\" << endl;" << endl;
	m_osficc << "}" << endl;
	m_osficc << endl;
}

void Ctest::EcritArret()
{
	m_osficc << "int C" << m_nom << "::Arret()" << endl;
	m_osficc << "{" << endl;
	m_osficc << "\tint err = 0;" << endl;
	m_osficc << endl;
	m_osficc << "\treturn err;" << endl;
	m_osficc << "}" << endl;
}

void Ctest::EcritInfoH()
{
	m_osfich << "Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus" << endl;
	m_osfich << " industriels developpes en C++ standard et en Python par la Societe ZAMIREN (http://zamiren.fr)." << endl;
	m_osfich << "Ces developpements reposent sur les normes IEC et en particulier la norme IEC 61850" << endl;
	m_osfich << "ainsi que sur des standards en matiere de composants C++ ou Python." << endl;
	m_osfich << "La mise en oeuvre de la norme IEC61850 repose sur la bibliotheque \"LIBIEC61850\"" << endl;
	m_osfich << "developpee et geree par MZ Automation GmbH (http://libiec61850.com)." << endl;
	m_osfich << "La liste complete des sources est fournie dans le fichier \"msources_zamiren.txt\" »" << endl;
}

void Ctest::EcritInfoC()
{
	m_osficc << "Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus" << endl;
	m_osficc << " industriels developpes en C++ standard et en Python par la Societe ZAMIREN (http://zamiren.fr)." << endl;
	m_osficc << "Ces developpements reposent sur les normes IEC et en particulier la norme IEC 61850" << endl;
	m_osficc << "ainsi que sur des standards en matiere de composants C++ ou Python." << endl;
	m_osficc << "La mise en oeuvre de la norme IEC61850 repose sur la bibliotheque \"LIBIEC61850\"" << endl;
	m_osficc << "developpee et geree par MZ Automation GmbH (http://libiec61850.com)." << endl;
	m_osficc << "La liste complete des sources est fournie dans le fichier \"sources_zamiren.txt\"" << endl;
}

int Ctest::EcritVariables()
{
	int err = 0;
	try
	{
		string sfich, sficc;
		string styp;
		string sda;
		map<string, string> mln;
		for (auto ild = m_ied->mapLD.begin(); ild != m_ied->mapLD.end(); ++ild)
		{
			auto pld = ild->second;
			for (auto iln = pld->mapln.begin(); iln != pld->mapln.end(); ++iln)
			{
				auto pln = iln->second;
				m_pln = pln;
				m_nom = "Zam" + pln->lnClass;
				string sclasse = "C" + m_nom;
				sfich = "LNs/" + m_nom + ".h";
				sficc = "LNs/" + m_nom + ".cpp";
				m_osfich.open(sfich, ios::out | ios::trunc);
				m_osficc.open(sficc, ios::out | ios::trunc);
				EcritEntete_H();
				for (auto iv = pln->m_mapvar.begin(); iv != pln->m_mapvar.end(); ++iv)
				{
					auto v = iv->second;
					sda = PtoU(v->m_da);
					if ((sda != "q") && (sda != "t"))
					{
						int pos = v->m_nom.find_first_of('.');
						string s = v->m_nom.substr(pos+1);
						m_osfich << "\t" << v->m_type << " m_" << PtoU(s) << ";" << endl;
					}
				}
				m_osfich << "};" << endl;
				m_osfich.flush();
				m_osfich.close();
				EcritEntete_CPP();
				EcritInit();
				EcritProc();
				EcritArret();
				m_osficc.flush();
				m_osficc.close();
			}
		}	 // LD
	}	//try
	catch (string exc)
	{
		cout << "Echec ecriture variables dans ZamModel" << endl;
		if (err == 0)
			err = 3;
	}
	catch (exception e)
	{
		cout << string(e.what()) + " dans ZamModel" << endl;
		err = 1;
	}
	catch (...)
	{
		cout << "Exception inattendue dans ZamModel" << endl;
		err = 2;
	}
	return err;
}

string Ctest::Valitype(const string &btype)
{
	string res = "inconnu";
	if (btype == "Enum")
		res = "int";
	if (btype == "Quality")
		res = "int";
	if (btype == "Timestamp")
		res = "unsigned long long";
	if (btype == "FLOAT32")
		res = "float";
	if (btype == "FLOAT64")
		res = "double";
	if (btype == "VisString255")
		res = "string";
	if (btype == "Unicode255")
		res = "wstring";
	if (btype == "VisString64")
		res = "string";
	if (btype == "VisString65")
		res = "string";
	if (btype == "VisString129")
		res = "string";
	if (btype == "BOOLEAN")
		res = "bool";
	if (btype == "ObjRef")
		res = "string";
	if (btype == "INT16U")
		res = "unsigned int";
	if (btype == "INT8")
		res = "short";
	if (btype == "INT8U")
		res = "unsigned short";
	if (btype == "INT32")
		res = "int";
	if (btype == "INT32U")
		res = "unsigned int";
	if (btype == "Tcmd")
		res = "int";
	if (btype == "INT64")
		res = "long long";
	if (btype == "Struct")
		res = "int";
	if (btype == "Check")
		res = "unsigned int";
	if (btype == "Octet64")
		res = "string";
	if (btype == "Dbpos")
		res = "int";

	if (res == "inconnu")
		cout << "type inconnu " << btype << endl;
	return res;
}

void Ctest::Dad1Do(shared_ptr<CZamDO> ob)
{
	auto dot = mapdotype[ob->type];
	// parcours des DA du DOtype
	for (auto ida = dot->mapda.begin(); ida != dot->mapda.end(); ++ida)
	{
		// un DA du DOtype
		auto tda = ida->second;
		// creation du DA
		auto da(make_shared<CZamDA>());
		// recopie
		da->name = tda->name;
		da->fc = tda->fc;
		da->type = tda->type;
		da->bType = tda->bType;
		da->typval = m_maptyp[da->bType];
		da->itype = tda->itype;
		da->dchg = tda->dchg;
		da->dupd = tda->dupd;
		da->qchg = tda->qchg;
		ob->mapda[da->name] = da;
		if (da->bType == "Struct")
		{
			auto dat = mapdatype[da->type];
			for (auto ibda = dat->vecbda.begin(); ibda != dat->vecbda.end(); ++ibda)
			{
				auto bda = *ibda;
				auto das(make_shared<CZamDA>());
				das->name = bda->name;
				das->fc = tda->fc;
				das->type = bda->type;
				das->bType = bda->bType;
				das->typval = m_maptyp[das->bType];
				das->itype = bda->itype;
				das->dchg = tda->dchg;
				das->dupd = tda->dupd;
				das->qchg = tda->qchg;
				da->mapda[das->name] = das;
				da->vecda.push_back(das);
				if (das->bType == "Struct")
				{
					auto dat1 = mapdatype[das->type];
					for (auto ibda1 = dat1->vecbda.begin(); ibda1 != dat1->vecbda.end(); ++ibda1)
					{
						auto bda1 = *ibda1;
						auto das1(make_shared<CZamDA>());
						das1->name = bda1->name;
						das1->fc = tda->fc;
						das1->type = bda1->type;
						das1->bType = bda1->bType;
						das1->typval = m_maptyp[das1->bType];
						das1->itype = bda1->itype;
						das1->dupd = tda->dupd;
						das1->qchg = tda->qchg;
						das->mapda[das1->name] = das1;
						das->vecda.push_back(das1);
						if (das1->bType == "Struct")
						{
							auto dat2 = mapdatype[das1->type];
							for (auto ibda2 = dat2->vecbda.begin(); ibda2 != dat2->vecbda.end(); ++ibda2)
							{
								auto bda2 = *ibda2;
								auto das2(make_shared<CZamDA>());
								das2->name = bda2->name;
								das2->fc = tda->fc;
								das2->type = bda2->type;
								das2->bType = bda2->bType;
								das2->typval = m_maptyp[das2->bType];
								das2->itype = bda2->itype;
								das2->dchg = tda->dchg;
								das2->dupd = tda->dupd;
								das2->qchg = tda->qchg;
								das1->mapda[das2->name] = das2;
								das1->vecda.push_back(das2);
								if (das2->bType == "Struct")
								{
									auto dat3 = mapdatype[das2->type];
									for (auto ibda3 = dat3->vecbda.begin(); ibda3 != dat3->vecbda.end(); ++ibda3)
									{
										auto bda3 = *ibda3;
										auto das3(make_shared<CZamDA>());
										das3->name = bda3->name;
										das3->fc = tda->fc;
										das3->type = bda3->type;
										das3->bType = bda3->bType;
										das3->typval = m_maptyp[das3->bType];
										das3->itype = bda3->itype;
										das3->dchg = tda->dchg;
										das3->dupd = tda->dupd;
										das3->qchg = tda->qchg;
										das2->mapda[das3->name] = das3;
										das2->vecda.push_back(das3);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void Ctest::Constantes()
{
	m_maptyp.clear();
	m_maptyp["BOOLEAN"] = 0;
	m_maptyp["INT8"] = 1;
	m_maptyp["INT16"] = 2;
	m_maptyp["INT32"] = 3;
	m_maptyp["INT64"] = 4;
	m_maptyp["INT128"] = 5;
	m_maptyp["INT8U"] = 6;
	m_maptyp["INT16U"] = 7;
	m_maptyp["INT24U"] = 8;
	m_maptyp["INT32U"] = 9;
	m_maptyp["FLOAT32"] = 10;
	m_maptyp["FLOAT64"] = 11;
	m_maptyp["Enum"] = 12;
	m_maptyp["Octet64"] = 13;
	m_maptyp["OCTET_STRING_6"] = 14;
	m_maptyp["OCTET_STRING_8"] = 15;
	m_maptyp["Visible_32"] = 16;
	m_maptyp["VisString64"] = 17;
	m_maptyp["VisString65"] = 18;
	m_maptyp["VisString129"] = 19;
	m_maptyp["VisString255"] = 20;
	m_maptyp["ObjRef"] = 20;
	m_maptyp["UNICODE_STRING_255"] = 21;
	m_maptyp["Timestamp"] = 22;
	m_maptyp["Quality"] = 23;
	m_maptyp["Check"] = 24;
	m_maptyp["CODEDENUM"] = 24;
	m_maptyp["GENERIC_BITSTRING"] = 26;
	m_maptyp["Struct"] = 27;
	m_maptyp["ENTRY_TIME"] = 28;
	m_maptyp["PHYCOMADDR"] = 29;
	m_maptyp["CURRENCY"] = 30;
	m_maptyp["OPTFLDS"] = 31;
	m_maptyp["TRGOPS"] = 32;

	m_maptyps.clear();
	m_maptyps[0] = "BOOLEAN";
	m_maptyps[1] = "INT8";
	m_maptyps[2] = "INT16";
	m_maptyps[3] = "INT32";
	m_maptyps[4] = "INT64";
	m_maptyps[5] = "INT128";
	m_maptyps[6] = "INT8U";
	m_maptyps[7] = "INT16U";
	m_maptyps[8] = "INT24U";
	m_maptyps[9] = "INT32U";
	m_maptyps[10] = "FLOAT32";
	m_maptyps[11] = "FLOAT64";
	m_maptyps[12] = "ENUMERATED";
	m_maptyps[13] = "OCTET_STRING_64";
	m_maptyps[14] = "OCTET_STRING_6";
	m_maptyps[15] = "OCTET_STRING_8";
	m_maptyps[16] = "VISIBLE_STRING_32";
	m_maptyps[17] = "VISIBLE_STRING_64";
	m_maptyps[18] = "VISIBLE_STRING_65";
	m_maptyps[19] = "VISIBLE_STRING_129";
	m_maptyps[20] = "VISIBLE_STRING_255";
	m_maptyps[21] = "UNICODE_STRING_255";
	m_maptyps[22] = "TIMESTAMP";
	m_maptyps[23] = "QUALITY";
	m_maptyps[24] = "CHECK";
	m_maptyps[25] = "CODEDENUM";
	m_maptyps[26] = "GENERIC_BITSTRING";
	m_maptyps[27] = "CONSTRUCTED";
	m_maptyps[28] = "ENTRY_TIME";
	m_maptyps[29] = "PHYCOMADDR";
	m_maptyps[30] = "CURRENCY";
	m_maptyps[31] = "OPTFLDS";
	m_maptyps[32] = "TRGOPS";

	m_mapfc.clear();
	m_mapfc["ST"] = 0;
	m_mapfc["MX"] = 1;
	m_mapfc["SP"] = 2;
	m_mapfc["SV"] = 3;
	m_mapfc["CF"] = 4;
	m_mapfc["DC"] = 5;
	m_mapfc["SG"] = 6;
	m_mapfc["SE"] = 7;
	m_mapfc["SR"] = 8;
	m_mapfc["OR"] = 9;
	m_mapfc["BL"] = 10;
	m_mapfc["EX"] = 11;
	m_mapfc["CO"] = 12;
	m_mapfc["US"] = 13;
	m_mapfc["MS"] = 14;
	m_mapfc["RP"] = 15;
	m_mapfc["BR"] = 16;
	m_mapfc["LG"] = 17;
	m_mapfc["GO"] = 18;
	m_mapfc["ALL"] = 99;
	m_mapfc["NONE"] = -1;
}

