#pragma once

#include <string>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/signals2/mutex.hpp> 
//#include "ZamMem.hpp"

using namespace std;

class CZamVar
{
public:
	CZamVar();
	~CZamVar();
	void setval(bool b);
	void setval(int i);
	void setval(float f);
	void setval(double d);
	void setval(unsigned long long t);
	void setval(string& s);
	bool getvalb();
	int getvali();
	float getvalf();
	double getvald();
	unsigned long long getvalt();
	string getvals();
	string getsval();
public:
//	shared_ptr<CZamMem> m_zm;
//	boost::interprocess::shared_memory_object m_mempar;
	bool m_valbool = false; 	// valeur bool
	int m_valint = 0; 		// valeur int
	float m_valfloat = 0.; 	// valeur float
	double m_valdouble = 0.;
	string m_valstring = ""; // valeur string
	unsigned long long m_valt = 0;
public :
	string m_nied;				// nom IED  ou GLOBAL
	string m_nld;				// nom LD 
	string m_ln;				// nom LN
	string m_do;				// nom DO
	string m_da;				// nom DA
	string m_nom; 				// nom de la variable au complet
	string m_type;				// Type (bool, int, float)
	string m_fc;				// Fonctional Constraint 61850
	string m_type_61850;		// Type 61850
//	string m_nomequip;			// Equipement (Logical Node 61850)
//	string m_instunit;			// Unite fonctionnelle Logical Device 61850)
//	string m_nomprocess;		// nom du Process (IED 61850)
//	bool m_bmes = false; 			// vrai si on privilégie la valeur mesurée
//	unsigned short m_mes_unit = 0; 	// identifiant de la machine de mesure
//	unsigned short m_mes_dev  = 0;	// identifiant device de mesure
//	unsigned short m_mes_var  = 0;	// identifiant de la variable de mesure
//	bool m_mes_ecr = false;			// vrai si ecriture, faux si lecture
	boost::signals2::mutex m_mutvar;
};
