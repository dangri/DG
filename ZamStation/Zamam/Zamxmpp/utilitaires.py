#!/usr/bin/python
""" ensembles de fonctions utilitaires
message(texte,type="Information",boutons=["Ok"]) # montre un dialogue de message 'message' avec des boutons
"""
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v1.1.0  | 03/03/19   |  version initiale avec bogues corrigées                     |  AM  #
# v1.1.1  | 08/09/21   |  - echanges  memoire partagee                               | AM   #
#                                                                                           #
#############################################################################################
from PyQt5 import QtCore, QtGui, QtWidgets
import threading
import time
import platform
import struct
import sqlite3
import random
import pdb
import platform
import os
import subprocess
import math
import sys


if(platform.platform().__contains__('Linux')):
  #  import iec61850
  pass

# ----------------------- variables globales -------------------"
simulind=0 # rg d'un point sur x
#----------------------------------------------------------------

def message(texte,type="Information",boutons=["Ok"]):
    try:
         msg = QtWidgets.QMessageBox()
         if(type=="Information"): msg.setIcon(QtWidgets.QMessageBox.Information)
         if(type=="NoIcon"): msg.setIcon(QtWidgets.QMessageBox.NoIcon)
         if(type=="Question"): msg.setIcon(QtWidgets.QMessageBox.NoIcon)
         if(type=="Warning"): msg.setIcon(QtWidgets.QMessageBox.NoIcon)
         if(type=="Critical"): msg.setIcon(QtWidgets.QMessageBox.NoIcon)

         msg.setInformativeText(texte)
         msg.setWindowTitle("annonce")
         #msg.setText("information")
         #msg.setDetailedText("The details are as follows:")
         buttons=list()
         for b in boutons:
             if(b=="Ok"): buttons.append(QtWidgets.QMessageBox.Ok)
             if(b=="Open"): buttons.append(QtWidgets.QMessageBox.Open)
             if(b=="Save"): buttons.append(QtWidgets.QMessageBox.Save)
             if(b=="Cancel"): buttons.append(QtWidgets.QMessageBox.Close)
             if(b=="Discard"): buttons.append(QtWidgets.QMessageBox.CDiscard)
             if(b=="Apply"): buttons.append(QtWidgets.QMessageBox.Apply)
             if(b=="Reset"): buttons.append(QtWidgets.QMessageBox.Reset)
             if(b=="RestoreDefaults"): buttons.append(QtWidgets.QMessageBox.RestoreDefaults)
             if(b=="Help"): buttons.append(QtWidgets.QMessageBox.Help)
             if(b=="SaveAll"): buttons.append(QtWidgets.QMessageBox.SaveAll)
             if(b=="Yes"): buttons.append(QtWidgets.QMessageBox.Yes)
             if(b=="YesToAll"): buttons.append(QtWidgets.QMessageBox.YesToAll)
             if(b=="No"): buttons.append(QtWidgets.QMessageBox.No)
             if(b=="NoToAll"): buttons.append(QtWidgets.QMessageBox.NoToAll)
             if(b=="Abort"): buttons.append(QtWidgets.QMessageBox.Abort)
             if(b=="Retry"): buttons.append(QtWidgets.QMessageBox.Retry)
             if(b=="Ignore"): buttons.append(QtWidgets.QMessageBox.Ignore)
             if(b=="NoButton"): buttons.append(QtWidgets.QMessageBox.NoButton)
         ri=range(1,len(buttons))
         mask=buttons[0]
         for i in ri:
            mask=mask | buttons[i]
            msg.setStandardButtons(mask)
         retval = msg.exec_()
         return retval
         #print ("value of pressed message box button:", str(retval))
    except Exception as ee:
        print("erreur "+ str(ee))

def gettyp(val):
    """ donne une chaine du type d'une variable"""
    tt=str(type(val))
    tt=tt.strip('<>')
    valtyp=tt.split(' ')[1]
    valtyp=eval(valtyp)
    return valtyp

def getTemps(temps,mempart,reportlog=None):
    try:
         if(mempart != None):
             tt=mempart.read8()
             #20ttt=struct.unpack('d',tt)
             ttt=struct.unpack('Q',tt)
             t=ttt[0]
             #t=time.time()*1000 # on repasse aux ms provisoirement
             #if(reportlog != None): reportlog.info("TEST TEMPS t , t/1000="  + str(t) +"," + str(t/1000) )  
             #print("TEST TEMPS t , t/1000="  + str(t) +"," + str(t/1000))
         else:
             t=temps 
         return t
    except Exception as ee:
        #vars.reportlog.error("getTemps " + str(ee))
        raise Exception("getTemps " + str(ee))


def getDetailTemps(mempart):
    t=None
    if(mempart != None):
             tt=mempart.read8()
             ttt=struct.unpack('Q',tt)
             t=(tt,ttt[0])
    return t
    



#--------------------------------------------------------
class monohrloge(threading.Thread): 
    """ classe de creation et de demarrage d'un thread pour la mise a jour du temps simule
    les calculs cycliques sont fait tous les cycl fois
    """
    def __init__(self,name=None,bdd=None,variables=None,cycl=10):
        self.bd=bdd
        self.vv=variables
        self.myname=name
        self.K=0
        self.horloge=None
        threading.Thread.__init__(self,name=self.myname,target=self.majtime)
        self.cycl=cycl #les calculs cycliques sont fait tous les cycl fois
        self.repet=0


    def majtime(self):
            """ 
            met a jour le temps simule et lance calculs cycliques sur la borne
              """
            try:
                if((self.vv==None) or (self.myname==None )): raise Exception(" paramètres bdd ou variables non renseigné")
                if(self.K == 0 ):        
                    self.mybas=mybase(self.vv.db,self.vv.reportlog,variables=self.vv)
                    poursel=" SELECT valeur FROM gestion WHERE nom=\"K\""
                   
                    self.K=int(self.mybas.fetchone(poursel)[0])
                    if(not platform.platform().__contains__('Linux')):
                         self.vv.tempssimule=0 # on fixe le temps initial à 0
                    fin=False
                    tempscourrant=self.vv.util.getTemps(self.vv.tempssimule,self.vv.mempart)
                while(not fin):
                   if(not platform.platform().__contains__('Linux')):
                         self.vv.tempssimule=self.vv.tempssimule+1000 # on s'aligne sur starton pour exprimer le temps en ms
   
                   if(self.repet==0):
                     self.calculsCycliques("mesureWCurrent")
                     self.calculsCycliques("mesuresACPourEVSE")
                     self.repet=self.cycl
                     if(not platform.platform().__contains__('Linux')):
                         self.vv.ui.label_3.setText(time.strftime("%H:%M:%S", time.gmtime(self.vv.util.getTemps((self.vv.tempssimule)/1000,self.vv.mempart))))
                         self.vv.ui.label_3.show()
                   else:
                       self.repet=self.repet-1
                   time.sleep(1/self.K)  # lorsque K=60 1 seconde reelle = 1s de temps simulee
                   #if(self.vv.util.getTemps(self.vv.tempssimule,self.vv.mempart)==tempscourrant):  #le temps ne bouge plus il faut s'arrêter
                     #  self.vv.mesacts.monarr_ter()
            except Exception as ee:
                print(" pb ds majtime " + str(ee))
   
    
    def maj(self):
        """incremente le temps"""
        #self.vv.tempssimule=self.vv.tempssimule+1
        self.vv.ui.label_3.setText(time.strftime("%H:%M:%S", time.gmtime(self.vv.tempssimule)))
        self.vv.ui.label_3.show()
#-------------------------------------------------------------------
def getConvertedH(self,h, typ="int"):
    """ donne heure en texte si en s et en secons si en texte"""
    if (typ == "int"):
        hour = h.split("h")
        return int(hour[1]) + (int(hour[0]) * 3600)
    else:
        hour = math.floor(h / 3600)
        s = math.floor(h - hour * 3600)
        return str(h) + "h" + str(s)

def getHCourante(self,vv, typ="int"):
    """ heure au format zamxmpp  xxhyy"""
    if (platform.platform().__contains__("Linux")):
        h = vv.mempart.dicread("temps")
    else:
        h = vv.tempsimule
    if (typ != "int"):
        return self.getConvertedH(h, typ="string")
    else:
        return h

def initMem(vv):
    """ initialise le contenu de la memoire partagee sur window/Linux"""
    try:
        if(sys.platform != 'linux'):
            for var in vv.vars:
                writeMem(vv,var,str(vv.memvars[var]))
           
        else:
            vv.reportlog.info("initMem progvars={0}".format(vv.progvars))
            listvars=vv.progvars.split("|")
            for var in listvars:
                writeMem(vv,var,str(vv.memvars[var]))
            
            prov="prov"
            
    except Exception as ee:
        raise Exception("initMem " + str(ee))

def writeMem(vv,varname,val):
    try:
        vv.reportlog.info("writeMem varname={0} val={1}".format(varname,val))
        if(platform.platform().__contains__('Linux')):
            vv.mempart.dicwrite(varname,val)
        else:
            poursel=" UPDATE  memoire SET value=\"" + str(val) + "\"  WHERE name=\"" + varname +"\""
            execindb(vv.db,vv,poursel)

        if(varname in vv.memvarsind):
            vv.ui.tableWidget.item(vv.memvarsind[varname],1).setText(str(val))
        prov="prov"
    except Exception as ee:
        raise Exception("writeMem " + str(ee))
    
def readMem(vv,varname):
    try:
        if(platform.platform().__contains__('Linux')):
            return vv.mempart.dicread(varname)
        else:
            poursel="SELECT type,value FROM memoire WHERE name='"+varname+"'"
            resul=fetchindb(vv.db,vv,poursel)
        ret=None
        if(resul !=None):
            typ=resul[0]
            val=resul[1]
            if(typ=="int"):
                ret=int(val)
            elif(typ=="ull"):
                ret=int(val)
            elif(typ=="bool"):
                ret=bool(val)
            elif(typ=="float"):
                ret=float(val)
            elif(typ=="double"):
                ret=float(val)
            elif(typ=="string"):
                ret=str(val)
        if(varname in vv.memvarsind):
            vv.ui.tableWidget.item(vv.memvarsind[varname],1).setText(str(ret))
        return ret
    except Exception as ee:
        raise Exception("readMem " + str(ee))

def getSimulVar(ind,deltay=0.1,inity=200,maxy=400,offset=0.05):
    """ simule la valeur y dans une courbe avec x de pas fixe à l'index  ind
       
        sortie=[ind*pas,inity+ind*deltay+randomOffset()]
    """
    try:
        ret= inity+ind*deltay+randomOffset(max=offset)
        if(ret < inity):ret=inity
        return ret   
    except Exception as ee:
        raise Exception("get Simulvar " +str(ee))

def writeSimulVar(vv,deltay=0.1,inity=200,maxy=400,type="linear"):
    try:
        simulind=0
        while(True):
            val=getSimulVar(simulind,deltay=deltay,inity=inity,maxy=maxy)
            if(val > maxy):
                val=maxy
            writeMem(vv,"Vder_module",val)
            simulind=simulind+1
            time.sleep(1)
    except Exception as ee:
        raise Exception("writeSimulvar " +str(ee))

    
def randomOffset(max=10):
    """ fourni un nombre aleatoire flottant entre -max et max"""
    return random.uniform(-1*max,max)

def execindb(db,vv,poursel):
    """
    effectue une action sur la base de donnee db

    """
    try:
     # vv.reportlog.info("avant acquire execind poursel=" + pourcel)
      if(vv.sem != None): vv.sem.acquire() #(timeout=float(2))
      if(vv.ll!=None): vv.ll.acquire(blocking=True,timeout=2)
      conn=sqlite3.connect(db) 
      conn.execute(poursel)
      conn.commit()
      conn.close()   
     # vv.reportlog.info("avant release execind poursel=" + pourcel)
      if(vv.ll!=None) :vv.ll.release()
      if(vv.sem != None): vv.sem.release()
    except Exception as ee:
         if(vv.ll!=None) :vv.ll.release()
         if(vv.sem != None): vv.sem.release()
         raise Exception("execindb " + str(ee))


def getWidsForEvse(vv,memname):
    """
    pour interface zamxmppim
    recupère les valeurs désignées 
    :return parameters
    evse 
    val  valeur du choix 
    h    heure  
    wh  charhe cible ou totale
    """
    ri=range(1,vv.nbbornes+1)
    evse=""
    cas=1
    for i in ri:
        if(memname=="ChangeChargeStatus"):
            cas=2
            cbname="CB" +memname+"_"+str(i)
            cb=vv.ui.__dict__[cbname]
            cbhname="CB"+memname+"H_"+str(i)
            cbh=vv.ui.__dict__[cbhname]
            cbwname="LEEndWhtot_"+str(i)
            cbw=vv.ui.__dict__[cbwname]
            if(cb.currentText() != "---"):
                evse="EVSE"+str(i)
                val=cb.currentText()
                h=cbh.currentText()
                wh=cbw.text()
                break
 
        else:
            cas=1
            cbname="CB" +memname+"_"+str(i)
            cb=vv.ui.__dict__[cbname]
            cbhname="CB"+memname+"H_"+str(i)
            cbh=vv.ui.__dict__[cbhname]
            if(cb.currentText() != "---"):
                evse="EVSE"+str(i)
                val=cb.currentText()
                h=cbh.currentText()
                break
    if(evse==""):
        message(" you must at least one EVSE with  status != '---'")
        val=""
        h=""
    if(cas==1):
        return evse,val,h
    elif(cas==2):
        return evse,val,h,wh

def fetchindb(dbpath,vv,poursel,mode="one",size=500):
  try:
     #vv.reportlog.info("avant acquire fetchinbd poursel=" + pourcel)
     if(vv.sem != None): vv.sem.acquire()  #(timeout=float(2))
     if(vv.ll!=None): vv.ll.acquire(blocking=True,timeout=2)
     #pdb.set_trace()
     conn=sqlite3.connect(dbpath) 
     c=conn.execute(poursel)
     if(mode=="one"):
        res = c.fetchone()
     else:
        res= c.fetchmany(size)
     conn.close() 
     #vv.reportlog.info("avant release execind poursel=" + pourcel)
     if(vv.ll!=None) :vv.ll.release()
     if(vv.sem != None): vv.sem.release()
     return res
  except Exception as ee:
     if(vv.ll!=None) :vv.ll.release()
     if(vv.sem != None): vv.sem.release()
     raise Exception("fetchindb " + str(ee))

def getCodeFromCapacite(val):
    """ calcul le code de capacite en fonction de la capacite du cable"""
    try:
        cap=0
        if (val == 13):
            cap = 1
        elif (val == 20):
            cap = 2
        elif (val == 32):
            cap = 3
        elif (val == 63):
            cap = 4
        elif (val == 70):
            cap = 5
        elif (val == 80):
            cap = 6
        elif (val == 125):
            cap = 7
        elif (val == 200):
            cap = 8
        elif (val == 250):
            cap = 9
        elif (val == 400):
            cap = 10
        return cap
    except Exception as ee:
         raise (" getCodeFromCapacite " + str(ee))

def getConvertedH(h,typ="int"):
    """ donne heure en texte si en s et en seconds si en texte"""
    if(typ=="int"):
        hour=h.split("h")
        return (int(hour[1])*60)+(int(hour[0])*3600)
    else:
        hour=int(math.floor(h/3600))
        mn=int(math.floor(h-(hour*3600))/60)
        return str(hour) + "h" + str(mn)

def getHCourante(vv,typ="string"):
    """ heure au format zamxmpp  xxhyy"""
    if(platform.platform().__contains__("Linux")):
        h=vv.mempart.dicread("temps")
    else:
        h=vv.tempsimule
    if(typ=="string"):
        return getConvertedH(h,typ="string")
    else:
        return h

def sendMessage(vers=None,message="",variables=None):
        """ envoi d'un message à une station"""
        try:
            vv=variables
            if(vv==None):
                raise Exception("utilitaires.sendMessage call without variables")
            l=len(message)
            mes=""
            if(l<10):
                mes=message
            else:
                mes=message[0:10]
            
            vv.reportlog.info(" envoi d'un message to={0} debmes={1}".format(vers, mes))
            station=vv.xmlp.getConfigValue("station")
            id, passwd = vv.xmlp.getClxmppValue(station)
            id=str(id)
            passwd=str(passwd)
            jid = id + "@" + vv.adserver
            to=str(vers) + "@" + vv.adserver
            debug="-d"
            if(platform.platform().__contains__("Linux")):
               ppath="/home/pi/zamam/Zamxmpp/"
            else:
               homepath="C:" + os.environ['HOMEPATH'] + "\\"
               ppath=homepath+"Documents\\depotsim\\trunk\\Sys\\ZamStation\\Zamam\\Zamxmpp\\"
            if(debug==""):
                   p=subprocess.Popen(["python",ppath+ "envoimessages.py", \
                                "-j",jid,\
                                "-p",passwd,\
                                "-t",to,\
                                "-m",message],stdin=subprocess.PIPE)
            else:
                                p=subprocess.Popen(["python",ppath+ "envoimessages.py", debug,\
                                "-j",jid,\
                                "-p",passwd,\
                                "-t",to,\
                                "-m",message],stdin=subprocess.PIPE)
            #f=io.StringIO("toto")
            #pipes=p.communicate(input=f)
            #p.stdin.write("titi".encode())
            #a=p.stdout.read()
            prov="prov"
        except Exception as ee:
            print("utilitaires.sendMessage {0}".format(ee))


def getBorneId(vv,name):
        """ donne le no de borne connaissant son rang dans la station
        on suppose que les bornes s'appelle EVSEx ou x est le rang de la station
        """

        poursel="SELECT Id  FROM bornes WHERE  name=\""+ name + "\""
        row=fetchindb(vv.db,vv,poursel)
        if(row==None): raise Exception("there is no borne {0}.{1}".format(station,name))
        return row[0]

def getBorneName(vv,bornid):
        """ give station and bornename give its id"""
        poursel="SELECT station,name FROM bornes WHERE id="+str(bornid)
        row = fetchindb(vv.db, vv, poursel)
        if (row == None): raise Exception("there is no borne {0}".format(bornid))
        return row[1]

def cleanint(ss):
    if(type(ss)==type(1)):
        return ss
    if(not ss.__contains__("\"")):
        return int(ss)
    else:
        return int(ss.replace("\"", ""))

def cleanstr(ss):
    if(not ss.__contains__("\"")):
        return ss
    else:
        return ss.replace("\"", "")

def dicvarsadd(vv,clef,typ='string',lo=8,ad=100):
    """ entre une  clef  dans le dictionnaire d
        type= type de la valeur  long=nb d'octets
        ull | int | float | | ip | bool | string 
         8      4     8        20   4       256
        si ll clef existe la valeur est remplacee sinon 
              elle est cree
    """
    try:
        #pdb.set_trace()
        if((vv==None) or ( vv.dicvars==None)):raise Exception("le dictionnaire n'est pas defini")
        prv=1
        if(not clef in vv.dicvars):
            vv.dicvars[clef]=list()
            liste=vv.dicvars[clef]
            liste.append(typ)
            liste.append(lo)
            liste.append(ad)   
    except Exception as ee:
         #print(" erreur acces memoire   partagee " + str(ee))
         if((vv !=None) and (vv.reportlog != None)): vv.reportlog.error("dicvarsadd " + str(ee))
                    
def getHourFromUTC(self,utc):
    """ convertit une chaine UTC en ignorant l'annee sous la forme xxhss"""
    #'2001-12-17T08:30:47Z'
    h = utc.split('T')
    hh = h[1].split(':')
    return hh[0] + "h" + hh[1]           