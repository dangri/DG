/*

ZamClient.cpp

Ce fichier est un des elements constituant un ensemble de logiciels de controle-commande de processus industriels developpes en C++ standard et en Python
 par la Societe ZAMIREN (http://zamiren.fr).
Ces developpements s’appuient sur les normes de l’IEC et en particulier la norme IEC 61850 ainsi que sur des standards en matiere de composants C++ ou Python.
La mise en œuvre de la norme IEC61850 repose sur la bibliotheque "LIBIEC61850" developpee et geree par MZ Automation GmbH (http://libiec61850.com). 
La liste complete des sources est fournie dans le fichier « sources_zamiren.txt »
*/

#include "ZamClient.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <filesystem>
#include <boost/lexical_cast.hpp>

using namespace std;
namespace fs = std::filesystem;

CZamClient::CZamClient()
{
}

CZamClient::~CZamClient()
{
}

CZamClient::CZamClient(const string& nom)
{
	m_nom = nom;
}

CZamClient::CZamClient(const string& nom, const string& adresse)
{
	m_nom = nom;
	m_adresse = adresse;
}

IedConnection CZamClient::getcon()
{
	IedClientError erreur;
	m_con = IedConnection_create();
	IedConnection_connect(m_con, &erreur, m_adresse.data(), m_port);
	if (erreur != IED_ERROR_OK)
	{
		cout << "Erreur de connexion au dispositif\n" << endl;
		IedConnection_destroy(m_con);
		return 0;
	}
	return m_con;
}

IedConnection CZamClient::getcons()
{
	IedClientError erreur;
	m_con = IedConnection_create();
	IedConnection_connect(m_con, &erreur, m_adresse.data(), m_port);
	if (erreur != IED_ERROR_OK)
	{
		IedConnection_destroy(m_con);
		return 0;
	}
	return m_con;
}

void CZamClient::decon()
{
	if (m_con)
	{
		IedConnection_close(m_con);
		IedConnection_destroy(m_con);
	}
}

bool CZamClient::etacon()
{
//	IED_STATE_CLOSED 	
//	IED_STATE_CONNECTING 	
//	IED_STATE_CONNECTED 	
//	IED_STATE_CLOSING 
	if (! m_con) return false;
	IedConnectionState ics = IedConnection_getState(m_con);
	if (ics == IED_STATE_CONNECTED)
		return true;
	return false;
}

string CZamClient::getNom()
{
	IedClientError erreur;
	LinkedList deviceList = IedConnection_getLogicalDeviceList(m_con, &erreur);
	LinkedList device = LinkedList_getNext(deviceList);
	if (device == NULL) return "???";
	string s = string((char*)device->data);
	int ind = s.find("LD");
	s = s.substr(0,ind);
	return s;
}

float CZamClient::getfloat(const string& sref, const string& sfc)
{
	float fval = -1;
	try
	{
		IedClientError error;
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		 fval = IedConnection_readFloatValue(m_con, &error, sref.data(), fc);
		if (error != IED_ERROR_OK) throw string("getfloat");
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans getfloat" << endl;
	}
	return fval;
}

double CZamClient::getdouble(const string& sref, const string& sfc)
{
	double dval = -1;
	try
	{
		IedClientError error;
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		dval = IedConnection_readFloatValue(m_con, &error, sref.data(), fc);
		if (error != IED_ERROR_OK) throw string("getdouble");
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans getdouble" << endl;
	}
	return boost::lexical_cast<double>(dval);
}

double CZamClient::getutct(const string& sref, const string& sfc)
{
	unsigned long long ulval;
	double dval;
	try
	{
		IedClientError error;
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		MmsValue* pmms = IedConnection_readObject(m_con, &error, sref.data(), fc);	
		if (error != IED_ERROR_OK) throw string("getutct");
		if (!pmms) return 0;
		ulval = MmsValue_getUtcTimeInMs(pmms); 
		dval = (double) ulval;
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans getdouble" << endl;
	}
	return dval;
}


bool CZamClient::getbool(const string& sref, const string& sfc)
{
	bool bval = false;
	try
	{
		IedClientError error;
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		bval = IedConnection_readBooleanValue(m_con, &error, sref.data(), fc);
		if (error != IED_ERROR_OK) throw string("getbool");
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans getbool" << endl;
	}
	return bval;
}
	
int CZamClient::getint(const string& sref, const string& sfc)
{
	int ival = -1;
	try
	{
		IedClientError error;
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		ival = IedConnection_readInt32Value(m_con, &error, sref.data(), fc);
		if (error != IED_ERROR_OK) throw string("getint");
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans getint" << endl;
	}
	return ival;
}
	
string CZamClient::getstring(const string& sref, const string& sfc)
{
	IedClientError error;
	string sval = "";
	try
	{
		IedClientError error;
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		sval = IedConnection_readStringValue(m_con, &error, sref.data(), fc);
		if (error != IED_ERROR_OK) throw string("getstring");
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans gestring" << endl;
	}
	return sval;
}

int CZamClient::setbool(const string& sref, const string& sfc, const bool b)
{
	IedClientError error;
	try
	{
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		IedConnection_writeBooleanValue(m_con, &error, sref.data(), fc, b);
		if (error != IED_ERROR_OK) throw string(sref+" " + sfc);
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans setbool" << endl;
		return -1;
	}
	return 0;
}

int CZamClient::setint(const string& sref, const string& sfc, const int i)
{
	IedClientError error;
	try
	{
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		IedConnection_writeInt32Value(m_con, &error, sref.data(), fc, i);
		if (error != IED_ERROR_OK) throw string(sref + " " + sfc);
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans setint" << endl;
		return -1;
	}
	return 0;
}

int CZamClient::setuint(const string& sref, const string& sfc, const int i)
{
	IedClientError error;
	try
	{
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		IedConnection_writeUnsigned32Value(m_con, &error, sref.data(), fc, (unsigned int)i);
		if (error != IED_ERROR_OK) throw string(sref + " " + sfc);
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans setint" << endl;
		return -1;
	}
	return 0;
}


int CZamClient::setfloat(const string& sref, const string& sfc, const float f)
{
	IedClientError error;
	try
	{
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		IedConnection_writeFloatValue(m_con, &error, sref.data(), fc, f);
		if (error != IED_ERROR_OK) throw string(sref + " " + sfc) ;
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans setfloat" << endl;
		return -1;
	}
	return 0;
}

int CZamClient::setdouble(const string& sref, const string& sfc, const double d)
{
	IedClientError error;
	try
	{
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		IedConnection_writeFloatValue(m_con, &error, sref.data(), fc, d);
		if (error != IED_ERROR_OK) throw string(sref + " " + sfc) ;
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans setdouble" << endl;
		return -1;
	}
	return 0;
}

int CZamClient::setutct(const string& sref, const string& sfc, const unsigned long long ull)
{
	IedClientError error;
	try
	{
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		MmsValue* pmms = IedConnection_readObject(m_con, &error, sref.data(), fc);	
		if (pmms == 0) throw string(sref + " " + sfc);
		MmsValue_setUtcTimeMs(pmms, ull);
		if (error != IED_ERROR_OK) throw string(sref + " " + sfc) ;
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans setutc" << endl;
		return -1;
	}
	return 0;
}

int CZamClient::setstring(const string& sref, const string& sfc, const string& s)
{
	IedClientError error;
	try
	{
		FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data());
		IedConnection_writeVisibleStringValue(m_con, &error, sref.data(), fc, (char*)s.data());
		if (error != IED_ERROR_OK) throw string(sref+ " " + sfc);
	}
	catch (string exc)
	{
		cout << "Exception : " << exc << " dans setstring" << endl;
		return -1;
	}
	return 0;
}

static void commandTerminationHandler(void *parameter, ControlObjectClient connection)
{
	CZamClient *ps = static_cast<CZamClient*>(parameter);
	ps->OutTermination(connection);
}

void CZamClient::OutTermination(ControlObjectClient coc)
{
	LastApplError lastApplError = ControlObjectClient_getLastApplError(coc);
	cout << "Confirmation recue" << endl;
	if (lastApplError.error != 0) 
	{
		cout << " LastApplError: " << lastApplError.error << endl;
		cout << "      addCause: " << lastApplError.addCause << endl;
	}
}
IedClientError CZamClient::cdeBool(const string& sref, bool b, bool btest)
{
	try
	{
		IedClientError erreur;
		ControlObjectClient control = ControlObjectClient_create(sref.c_str(), m_con);
		if (!control) throw string("objet control nul");
		ControlModel cm = ControlObjectClient_getControlModel(control);
		MmsValue* ctlVal = MmsValue_newBoolean(b);
		ControlObjectClient_setOrigin(control, NULL, 3);
		ControlObjectClient_setTestMode(control, btest);
		if ((cm == CONTROL_MODEL_DIRECT_NORMAL) || (cm == CONTROL_MODEL_STATUS_ONLY))
		{
			if (!ControlObjectClient_operate(control, ctlVal, 0 /* operate now */))
			{
				OutTermination(control);
				throw string("commande refusee");
			}
			bool stVal = b;
//			cout << "Commande executee ";
//			if (btest)
//				cout << " (test)";
//			cout <<  sref << " " << b << endl;
		}
		if (cm == CONTROL_MODEL_SBO_NORMAL)
		{
			if (!ControlObjectClient_select(control)) throw string("mauvaise selection");
			if (!ControlObjectClient_operate(control, ctlVal, 0 /* operate now */))
			{
				OutTermination(control);
				throw string("commande refusee");
			}
			bool stVal = b;
//			cout << "Commande executee ";
//			if (btest)
//				cout << "(test) ";
//			cout << sref << ".stVal = " << stVal << endl;
		}
		if (cm == CONTROL_MODEL_DIRECT_ENHANCED)
		{
			ControlObjectClient_setCommandTerminationHandler(control, commandTerminationHandler, (void*) this);
			if (!ControlObjectClient_operate(control, ctlVal, 0 /* operate now */))
			{
				OutTermination(control);
				throw string("commande refusee");
			}
			sleep(1000);
			bool stVal = getbool(sref + ".stVal", "ST");
//			cout << "Commande executee ";
//			if (btest) cout << "(test) ";
//			cout << sref << ".stVal = " << stVal << endl;
		}
		if (cm == CONTROL_MODEL_SBO_ENHANCED)
		{
			ControlObjectClient_setCommandTerminationHandler(control, commandTerminationHandler, (void*) this);
			if (!ControlObjectClient_selectWithValue(control, ctlVal))
			{
				OutTermination(control);
				throw string("commande refusee");
			}
			if (!ControlObjectClient_operate(control, ctlVal, 0 /* operate now */))
			{
				OutTermination(control);
				throw string("commande refusee");
			}
			sleep(1000);
			bool stVal = getbool(sref + ".stVal", "ST");
//			cout << "Commande executee ";
//			if (btest)
//				cout << "(test) ";
//			cout << sref << ".stVal = " << stVal << endl;
		}
		MmsValue_delete(ctlVal);
		ControlObjectClient_destroy(control);
	}	//try
	catch(string exc)
	{
		cout << "Exception : " << exc << " dans cmdBool" << endl;
		return IED_ERROR_UNKNOWN;
	}
	return IED_ERROR_OK;
}

IedClientError CZamClient::cdeInt(const string &sref, int i, bool btest)
{
	try
	{
		IedClientError erreur;
		ControlObjectClient control = ControlObjectClient_create(sref.data(), m_con);
		if (!control) throw string("objet control nul");
		ControlModel cm = ControlObjectClient_getControlModel(control);
		MmsValue* ctlVal = MmsValue_newIntegerFromInt32(i);
		ControlObjectClient_setOrigin(control, NULL, 3);
		ControlObjectClient_setTestMode(control, btest);
		if (cm == CONTROL_MODEL_DIRECT_NORMAL)
		{
			if (!ControlObjectClient_operate(control, ctlVal, 0 /* operate now */))
			{
				OutTermination(control);
				throw string("commande refusee");
			}
			int stVal = i;
//			cout << "Commande executee ";
//			if (btest)
//				cout << "(test) ";
//			cout << sref << ".stVal = " << stVal << endl;
		}
		if (cm == CONTROL_MODEL_SBO_NORMAL)
		{
			if (!ControlObjectClient_select(control)) throw string("mauvaise selection");
			if (!ControlObjectClient_operate(control, ctlVal, 0 /* operate now */)) throw string("commande refusee");
			int stVal = i;
//			cout << "Commande executee ";
//			if (btest)
//				cout << "(test) ";
//			cout << sref << ".stVal = " << stVal << endl;
		}
		if (cm == CONTROL_MODEL_DIRECT_ENHANCED)
		{
			ControlObjectClient_setCommandTerminationHandler(control, commandTerminationHandler, (void*) this);
			if (!ControlObjectClient_operate(control, ctlVal, 0 /* operate now */))
			{
				OutTermination(control);
				throw string("commande refusee");
			}
			sleep(1000);
			int stVal = getint(sref + ".stVal", "ST");
//			cout << "Commande executee ";
//			if (btest)
//				cout << "(test) ";
//			cout << sref << ".stVal = " << stVal << endl;
		}
		if (cm == CONTROL_MODEL_SBO_ENHANCED)
		{
			ControlObjectClient_setCommandTerminationHandler(control, commandTerminationHandler, (void*) this);
			if (!ControlObjectClient_selectWithValue(control, ctlVal)) throw string("selection refusee");
			if (!ControlObjectClient_operate(control, ctlVal, 0 /* operate now */))
			{
				OutTermination(control);
				throw string("commande refusee");
			}
			sleep(1000);
			int stVal = getint(sref + ".stVal", "ST");
//			cout << "Commande executee ";
//			if (btest)
//				cout << "(test) ";
//			cout << sref << ".stVal = " << stVal << endl;
		}
		MmsValue_delete(ctlVal);
		ControlObjectClient_destroy(control);
	}	//try
	catch(string exc)
	{
		cout << "Exception : " << exc << " dans cdeInt" << endl;
		return IED_ERROR_UNKNOWN;
	}
	return IED_ERROR_OK;
}

IedClientError CZamClient::cdeFloat(const string& sref, float f, bool btest)
{
	try
	{
		IedClientError erreur;
		ControlObjectClient control = ControlObjectClient_create(sref.data(), m_con);
		if (!control) throw string("objet control nul");
		ControlModel cm = ControlObjectClient_getControlModel(control);
		MmsValue* ctlVal = MmsValue_newFloat(f);
		ControlObjectClient_setOrigin(control, NULL, 3);
		ControlObjectClient_setTestMode(control, btest);
		if (cm == CONTROL_MODEL_DIRECT_NORMAL)
		{
			if (!ControlObjectClient_operate(control, ctlVal, 0 /* operate now */))
			{
				OutTermination(control);
				throw string("commande refusee");
			}
			float stVal = f;
//			cout << "Commande executee ";
//			if (btest)
//				cout << "(test) ";
//			cout << sref << ".stVal = " << stVal << endl;
		}
		if (cm == CONTROL_MODEL_SBO_NORMAL)
		{
			if (!ControlObjectClient_select(control)) throw string("mauvaise selection");
			if (!ControlObjectClient_operate(control, ctlVal, 0 /* operate now */)) throw string("commande refusee");
			float stVal = f;
//			cout << "Commande executee ";
//			if (btest)
//				cout << "(test) ";
//			cout << sref << ".stVal = " << stVal << endl;
		}
		if (cm == CONTROL_MODEL_DIRECT_ENHANCED)
		{
			ControlObjectClient_setCommandTerminationHandler(control, commandTerminationHandler, (void*) this);
			if (!ControlObjectClient_operate(control, ctlVal, 0 /* operate now */))
			{
				OutTermination(control);
				throw string("commande refusee");
			}
			sleep(1000);
			float stVal = getfloat(sref + ".stVal", "ST");
//			cout << "Commande executee " << sref << ".stVal = " << stVal << endl;
		}
		if (cm == CONTROL_MODEL_SBO_ENHANCED)
		{
			ControlObjectClient_setCommandTerminationHandler(control, commandTerminationHandler, (void*) this);
			if (!ControlObjectClient_selectWithValue(control, ctlVal))
			{
				OutTermination(control);
				throw string("commande refusee");
			}
			if (!ControlObjectClient_operate(control, ctlVal, 0 /* operate now */))
			{
				OutTermination(control);
				throw string("commande refusee");
			}
			sleep(1000);
			float stVal = getfloat(sref + ".stVal", "ST");
//			cout << "Commande executee ";
//			if (btest)
//				cout << "(test) ";
//			cout << sref << ".stVal = " << stVal << endl;
		}
		MmsValue_delete(ctlVal);
		ControlObjectClient_destroy(control);
	}	//try
	catch(string exc)
	{
		cout << "Exception : " << exc << " dans cmdFloat" << endl;
		return IED_ERROR_UNKNOWN;
	}
	return IED_ERROR_OK;
}

void CZamClient::getval(MmsValue* pmms, shared_ptr<CZamVarc> val)
{
	if(!pmms) return;
	MmsType mmstype = MmsValue_getType(pmms);
	switch (mmstype)
	{
	case MMS_ARRAY:
		//	val->setval();
		val->m_type = "string";
		val->setval("todo");
	case MMS_STRUCTURE:
		//	val->setval();
		val->m_type = "string";
		val->setval("todo");
		break;
	case MMS_BOOLEAN:
		val->m_type = "bool";
		val->setval(MmsValue_getBoolean(pmms));
		break;
	case MMS_BIT_STRING:
		val->m_type = "int";
		val->setval((unsigned int)MmsValue_getBitStringAsInteger(pmms));
		break;
	case MMS_INTEGER:
		val->m_type = "int";
		val->setval(MmsValue_toInt32(pmms));
		break;
	case MMS_UNSIGNED:
		val->m_type = "int";
		val->setval((int)(MmsValue_toUint32(pmms)));
		break;
	case MMS_FLOAT: 
		val->m_type = "float";
		val->setval(MmsValue_toFloat(pmms));
		val->setval((double) MmsValue_toFloat(pmms));
		break;
	case MMS_OCTET_STRING:
		val->m_type = "int";
		val->setval((unsigned int)(MmsValue_getBitStringAsInteger(pmms)));
		break;
	case MMS_VISIBLE_STRING:
		val->m_type = "string";
		val->setval(string(MmsValue_toString(pmms)));
		break;
	case MMS_GENERALIZED_TIME:	
		//	val->setval(MmsValue_getUtcTimeInMs(pmms));
		val->m_type = "double";
		val->setval((double) MmsValue_toFloat(pmms));
		break;
	case MMS_BINARY_TIME:	
		//	val->setval(MmsValue_getUtcTimeInMs(pmms));
		val->m_type = "double";
		val->setval((double) MmsValue_toFloat(pmms));
		break;
	case MMS_BCD:
		//	spda->m_val = MmsValue_getBitStringAsInteger(mmsval));
		break;
	case MMS_OBJ_ID:	
		//				spda->m_val = MmsValue_getBitStringAsInteger(mmsval));
		break;
	case MMS_STRING:	
		//				spda->m_val = MmsValue_getBitStringAsInteger(mmsval));
		break;
	case MMS_UTC_TIME: 	
		val->setval(MmsValue_getUtcTimeInMs(pmms));
		val->m_type = "unsigned long long";
		val->m_valull /= 1000;
		break;
	case MMS_DATA_ACCESS_ERROR:
		val->m_type = "string";
		val->setval(string("erreur d'acces"));
		break;
	default:
		break;
	}
	MmsValue_deleteConditional(pmms);
}	

void CZamClient::getDataset(vector<shared_ptr<CZamVarc>>& res, const string& sRef)
{
	IedClientError erreur;
	LinkedList lst;
	bool b;
	try
	{
		res.clear();
		lst = IedConnection_getDataSetDirectory(m_con, &erreur, sRef.data(), &b);
		if (erreur != IED_ERROR_OK) throw (string("Connexion perdue"));
		LinkedList fcda = LinkedList_getNext(lst);
		while (fcda != NULL)
		{
			auto val(make_shared<CZamVarc>());
			val->m_nom = (char*)fcda->data;
			int l = val->m_nom.find('[');
			string sfc = val->m_nom.substr(l + 1, 2);
			val->m_fc = sfc;
			val->m_nom = val->m_nom.substr(0, l);
			FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data()); 
			MmsValue* pmms = IedConnection_readObject(m_con, &erreur, val->m_nom.data(), fc);
			getval(pmms, val);
			res.push_back(val);
			fcda = LinkedList_getNext(fcda);
		}	// fcda
	}
	catch (string exc)
	{
		cout << "exception dans getDataset :" << exc << endl; 
		return;
	}

	LinkedList_destroy(lst);
}


void reportCallbackFunction(void* parameter, ClientReport report)
{
	//	reinterpret_cast<CZamClient*>(parameter)->recrcb(report);
	CZamClient *pc = static_cast<CZamClient*>(parameter);
	pc->recrcb(report);
}

int CZamClient::install_report(vector<shared_ptr<CZamVarc>>& res, const string& sRef, const string& ds, bool del)
{
	IedClientError erreur;
	if (!del)
	{
		getDataset(res, ds);
		m_ds = res;
		m_rcb = IedConnection_getRCBValues(m_con, &erreur, sRef.data(), NULL);
		if (erreur != IED_ERROR_OK) return -4;
		//	get_rcb(sRef);
		//	ClientReportControlBlock_setResv(rcb, true);
		//	ClientReportControlBlock_setDataSetReference(rcb, dsr.data()); /* NOTE the "$" instead of "." ! */
		//	ClientReportControlBlock_setGI(rcb, true);
		IedConnection_installReportHandler(m_con, sRef.data(), ClientReportControlBlock_getRptId(m_rcb), reportCallbackFunction, (void*) this);
		ClientReportControlBlock_setRptEna(m_rcb, true);
		IedConnection_setRCBValues(m_con, &erreur, m_rcb, RCB_ELEMENT_RESV | RCB_ELEMENT_DATSET | RCB_ELEMENT_RPT_ENA, true);		
	}
	else
	{
		ClientReportControlBlock_setRptEna(m_rcb, false);
		IedConnection_setRCBValues(m_con, &erreur, m_rcb, RCB_ELEMENT_RPT_ENA, true);
		ClientReportControlBlock_destroy(m_rcb);
	}
	return 0;
}

void CZamClient::recrcb(ClientReport rep)
{
	IedClientError erreur;
	MmsValue* mmsv = ClientReport_getDataSetValues(rep);
	MmsType mmst = MmsValue_getType(mmsv);
	MmsValue* mmsva;
	ReasonForInclusion reason;
	// faire une liste  des valeurs recuperees 
	// a faire correspondre ensuite avec le Dataset
	// les valeurs qui n'ont pas changé sont nulles
	int i = 0;
	for (auto v = m_ds.begin(); v != m_ds.end(); ++v)
	{
		auto pv = *v;
		reason =  ClientReport_getReasonForInclusion(rep, i);
		if (reason != IEC61850_REASON_NOT_INCLUDED)
		{
			string nom = pv->m_nom;
			mmsva = MmsValue_getElement(mmsv, i);
			getval(mmsva, pv);
			pv->ts = ClientReport_getTimestamp(rep);
			pv->m_change = true;
		}
		i++;	
	}
}

ClientReportControlBlock CZamClient::get_rcb(const string& srcb)
{
	IedClientError erreur;
	ClientReportControlBlock crcb;
	ClientReportControlBlock prcb = nullptr;
	crcb = IedConnection_getRCBValues(m_con, &erreur, srcb.data(), prcb);
	if (crcb != NULL)
	{
		getDataset(m_ds, ClientReportControlBlock_getDataSetReference(crcb));
		ClientReportControlBlock_destroy(crcb);
	}
	return crcb;
}

static void comHandler(void *parameter, ControlObjectClient connection)
{
	LastApplError lastApplError = ControlObjectClient_getLastApplError(connection);
	// if lastApplError.error != 0 this indicates a CommandTermination-
	if(lastApplError.error != 0)
	{
		cout << "Received CommandTermination " << endl;
		cout << " LastApplError: " << lastApplError.error << endl;
		cout << "      addCause: " << lastApplError.addCause << endl;
	}
	else
	    cout << "Confirmation de Cde recue" << endl;
}

void CZamClient::InstallConClose()
{
	IedConnection_installConnectionClosedHandler(m_con, closecon, (void*) this);
}

static void closecon(void *parameter, IedConnection con)
{
	CZamClient*pc = static_cast<CZamClient*>(parameter);
	pc->m_conperdu = true;
}

//ControlObjectClient CZamClient::createControl(const string& cRef, IedConnection con)
//{
//	ControlObjectClient self = NULL;
//	/* request control model from server */
//	string reference = cRef + ".ctlModel";
//	IedClientError error;
//	uint32_t ctlModel = IedConnection_readUnsigned32Value(con, &error, reference.data(), IEC61850_FC_CF);
//	if (error != IED_ERROR_OK)
//	{
//		cout << "IED_CLIENT: ControlObjectClient_create: failed to get " << reference  <<  "from server" << endl;
//		return self;
//	}
//	MmsVariableSpecification* ctlVarSpec = IedConnection_getVariableSpecification(con, &error, cRef.data(), IEC61850_FC_CO);
//	if (error != IED_ERROR_OK)
//	{
//		cout << "IED_CLIENT: ControlObjectClient_create: failed to get data directory of control object" <<  endl;
//		return self;
//	}
//	/* check what control elements are available */
//	bool hasOper = false;
//	bool hasTimeActivatedControl = false;
//	bool hasCtlNum = false;
//	bool isAPC = false;
//	MmsVariableSpecification* ctlVal = NULL;
//	MmsVariableSpecification* t = NULL;
//	if (MmsVariableSpecification_getType(ctlVarSpec) == MMS_STRUCTURE)
//	{
//		MmsVariableSpecification* oper = MmsVariableSpecification_getNamedVariableRecursive(ctlVarSpec, "Oper");
//		if (oper)
//		{
//			hasOper = true;
//			ctlVal = MmsVariableSpecification_getNamedVariableRecursive(oper, "ctlVal");
//			if (MmsVariableSpecification_getType(ctlVal) == MMS_STRUCTURE)
//			{
//				isAPC = true;
//			}
//			MmsVariableSpecification* operTm = MmsVariableSpecification_getNamedVariableRecursive(oper, "operTm");
//			if (operTm)
//			{
//				hasTimeActivatedControl = true;
//			}
//			MmsVariableSpecification* ctlNum = MmsVariableSpecification_getNamedVariableRecursive(oper, "ctlNum");
//			if (ctlNum)
//			{
//				hasCtlNum = true;
//			}
//			t = MmsVariableSpecification_getNamedVariableRecursive(oper, "T");
//		}
//	}
//	if (hasOper == false)
//	{
//		cout << "IED_CLIENT: control is missing required element Oper" << endl;
//		return self;
//	}
//	if (ctlVal == NULL)
//	{
//		cout << "IED_CLIENT: Oper is missing required element ctlVal" << endl;
//		MmsVariableSpecification_destroy(ctlVarSpec);
//		return self;
//	}
//	if (t == 0)
//	{
//		cout << "IED_CLIENT: Oper is missing required element t" << endl;
//		MmsVariableSpecification_destroy(ctlVarSpec);
//		return self;
//	}
//
//	self = (ControlObjectClient) GLOBAL_CALLOC(1, sizeof(struct sControlObjectClient));
//	if (self == NULL)
//	{
//		return self;
//	}
//	self->objectReference = StringUtils_copyString(cRef.data());
//	self->connection = con;
//	self->ctlModel = (ControlModel) ctlModel;
//	self->hasTimeActivatedMode = hasTimeActivatedControl;
//	self->hasCtlNum = hasCtlNum;
//	self->ctlVal = MmsValue_newDefaultValue(ctlVal);
//	if (isAPC)
//	{
//		self->analogValue = MmsValue_createEmptyStructure(1);
//	}
//	else
//	{
//		self->analogValue = NULL;
//	}
//	/* Check for T element type (Binary time -> Ed.1,UTC time -> Ed.2) */
//	if (MmsVariableSpecification_getType(t) == MMS_BINARY_TIME)
//	{
//		self->edition = 1;
//	}
//	else
//	{
//		self->edition = 2;
//	}
//	private_IedConnection_addControlClient(con, self);
//	MmsVariableSpecification_destroy(ctlVarSpec);
//	return self;
//}
//
//char* StringUtils_copyString(const char* string)
//{
//	int newStringLength = strlen(string) + 1;
//	char* newString = (char*) malloc(newStringLength);
//	memcpy(newString, string, newStringLength);
//	return newString;
//}
//
//void private_IedConnection_addControlClient(IedConnection self, ControlObjectClient control)
//{
//	LinkedList_add(self->clientControls, control);
//}
//
//static void resetLastApplError(ControlObjectClient self)
//{
//	self->lastApplError.error = 0;
//	self->lastApplError.addCause = ADD_CAUSE_UNKNOWN;
//	self->lastApplError.ctlNum = 0;
//}
//
//static MmsValue* createOriginValue(ControlObjectClient self)
//{
//	MmsValue* origin = MmsValue_createEmptyStructure(2);
//	if (origin == NULL) return origin;
//	MmsValue* orCat = MmsValue_newIntegerFromInt16(self->orCat);
//	if (orCat == NULL)
//	{
//		MmsValue_delete(origin);
//		origin = NULL;
//		return origin;
//	}
//	MmsValue_setElement(origin, 0, orCat);
//	MmsValue* orIdent;
//	if (self->orIdent != NULL)
//	{
//		int octetStringLen = strlen(self->orIdent);
//		orIdent = MmsValue_newOctetString(0, octetStringLen);
//		if (orIdent == NULL)
//		{
//			MmsValue_delete(origin);
//			origin = NULL;
//			return origin;
//		}
//		MmsValue_setOctetString(orIdent, (uint8_t*) self->orIdent, octetStringLen);
//	}
//	else
//	{
//		orIdent = MmsValue_newOctetString(0, 0);
//	}
//	if (orIdent == NULL)
//	{
//		MmsValue_delete(origin);
//		origin = NULL;
//		return origin;
//	}
//	MmsValue_setElement(origin, 1, orIdent);
//	return origin;
//}
//
//char* MmsMapping_getMmsDomainFromObjectReference(const char* objectReference, char* buffer)
//{
//	int objRefLength = strlen(objectReference);
//	char* domainName = NULL;
//	/* check for object reference size limit VISIBLESTRING129 */
//	if (objRefLength > 129)
//	{
//		return domainName;
//	}
//	/* check if LD name is present */
//	int i;
//	for (i = 0; i < objRefLength; i++)
//	{
//		if (objectReference[i] == '/')
//		{
//			break;
//		}
//	}
//	/* check for LD name limit (=64 characters) */
//	if (i > 64)
//	{
//		return domainName;
//	}
//	if (i == objRefLength)
//	{
//		return domainName;
//	}
//	if (buffer == NULL)
//	{
//		domainName = (char*) malloc(i + 1);
//	}
//	else
//	{
//		domainName = buffer;
//	}
//	int j;
//	for (j = 0; j < i; j++)
//	{
//		domainName[j] = objectReference[j];
//	}
//	domainName[j] = 0;
//	return domainName;
//}
//
//static void convertToMmsAndInsertFC(char* newItemId, const char* originalObjectName, const char* fc)
//{
//	int originalLength = strlen(originalObjectName);
//	int srcIndex = 0;
//	int dstIndex = 0;
//	while (originalObjectName[srcIndex] != '.')
//	{
//		newItemId[dstIndex] = originalObjectName[srcIndex];
//		srcIndex++;
//		dstIndex++;
//	}
//	newItemId[dstIndex++] = '$';
//	newItemId[dstIndex++] = fc[0];
//	newItemId[dstIndex++] = fc[1];
//	newItemId[dstIndex++] = '$';
//	srcIndex++;
//	while (srcIndex < originalLength)
//	{
//		if (originalObjectName[srcIndex] == '.')
//		{
//			newItemId[dstIndex] = '$';
//		}
//		else
//		{
//			newItemId[dstIndex] = originalObjectName[srcIndex];
//		}
//		dstIndex++;
//		srcIndex++;
//	}
//	newItemId[dstIndex] = 0;
//}
//
//bool  CZamClient::Operate(ControlObjectClient self, MmsValue* ctlVal, uint64_t operTime)
//{
//	bool success = false;
//	if (ctlVal == NULL)
//	{
//		return success;
//	}
//	resetLastApplError(self);
//	MmsValue* operParameters;
//	int operElementCount = 5;
//	if (self->hasTimeActivatedMode)
//	{
//		operElementCount++;
//	}
//	if (self->hasCtlNum)
//	{
//		operElementCount++;
//	}
//	operParameters = MmsValue_createEmptyStructure(operElementCount);
//	/* support simplified usage of APC controls - user doesn't need to create the structure */
//	if (self->analogValue != NULL)
//	{
//		if (MmsValue_getType(ctlVal) != MMS_STRUCTURE)
//		{
//			MmsValue_setElement(self->analogValue, 0, ctlVal);
//			ctlVal = self->analogValue;
//		}
//	}
//	MmsValue_setElement(operParameters, 0, ctlVal);
//	int index = 1;
//	if (self->hasTimeActivatedMode)
//	{
//		MmsValue* operTm = MmsValue_newUtcTimeByMsTime(operTime);
//		MmsValue_setElement(operParameters, index++, operTm);
//	}
//	if (!((self->ctlModel == CONTROL_MODEL_SBO_NORMAL) || (self->ctlModel == CONTROL_MODEL_SBO_ENHANCED)))
//	{
//		self->ctlNum++;
//	}
//	uint64_t timestamp;
//	if ((self->ctlModel == CONTROL_MODEL_SBO_ENHANCED) && (self->useConstantT))
//	{
//		timestamp = self->constantT;
//	}
//	else
//	{
//		timestamp = Hal_getTimeInMs();
//	}
//	if (self->useConstantT)
//	{
//		self->constantT = timestamp;
//	}
//	MmsValue* ctlTime;
//	if (self->edition == 2)
//	{
//		ctlTime = MmsValue_newUtcTimeByMsTime(timestamp);
//	}
//	else 
//	{
//		ctlTime = MmsValue_newBinaryTime(false);
//		MmsValue_setBinaryTime(ctlTime, timestamp);
//	}
//	MmsValue* check = MmsValue_newBitString(2);
//	MmsValue_setBitStringBit(check, 1, self->interlockCheck);
//	MmsValue_setBitStringBit(check, 0, self->synchroCheck);
//	MmsValue* origin = createOriginValue(self);
//	MmsValue* ctlTest = MmsValue_newBoolean(self->test);
//	MmsValue_setElement(operParameters, index++, origin);
//	if (self->hasCtlNum)
//	{
//		MmsValue* ctlNum = MmsValue_newUnsignedFromUint32(self->ctlNum);
//		MmsValue_setElement(operParameters, index++, ctlNum);
//	}
//	MmsValue_setElement(operParameters, index++, ctlTime);
//	MmsValue_setElement(operParameters, index++, ctlTest);
//	MmsValue_setElement(operParameters, index++, check);
//	char domainId[65];
//	char itemId[65];
//	MmsMapping_getMmsDomainFromObjectReference(self->objectReference, domainId);
//	convertToMmsAndInsertFC(itemId, self->objectReference + strlen(domainId) + 1, "CO");
//	int controlObjectItemIdLen = strlen(itemId);
//	strncat(itemId, "$Oper", 64 - controlObjectItemIdLen);
//	MmsError mmsError;
//	MmsConnection_writeVariable(IedConnection_getMmsConnection(self->connection), &mmsError, domainId, itemId, operParameters);
//	MmsValue_setElement(operParameters, 0, NULL);
//	MmsValue_delete(operParameters);
//	if (mmsError != MMS_ERROR_NONE)
//	{
//		return success;
//	}
//	MmsValue_update(self->ctlVal, ctlVal);
//	if (self->analogValue)
//	{
//		MmsValue_setElement(self->analogValue, 0, NULL);
//	}
//	self->opertime = operTime;
//	success = true;
//	return success;
//}
//
//
//int CZamClient::getSpecs(IedConnection con, const string& sref, const string& sfc)
//{
//	IedClientError error;
//	FunctionalConstraint fc = FunctionalConstraint_fromString(sfc.data()); 
//	MmsValue* value = IedConnection_readObject(con, &error, sref.data(), fc);
//	if (value == NULL)
//	{
//		IedConnection_close(con);
//		return -1;
//	}
//	string type = string(MmsValue_getTypeString(value));
//	MmsVariableSpecification mmsvs;
//	//	int dimension = MmsVariableSpecification_getSize(mmsvs);
//	//	fval = MmsValue_toFloat(value);
//		MmsValue_delete(value);
//	return 0;
//}
//
//static void resetLastApplError(ControlObjectClient self)
//{
//	self->lastApplError.error = CONTROL_ERROR_NO_ERROR;
//	self->lastApplError.addCause = ADD_CAUSE_UNKNOWN;
//	self->lastApplError.ctlNum = 0;
//}
//
//static MmsValue * createOriginValue(ControlObjectClient self)
//{
//	MmsValue *origin = MmsValue_createEmptyStructure(2);
//
//	if (origin == NULL)
//		return origin;
//
//	MmsValue *orCat = MmsValue_newIntegerFromInt16(self->orCat);
//
//	if (orCat == NULL)
//		goto cleanup_on_error;
//
//	MmsValue_setElement(origin, 0, orCat);
//
//	MmsValue *orIdent;
//
//	if (self->orIdent != NULL)
//	{
//		int octetStringLen = strlen(self->orIdent);
//		orIdent = MmsValue_newOctetString(0, octetStringLen);
//
//		if (orIdent == NULL)
//			goto cleanup_on_error;
//
//		MmsValue_setOctetString(orIdent, (uint8_t *)self->orIdent, octetStringLen);
//	}
//	else
//		orIdent = MmsValue_newOctetString(0, 0);
//
//	if (orIdent == NULL)
//		goto cleanup_on_error;
//
//	MmsValue_setElement(origin, 1, orIdent);
//
//	goto exit_function;
//
//cleanup_on_error:
//	MmsValue_delete(origin);
//	origin = NULL;
//
//exit_function:
//	return origin;
//}
//
//char * MmsMapping_getMmsDomainFromObjectReference(const char *objectReference, char *buffer)
//{
//	int objRefLength = strlen(objectReference);
//	char *domainName = NULL;
//
//	/* check for object reference size limit VISIBLESTRING129 */
//	if (objRefLength > 129)
//		goto exit_function;
//
//	/* check if LD name is present */
//	int i;
//	for (i = 0; i < objRefLength; i++)
//	{
//		if (objectReference[i] == '/')
//		{
//			break;
//		}
//	}
//
//	/* check for LD name limit (=64 characters) */
//	if (i > 64)
//		goto exit_function;
//
//	if (i == objRefLength)
//		goto exit_function;
//
//	if (buffer == NULL)
//		domainName = (char *) malloc(i + 1);
//	else
//		domainName = buffer;
//
//	int j;
//	for (j = 0; j < i; j++)
//	{
//		domainName[j] = objectReference[j];
//	}
//
//	domainName[j] = 0;
//
//exit_function:
//	return domainName;
//}
//
//static void
//convertToMmsAndInsertFC(char *newItemId, const char *originalObjectName, const char *fc)
//{
//	int originalLength = (int)strlen(originalObjectName);
//
//	int srcIndex = 0;
//	int dstIndex = 0;
//
//	while (originalObjectName[srcIndex] != '.')
//	{
//		newItemId[dstIndex] = originalObjectName[srcIndex];
//		srcIndex++;
//		dstIndex++;
//	}
//
//	newItemId[dstIndex++] = '$';
//	newItemId[dstIndex++] = fc[0];
//	newItemId[dstIndex++] = fc[1];
//	newItemId[dstIndex++] = '$';
//	srcIndex++;
//
//	while (srcIndex < originalLength)
//	{
//		if (originalObjectName[srcIndex] == '.')
//			newItemId[dstIndex] = '$';
//		else
//			newItemId[dstIndex] = originalObjectName[srcIndex];
//
//		dstIndex++;
//		srcIndex++;
//	}
//
//	newItemId[dstIndex] = 0;
//}
//
//static MmsValue * prepareOperParameters(ControlObjectClient self, MmsValue *ctlVal, uint64_t operTime)
//{
//	resetLastApplError(self);
//
//	MmsValue *operParameters;
//
//	int operElementCount = 5;
//
//	if (self->hasTimeActivatedMode)
//		operElementCount++;
//
//	if (self->hasCtlNum)
//		operElementCount++;
//
//	operParameters = MmsValue_createEmptyStructure(operElementCount);
//
//	/* support simplified usage of APC controls - user doesn't need to create the structure */
//	if (self->analogValue != NULL)
//	{
//		if (MmsValue_getType(ctlVal) != MMS_STRUCTURE)
//		{
//			MmsValue_setElement(self->analogValue, 0, ctlVal);
//			ctlVal = self->analogValue;
//		}
//	}
//
//	MmsValue_setElement(operParameters, 0, ctlVal);
//
//	int index = 1;
//
//	if (self->hasTimeActivatedMode)
//	{
//		MmsValue *operTm = MmsValue_newUtcTimeByMsTime(operTime);
//		MmsValue_setElement(operParameters, index++, operTm);
//	}
//
//	MmsValue *origin = createOriginValue(self);
//	MmsValue_setElement(operParameters, index++, origin);
//
//	if (!((self->ctlModel == CONTROL_MODEL_SBO_NORMAL) ||
//		  (self->ctlModel == CONTROL_MODEL_SBO_ENHANCED)))
//	{
//		self->ctlNum++;
//	}
//
//	if (self->hasCtlNum)
//	{
//		MmsValue *ctlNum = MmsValue_newUnsignedFromUint32(self->ctlNum);
//		MmsValue_setElement(operParameters, index++, ctlNum);
//	}
//
//	uint64_t timestamp;
//
//	if ((self->ctlModel == CONTROL_MODEL_SBO_ENHANCED) && (self->useConstantT))
//		timestamp = self->constantT;
//	else
//		timestamp = Hal_getTimeInMs();
//
//	if (self->useConstantT)
//		self->constantT = timestamp;
//
//	MmsValue *ctlTime;
//
//	if (self->edition == 2)
//	{
//		ctlTime = MmsValue_newUtcTimeByMsTime(timestamp);
//
//		if (self->connection)
//			MmsValue_setUtcTimeQuality(ctlTime, self->connection->timeQuality);
//	}
//	else
//	{
//		ctlTime = MmsValue_newBinaryTime(false);
//		MmsValue_setBinaryTime(ctlTime, timestamp);
//	}
//
//	MmsValue_setElement(operParameters, index++, ctlTime);
//
//	MmsValue *ctlTest = MmsValue_newBoolean(self->test);
//	MmsValue_setElement(operParameters, index++, ctlTest);
//
//	MmsValue *check = MmsValue_newBitString(2);
//	MmsValue_setBitStringBit(check, 1, self->interlockCheck);
//	MmsValue_setBitStringBit(check, 0, self->synchroCheck);
//	MmsValue_setElement(operParameters, index++, check);
//
//	char domainId[65];
//	char itemId[65];
//
//	MmsMapping_getMmsDomainFromObjectReference(self->objectReference, domainId);
//
//	convertToMmsAndInsertFC(itemId, self->objectReference + strlen(domainId) + 1, "CO");
//
//	int controlObjectItemIdLen = strlen(itemId);
//
//	strncat(itemId, "$Oper", 64 - controlObjectItemIdLen);
//
//	return operParameters;
//}
//
//bool ControlObjectClient_operate(ControlObjectClient self, MmsValue *ctlVal, uint64_t operTime)
//{
//	bool success = false;
//
//	if (ctlVal == NULL)
//	{
//		return false;
//	}
//
//	MmsValue *operParameters = prepareOperParameters(self, ctlVal, operTime);
//
//	char domainId[65];
//	char itemId[65];
//
//	MmsMapping_getMmsDomainFromObjectReference(self->objectReference, domainId);
//
//	convertToMmsAndInsertFC(itemId, self->objectReference + strlen(domainId) + 1, "CO");
//
//	int controlObjectItemIdLen = strlen(itemId);
//
//	strncat(itemId, "$Oper", 64 - controlObjectItemIdLen);
//
////	if (DEBUG_IED_CLIENT)
////		printf("IED_CLIENT: operate: %s/%s\n", domainId, itemId);
//
//	MmsError mmsError;
//
//	MmsDataAccessError writeResult = MmsConnection_writeVariable(IedConnection_getMmsConnection(self->connection),
//																 &mmsError, domainId, itemId, operParameters);
//
//	MmsValue_setElement(operParameters, 0, NULL);
//	MmsValue_delete(operParameters);
//
//	self->lastMmsError = mmsError;
//	self->lastAccessError = writeResult;
//
//	if (mmsError != MMS_ERROR_NONE)
//	{
////		if (DEBUG_IED_CLIENT)
////			printf("IED_CLIENT: operate failed!\n");
//
//		goto exit_function;
//	}
//
//	MmsValue_update(self->ctlVal, ctlVal);
//
//	self->opertime = operTime;
//
//	success = true;
//
//exit_function:
//
//	if (self->analogValue)
//		MmsValue_setElement(self->analogValue, 0, NULL);
//
//	return success;
//}
//
//static MmsOutstandingCall checkForOutstandingCall(MmsConnection self, uint32_t invokeId)
//{
//	int i = 0;
//
//	Semaphore_wait(self->outstandingCallsLock);
//
//	for (i = 0; i < OUTSTANDING_CALLS; i++)
//	{
//		if (self->outstandingCalls[i].isUsed)
//		{
//			if (self->outstandingCalls[i].invokeId == invokeId)
//			{
//				Semaphore_post(self->outstandingCallsLock);
//				return &(self->outstandingCalls[i]);
//			}
//		}
//	}
//
//	Semaphore_post(self->outstandingCallsLock);
//
//	return NULL;
//}
