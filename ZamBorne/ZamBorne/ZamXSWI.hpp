/*

ZamXSWI.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once

#include "ZamEquip.hpp"

class CZamXSWI : public CZamEquip
{
public:
	CZamXSWI();
	virtual ~CZamXSWI();
	int Demarrage();
	bool Initialisation();
	int Lance();
	void ZamProc(int);
	int Arret();
private:
	int m_Beh_q;
	unsigned long long m_Beh_t;
	unsigned int m_BlkCls_Oper_Check;
	unsigned long long m_BlkCls_Oper_T;
	bool m_BlkCls_Oper_Test;
	unsigned int m_BlkCls_Oper_ctlNum;
	bool m_BlkCls_Oper_ctlVal;
	unsigned long long m_BlkCls_Oper_operTm;
	int m_BlkCls_Oper_origin_orCat;
	string m_BlkCls_Oper_origin_orIdent;
	int m_BlkCls_ctlModel;
	int m_BlkCls_q;
	bool m_BlkCls_stVal;
	unsigned long long m_BlkCls_t;
	unsigned int m_BlkOpn_Oper_Check;
	unsigned long long m_BlkOpn_Oper_T;
	bool m_BlkOpn_Oper_Test;
	unsigned int m_BlkOpn_Oper_ctlNum;
	bool m_BlkOpn_Oper_ctlVal;
	unsigned long long m_BlkOpn_Oper_operTm;
	int m_BlkOpn_Oper_origin_orCat;
	string m_BlkOpn_Oper_origin_orIdent;
	int m_BlkOpn_ctlModel;
	int m_BlkOpn_q;
	bool m_BlkOpn_stVal;
	unsigned long long m_BlkOpn_t;
	int m_Loc_q;
	bool m_Loc_stVal;
	unsigned long long m_Loc_t;
	string m_NamPlt_swRev;
	string m_NamPlt_vendor;
	int m_OpCnt_q;
	int m_OpCnt_stVal;
	unsigned long long m_OpCnt_t;
	unsigned int m_Pos_Oper_Check;
	unsigned long long m_Pos_Oper_T;
	int m_Pos_q;
	unsigned long long m_Pos_t;
	int m_SwTyp_q;
	int m_SwTyp_stVal;
	unsigned long long m_SwTyp_t;
public:
	int m_Beh_stVal;
	int m_Pos_stVal;
	int m_Pos_ctlModel;
};

