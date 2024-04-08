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