/*

ZamSQL.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamSQL.hpp"
#include <boost/thread.hpp>
#include <pthread.h>
#include <boost/lexical_cast.hpp>
#include <string>

static pthread_mutex_t sq_mutex =  PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

int Select(const string& nom_base, const string& val, const string& valwhere, string& resultat)
{
	pthread_mutex_lock(&sq_mutex);
	int err = 0;
	string sreq;
	sqlite3 *pdb;
	int col;
//	if(sqlite3_open_v2(nom_base.c_str(), &pdb, SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX, nullptr) != SQLITE_OK)
	if (sqlite3_open(nom_base.c_str(), &pdb) != SQLITE_OK)
	{
		cout << "Ouverture base SQL impossible" << endl;
		return -1;
	}
	sreq = "SELECT " + val + " FROM nodes WHERE name =\"" + valwhere + "\"";
	sqlite3_stmt *res;
	if (sqlite3_prepare_v2(pdb, sreq.c_str(), sreq.length(), &res, 0) != SQLITE_OK)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl;
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return -2;
	}
	err = sqlite3_step(res);
	if (err != SQLITE_ROW && err != SQLITE_DONE)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl; 
		sqlite3_finalize(res);
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return -3;
	}
	if (err == SQLITE_DONE)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl; 
		sqlite3_finalize(res);
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return -4;
	}
	col = sqlite3_column_count(res);
	for (int i = 0; i < col; i++)
	{
		resultat += (char*) sqlite3_column_text(res, i);
		if (col > 1) // une seule colonne, pas de split
		{
			resultat += ";";
		}
	}
	sqlite3_finalize(res);
	sqlite3_close(pdb);
	pthread_mutex_unlock(&sq_mutex);
	return 0;
}

int SelectI(const string& nom_base, const string& val, const string& valwhere)
{
	pthread_mutex_lock(&sq_mutex);
	int err = 0;
	string sreq;
	sqlite3 *pdb;
	int resultat;
//	if (sqlite3_open_v2(nom_base.c_str(), &pdb, SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX, nullptr) != SQLITE_OK)
	if (sqlite3_open(nom_base.c_str(), &pdb) != SQLITE_OK)
	{
		cout << "Ouverture base SQL impossible" << endl;
		pthread_mutex_unlock(&sq_mutex);
		return -1;
	}
	sreq = "SELECT " + val + " FROM nodes WHERE name =\"" + valwhere + "\"";
	sqlite3_stmt *res;
	if (sqlite3_prepare_v2(pdb, sreq.c_str(), sreq.length(), &res, 0) != SQLITE_OK)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl;
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return -1;
	}
	err = sqlite3_step(res);
	if (err != SQLITE_ROW && err != SQLITE_DONE)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl; 
		sqlite3_finalize(res);
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return -1;
	}
	if (err == SQLITE_DONE)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl; 
		sqlite3_finalize(res);
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return -1;
	}
	resultat = sqlite3_column_int(res, 0);
	sqlite3_finalize(res);
	sqlite3_close(pdb);
	pthread_mutex_unlock(&sq_mutex);
	return resultat;
}

bool SelectB(const string& nom_base, const string& val, const string& valwhere)
{
	pthread_mutex_lock(&sq_mutex);
	int err = 0;
	string sreq;
	sqlite3 *pdb;
	int resultat;
//	if (sqlite3_open_v2(nom_base.c_str(), &pdb, SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX, nullptr) != SQLITE_OK)
	if (sqlite3_open(nom_base.c_str(), &pdb) != SQLITE_OK)
	{
		cout << "Ouverture base SQL impossible" << endl;
		pthread_mutex_unlock(&sq_mutex);
		return false;
	}
	sreq = "SELECT " + val + " FROM nodes WHERE name =\"" + valwhere + "\"";
	sqlite3_stmt *res;
	if (sqlite3_prepare_v2(pdb, sreq.c_str(), sreq.length(), &res, 0) != SQLITE_OK)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl;
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return false;
	}
	err = sqlite3_step(res);
	if (err != SQLITE_ROW && err != SQLITE_DONE)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl; 
		sqlite3_finalize(res);
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return false;
	}
	if (err == SQLITE_DONE)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl; 
		sqlite3_finalize(res);
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return false;
	}
	resultat = sqlite3_column_int(res, 0);
	sqlite3_finalize(res);
	sqlite3_close(pdb);
	if (!resultat)
	{
		pthread_mutex_unlock(&sq_mutex);
		return false;
	}
	pthread_mutex_unlock(&sq_mutex);
	return true;
}

double SelectD(const string& nom_base, const string& val, const string& valwhere)
{
	pthread_mutex_lock(&sq_mutex);
	int err = 0;
	string sreq;
	sqlite3 *pdb;
	double resultat;
//	if (sqlite3_open_v2(nom_base.c_str(), &pdb, SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX, nullptr) != SQLITE_OK)
	if (sqlite3_open(nom_base.c_str(), &pdb) != SQLITE_OK)
	{
		cout << "Ouverture base SQL impossible" << endl;
		pthread_mutex_unlock(&sq_mutex);
		return -1;
	}
	sreq = "SELECT " + val + " FROM nodes WHERE name =\"" + valwhere + "\"";
	sqlite3_stmt *res;
	if (sqlite3_prepare_v2(pdb, sreq.c_str(), sreq.length(), &res, 0) != SQLITE_OK)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl;
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return -1;
	}
	err = sqlite3_step(res);
	if (err != SQLITE_ROW && err != SQLITE_DONE)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl; 
		sqlite3_finalize(res);
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return -1;
	}
	if (err == SQLITE_DONE)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl; 
		sqlite3_finalize(res);
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return -1;
	}
	resultat = sqlite3_column_double(res, 0);
	sqlite3_finalize(res);
	sqlite3_close(pdb);
	pthread_mutex_unlock(&sq_mutex);
	return resultat;
}


string SelectS(const string& nom_base, const string& val, const string& valwhere)
{
	pthread_mutex_lock(&sq_mutex);
	int err = 0;
	string sreq;
	sqlite3 *pdb;
	string resultat;
//	if (sqlite3_open_v2(nom_base.c_str(), &pdb, SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX, nullptr) != SQLITE_OK)
	if (sqlite3_open(nom_base.c_str(), &pdb) != SQLITE_OK)
	{
		cout << "Ouverture base SQL impossible" << endl;
		pthread_mutex_unlock(&sq_mutex);
		return "erreur SQL open";
	}
	sreq = "SELECT " + val + " FROM nodes WHERE name =\"" + valwhere + "\"";
	sqlite3_stmt *res;
	if (sqlite3_prepare_v2(pdb, sreq.c_str(), sreq.length(), &res, 0) != SQLITE_OK)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl;
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return "erreur prepare";
	}
	err = sqlite3_step(res);
	if (err != SQLITE_ROW && err != SQLITE_DONE)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl; 
		sqlite3_finalize(res);
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return "erreur step";
	}
	if (err == SQLITE_DONE)
	{
		cout << string(sqlite3_errmsg(pdb)) << endl; 
		sqlite3_finalize(res);
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return "erreur vide";
	}
	resultat = string((char*)sqlite3_column_text(res, 0));
	sqlite3_finalize(res);
	sqlite3_close(pdb);
	pthread_mutex_unlock(&sq_mutex);
	return resultat;
}


int Update(const string& nom_base, const string& valwhere, const string& valeur, const string& st)
{
	// mettre value actuelle dans valueN-1, timestamp dans timestamN-1, valeur dans value et time dans timestamp
//	pthread_mutex_lock(&sq_mutex);
	int erreur = 0;
	int rc;
	string sreq;
	char *errmsg = NULL;
	char *serr;
	sqlite3 *pdb;
	string resultat;
	string val_actuelle;
	string time_actuel;
//	pthread_mutex_unlock(&sq_mutex);
	erreur  = Select(nom_base, "value,timestamp", valwhere, resultat);
	if (erreur != SQLITE_OK)
	{
		cout << "erreur update " << valwhere << " : " << erreur << endl;
//		pthread_mutex_unlock(&sq_mutex);
		return -1;
	}
	val_actuelle = Split(resultat, 0);
	if (val_actuelle == valeur)
	{
		return 0;
	}
	time_actuel = (Split(resultat, 1));
//	if (!sqlite3_open_v2(nom_base.c_str(), &pdb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, nullptr) == SQLITE_OK)
	if (!sqlite3_open(nom_base.c_str(), &pdb) == SQLITE_OK)
	{
		cout << "Pb SQL" << endl;	
//		pthread_mutex_unlock(&sq_mutex);
		return -3;
	}
	sreq = "UPDATE nodes SET valueNmoins1='" + val_actuelle + "', timestampNmoins1='" + time_actuel + "', value='" + valeur + "', timestamp='" + st + "' WHERE name='" + valwhere + "'";
	pthread_mutex_lock(&sq_mutex);
	rc = sqlite3_exec(pdb, sreq.c_str(), NULL, NULL, &errmsg);
	if (rc != SQLITE_OK) 
	{
		cout << "SQL error: " << errmsg << endl;
		sqlite3_free(errmsg);
		sqlite3_close(pdb);
		pthread_mutex_unlock(&sq_mutex);
		return -4;
	}
	sqlite3_close(pdb);
	pthread_mutex_unlock(&sq_mutex);
	return 0;
}

string Split(string& res, int ind)
{
	string demande(res);
	int ldemande = demande.length();
	int k=0;
	while (ldemande > 0)
	{
		int lparam = demande.find(';');
		if (lparam == string::npos)
		{
			cout << "erreur lecture resutat" << endl;
			return "";
		}
		if (k == ind)
		{
			demande = demande.substr(0, lparam);
			return demande;
		}
		else
		{
			demande = demande.substr(lparam + 1, demande.length() - lparam);
			ldemande = demande.length();
			k++;
		}
	}
	cout << "erreur lecture resutat" << endl;
	return "";
}
