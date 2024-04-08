#/usr/bib/python3
# Premiere version de Zamxmpp
#
#
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v1.3.2  janvier   2022  |                                                          | AM  #
#                                                                                           #
#                                                                                           #
#-------------------------------------------------------------------------------------------#
#                                                                                           #
#############################################################################################
#------------------------------------------------
#            paramètres
#-----------------------------------------------

#-----------------------------------------------
import pdb
from PyQt5 import  QtWidgets, QtCore, QtGui
from PyQt5.QtCore import Qt
from lxml import etree
import sqlite3
import os
import time
import signal
import datetime
import threading
import logging
import platform
import os.path
import pdb
import struct
import utilitaires
from actions import actions as actions
from zamxmppim import Ui_MainWindow
import subprocess
import sys
import messageTemplates
import json
import math
import shutil

simulind=0

class XMLP:
    """lecture des fichiers XML de configuration """

    def __init__(self, surappdata=False):
        """constructeur de XmlParser"""
        if(sys.platform=="linux"):
            self.activeConfigPath=dirname+"Zamxmpp-Config.xml"
        else:
            self.activeConfigPath=dirname+"\\Zamxmpp-Config.xml"
        try:
            # pdb.set_trace()
            curdir = os.getcwd()
            # pdb.set_trace()
            self.tree = etree.parse(self.activeConfigPath)

            self.root = self.tree.getroot()
        except Exception as ff:
            print("erreur construction xmlp " + str(ff))

    def getConfigValue(self, select):
        """ recupere la valeur de la config de nom name """
        try:
            express = "./appSettings/configuration[@name=\"" + str(select) + "\"]"
            self.e = self.root.find(express)
            if (self.e != None):
                return self.e.attrib["value"]
            else:
                return ""
        except Exception as ee:
            print("XMLP.getConfigValue " + str(ee))


    def setConfigValue(self, select, value):
        """ sauvegarde la valeur de la config de nom name """
        try:
            express = "./appSettings/configuration[@name=\"" + str(select) + "\"]"
            self.e = self.root.find(express)
            if (self.e != None):
                self.e.attrib["value"] = str(value)
                self.tree.write("prov.txt")
                with open("prov.txt",mode='r') as f:
                    a = f.read()
                entete="<?xml version=\"1.0\"?>\n"
                b=entete+a
                with open(self.activeConfigPath, mode='w') as ff:
                    ff.write(b)
            else:
                return ""
        except Exception as ee:
            print("XMLP.setConfigValue " + str(ee))


    def getClxmppValue(self, select):
        """ recupere la valeur le clxmpp de nom name """
        try:
            express = "./appSettings/clxmpp[@name=\"" + str(select) + "\"]"
            self.e = self.root.find(express)
            if (self.e != None):
                id=self.e.attrib["id"]
                passwd=self.e.attrib["passwd"]
                return (id,passwd)
            else:
                return ()
        except Exception as ee:
            print("XMLP.getClxmppValue " + str(ee))




    def getClxmppValues(self):
        """ recupere les clxmpp de nom name """
        ret = ()
        try:
            express = "./stations/clxmpp"
            self.e = self.root.findall(express)
            if (len(self.e) != 0):
                for cl in self.e:
                    id = cl.attrib["id"]
                    passwd = cl.attrib[passwd]
                    ret.append((id, passwd))
            else:
                return ()
        except Exception as ee:
            print("XMLP.getClxmppValue " + str(ee))

    def getBorneValue(self, station,borne):
        """ recupere la valeur le clxmpp de nom name """
        try:
            ret=dict()
            express = "./stations/borne[(@name=\"" + str(borne) + "\") and (@station=\"" + str(station) + "\")]"
            self.e = self.root.find(express)
            if (self.e != None):
                ret["type"]=self.e.attrib["type"]
                ret["status"]=self.e.attrib["status"]
                return ret
            else:
                return ()
        except Exception as ee:
            print("XMLP.getBorneValue " + str(ee))




    def getBorneValues(self):
        """ recupere les clxmpp de nom name """
        ret = list()
        try:
            express = "./appSettings/clxmpp"
            self.e = self.root.findall(express)
            if (len(self.e) != 0):
                for cl in self.e:
                    res=dict()
                    res["name"]=cl.attrib["name"]
                    res["station"] = cl.attrib["station"]
                    res["type"] = cl.attrib["type"]
                    res["status"]=cl.attrib["status"]
                    ret.append(res)
            else:
                return ()
        except Exception as ee:
            print("XMLP.getBorneValues " + str(ee))


    def setBorneValue(self, station, borne,type=None,status=None):
        """ sauvegarde la valeur de la config de nom name """
        try:
            express = "./stations[(@name=\"" + str(borne) + "\") and (@station=\"" + str(station) + "\")]"
            self.e = self.root.find(express)
            if (self.e != None):
                if(type !=None):
                    self.e.attrib["type"] = str(type)
                if (status !=None):
                    self.e.attrib["status"] = str(status)
                self.tree.write("prov.txt")
                with open("prov.txt",mode='r') as f:
                    a = f.read()
                entete="<?xml version=\"1.0\"?>\n"
                b=entete+a
                with open(self.activeConfigPath, mode='w') as ff:
                    ff.write(b)
            else:
                return ""
        except Exception as ee:
            print("XMLP.setConfigValue " + str(ee))

# ------
def traitmem(vv,name="",value=""):
    try:
        vv.reportlog.info("traitmem  variable {0} valeur={1}".format(name,value))
        #print("--traitmem-----------------------------------")
        #print("INFO la varible {0} a changé de valeur={1}".format(name,value))
        #print("--traitmem-----------------------------------")
        if(name=="int_RQSTConnect"): # demande de connection d'un vehicule
            """
            La ChargingStation demande à l’interface le droit pour  EMAId  de se connecter électriquement.
                 • Si il n'y a aucune borne libre la reponse est  int_RSPConnect= nok 
                 • Si il y a au moins une borne libre 
                        - Si l’interface trouve une réservation pour EMAId  à ce créneau horaire ,			  ( |(Harrival-HeureBranchement| < 1200s)
		                        -si il y a une offre (dans nos conventions de decharge)
                                    • si plus d’une borne libre: 
                                        int_RSPConnect= ok, charge|decharge , HDebut, Hfin , Wh
                                    • si au plus une borne libre:
                                        int_RSPConnect= EVSE, charge|decharge , HDebut, Hfin , Wh
				                -si il n’y a pas d’offre (bid)
                                    • si plus d’une borne libre: 
                                        int_RSPConnect= ok, charge|decharge 
                                    • si au plus une borne libre:
                                        int_RSPConnect= EVSE, charge|decharge 
                    -Si l’interface ne trouve aucune réservation pour EMAId  à ce créneau 	horaire (dans nos convention il ne peut s’agir que d’une charge),
                                    • si plus d’une borne libre:
			                             int_RSPConnect= ok
                                    • si au plus une borne libre:
			                             int_RSPConnect= EVSE
              

            """
            try:
                #pdb.set_trace()
                #vv.mempart.dicwrite("int_RSPConnect","")
                ok=False
                emaid=value
                if(sys.platform!="linux"):
                    poursel="SELECT value FROM memoire WHERE name=\"temps\""
                    row=utilitaires.fetchindb(vv.db,vv,poursel)
                    #h=utilitaires.getConvertedH(row[0],typ="int")
                    h=int(row[0])
                    vv.tempsimule=h
                heure=utilitaires.getHCourante(vv,typ="string")
                hasresrved=False # cet emaid a-t-il reserve?
                harrival=None #heure de reservation
                reservID=None
                islicite=False # arivee dans un limite de 15mn de l'heure de reservation
                hcourante=math.floor(utilitaires.getHCourante(vv,typ="int")/1000)
                poursel="SELECT reservID,HArrival FROM reservations WHERE Emaid=\""+emaid+"\" ORDER by HArrival"
                ress=utilitaires.fetchindb(vv.db,vv,poursel,mode="many")
                #vv.reportlog.info("RQSTConnect  ress emaid={0} ress={1}".format(emaid,ress))
                if(len(ress)>0):
                    harrival=utilitaires.getConvertedH(ress[0][1],typ="int") #il y a une reservation
                    reservID=int(ress[0][0])
                    vv.reportlog.info("int_RQSTConnect il y a une reservation reserv={0} harrival={1} hcourante={2}".format(reservID,harrival,hcourante))
                    if((harrival != None) and (abs(harrival - hcourante) < vv.delaiGarde)):
                         islicite=True
                poursel="SELECT name FROM bornes WHERE state=\"F\""
                bornelibres=utilitaires.fetchindb(vv.db,vv,poursel,mode="many")
                isLinux=sys.platform=="linux"
                if(len(bornelibres)==0):  # il n'y a pas de borne libres
                    vv.reportlog.info("int_RQSTConnect  pas de borne libre")
                    if(harrival==None):
                        pass
                    else:
                        if(islicite):
                     ## il devrait y avoir une reservation

                             print("--DIALOGCS-----------------------------------")
                             print("ERROR there is a legid reservation and no free EVSE")
                             print("--DIALOGCS-----------------------------------")
                      
                        else:
                             print("--DIALOGCS-----------------------------------")
                             print("ERROR reserved but arrival outside the legid period")
                             print("--DIALOGCS-----------------------------------")
                         #utilitaires.message("The car had reserved but try to connect out of delay")
                    # envoi refus de connexion
                    if(platform.platform().__contains__("Linux")):
                        vv.mempart.dicwrite("int_RSPConnect","nok")
                        vv.reportlog.info("int_RSPConnect nok")
                    else:
                        poursel="UPDATE  memoire SET value=\"nok\" WHERE name=\"{0}\"".format("int_RSPConnect")
                        utilitaires.execindb(vv.db,vv,poursel)
                    #poursel="SELECT reservID,HArrival,EmaId FROM reservations ORDER by HArrival"
                    #reserveds=utilitaires.fetchindb(vv.db,vv,poursel,mode="many")
                else: # il y a des bornes libres
                    vv.reportlog.info("int_RQSTConnect nb bornes libres {0} islicite={1}".format(len(bornelibres),islicite))
                    if(len(bornelibres)>1):
                        if(isLinux):
                            if(islicite):
                                poursel="SELECT  bidId,HDepart,HBegin,HEnd,flow,Wh FROM reservations WHERE reservID="+str(reservID)
                                r=utilitaires.fetchindb(vv.db,vv,poursel)
                                if(r[4]=="in"):
                                    sens="load"
                                else:
                                    sens="unload"
                                if((r[0] != None) and (r[0] != "")):
                                    valeur="ok,{0},{1},{2},{3}".format(sens,r[2],r[3],r[5])
                                    utilitaires.writeMem(vv,"int_RSPConnect",valeur)
                                    #vv.reportlog.info("int_RQSTConnect ok,{0},{1},{2},{3}".format(sens,r[2],r[3],r[5]))
                                    vv.reportlog.info("traitint_RQSTConnect_read {0}".format(utilitaires.readMem(vv,"int_RSPConnect")))
                                else:
                                    utilitaires.writeMem(vv,"int_RSPConnect","ok,{0}".format(sens))
                                    #vv.reportlog.info("int_RQSTConnect ok,{0}".format(sens))
                                    vv.reportlog.info("traitint_RQSTConnect_read {0}".format(utilitaires.readMem(vv,"int_RSPConnect")))
                            else:
                                 utilitaires.writeMem(vv,"int_RSPConnect","ok") # ok dans tous les cas
                                 #vv.reportlog.info("int_RQSTConnect ok")
                                 vv.reportlog.info("traitint_RQSTConnect_read {0}".format(utilitaires.readMem(vv,"int_RSPConnect")))
                            #utilitaires.writeMem(vv,"int_heuremax","")
                        else:
                            poursel="UPDATE memoire SET value=\"ok\" WHERE name=\"int_RSPConnect\""
                            utilitaires.execindb(vv.db,vv,poursel)
                            #poursel="UPDATE memoire SET value=\"\" WHERE name=\"int_heuremax\""
                            #utilitaires.execindb(vv.db,vv,poursel)
                    else:
                        vv.reportlog.info("in_RQSTConnect lenborne ={2} harrival={0} islicite={1}".format(harrival,islicite,len(bornelibres)))
                        if((harrival != None) and islicite):
                            if(isLinux):
                                poursel="SELECT  bidId,HDepart,HBegin,HEnd,flow,Wh FROM reservations WHERE reservID="+str(reservID)
                                r=utilitaires.fetchindb(vv.db,vv,poursel)
                                vv.reportlog.info("in_RQSTConnect r ={0}".format(r))
                                if(r[4]=="in"):
                                    sens="load"
                                else:
                                    sens="unload"
                                if((r[0] != None) and (r[0] != "")):
                                    valeur="ok,{0},{1},{2},{3}".format(sens,r[2],r[3],r[5])
                                    #vv.reportlog.info("ecriture memoire int_RSPConnect valeur=" + valeur)
                                    utilitaires.writeMem(vv,"int_RSPConnect",valeur)
                                    #vv.reportlog.info("int_RSPConnect ok,{0},{1},{2},{3}".format(sens,r[2],r[3],r[5]))
                                    vv.reportlog.info("traitint_RQSTConnect_read {0}".format(utilitaires.readMem(vv,"int_RSPConnect")))
                                else:
                                    utilitaires.writeMem(vv,"int_RSPConnect","ok,{0}".format(sens))
                                    #vv.reportlog.info("traitint_RSPConnect ok,{0}".format(sens))
                                    vv.reportlog.info("traitint_RQSTConnect_read {0}".format(utilitaires.readMem(vv,"int_RSPConnect")))
                            else:
                                poursel="UPDATE memoire SET value=\"{0}\" WHERE name=\"int_RSqConnect\"".format(bornelibres[0][0])
                                utilitaires.execindb(vv.db,vv,poursel)
                                #poursel="UPDATE memoire SET value=\"\" WHERE name=\"int_heuremax\""
                                #utilitaires.execindb(vv.db,vv,poursel)
                        else:  #autorise jusqu'à heuremax
                            if(platform.platform().__contains__("Linux")):
                                utilitaires.writeMem(vv,"int_RSPConnect","nok")
                                vv.reportlog.info("int_RQSTConnect heure depassee nok")
                            else:
                                 poursel="UPDATE  memoire SET value=\"nok\" WHERE name=\"{0}\"".format("int_RSPConnect")
                                 utilitaires.execindb(vv.db,vv,poursel)
                                 """
                            poursel="SELECT HArrival FROM reservations ORDER by HArrival"
                            reserveds=utilitaires.fetchindb(vv.db,vv,poursel,mode="many")
                            #heuremax=reserveds[0][0]
                            if(isLinux):
                                vv.reportlog.info("traitint_RSPConnect cas autorisation heuremac ")
                                vv.mempart.dicwrite("int_RSPConnect","ok")
                                val=utilitaires.readMem(vv,"int_RSPConnect")
                                print("--mempart-----------------------------------")
                                print("ecriture  int_RSPConnect value={0}".format(val))
                                print("--mempart-----------------------------------")
                                #vv.mempart.dicwrite("int_heuremax",heuremax)  
                            else:
                                poursel="UPDATE memoire SET value=\"ok\" WHERE name=\"int_RSPConnect\""
                                utilitaires.execindb(vv.db,vv,poursel)
                                #poursel="UPDATE memoire SET value=\"\" WHERE name=\"int_heuremax\""
                                #utilitaires.execindb(vv.db,vv,poursel)   
                        #if(reservID != None): vv.selectedReserv=reservID # se poser la question sur une arrivée precoce ou tardive
         
                        """
            except Exception as ee:
                vv.reportlog.info(" int_RSQconnect  err={0}".format(ee))
                raise Exception(" int_RSQconnect   err={0}".format(ee))

        if(name=="int_Connect"):
            try:
                """ la station notifie la connection d'un vehicule """
                vv.reportlog.info(" int_Connect  val={0}".format(value))
                values=value.split(",")
                evse=values[0]
                emaid=values[1]
                h=values[2] # heure  xxhyy
                heure=utilitaires.getConvertedH(h,typ="int")
                hh=time.strftime("%Y-%m-%dT%H:%M:%SZ",time.gmtime(heure)) #heure UTC        
                if(not platform.platform().__contains__("Linux")):
                    vv.tempsimule=utilitaires.getConvertedH(h,typ="int")    
                poursel="SELECT reservID,HArrival FROM reservations WHERE Emaid=\""+emaid+"\" ORDER by HArrival"
                ress=utilitaires.fetchindb(vv.db,vv,poursel,mode="many")
                if(len(ress)>0):
                    harrival=utilitaires.getConvertedH(ress[0][1],typ="int") #il y a une reservation
                    reservID=int(ress[0][0])
               # if((harrival != None) and (abs(harrival - heureconnect) < 1200)):
                   #  islicite=True
                else:
                    reservID=vv.sansreservID+1 #il n'y a pas de reservation
                m13=messageTemplates.EVConnexion(evse=evse,emaid=emaid,heure=hh,reserv=str(reservID),connexiontype="CablePlugIn")
                csms=vv.xmlp.getConfigValue("csms")
                utilitaires.sendMessage(vers=csms,message=m13.message,variables=vv)
                vv.selectedReserv=reservID
                poursel="UPDATE bornes SET HConnection=\""+h+"\",state=\"B\",Emaid=\""+emaid+"\",reservID="+ str(vv.selectedReserv)+" WHERE name=\""+evse+"\""
                utilitaires.execindb(vv.db,vv,poursel)
                if(reservID < vv.sansreservID):
                    poursel="UPDATE reservations SET borne="+ str(utilitaires.getBorneId(vv,evse))+" WHERE reservID=" + str(vv.selectedReserv)
                    utilitaires.execindb(vv.db,vv,poursel)
            except Exception as ee:
                vv.reportlog.info("traitem int_connect  err={0}".format(ee))
                raise Exception(" traitem int_connect   err={0}".format(ee))

        if(name=="int_ChangeChargeStatus"):
            try:
                vv.reportlog.info(" int_ChangeChargeStatus  val={0}".format(value))
                values=value.split(",")
                evse=values[0]
                status=values[1].strip(' ')
                h=values[2]
                heure=utilitaires.getConvertedH(h,typ="int")
                hh=time.strftime("%Y-%m-%dT%H:%M:%SZ",time.gmtime(heure)) #heure UTC        
                wh=values[3]
                if(not platform.platform().__contains__("Linux")):
                    vv.tempsimule=utilitaires.getConvertedH(h,typ="int")
                m5=messageTemplates.EVSEChargeStatus(evse=evse,status=status,heure=hh,wh=wh)
                csms=vv.xmlp.getConfigValue("csms")
                utilitaires.sendMessage(vers=csms,message=m5.message,variables=vv)
                poursel="SELECT reservID FROM bornes WHERE name=\"{0}\"".format(evse)
                row=utilitaires.fetchindb(vv.db,vv,poursel)
                reservID=row[0]
                borneid=utilitaires.getBorneId(vv,evse)
                sd=(status=="dischargestart")
                ss=(status=="chargestart") or (status=="dischargestart")
                vv.reportlog.info("changed status status=|{0}| ss={1} sd={2}".format(status,ss,sd))
                if((status=="chargestart") or (status=="dischargestart")):
                    poursel="UPDATE bornes SET HBegin=\""+h+"\" WHERE name=\""+evse+ "\""
                    utilitaires.execindb(vv.db,vv,poursel)
                    if(reservID < vv.sansreservID):
                        poursel="UPDATE reservations SET status=\"go\",HBegin=\""+h+"\""+",borne="+str(borneid)+",Wh=\""+wh+"\" WHERE  reservID=" + str(reservID)
                        utilitaires.execindb(vv.db,vv,poursel)
                else:
                    poursel="UPDATE bornes SET HEnd=\""+h+"\" WHERE name=\""+evse+ "\""
                    utilitaires.execindb(vv.db,vv,poursel)
                    if(reservID < vv.sansreservID):
                        poursel="UPDATE reservations SET HEnd=\""+h+"\", WhTot=\""+wh+"\" WHERE  reservID=" + str(reservID)
                        utilitaires.execindb(vv.db,vv,poursel)

            except Exception as ee:
                vv.reportlog.info("traitem ChangeChargeStatus status={0} err={1}".format(status,ee))
                raise Exception(" ChangeChargeStatus status={0} err={1}".format(status,ee))

        if(name=="int_Disconnect"):
            vv.reportlog.info("traitem int_Disconnect")
            values=value.split(",")
            evse=values[0]
            emaid=values[1]
            h=values[2] #
            heure=utilitaires.getConvertedH(h,typ="int")
            hh=time.strftime("%Y-%m-%dT%H:%M:%SZ",time.gmtime(heure)) #heure UTC      
            if(not platform.platform().__contains__("Linux")):
                vv.tempsimule=utilitaires.getConvertedH(h,typ="int")
            m14=messageTemplates.EVConnexion(evse=evse,emaid=emaid,heure=hh,connexiontype="CablePlugOut")
            csms=vv.xmlp.getConfigValue("csms")
            utilitaires.sendMessage(vers=csms,message=m14.message,variables=vv)
            poursel="SELECT reservID FROM bornes WHERE name=\""+evse+"\""
            row=utilitaires.fetchindb(vv.db,vv,poursel)
            if(row==None): raise Exception("traitDisconnect evse {0} not found".format(evse))
            reservID=row[0]
            if((reservID != None ) and (reservID !=0)):
                poursel="UPDATE reservations SET status=\"ended\",HDepart=\""+h+"\"  WHERE reservID=" + str(reservID)
                utilitaires.execindb(vv.db,vv,poursel)
            poursel="UPDATE bornes SET HDisConnection=\""+h+"\",state=\"F\",Emaid=\""+emaid +"\",reservID="+ "0"+ " WHERE name=\""+evse+"\""
            utilitaires.execindb(vv.db,vv,poursel)
            
    except Exception as ee:
             raise Exception("traitmem name={0} value={1} err={2}".format(name,value,ee))

def getChangedMemvars(vv):
    """
    donne la liste de memoires qui ont changé de valeurs 
    et leur nouvelle valeur
    """
    ret=list()

    try:
        for var in vv.vars:
            val=utilitaires.readMem(vv,var)
            valpred=vv.memvars[var]
            #vv.reportlog.info("getChangedMemvars var={0} valpred={1}  val={2}".format(var,valpred,val))
            if(val != valpred):
                if(var != "temps"):
                    vv.reportlog.info("getChangedMemvars var={0} valpred={1}  val={2}".format(var,valpred,val))
                ret.append((var,val))
        return ret
    except Exception as ee:
        raise Exception("getChangedMemvars " + str(ee))

def getChangedMemvars_cleaned(vv):
    """
    donne la liste de memoires qui ont changé de valeurs 
    et leur nouvelle valeur
    """
    ret=list()

    try:
        vars=list(vv.memvars)
        #pdb.set_trace()
        for var in vars:
            val=utilitaires.readMem(vv,var)
            valpred=vv.memvars[var]
            if(False):
                if((vv.dicvars[var][0])=="ip") or (vv.dicvars[var][0]=="string"):
                     valpred=utilitaires.cleanstr(vv.memvars[var])
                else:
                    valpred=utilitaires.cleanint(vv.memvars[var])

            if(val != valpred):
               ret.append((var,val))
        return ret
    except Exception as ee:
        raise Exception("getChangedMemvars " + str(ee))

def simulVariations(vv,i,pasx=15,deltay=2,inity=200,reset=True):
    """ simule les variations d'une liste de variables en memoire 
        sortie=[i,
    """
    try:
        variables=vars
        if(reset==True): simulind=0
        for var in variables:
            val=utilitaires.randomOffset()
            utilitaires.writeMem(vv,val)
    except Exception as ee:
        print("default surveillance " +str(ee))

def handler(signum, frame):
    global variables
    print("Zamxmpp reception signal {0}".format(signum))
    
     
def survstation(vv):
    """
    tache qui lit la mémoire partagée cycliquement pour détecter des changements d'etat
    """
    try:
        i=0
        vv.reportlog.info("SURV situation de depard: memvars={0}".format(vv.memvars))
        listx=list()
        listy=list()
        settings=vv.xmlp.getConfigValue("SimulSettings")
        jsettings=json.loads(settings)
        minx=jsettings["minx"]
        if(sys.platform=="linux"):
            minx=int(vv.mempart.dicread("temps")*6/1000/60)
        deltax=jsettings["deltax"]
        jsettings["maxx"]=minx +deltax
        maxx=jsettings["maxx"]
        while(not vv.fin):
           
            if(vv.myvplot != None):
                t=utilitaires.readMem(vv,"temps")
                if(sys.platform=="linux"):
                    t=6*t/1000
                h=str(int(t))
                vv.ui.LTemps.setText(h)
                tt=float(t)
                t=(tt/60) # temps en mn
                #vv.reportlog.info("SURV maxx={0} t={1}".format(maxx,t))
                if(t > maxx):
                    minx=minx+deltax
                    maxx=maxx+deltax
                    vv.myvplot.resize(newminx=minx,newmaxx=maxx) 
                volt=utilitaires.readMem(vv,"Vder_module")
                vv.ui.LTension.setText(str(volt))
                listx.append(t)
                listy.append(volt)
                vv.myvplot.trace(listx,listy)
                #vv.reportlog.info("SURV trace t={0} v={1}".format(t,volt))
                vv.ui.canvas.draw()
            changedMems=getChangedMemvars(vv)
            #vv.reportlog.info("SUIVI changedMems={0}".format(changedMems))
            for cm in changedMems:
                if(cm[0]=="temps"): continue
                # mise à jour de memvars
                vv.memvars[cm[0]]=cm[1]
                #vv.reportlog.info("SUIVI changedMems={0}  traitement={1}".format(changedMems,cm[0]))
                traitmem(vv,name=cm[0],value=vv.memvars[cm[0]])
            time.sleep(1)
            if(sys.platform !="linux"):
                vv.tempsimule=vv.tempsimule+6 # on suppose une acceleration du temps de 6 fois
                utilitaires.writeMem(vv,"temps",str(vv.tempsimule))
            i=i+1
    except Exception as ee:
        print("default surveillance " +str(ee))
        #vv.message("default surveillance")

def initTableview(vv):
    """ initialise la table de vison de la mémoire
    """
    nbcol=2
    nbrow=len(vv.memvars)
    tablew=vv.ui.tableWidget
    tablew.setRowCount(nbrow)
    tablew.setColumnCount(nbcol)
    ri=range(0,nbrow)
    vars=list(vv.memvars)
    i=0
    vv.memvarsind=dict()
    for var in vars:
        tablew.setItem (i,0, QtWidgets.QTableWidgetItem (var))
        tablew. setItem (i,1,QtWidgets.QTableWidgetItem (vv.memvars[var]) )
        vv.memvarsind[var]=i #index de la variable à laffichage
        i=i+1
    tablew.show()
#------
class variables:
    def __init__(self):
        """ constructeur des variables """
        try:
             self.curDir=os.getcwd() # repertoire courant
             self.loghandler = logging.FileHandler("/home/pi/logs/Zamreportxmpp.log",mode='w',encoding=None,delay=False)
             self.loghandlerconsole = logging.StreamHandler(stream=sys.stdout)
             #print(" logging dans :"/home/pi/logs/Zamreportxmpp.log" )
             self.formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')  
             self.loghandler.setFormatter(self.formatter)
             self.reportlog=logging.getLogger('reportlog')
             self.reportlog.setLevel(logging.INFO)
             self.loghandlerconsole.setLevel(logging.ERROR)
             self.reportlog.addHandler(self.loghandler)
             self.reportlog.addHandler(self.loghandlerconsole)
             self.xmlp=XMLP()
             self.mode=self.xmlp.getConfigValue("Mode")
             self.curDir=None  # la fenetre centrale
             self.mesacts=None  # pointe sur les actions
             self.nbbornes=None
             self.dialreserve=None
             self.dbpath=None
             self.fin=False
             self.selectedReserv=None  # reservation retenue
             self.tempsimule=0
             self.sansreservID=1000
             self.progvars=self.xmlp.getConfigValue("progvars") # liste des memoire definies pour le programe
             self.memvars=dict() # dictionnaire memorisation valeurs  memoire de partagée
             self.memvarsind=dict() #inexes pour affichage
             self.csms=self.xmlp.getConfigValue("csms")
             self.station=self.xmlp.getConfigValue("station")
             g=self.xmlp.getConfigValue("Garde")
             self.delaiGarde=int(g)
             self.zamstationtest=self.xmlp.getConfigValue("Bdtest")
             if(platform.platform().__contains__('Linux')):
                 self.db='/home/zamam/ZamStation.db'
                 from mempart import MemPart as MemPart
                 self.mempart=MemPart(adresse="/ZAMVARCST",vs=self)
                 self.dicvars=dict()  # le dictionaire des variables partagees
                                      # nomvar ArrayList(type,adresse,nboctet)
                 import posix_ipc
                 self.sem=posix_ipc.Semaphore("/sem.ZAMSEMAL")
             else:
                 self.db='C:\\Users\\andell3\\Documents\\depotsim\\trunk\\Sys\\ZamStation\\Zamam\\ZamStation.db'
                 self.sem=None
             self.vars=list() # list des noms de variables  
             self.actions=actions(self)
             self.ll=threading.RLock()
             self.ui= None # fenêtre principale
             #self.util=utilitaires(self)
             self.adserver=self.xmlp.getConfigValue("SelectedServerAdresse")  #
             self.servport = "5022"
             self.mymutexList = threading.RLock()
             self.mymutex = threading.RLock
             self.essaiEnvois=self.xmlp.getConfigValue("essaiEnvois").split("|")
             self.couleurs={"vert":"rgb(190, 255, 208)","orange":"rgb(255, 170, 0)","rose":"rgb(255, 170, 255)","rouge":"rgb(255, 85, 127)"}
             self.myvplot=None # courbe d'affichage de la tension
        except Exception as ee:
            self.reportlog.error(" variables " + str(ee))
            print(" erreur variables " + str(ee))

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
             ("Vder_module","double",8,3000), \
             ("Vder_angle","double",8,3008), \
             ("mdr","int",4,3016) \
              ]
            
             for var in varlist: #initialisation de dicvars
                utilitaires.dicvarsadd(self,var[0],typ=var[1],lo=var[2],ad=var[3])
             for var in varlist: #initialisation de memvars
                 self.vars.append(var[0])
                 typ=self.dicvars[var[0]][0]
                 self.memvars[var[0]]="??????"
                 if((typ=="ip") or (typ=="string")):
                     self.memvars[var[0]]="??????"
                 else:
                     self.memvars[var[0]]=0
             echvars=["int_Connect","int_ChangeChargeStatus","int_End","int_Disconnect","int_Heuremax"]
             for ev in echvars:
                mem=utilitaires.readMem(self,ev)
                if(mem != "??????"):
                    ri=range(0,255)
                    buf=""
                    for i in ri:
                        buf=buf+"\0"
                    utilitaires.writeMem(self,ev,buf)
                    utilitaires.writeMem(self,ev,"??????")
                self.memvars[ev]="??????"
             valechvars=list()
             for ev in echvars:
                 val=utilitaires.readMem(self,ev)
                 valechvars.append([ev,val])
                 
             self.reportlog.info("initmemvars {0}".format(valechvars))
                 
                
                 

#------------------- programe racine --------------------------------

if __name__ == "__main__":
    ff=open("toterr.log",'w')    
    ff.close()
    #pdb.set_trace()
    if(sys.platform=="linux"):
        dirname="/home/pi/zamam/Zamxmpp/"
    else:
        dirname="C:\\Users\\andell3\\Documents\\depotsim\\trunk\\Sys\\ZamStation\\Zamam\\Zamxmpp\\"
    args=sys.argv # liste des parametre d'appel
    ri = range(0, len(args))
    for i in ri:
        if("Zamxmmp.py" == os.path.basename(args[i])):
            dirname=os.path.dirname(args[i])
            break

    vv=variables()
    try:       
                
        if(len(args)<=1):
            vv.mode="test"
            if(platform.platform().__contains__('Linux')):
                bddrep='/home/pi/zamam'
                vv.db=bddrep+"/ZamStation.db"
                #vv.mybas=mybase(vv.dbpath,vv.reportlog,variables=vv)
                #valeur=utilitaires.readMem(vv,"int_adresseCS")
                             
            else:
                bddrep='C:\\Users\\andell3\\Documents\\depotsim\\trunk\\Sys\\ZamStation\\Zamam'
                vv.db=bddrep+"\\ZamStation.db"
                #vv.mybas=mybase(vv.dbpath,vv.reportlog,variables=v         
        else:
            ri=range(1,len(args))
            dicargs=dict()
            for i in ri:
                ll=args[i].split('=')
                dicargs[ll[0]]=ll[1]
            for param in dicargs.keys():      
                if(param=="bddrep"):
                    bddrep=dicargs["bddrep"]
                    if(platform.platform().__contains__('Linux')):
                        vv.db=bddrep +"/ZamStation.db"
                    else:
                        vv.db=bddrep  +"..\ZamStation.db"
                        #vv.mybas=mybase(vv.dbpath,vv.reportlog,variables=vv)
                elif(param=="mode"):
                    vv.mode=dicargs["mode"]
       
  
        poursel="SELECT * FROM bornes"
        bornes=utilitaires.fetchindb(vv.db,vv,poursel,mode="many")
        vv.nbbornes=len(bornes)
        app = QtWidgets.QApplication(sys.argv)
        vv.app=app
        MainWindow = QtWidgets.QMainWindow()
        ui = Ui_MainWindow()
        vv.ui=ui
        ui.setupUi(MainWindow)
        vv.dicvars=dict()
        vv.initdicvars() 
        utilitaires.initMem(vv)
        if(sys.platform == 'linux'):
            vv.mempart.setVars(vv)
        # ------------- initialisation widgets ---------------------
        lev=["---","andre","daniel","jean-luc"]
        lok=["ok","nok","EVSE1","EVSE2"]
        lcs=["---","chargestart","chargeend","dischargestart","dischargeend"]
        lh=list()
        ri=range(0,24)
        rj=range(0,4)
        for i in ri:
            for j in rj:
                a="{0:02d}".format(i)
                b="{0:02d}".format(j*15)
                lh.append(a+"h"+b)
       
        le=vv.essaiEnvois
        ui.CB_envois.addItems(le)
       
        
        # ------------------ actions ----------------------------
       
       
        ui.CB_envois.activated.connect(vv.actions.benvois)
        ui.PB_Courbes.clicked.connect(vv.actions.bcourbes)
        
        
        # -------------------------------------------------------
       
        initTableview(vv)
        
   #----------------------------------------------------------------    
   ##PROVISOIRE
        """
        import shutil
        if(sys.platform=="linux"):
                shutil.copyfile("/home/pi/zamam/ZamStation-exemple.db","/home/pi/zamam/ZamStation.db")
        """
   #----------------------------- 
        a=list()
        a.append(vv)
        t=threading.Thread(target=survstation,args=(a))
        t.start()
        
        station=vv.xmlp.getConfigValue("station")

        id, passwd = vv.xmlp.getClxmppValue(station)
        jid = id + "@" + vv.adserver
            #
        debug="-d" #"-d"
        if(platform.platform().__contains__("Linux")):
            ppath="/home/pi/zamam/Zamxmpp/"
        else:
            homepath="C:" + os.environ['HOMEPATH'] + "\\"
            ppath=homepath+"Documents\\depotsim\\trunk\\Sys\\ZamStation\\Zamam\\Zamxmpp\\"
        
        if(debug=="-d"):
            p=subprocess.Popen(["python",ppath+ "interfacexmpp.py", debug,\
                                "-j",jid,\
                                "-p",passwd \
                               ])
        else:
            p=subprocess.Popen(["python",ppath+ "interfacexmpp.py", \
                                "-j",jid,\
                                "-p",passwd \
                               ])
        
        print("--- ENVOI  CG -----------------")
        m1=messageTemplates.M11QueryReservationCG()
        prov="prov"
        csms=vv.xmlp.getConfigValue("csms")
        utilitaires.sendMessage(vers=str(csms), message=m1.message,variables=vv)
        
        #-----------------------------------------------------------
        from ComplementsIHM import  vplot as vplot
        vv.myvplot=vplot(vv)
        if(sys.platform != "linux"):
            vv.simulmem=threading.Thread(target=utilitaires.writeSimulVar,args=[vv],kwargs={'deltay':vv.myvplot.jsettings["deltay"],'inity':vv.myvplot.jsettings["miny"],'maxy':vv.myvplot.jsettings["maxy"]})
            vv.simulmem.start()
        
        #-----------------------------------------------------------
        if(vv.mode=="test"): 
            if (os.path.exists(vv.db )):
                oldfile=vv.db + "_old"
                shutil.copyfile(vv.db, oldfile)
                shutil.copyfile(vv.zamstationtest,vv.db)
                vv.ui.L_envoi.show()
                vv.ui.CB_envois.show()
                vv.ui.PB_Courbes.show()

            #os.remove()
            vv.ui.LPid.setText(str(threading.main_thread().ident))
            #MainWindow.show()
        else:
            vv.ui.L_envoi.hide()
            vv.ui.CB_envois.hide()
            vv.ui.PB_Courbes.hide()
            vv.ui.LPid.setText(str(threading.main_thread().ident))
            #MainWindow.show() 
            """
        #---------------  synchro ------------------
        #le handler de SIGUSER1

         """
        sys.exit(app.exec_())
        ff.flush()
    except Exception as ee:
        print(" Main erreur:" + str(ee))
    finally:
        vv.app.closeAllWindows()
        vv.app.closingDown()  
        vv.reportlog.warning(" finally")
        os.abort()
