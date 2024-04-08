/*

ZamDEREV.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamProcess.hpp"

using namespace std;

int main()
{
	int err = 0;
	auto process(make_shared<CZamProcess>());
	process->m_zp = process;
	process->m_pidevv = getpid();
	process->m_nomvar = "ZAMVAREVE";
	process->m_taille_var = 4000;
//	system("sudo ./kp");
	err = process->Demarrage();
	return err;
}
