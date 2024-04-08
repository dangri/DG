#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#############################################################################################
# version #   date     #              interfacexmpp                                  # auth #
#############################################################################################
# v1.1.0 | mai  2021  |                                                             | AM    #
#                                                                                           #
#                                                                                           #
#-------------------------------------------------------------------------------------------#
#                                                                                           #
#############################################################################################
"""
    Derived from
    Slixmpp: The Slick XMPP Library
    Copyright (C) 2010  Nathanael C. Fritz
"""

import logging
from getpass import getpass
from argparse import ArgumentParser

import slixmpp
import asyncio
from XMLParser import XMLP as XMLP 
from lxml import etree
from io import  StringIO, BytesIO
import utilitaires
import sqlite3
import sys
import os
import pdb
import threading
import platform
import messageTemplates

class echangesxmpp(slixmpp.ClientXMPP):

    """
    Ce processus
    - receptionne les messages provenant d'un autre agent xmpp par l'intermédiare du serveur xmpp
       effectue les traiements correspondants
    - surveille la memoire partagée de la station
       effectue les traitements correspondant aux changement d'état
    """
    vv=None
    def __init__(self, jid, password):
        slixmpp.ClientXMPP.__init__(self, jid, password)

        # The session_start event will be triggered when
        # the program establishes its connection with the server
        # and the XML streams are ready for use. We want to
        # listen for this event so that we we can initialize
        # our roster.
        self.vv=vv
        print("TEST ----------------------------------")
        print("variables {0} db {1}".format(vv,vv.db))
        print("TEST ----------------------------------")
        self.add_event_handler("session_start", self.start)

        # The message event is triggered whenever a message
        # stanza is received. 
        self.add_event_handler("message", self.message)
        #self.add_event_handler("roster_update",self.updateroster)

    def updateroster(self,suite):
        print("recu update-roster" + str(suite))

    async def start(self, event):
        """
        Process the session_start event.

        Typical actions for the session_start event are
        requesting the roster and broadcasting an initial
        presence stanza.

        Arguments:
            event -- An empty dictionary. The session_start
                     event does not provide any additional
                     data.
        """
        self.send_presence()
        await self.get_roster()
        fin=False
        i=0
        #while(not fin):
         # i=await survstation(vv,i)  
         # asyncio.sleep(0.5)

    async def survstation(self,vv,i):
        """
            programme qui lit la mémoire partagée cycliquement pour détecter des changements d'etat
        """
        try:
            poursel="SELECT name,type,value FROM memoire"
            variables=utilitaires.fetchindb(vv.db,vv,poursel,mode="many")
            if(len(variables)==0): 
                pass
            else:
                    vv.reportlog.info("surveillance "+ str(i))
            i=i+1
            return i
        except Exception as ee:
            vv.message("default surveillance")



    def message(self, msg):
        """
        Process incoming message stanzas.  It is usually
        a good idea to check the messages's type before processing
        or sending replies.

        Arguments:
            msg -- The received message stanza. See the documentation
                   for stanza objects and the Message stanza to see
                   how it may be used.
        """
        if msg['type'] in ('chat', 'normal'):
            de=msg['from']
            mess=msg['body']
            print("TEST ----------------------------------")
            print("message {0} ".format(msg))
            print("TEST ----------------------------------")
            tree=etree.fromstring(mess)
            typeelt=tree.find("./header/Noun")
            messtype=typeelt.text
            print("TEST ----------------------------------")
            print("messtype {0} ".format(messtype))
            print("TEST ----------------------------------")
            if(messtype=="M1CreateReservation"):
                reservID=self.traitM1CreateReservation(tree)
                rsp={"id":reservID,"acquit":"exists"}
                resack=messageTemplates.ReservationAcknowledge(rsp)
                m2=messageTemplates.M2ReservationAcknowledge(resack)
                #msg.reply(m2.message).send()
            elif(messtype=="M12RSPReservationCG"):
                self.traitM12RSPReservationCG(tree)
            else:
                print("Interfacexmpp ----------------------------------")
                print("ERROR MessageType {0} not recognised".format(messtype))
                print("Interfacexmpp ----------------------------------")

    def traitM1CreateReservation(self,tree):
        """ traite l'arrivee d'une reservation : on repond ok """
        try:
            reservelt=tree.find("./payload/reservation")
            if(reservelt==None): 
                print("Interfacexmpp ----------------------------------")
                print("ERROR {0} not found payload ".format("traitM1CreateReservation"))
                print("Interfacexmpp ----------------------------------")
            idel=reservelt.attrib["id"]  
            print("Interfacexmpp ----------------------------------")
            print("idel {0}  ".format(str(idel)))
            print("Interfacexmpp ----------------------------------")
            reservID=idel
            EmaId=(tree.find("./payload/reservation/EmaId")).text
            HDepart=(tree.find("./payload/reservation/HDepart")).text 
            HArrival=(tree.find("./payload/reservation/HArrival")).text
            flow=(tree.find("./payload/reservation/flow")).text
            status=(tree.find("./payload/reservation/status")).text
            bidId=(tree.find("./payload/reservation/bidId")).text
            HBegin=(tree.find("./payload/reservation/HBegin")).text
            HEnd=(tree.find("./payload/reservation/HEnd")).text
            Wh=(tree.find("./payload/reservation/Wh")).text
            ex=os.path.exists(self.vv.db)
            print("Interfacexmpp ----------------------------------")
            print("db={0} ex={1} ".format(self.vv.db,ex))
            print("Interfacexmpp ----------------------------------")
            poursel="SELECT reservID,bidID FROM reservations WHERE reservID="+str(reservID)
            #pdb.set_trace()
            rows = utilitaires.fetchindb(self.vv.db, self.vv, poursel)
            if(rows==None):     
                poursel="INSERT INTO reservations (reservID, EmaId, HArrival,bidId,HDepart,HBegin,HEnd,Wh) VALUES \
                  (" + reservID  +"," +EmaId +","+ HArrival + "," + bidId+"," + HDepart + "," + HBegin + "," + HEnd +","+ Wh +")"                               
                utilitaires.execindb(self.vv.db, self.vv, poursel)
                print("Interfacexmpp ----------------------------------")
                print("fin traitM1CreateReservation")
                print("Interfacexmpp ----------------------------------")
            else:
                return 0
            return reservID
           
        except Exception as e:
            print("Interfacexmpp ----------------------------------")
            print("M11 ERROR {0}  ".format(str(e)))
            print("Interfacexmpp ----------------------------------")

    def traitM12CreateReservation(self,de,tree):
        """ traite l'arrivee d'une reservation : on repond ok """
        try:
            csms=de
            reservelts=tree.findall("./payload/reservations/reservation")
            if(len(reservelts)==0): 
                print("Interfacexmpp ----------------------------------")
                print(" WARNING {0}  payload empty ".format("traitM12CreateReservation"))
                print("Interfacexmpp ----------------------------------")
                return
            else:
                poursel="DELETE * FROM reservations"
                self.vv.util.execindb(self.vv.db,self.vv,poursel)
                for reservelt in reservelts:
                    idel=reservelt.attrib["id"]
                    print("Interfacexmpp ----------------------------------")
                    print("M12 idel {0}  ".format(str(idel)))
                    print("Interfacexmpp ----------------------------------")
                    reservID=idel
                    EmaId=(tree.find("./payload/reservation/EmaId")).text
                    HDepart=(tree.find("./payload/reservation/HDepart")).text 
                    HArrival=(tree.find("./payload/reservation/HArrival")).text
                    flow=(tree.find("./payload/reservation/flow")).text
                    status=(tree.find("./payload/reservation/status")).text
                    bidId=(tree.find("./payload/reservation/bidId")).text
                    HBegin=(tree.find("./payload/reservation/HBegin")).text
                    HEnd=(tree.find("./payload/reservation/HEnd")).text
                    Wh=(tree.find("./payload/reservation/Wh")).text
                    ex=os.path.exists(self.vv.db)
                    print("Interfacexmpp ----------------------------------")
                    print("db={0} ex={1} ".format(self.vv.db,ex))
                    print("Interfacexmpp ----------------------------------")
                    poursel="SELECT reservID,bidID FROM reservations WHERE reservID="+str(reservID)
                    #pdb.set_trace()
                    rows = utilitaires.fetchindb(self.vv.db, self.vv, poursel)
                    if(rows==None):     
                        poursel="INSERT INTO reservations (reservID, EmaId, HArrival,bidId,HDepart,HBegin,HEnd,Wh) VALUES \
                            (" + reservID  +"," +EmaId +","+ HArrival + "," + bidId+"," + HDepart + "," + HBegin + "," + HEnd +","+ Wh +")"                               
                        self.vv.util.execindb(self.vv.db, self.vv, poursel)
                print("Interfacexmpp ----------------------------------")
                print("fin traitM12RSPReservationCG")
                print("Interfacexmpp ----------------------------------")


        except Exception as e:
            print("Interfacexmpp ----------------------------------")
            print("M11 ERROR {0}  ".format(str(e)))
            print("Interfacexmpp ----------------------------------")

class variables:
    def __init__(self):
        """ constructeur des variables """
        try:
             self.curDir=os.getcwd() # repertoire courant
             self.loghandler = logging.FileHandler(self.curDir + "/" + "reportZamxmpp.txt",mode='w',encoding=None,delay=False)
             print(" logging dans : " + self.curDir + "/" + "reportZamxmpp.txt")
             self.formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
             self.loghandler.setFormatter(self.formatter)
             self.reportlog=logging.getLogger('reportlog')
             self.reportlog.setLevel(logging.INFO)
             self.reportlog.addHandler(self.loghandler)
             self.sem=None
             self.dicvars=None
             self.tempsimule=0
             if(platform.platform().__contains__("Linux")):
                self.db="/home/pi/ZamStation/Zamam/ZamStation.db"
                from mempart import MemPart as MemPart
                self.mempart=MemPart(adresse="/ZAMVARCST",vs=self)
                self.dicvars=dict()  # le dictionaire des variables partagees
                                  # nomvar ArrayList(type,adresse,nboctet)
                import posix_ipc
                self.sem=posix_ipc.Semaphore("/sem.ZAMSEMAL")
             else:
                self.db="C:\\Users\\andell3\\Documents\\depotsim\\trunk\\Sys\\ZamStation\\Zamam\\ZamStation.db"

             self.ll=threading.RLock()
                   
        except Exception as e:
           print("error init variables "+ str(e))
    def initdicvars(self):
        if(self.dicvars != None):
             varlist=[
             ("temps","ull",8,0), \
             ("int_adresseCS","ip",20,8), \
             ("mod_MA","int",4,28), \
             ("IEDCSLDCS/MMXU1.TotW.mag.f","float",4,32), \
             ("IEDCSLDCS/XSWI1.Pos.stVal","int",4,36), \
             ("int_PID","int",4,40), \
             ("int_RQSTConnect","string",256,44), \
             ("int_RSPConnect","string",256,300), \
             ("int_Connect","string",256,556), \
             ("int_ChangeChargeStatus","string",256,812), \
             ("int_End","string",256,1068), \
             ("int_Disconnect","string",256,1324), \
             ("int_Heuremax","string",256,1580), \
              ]
             for var in varlist:
                 self.mempart.dicvarsadd(var[0],typ=var[1],lo=var[2],ad=var[3])



if __name__ == '__main__':
        ff=open("toterr.log",'w')
        sys.stderr=ff
        #kk=open("totout.log",'w')
        #sys.strout=kk
    # Setup the command line arguments.
        parser = ArgumentParser(description=echangesxmpp.__doc__)

        # Output verbosity options.
        parser.add_argument("-q", "--quiet", help="set logging to ERROR",
                            action="store_const", dest="loglevel",
                            const=logging.ERROR, default=logging.INFO)
        parser.add_argument("-d", "--debug", help="set logging to DEBUG",
                            action="store_const", dest="loglevel",
                            const=logging.DEBUG, default=logging.INFO)

        # JID and password options.
        parser.add_argument("-j", "--jid", dest="jid",
                            help="JID to use")
        parser.add_argument("-p", "--password", dest="password",
                            help="password to use")

        args = parser.parse_args()

        # Setup logging.
        logging.basicConfig(level=args.loglevel,
                            format='%(levelname)-8s %(message)s')

        if args.jid is None:
            args.jid = input("Username: ")
        if args.password is None:
            args.password = getpass("Password: ")

        # Setup echangesxmpp and register plugins. Note that while plugins may
        # have interdependencies, the order in which you register them does
        # not matter.
       

        vv=variables()
        print("Interfacexmpp ----------------------------------")
        print("db {0}  ".format(vv.db))
        print("Interfacexmpp ----------------------------------")
        xmpp=echangesxmpp(args.jid,args.password)
        xmpp.register_plugin('xep_0030') # Service Discovery
        xmpp.register_plugin('xep_0004') # Data Forms
        xmpp.register_plugin('xep_0060') # PubSub
        xmpp.register_plugin('xep_0199') # XMPP Ping

        # Connect to the XMPP server and start processing XMPP stanzas.
        #xmpp.update_roster("csms1@192.168.1.25")
        loop=xmpp.loop
        xmpp.connect()
        #loop=xmpp.loop
        xmpp.process()
  

