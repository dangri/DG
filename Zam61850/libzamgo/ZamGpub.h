#pragma once
#include <string>
#include "goose_publisher.h"
#include "goose_receiver.h"
#include "goose_subscriber.h"

using namespace std;

class CZamGpub
{
public:
	CZamGpub();
	~CZamGpub();
	bool Prepare(const string &interface, const string &mac, string refg, string refd, int appid);
	bool Pub(LinkedList);
	void Del();
	int CtoI(char c);
  public:
	GoosePublisher m_pub;
	// interface, adresse mac, ref pub, ref dataset, message
};
