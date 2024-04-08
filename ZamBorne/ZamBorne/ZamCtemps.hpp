#pragma once
#include <string>
#include "ZamProcess.hpp"

using namespace std;

class CZamProcess;

class CZamCtemps
{

public:
	CZamCtemps();
	virtual ~CZamCtemps();
	void Ecoute(int i);
	int Demarre();
	shared_ptr<CZamProcess> m_zp;
	bool m_stop = false;
};

