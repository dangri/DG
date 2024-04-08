
import sys
import os
import lxml
import sqlite3
import utilitaires

class ReservationAcknowledge:
    """acquite une resevation
    resp: dict id, acquit
    """
    acknowledge=None
    id=None
    acquit=None
    def __init__(self,resp):
        self.id=resp["id"]
        self.acquit=resp["acquit"]
        body="<Acknowledge for=\"Reservation\">\n"
        body = body + "id" + str(self.id) + "</id>\n"
        body = body + "<acquit" + self.acquit + "</acquit>\n"
        body =body +  "</Acknowledge>"
        self.acknowledge=body

class Reservation:
    reservID=None
    EmaId=None
    HArrival=None
    HDepart=None
    flow=None
    status=None
    bidId=None
    HBegin=None
    HEnd=None
    Wh=None
    borne=None
    body=None
    def __init__(self,vv,reserv):
        """
        creation dun message de reservation à partir d'une liste dans l'ordre des attributs
        dont les clefs sont les noms des colonnes de la table
        de la variable globales vv
        """
        #poursel="SELECT * FROM reservation WHERE (status = \"planned\") OR (status = \"going\")"
        #reservations=utilitaires.fetchinbd(vv.db,vv,poursel)
        self.reservID=reserv["reservID"]
        self.EmaId=reserv["EmaId"]
        self.HArrival=reserv["HArrival"]
        self.HDepart = reserv["HDepart"]
        self.flow = reserv["flow"]
        self.status = reserv["status"]
        self.bidId = reserv["bidId"]
        self.HBegin = reserv["HBegin"]
        self.HEnd = reserv["HEnd"]
        self.Wh = reserv["Wh"]
        #self.borne = reserv["borne"]
        body = "<Reservation >\n"
        body = body + "<HDepart>" + self.HDepart + "</HDepart>\n"
        body = body + "<HArrival>" + self.HArrival + "</HArrival>\n"
        body = body + "<status>" + self.status + "</status>\n"
        body=body +"<Bid>\n"
        body=body + "<Credential>\n"
        body=body+"<ID>\""+ self.bidID + "\"</ID>\n"
        body=body + "<type>\""+ self.EmaId  +  "\"<type>"
        body = body + "/Credential>\n"
        body = body + "<EmEnergyTransferTransaction>\n"
        body = body + "<endTimestamp>\"" + self.HEnd + "\"</endTimestamp>"
        body = body + "<startTimestamp>\"" + self.HEnd + "\"</startTimestamp>"
        body = body + "<flow >" + self.flow + "</flow>\n"
        body = body + "<energyAmount multiplier=\"Y\" unit=\"Wh\" >" + self.Wh + "</energyAmount>\n"
        body = body + "</EmEnergyTransferTransaction>\n"
        body = body +"<EVSE>\n"
        body = body + "<name>"+ self.borne + "</name>\n"
        body = body +"</EVSE>\n"
        #body = body + "<borne>" +str(self.borne)  + "</>\n"
        body =body +  "</Reservation>"
        self.reservation=body


    def getReservation(self):
        return self.reservation



class M11QueryReservationCG:
    """
    la staion x s'est réinitialisée
    elle demande toutes les réservations y compris celle déclarée ended 
    et go qu'on reinitialise à planned
    """
    message=None
    def __init__(self,type="all"):
        message="<csmsMessage>\n"
        message=message+"<header>\n" \
                        "<Verb>create</Verb>\n" \
                        "<Noun>M1QueryReservationCG</Noun>\n" \
                        "</header>\n"
        message=message+"<payload>\n"

        message=message + "<query>\""+ type +"\"</query>" + "\n" \
            "</payload>\n"
        message=message+"</csmsMessage>\n"
        self.message=message

class M12RSPReservationCG:
    message=None
    def __init__(self,reserv):
        message="<csmsMessage>\n"
        message=message+"<header>\n" \
                        "<Verb>create</Verb>\n" \
                        "<Noun>M1CreateReservationCG</Noun>\n" \
                        "</header>\n"
        message=message+"<payload>\n"

        message=message + reserv.reservation + "\n" \
            "</payload>\n"
        message=message+"</csmsMessage>\n"
        self.message=message
class ReservationRequest:
    message=None
    def __init__(self,reserv):
        message="<csmsMessage>\n"
        message=message+"<header>\n" \
                        "<Verb>create</Verb>\n" \
                        "<Noun>ReservationRequest</Noun>\n" \
                        "</header>\n"
        message=message+"<payload>\n"

        message=message + reserv.reservation + "\n" \
            "</payload>\n"
        message=message+"</csmsMessage>\n"
        self.message=message

class M2ReservationAcknowledge:
    message=None
    def __init__(self,resack):  # resp= ok | nok 
        message="<csmsMessage>\n"
        message=message+"<header>\n" \
                        "<Verb>create</Verb>\n" \
                        "<Noun>M2ReservationAcknowledge</Noun>\n" \
                        "</header>\n"
        message=message+"<payload>\n"

        message=message + resack.acknowledge + "\n" \
            "</payload>\n"
        message=message+"</csmsMessage>\n"
        self.message=message

class EVConnexion:
    message=None
    def __init__(self,evse="",emaid="",heure="",reserv="",connexiontype=""):  # resp= ok | nok
        payload="<EVConnection>\n"
        payload = payload+ "<DeviceEvent>\n" 
        payload = payload + "<timeStamp>\"" + heure + "\"</timeStamp>\n"
        payload = payload + "<type>\"" + connexiontype + "\"</type>\n"
        payload = payload + "<Credential>\n"
        payload = payload + "<iD>\""+ emaid +"\"</iD>\n"
        payload = payload +"</Credential>\n"
        payload = payload + "<EVSE>\n"
        payload = payload + "<name>\"" + evse+ "\"</name>\n"
        payload = payload + "</EVSE>\n"
        payload = payload + "<Reservation>\n"
        payload = payload + "<iD>\"" + reserv + "\"</iD>\n"
        payload = payload + "</Reservation>\n"
        payload = payload+ "</DeviceEvent>\n" 
        payload=payload+"</EVConnection>\n"

        message="<csmsMessage>\n"
        message=message+"<header>\n" \
                        "<Verb>create</Verb>\n" \
                        "<Noun>EVConnexion</Noun>\n" \
                        "</header>\n"
        message=message+"<payload>\n"

        message=message + payload + "\n" \
            "</payload>\n"
        message=message+"</csmsMessage>\n"
        self.message=message



class EVSEChargeStatus:
    message=None
    def __init__(self,evse="",status="",flow="Out",heure="",wh=""):  #
        print("MESSAGE EVSEChargeStatus status={1} wh={0}".format(wh,status))
        mess="<EVSEChargeStatus>\n"
        mess = mess + "<EVSE>\n"
        mess = mess + "<name>\"" + evse + "\"</name>\n"
        mess = mess + "<EmEnergyTransferTransaction>\n"
        mess = mess + "<energyAmount multiplier=\"c\" unit=\"Wh\">\""+wh+"\"</energyAmount>\n"
        mess = mess + "<flow>\""+flow+"\"</flow>\n"
        mess = mess + "<EnergyTransferTransactionEvent>\n"
        mess = mess + "<energyTransferType>\""+status+"\"</energyTransferType>\n"
        mess = mess + "<timeStamp>\""+heure+"\"</timeStamp>\n"
        mess = mess + "</EnergyTransferTransactionEvent>\n"
        mess = mess + "</EmEnergyTransferTransaction>\n"
        mess = mess + "</EVSE>\n"
        mess = mess + "</EVSEChargeStatus>\n"
  
        message="<csmsMessage>\n"
        message=message+"<header>\n" \
                        "<Verb>create</Verb>\n" \
                        "<Noun>EVSEChargeStatus</Noun>\n" \
                        "</header>\n"
        message=message+"<payload>\n"
        message=message + mess + "\n" \
            "</payload>\n"
        message=message+"</csmsMessage>\n"
        self.message=message

class M6End:
    message=None
    def __init__(self,evse="",whtot="",heure=""):  # resp= ok | nok 
        branchement="<branchement>\n"
        branchement=branchement+ "<evse>\""+evse+"\"</evse>\n" \
                                  "<whtot>\""+whtot+"\"</whtot>\n" \
                                  "<hour>\""+heure+"\"</hour>\n"                       
        branchement=branchement+"</branchement>\n"
        
        message="<csmsMessage>\n"
        message=message+"<header>\n" \
                        "<Verb>create</Verb>\n" \
                        "<Noun>M6End</Noun>\n" \
                        "</header>\n"
        message=message+"<payload>\n"

        message=message + branchement + "\n" \
            "</payload>\n"
        message=message+"</csmsMessage>\n"
        self.message=message


class M14EVDisConnect:
    message=None
    def __init__(self,evse="",emaid="",heure=""):
        branchement="<branchement>\n"
        branchement=branchement+ "<evse>\""+evse+"\"</evse>\n" \
                                  "<emaid>\""+emaid+"\"</emaid>\n" \
                                  "<hour>\""+heure+"\"</hour>\n"                       
        branchement=branchement+"</branchement>\n"
        
        message="<csmsMessage>\n"
        message="<csmsMessage>\n"
        message=message+"<header>\n" \
                        "<Verb>create</Verb>\n" \
                        "<Noun>M14EVDisConnect</Noun>\n" \
                        "</header>\n"
        message=message+"<payload>\n"

        message=message + branchement + "\n" \
            "</payload>\n"
        message=message+"</csmsMessage>\n"
        self.message=message

