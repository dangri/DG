/*

ZamSignal.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamSignal.hpp"
#include <signal.h>
#include <errno.h>

bool SignalHandler::mbGotExitSignal = false;

SignalHandler::SignalHandler()
{
}

SignalHandler::~SignalHandler()
{
}

bool SignalHandler::gotExitSignal()
{
	return mbGotExitSignal;
}

void SignalHandler::setExitSignal(bool _bExitSignal)
{
	mbGotExitSignal = _bExitSignal;
}

void SignalHandler::exitSignalHandler(int _ignored)
{
	mbGotExitSignal = true;
}

void SignalHandler::setupSignalHandlers()
{
	signal((int)SIGINT, SignalHandler::exitSignalHandler);
}

