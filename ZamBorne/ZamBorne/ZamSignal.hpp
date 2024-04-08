/*

ZamSignal.hpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#pragma once
#include <stdexcept>
using std::runtime_error;

class SignalException : public runtime_error
{
  public:
	SignalException(const std::string &_message)
		: std::runtime_error(_message)
	{
	}
};

class SignalHandler
{
  protected:
	static bool mbGotExitSignal;

  public:
	SignalHandler();
	~SignalHandler();

	static bool gotExitSignal();
	static void setExitSignal(bool _bExitSignal);

	void setupSignalHandlers();
	static void exitSignalHandler(int _ignored);
};

