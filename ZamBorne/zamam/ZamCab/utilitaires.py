#!/usr/bin/python
""" ensembles de fonctions utilitaires
message(texte,type="Information",boutons=["Ok"]) # montre un dialogue de message 'message' avec des boutons
"""
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v0.0.1  | 03/03/19   |  version initiale avec bogues corrigées                     |  AM  #
# v0.1.0  | 08/04/20   |  - echanges  memoire partagee                               | AM   #
#                                                                                           #
#############################################################################################
import threading
import time
from Mybase import mybase as mybase
import platform
import struct
import sqlite3
import random

if(platform.platform().__contains__('Linux')):
  #  import iec61850
  pass


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
    



def verifcell(item, res):
    """verification de la valeur d'un nod  selon res (format,type)"""
    return True # pour l'instant toujours vrai

class mypoint:
    def __init__(self,x=0,y=0):
        self.x=x
        self.y=y
    def x(self):
        return self.x
    def y(self):
        return self.y
    def xy(self):
        return (self.x,self.y)
    def setX(self,x):
        self.x=x
    def setY(self,y):
        self.y=y
    def setXY(self, xy=(0,0)):
        self.x=xy[0]
        self.y=xy[1]

class mycourbe:
    """ une courbe est définie comme un ensemble de mypoints elle peut être définie par une liste de points d'entree ouien par calcul
"""
    def __init__(self,points=[]):
        try:
            self.points=list()
            self.yvsx=dict() # dictionnaire donnant y si y
            if(len(points) != 0):
                self.points=points
                for p in points:
                    self.yvsx[p[0]]=p[1]
                prov=1
        except Exception as ee:
            raise Exception("  mycourbe init " +str(ee))
    def setPoints(self,points):
          try:
            if(len(points != 0)):
                self.points=points
          except Exception as ee:
            raise Exception("  mycourbe setPoints vide " +str(ee))
    def dump(self):
        """ sort une impression des points de la courbes """
        print(str(self.points))

    def Y(self,x):
        """ donne la valeur extrapolée d'un point de la courbe"""
        try:
            xs=self.yvsx.keys()
            sxs=list()
            for xx in xs:
                sxs.append(xx)
            sxs.sort()
            xinf=0
            xsup=0
            ri=range(0,len(sxs)-1)
            for i in ri:
                if( (x >= sxs[i]) and (x <= sxs[i+1])):
                    xinf=sxs[i]
                    xsup=sxs[i+1]
                    break
            y=self.yvsx[xinf] + (x-xinf)*(self.yvsx[xsup]-self.yvsx[xinf])/(xsup-xinf)
            return y
        except Exception as ee:
            raise Exception( " erreur pour courbe.Y(x) " +  str(ee))




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
                    self.mybas=mybase(self.vv.dbpath,self.vv.reportlog,variables=self.vv)
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
                self.v.reportlog.error(" pb ds majtime " + str(ee))
   
    def calculsCycliques(self,type):
        """ ce programme effectue les differents calculs cycliques necessairs à la simulation
          ces programme sont organisés par type:
          type="mesuresACPourEVSE"
                calcul le courant et la puissance globale ou par phase suivant le type de connection du vehicule
          """
          
        try:
            if(type=="mesuresACPourEVSE"):

                resul=dict() 
                #si on n'est pas en charge ou decharge pas la peine de calculer 
                if(self.vv.chamarche <3 ): return
                dicval=dict() # on va transmettre les resultats sous forme de dictionnaire
                if(not platform.platform().__contains__('Linux')):
                     poursel="SELECT name,value,timestamp FROM nodes WHERE (name=\"can_Vcurrent\") or (name=\"can_AmpCurrent\") or (name=\"int_LDEVSE\")"
                     vals=self.mybas.fetchmany(poursel)
                     if(len(vals)==0): raise Exception("  donnees non trouvee")
                     for val in vals:
                           dicval[val[0]]=(val[1],val[2])
                else:
                    dicval["can_Vcurrent"]=(str(self.vv.mempart.dicread("can_Vcurrent")),0) # on met 0  car le timestamp n'est pas enregidtre    
                    dicval["can_AmpCurrent"]=(str(self.vv.mempart.dicread("can_AmpCurrent")),0)
                    dicval["int_LDEVSE"]=(str(self.vv.mempart.dicread("int_LDEVSE")),0)
        
                dicval["time"]=str(getTemps(self.vv.tempssimule,self.vv.mempart))
                resul= self.vv.mesacts.monclient.postVals(prog="Calc",data=dicval,jeeson=True)
                if(resul["status"] != "ok"):
                   self.vv.reportlog.error(" CalculsCycliques " + resul["status"])
                   #self.v.util.message("erreur serveur CalsulsCycliques",type=="Warning")
                   raise Exception(" echanges serveur nok" )
            if("mesureWCurrent"):
                # on suppose une perte de P % "
                if(not platform.platform().__contains__('Linux')):
                    poursel="SELECT name,value,timestamp FROM nodes WHERE (name=\"can_Vcurrent\") or (name=\"can_AmpCurrent\")"
                    vals=self.mybas.fetchmany(poursel)
                    if(len(vals)==0): raise Exception("  donnees non trouvee")
                    if(self.vv.chamarche != 0):
                        wcurrent=float(vals[0][1])*float(vals[1][1])
                        wperte=float(getValfromdb(self.vv.dbpath,self.vv,"int_WPertes"))
                        Wcurrent=("LDEV1_DEEV1_WCurrent_mag_f",str(wcurrent-(random.random()*wperte*wcurrent)/100),str(getTemps(self.vv.tempssimule,self.vv.mempart)))    
                        saveToutVal(self.vv.dbpath,self.vv,Wcurrent)
                        self.vv.reportlog.info("calculs cycliques av db WCurrent=" + str(Wcurrent))
                else:
                    if(self.vv.chamarche != 0):
                        vcurrent=self.vv.mempart.dicread("can_Vcurrent")
                        acurrent=self.vv.mempart.dicread("can_AmpCurrent")
                        wcurrent=float(vcurrent)*float(acurrent)
                        wperte=float(self.vv.mempart.dicread("int_WPertes"))
                        wc=self.vv.mempart.dicread("IEDEVLDEV1/DEEV1.WCurrent.mag.f")
                        self.vv.reportlog.info("calculs cycliques  AVANT WCurrent=" + str(wc))
                        self.vv.mempart.dicwrite("IEDEVLDEV1/DEEV1.WCurrent.mag.f",wcurrent-(random.random()*wperte*wcurrent)/100)   
                        wc=self.vv.mempart.dicread("IEDEVLDEV1/DEEV1.WCurrent.mag.f")
                        self.vv.reportlog.info("calculs cycliques WCurrent=" + str(wc))
                
        except Exception as ee:
            raise Exception("calculsCycliques " + str(ee))


    def maj(self):
        """incremente le temps"""
        #self.vv.tempssimule=self.vv.tempssimule+1
        self.vv.ui.label_3.setText(time.strftime("%H:%M:%S", time.gmtime(self.vv.tempssimule)))
        self.vv.ui.label_3.show()
#-------------------------------------------------------------------

def saveToutVal(db,vv,val):
    """ sauve une valeur et sa valeur passee en un seul ordre
    la valeur est donnee sous forme d'un tuple (name,val,time)
    """
    try:
      poursel=" UPDATE nodes SET valueNmoins1=\"" + str(val[1]) + "\", timestampNmoins1=" + str(val[2]) + ", value=\"" + str(val[1])  +  "\", timestamp=" + str(val[2]) + " WHERE name=\""  + str(val[0]) +"\""
      execindb(db,vv,poursel)
    except Exception as ee: 
       raise Exception(" saveToutVal " + str(ee))

def saveNodeindb(db,vv,dicvalues):
    """
    sauve une  ou des valeurs dans une base de donnée
    bd:  fichier de la base
    vv: variables globales
    dicvalue: dictionnaire {nom:(val,time)}
    dblock: lock eventuel
    """
    try:
        
        d=dicvalues
        keys=d.keys()
        if(vv.sem != None): vv.sem.acquire() #(timeout=float(2))
        if(vv.ll!=None): vv.ll.acquire(blocking=True,timeout=2)
        conn=sqlite3.connect(db)
        for k in keys:
          poursel=" SELECT value,timestamp FROM nodes WHERE name=\"" + str(k) +"\""
          c=conn.execute(poursel) 
          val=c.fetchone() 
          poursel=" UPDATE nodes SET valueNmoins1=\"" + str(val[0]) + "\", timestampNmoins1=" + str(val[1]) + " WHERE name=\""  + str(k) +"\""
                #self.reportlog.info(" mybase.SAVE poursel=" + poursel )
          conn.execute(poursel)
                #self.executer(poursel,commit=False)
          
          poursel=" UPDATE nodes  SET value=\"" +  str(d[k][0]) +  "\", timestamp=" + str(d[k][1]) + "  WHERE name=\""  + str(k) +"\""
          c=conn.execute(poursel)
        conn.commit()
        conn.close()
        if(vv.ll!=None) :vv.ll.release()
        if(vv.sem != None): vv.sem.release()
            
    except Exception as ee: 
        if(vv.ll!=None) :vv.ll.release()
        if(vv.sem != None): vv.sem.release()
        raise Exception("saveNodeindb " + str(ee))

def saveOptindb(db,vv,dicvalues,mode="name"):
    """
    sauve une  ou des valeurs dans une base de donnée
    de maniere plus optimisee avec nb de connextions et transactions minimales
    bd:  fichier de la base
    vv: variables globales
    dicvalue: dictionnaire {nom:(val,time)} 
       note: en mode id le nom est remplace par id
    dblock: lock eventuel
    """
    try:
        
        d=dicvalues
        keys=d.keys()
        if(vv.sem != None): vv.sem.acquire() #(timeout=float(2))
        if(vv.ll!=None): vv.ll.acquire(blocking=True,timeout=2)
        conn=sqlite3.connect(db)
        for k in keys:
          if(mode=="id"):
              poursel=" SELECT value,timestamp FROM nodes WHERE id=" + str(k) 
          else:
              poursel=" SELECT value,timestamp FROM nodes WHERE name=\"" + str(k) +"\""
          c=conn.execute(poursel) 
          val=c.fetchone() 
          if(mode=="id"):
             poursel=" UPDATE nodes SET valueNmoins1=\"" + str(val[0]) + "\", timestampNmoins1=" + str(val[1]) + ", value=\"" +  str(d[k][0]) +  "\", timestamp=" + str(d[k][1]) + " WHERE id=" + str(k)
          else:
             poursel=" UPDATE nodes SET valueNmoins1=\"" + str(val[0]) + "\", timestampNmoins1=" + str(val[1]) + ", value=\"" +  str(d[k][0]) +  "\", timestamp=" + str(d[k][1]) + " WHERE name=\""  + str(k) +"\""
                #self.reportlog.info(" mybase.SAVE poursel=" + poursel )
          c=conn.execute(poursel)
        conn.commit()
        conn.close()
        if(vv.ll!=None) :vv.ll.release()
        if(vv.sem != None): vv.sem.release()
            
    except Exception as ee: 
                if(vv.ll!=None) :vv.ll.release()
                if(vv.sem != None): vv.sem.release()
                raise Exception("saveOptindb " + str(ee))

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
def getValfromdb(db,vv,nomval):
    """ recupere une valeur d'une  base """
    poursel="SELECT value FROM nodes WHERE name=\""+ nomval + "\""
    val=fetchindb(db,vv,poursel,mode="one")[0]
    return val

def setValtobd(db,vv,nomval,value,time):
    """ met a jour  une valeur d'une  base """
    try:
        poursel=" UPDATE nodes SET value=\"" + value + "\", timestamp=" + str(time) + " WHERE name=\"" + nomval + "\""
        execindb(db,vv,poursel)
    except Exception as ee:
        vv.reportlog.error(" errer setval " + poursel + " " + str(ee))

def fetchindb(dbpath,vv,poursel,mode="one",size=500):
  try:
     #vv.reportlog.info("avant acquire fetchinbd poursel=" + pourcel)
     if(vv.sem != None): vv.sem.acquire()  #(timeout=float(2))
     if(vv.ll!=None): vv.ll.acquire(blocking=True,timeout=2)
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

