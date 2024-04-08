#include "ZamVal.hpp"

CZamVal::CZamVal()
{
}
CZamVal::~CZamVal()
{
}

CZamdouble::CZamdouble()
//{
//}
//
//CZamdouble::~CZamdouble()
//{
//}
//
//CZambool::CZambool()
//{
//}
//CZambool::~CZambool()
//{
//}
//
//CZamint::CZamint()
//{
//}
//CZamint::~CZamint()
//{
//}
//
//CZamstring::CZamstring()
//{
//}
//CZamstring::~CZamstring()
//{
//}

void CZamVal::setok(IedClientError er)
{
	erreur = er; 
}

bool ok()
{
	IedClientError erreur;
	return (erreur == IED_ERROR_OK) ? true : false;
}

void CZamfloat::set(float f, IedClientError e)
{
	erreur=e;
	val = f;
}

float CZamfloat::get()
{ 
	return val;
}

void CZamdouble::set(double d, IedClientError e)
{
	erreur=e;
	val=d;
}
	
double CZamdouble::get()
{
	return val;
}

void CZambool::set(bool b, IedClientError e)
{
	erreur =e ;
	val = b;
}

bool CZambool::get()
{
	return val;
}

void CZamint::set(int i, IedClientError e)
{
	erreur = e;
	val = i;
}

int CZamint::get()
{
	return val;
}

void CZamstring::set(string s, IedClientError e)
{
	erreur = e;
	val = s;
}

string CZamstring::get()
{ 
	return val;
}


