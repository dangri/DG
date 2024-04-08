/*

ZamCod.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamCod.hpp"
#include <cstring>

using namespace std;

bool decodb(const unsigned char *p)
{
	char r[4];
	r[3] = *p;
	r[2] = *(p + 1);
	r[1] = *(p + 2);
	r[0] = *(p + 3);
	void *v = r;
	int i =  *(int *)v;
	if (i) 	return true;
	return false;
}
void encodb(bool b, unsigned char *p)
{
	int i = 0;
	if (b) i = 1;
	*p = i;
	*(p + 1) = i;
	*(p + 2) = i;
	*(p + 3) = i;
}

int decodi(const unsigned char *p)
{
	char r[4];
	r[3] = *p;
	r[2] = *(p + 1);
	r[1] = *(p + 2);
	r[0] = *(p + 3);
	void *v = r;
	return *(int *)v;
}

void encodi(const int i, unsigned char *p)
{
	unsigned char res[4];
	void *pp = (void *)(&i);
	memcpy(res, pp, 4);
	*p = res[3];
	*(p + 1) = res[2];
	*(p + 2) = res[1];
	*(p + 3) = res[0];
}

void encodd(const double f, unsigned char *p)
{
	unsigned char res[8];
	void *pp = (void *)(&f);
	memcpy(res, pp, 8);
	*p = res[0];
	*(p + 1) = res[1];
	*(p + 2) = res[2];
	*(p + 3) = res[3];
	*(p + 4) = res[4];
	*(p + 5) = res[5];
	*(p + 6) = res[6];
	*(p + 7) = res[7];
}

short decods(const unsigned char *p)
{
	char r[2];
	r[0] = *(p + 1);
	r[1] = *p;
	void *v = r;
	return *(short *)v;
}

void encods(const short s, unsigned char *p)
{
	unsigned char res[2];
	void *pp = (void *)(&s);
	memcpy(res, pp, 2);
	*p = res[1];
	*(p + 1) = res[0];
}

double decodd(const unsigned char *p)
{
	unsigned char r[8];
	r[0] = *p;
	r[1] = *(p + 1);
	r[2] = *(p + 2);
	r[3] = *(p + 3);
	r[4] = *(p + 4);
	r[5] = *(p + 5);
	r[6] = *(p + 6);
	r[7] = *(p + 7);
	void *v = r;
	return *(double *)v;
}

void encodu (const unsigned long long u, unsigned char *p)
{
	unsigned char res[8];
	void *pp= (void *)(&u);
	memcpy (res, pp, 8);
	*p= res[0];
	*(p + 1)= res[1];
	*(p + 2)= res[2];
	*(p + 3)= res[3];
	*(p + 4)= res[4];
	*(p + 5)= res[5];
	*(p + 6)= res[6];
	*(p + 7)= res[7];
}

unsigned long long decodu (const unsigned char *p)
{
	unsigned char r[8];
	r[0]= *p;
	r[1]= *(p + 1);
	r[2]= *(p + 2);
	r[3]= *(p + 3);
	r[4]= *(p + 4);
	r[5]= *(p + 5);
	r[6]= *(p + 6);
	r[7]= *(p + 7);
	void *v= r;
	return *(unsigned long long *)v;
}

float decodf(const unsigned char* p)
{
	char r[4];
	r[3] = *p;
	r[2] = *(p + 1);
	r[1] = *(p + 2);
	r[0] = *(p + 3);
	void* v = r;
	return *(float*)v;
}

void encodf(const float f, unsigned char *p)
{
	unsigned char res[4];
	void *pp = (void *)(&f);
	memcpy(res, pp, 4);
	*p = res[3];
	*(p + 1) = res[2];
	*(p + 2) = res[1];
	*(p + 3) = res[0];
}

string UtoPc(const string& ref)
{
	string ret;
	char c;
	bool b = true;
	ret = ref;
	for (int i = 0; i != ref.length(); i++)
	{
		c = ret[i];
		if (c == '_')
		{
			if (b)
			{ 
				ret[i] = '/';
				b = false;
			}
			else ret[i] = '.';
		}
	}
	return ret;
}

string UtoP(const string& ref)
{
	string ret;
	char c;
	ret = ref;
	for (int i = 0; i != ref.length(); i++)
	{
		c = ret[i];
		if (c == '_') ret[i] = '.';
	}
	return ret;
}

string PtoU(const string& ref)
{
	string ret;
	char c;
	ret = ref;
	for (int i = 0; i != ref.length(); i++)
	{
		c = ret[i];
		if (c == '.') ret[i] = '_';
	}
	return ret;
}