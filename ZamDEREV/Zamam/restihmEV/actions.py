#/usr/bib/python3
# -*- coding: utf-8 -*-
#  classe regroupant les actions possibles des menus de l'interface IHM de la maquette vehiculle
#
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v1.2.1   |20/03/21 |  -   uart                                                     | AM   #
#-------------------------------------------------------------------------------------------#
#                                                                                           #
#-------------------------------------------------------------------------------------------#
#            paramètres                                                                     #
#-------------------------------------------------------------------------------------------#
TrialogRest="0.0.0.0:5001" # adresse de la tâche rest trialog
ZamirenRest="127.0.0.1:5000" # adresse de la tâche rest zamiren

#-----------------------------------------------
from PyQt5 import  QtWidgets,QtCore,QtGui
from PyQt5.QtCore import Qt
#from essai import Ui_MainWindow
from nouvelleihm import Ui_MainWindow
#import matplotlib
#matplotlib.use("qt5agg")
#import ComplementsIHM as cihm
import os
import time
import datetime
import threading
import logging
import utilitaires as util
from  Mytable import mytable as mytable
from Mybase import mybase as mybase
import sqlite3
import platform
from simulEV import simulEV as simulEV
from ZamPasserelleEV import passerelle as passerelle
from myclienthttp import myclient as myclient
import pdb
import echuart
if(not platform.platform().__contains__('Linux')):
    class cable:
        def __init__(self):
            pass
else:
    import cable



 
class mesactions: 
    cyclique=False
    timedepart=0
    timestart=0
    
    def __init__(self,variables):
        self.v=variables
        #self.timer=QtCore.QTimer(self.v.ui.tableWidget)
        #self.timer.timeout.connect(self.monaffichage)
        self.changedCells=list()
        self.chaSaisie=[0,0,0,0,0,0,0,0] # de 1 à 3 saisie heure 1ere ligne , 4-7 saisie heure 2 ieme ligne
        self.chamaxpos=0 # charge max possible a demander
        self.monuart = None
        self.transacID=0
  

    def afficOneShot(self):
        self.ui.PBSave.hide() # fait disparaitre le bouton de sauvegarde
        if(self.v.cyclique):
            self.timer.stop()
            self.cylclique=False

        self.monaffichage()

    def monaffichage(self):
        if(not platform.platform().__contains__('Linux')):
            try:
                self.cyclique=False
 
                if(self.v.withids):  
                 self.v.mybas.setTable(self.v.mytabl,action="afficsomeids",ids=self.v.idsaffich)
                else:
                 self.v.mybas.setTable(self.v.mytabl,action="all",ids=self.v.idsaffich)
                self.v.mytabl.initTableWidget(self.v.ui.tableWidget)
            
                self.v.ui.tableWidget.show()
                #self.v.reportlog.info(" mon affichage" + heure)
            
            except Exception as ee:
                self.v.reportlog.error("my display error" + str(ee))
        else:
            self.v.util.message("not implemented under Linux")


    def monafficher_cycliquement(self):
        if(not platform.platform().__contains__('Linux')):
            try:
              self.ui.PBSave.hide() # fait disparaitre le bouton de sauvegarde
              self.v.cyclique=False
              self.timer.start(5000) # 
              self.v.cyclique=True
              prov=1
            except Exception as ee:
                self.v.reportlog.info(" lancement du thread " + str(t) + " " + str(ee))
                self.v.reportlog.error("erreur appel monafficher_cycliquement " + str(ee))
        else:
            self.v.util.message("non implementee sous linux")


    
    def afficVarsBorne(self):
        if(not platform.platform().__contains__('Linux')):
            try:
                self.cyclique=False
                self.v.tableborne.reset(); # on remet la table à 0
                #on recupere la liste des noms des variables
                poursel="SELECT VAR FROM visuborne"
                lignes=self.v.util.fetchindb(self.v.dbpath,self.v,poursel,mode="many")
                nomlist=list()
                for ligne in lignes:
                    nom=ligne[0]
                    nomlist.append(nom)
            
                poursel="SELECT name,format,value,FC FROM nodes where (name="
                ri=range(0,len(nomlist))
                for i in ri:
                        if(i==len(nomlist)-1):
                             poursel=poursel + "\"" + nomlist[i] +"\")"
                        else:
                             poursel=poursel + "\""+  nomlist[i] +"\")"+ " or (name="
               
                vals=self.v.util.fetchindb(self.v.dbborne,self.v,poursel,mode="many")

                if(platform.platform().__contains__('Linux')):
                    # dans le cas de linux on accede aux donnees 61850
                   for val in vals:                
                       ad="IEDCS" + val[0]       
                       ad=val[0]
                       fc=val[3]
                       type=val[1]
                       value=self.v.util.getValeur([ad,fc,type],adserver=v.adservercs)
                       ligne=[val[0],value]
                       self.v.tableborne.setRow(100,ligne)
                else:
                   for val in vals:
                       ligne=[val[0],val[2]]
                       self.v.tableborne.setRow(100,ligne)
              
                self.v.tableborne.initTableWidget(self.v.ui.tableWidget_2)
                self.v.ui.frame_4.show()
                self.v.ui.tableWidget_2.show()
          
            except Exception as ee:
                self.v.reportlog.error(" AfficVarsBorne " + str(t) + " " + str(ee))
        else:
            self.v.util.message("not yet implemented under linux")

    def afficVarsBorneCycliquement(self):
        if(not platform.platform().__contains__('Linux')):
            try:
              self.ui.PBSave.hide() # fait disparaitre le bouton de sauvegarde
              self.v.cyclique=False
              self.timer.start(5000) # 
              self.v.cyclique=True
              prov=1
            except Exception as ee:
                self.v.reportlog.error(" monafficher_cycliquement  " + str(t) + " " + str(ee))
                
        else:
            self.v.util.message("non implementee sous linux")

    def monmise_jour(self):
        if(not platform.platform().__contains__('Linux')):
          self.monaffichage() # affiche tableWidget reflètée par mytabl
          self.ui.PBSave.show() # fait apparaitre le bouton de sauvegarde
          #util.message(" non implémenté")
          self.changedCells=list() # liste des cellules changées
          self.v.ui.tableWidget.cellChanged.connect(self.montracecell)
        else:
          self.v.util.message("non implementee sous linux")


    def montracecell(self):
        if(not platform.platform().__contains__('Linux')):
            try:
                """ verifie et accumule les changements de cellule """
                row=self.v.ui.tableWidget.currentRow()
                col=self.v.ui.tableWidget.currentColumn()
                if(col != 2):
                    util.message("not allowed on this column",type="Warning")
                else: # autorise
                   id=self.v.ui.tableWidget.item(row,0).text() # l'id de la mesures
                   poursel=" SELECT format,type FROM nodes WHERE id=" + str(id)
                  # cur = self.v.mybas.conn.cursor()
                  # cur.execute(poursel)
                  # res=cur.fetchone()
                   res=self.v.mybas.fetchone(poursel)
                   value=self.v.ui.tableWidget.item(row,col).text()
                   ok=util.verifcell(value,res)
                   if(ok):
                      self.changedCells.append((id,value))
            except Exception as ee:
                   self.v.reportlog.error(" montracecell " + str(ee))
        else:
            self.v.util.message("non implementee sous linux")

           
    def monsave(self):
        if(not platform.platform().__contains__('Linux')):
            try:
                for res in self.changedCells:
                    poursel=" SELECT value FROM nodes WHERE id=" + str(res[0])
                    val=self.v.mybas.fetchone(poursel)  
                    poursel=" UPDATE nodes SET valueNmoins1=" + str(val[0]) + " WHERE id=" + str(res[0])
                    self.v.mybas.executer(poursel,commit=True)
                    poursel=" UPDATE nodes  SET value=" + str(res[1]) +  "  WHERE id=" + str(res[0])
                    c=self.v.mybas.executer(poursel,commit=True)
                    prov="a"
                  #util.message(" non implémenté")
                    #self.monaffichage()
            except Exception as ee:
                  self.v.reportlog.error(" monsave " + str(ee))
        else:
                  self.v.util.message("non implementee sous linux")

    def monChaOn(self):
        try:
            self.v.reportlog.info(" monChaOn typ socmax={0}".format(type(self.v.socmax)))
            self.v.mode=""
            if(((self.chaSaisie[0]+self.chaSaisie[1])==0) and (self.v.chaAlerte==0)): self.v.mode="dynamic" # le fait qu'il s'agit du mode dynamique est fixé par la date depart=0           
            if((self.chaSaisie[0]+self.chaSaisie[1])>0) or (self.v.chaAlerte > 0 ): self.v.mode="schedule"# le demarage peut avoir lieu
            self.v.reportlog.info("monChaOn chasaisie={1} mode={0}".format(self.v.mode,self.chaSaisie))
            if(self.v.mode != ""):   
                if(not platform.platform().__contains__('Linux')):
                    poursel="SELECT value FROM nodes WHERE name=\"" + "LDEV1_DEEV1_EVSt_stVal\"" 
                    val=util.fetchindb(self.v.dbpath,self.v,poursel)[0]
                else:
                    val=str(self.v.mempart.dicread("IEDEVLDEV1/DEEV1.EVSt.stVal"))
                if(val != '1'):          #19=='1'):
                    util.message("  pause on/off ")
                    # si la simulation est en cours  on la pause sinon on la libere
                    if(self.v.pause.is_set()):
                        # on doit bloquer la simulation
                        self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.WCurrent.mag.f",0.0) 
                        self.v.ui.PBChaOn.setStyleSheet("color: rgb(0, 0, 0);")#noir
                        self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.EVSt.stVal",10+int(val))

                        self.v.pause.clear()

                    else:
                        self.v.ui.PBChaOn.setStyleSheet("color: rgb(0, 0, 255);")#bleu
                        if(int(val) > 10): self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.EVSt.stVal",int(val)-10)
                        self.v.pause.set()
                else:
                    self.v.reportlog.info("monChaON required soc={0}".format(self.v.socDemandee))
                    sens="positif"
                    #  si nous sommes en autonome il faut choisir une courbe de charge
                    if(self.v.ui.checkBox_2.isChecked()): # sens positif est-ce que le bouton charger est actionné
                        if(self.v.socDemandee==0):
                            self.v.socDemandee=self.v.socmax
                        self.ui.lineEdit.setText(str(int(self.v.socDemandee)))
                        poursel="SELECT valeur FROM gestion WHERE nom=\"CBATCha\""
                    else:#sens negatif
                        if(self.v.socDemandee ==0):
                            self.v.socDemandee = self.v.socmin
                        self.ui.lineEdit.setText(str(int(self.v.socDemandee)))
                        self.v.reportlog.info(" action ecriture lineEdit {0} socdemandee".format(self.v.socDemandee))
                           # print("ds action ligne 179  " + str(self.v.socDemandee))
                        poursel="SELECT valeur FROM gestion WHERE nom=\"CBATDecha\""
                        sens="negatif"
                        #self.v.reportlog.info("TEST ligne 186") #test
                    val=self.v.mybas.fetchone(poursel)
                    if(len(val)==0): raise Exception("pas de courbe de batterie trouvee")
                    self.v.ui.PBChaOn.setStyleSheet("color: rgb(0, 0, 255);")
                    poursel="SELECT cVPtBas,cVLin,cVIntermed,cVPtHaut,cVPas,cVOffset,irtg FROM BatCourbes WHERE id=\""+ val[0][0] + "\""
                    vals=self.v.mybas.fetchone(poursel)
                    if(len(vals)==0): raise Exception(" erreur dans demarrage chargement ")
                    prov=vals
                    min=eval(vals[0])
                    lineaire=eval(vals[1])
                    intermed=eval(vals[2])
                    max=eval(vals[3])
                    pas=vals[4]
                    offset=vals[5]
                    irtg=vals[6]
                   
                    #---calcule courbe de charge V/SOC -------------------
                    #if(self.v.ui.checkBox_4.isChecked()):
                       # if(self.v.ui.checkBox_2.isChecked()): # sens positif 
                    #if(type.__contains__("positif")):  
                    pointsV=self.v.util.calculCourbeCharge(min=min,lineaire=lineaire,intermed=intermed,max=max,pas=pas,offset=offset,sens=sens)
                    ri=range(0,len(pointsV))
                    pointsI=list()
                    for i in ri:
                        point=pointsV[i]
                        #chamax=float(self.v.chaMax)
                        pointI=pas*self.v.chaMax/point[1]
                        pointsI.append((point[0],pointI))
                    self.v.courbeV=self.v.util.mycourbe(pointsV,self.v)
                    #self.v.courbeV.dump()
                    self.v.courbeI=self.v.util.mycourbe(pointsI)
               
                    dicval=dict()
                    if(self.v.mode=="dynamic"):
                         if(not platform.platform().__contains__('Linux')):
                            dicval[str(self.v.NomVersIdDic["DptTm"])]=[ str(0) ,str(self.v.util.getTemps(self.v.tempssimule,self.v.mempart))] 
                         else:
                            self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.DptTm.setTm",0)
                            self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.StartTm.setTm",0)
                         if(sens=="positif"):
                             self.chamaxpos=(self.v.socmax -self.v.initsoc)*self.v.chaMax/100
                    else:
                         if(not platform.platform().__contains__('Linux')):
                            dicval[str(self.v.NomVersIdDic["DptTm"])]=[ str(self.timedepart) ,str(self.v.util.getTemps(self.v.tempssimule,self.v.mempart))]
                         else:
                            if(sens=="positif"):
                                self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.DptTm.setTm",self.v.timedepart)
                                self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.StartTm.setTm",self.v.timestart)
                                self.chamaxpos=(self.v.socDemandee -self.v.initsoc)*self.v.chaMax/100
                            else:
                                self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.StartTm.setTm",self.v.timedepart)
                                self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.DptTm.setTm",self.v.timestart)
                                self.chamaxpos=abs((self.v.socDemandee -self.v.initsoc))*self.v.chaMax/100 #self.v.whreserve

                    st=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.StartTm.setTm")
                    dt=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.DptTm.setTm")
                    self.v.reportlog.info(" monChaOn DptTm={0} StartTm={1}".format(dt,st))
                    if(not platform.platform().__contains__('Linux')):
                        dicval[str(self.v.NomVersIdDic["WhTgt"])]=[  str(self.chamaxpos)  ,str(self.v.util.getTemps(self.v.tempssimule,self.v.mempart))] 
                        self.v.mybas.save(dicval)
                    else:
                        
                        self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.WhTgt.setMag.f",self.chamaxpos)
                        self.v.reportlog.info("Action ecriture dans IEDEVLDEV1/DEEV1.WhTgt.setMag.f de {0}".format(self.chamaxpos))

                    #---------------------
                    #self.v.chamarche=1 #la fonction sera démaree par la passerelle sur autorisation de la borne

                   # self.v.ui.comboBox.setEditable(False) #empêche clicks
                   # self.v.ui.comboBox_2.setEditable(False)
                    self.v.ui.lineEdit.setReadOnly(True)
                    
                    #self.v.sim.start() #la fonction sera démaree par la passerelle sur autorisation de la borne
                    if(not platform.platform().__contains__('Linux')):
                        util.saveToutVal(self.v.dbpath,self.v,("int_ChargeMarche",0,0))
                        dicval=dict()
                        #dicval["LDEV1_DEEV1_EVSt_stVal"]=('2',str(self.v.util.getTemps(self.v.tempssimule,self.v.mempart)))
                        dicval["int_ChargeMarche"]=('2',str(self.v.util.getTemps(self.v.tempssimule,self.v.mempart)))
                        util.saveOptindb(self.v.dbpath,self.v,dicval) # on positionne EVSt a ready
                    else:
                        if(sens=="positif"):
                            self.v.mempart.dicwrite("int_ChargeMarche",2) # charge
                        else:
                            self.v.mempart.dicwrite("int_ChargeMarche",3) # decharge
                        #self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.EVSt.stVal",2)     
                    self.v.reportlog.info("passage par Marche int_charge=" + "2 ou 3"  ) #test
            else:
               util.message(" demmarrage  saisie incomplète")
               self.v.reportlog.info(" demmarrage  saisie incomplète" )
        except Exception as ee:
              self.v.reportlog.error(" monChaOn " + str(ee))
              raise Exception(" monChaOn " + str(ee)) 

    def monChaOff(self):
        try:
             if(not platform.platform().__contains__('Linux')):
                 poursel=" SELECT value FROM nodes WHERE name=\"LDEV1_DEEV1_EVSt_stVal\""
                 val=self.v.util.fetchindb(self.v.dbpath,self.v,poursel)[0]
             else:
                 val=str(self.v.mempart.dicread("IEDEVLDEV1/DEEV1.EVSt.stVal"))
             
             if(val != '1'): # VE n'est pas au repos
                 self.v.pause.is_set()
                 self.chaSaisie=[0,0,0,0,0]
                 self.v.ui.lineEdit.setText("00000.00")
                # self.v.ui.comboBox.setEditable(True) #empêche clicks
                # self.v.ui.comboBox_2.setEditable(True)
                 #self.v.ui.lineEdit.setReadOnly(False)
                 self.chamaxpos=0.0
                
                 self.v.reportlog.info("MonChaOff chamaxpos mis à {0}".format(self.chamaxpos))
                 self.ui.LChaMaxPosVal.text=str(self.chamaxpos)
                 self.v.ui.PBChaOn.setStyleSheet("color: rgb(0, 0, 0);")
                 
                 if(not platform.platform().__contains__('Linux')):
                     util.setValtobd(self.v.dbpath,self.v,"int_ChargeMarche","0",self.v.util.getTemps(self.v.tempssimule,self.v.mempart));
                     util.setValtobd(self.v.dbpath,self.v,"LDEV1_DEEV1_EVSt_stVal","1",self.v.util.getTemps(self.v.tempssimule,self.v.mempart));
                 else:
                     self.v.mempart.dicwrite("int_ChargeMarche",0)
                     self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.EVSt.stVal",1)
                 if((self.v.chamarche==1) or (self.v.chamarche==3)):
                  #self.v.sim.stop() # on arrete la simulation
                    #self.v.chamarche=0 # on est au repos 
                    if(self.v.chamarche==1):
                        self.v.chamarche=2 # on demande l'arret au simul
                        self.ui.checkBox_2.setChecked(False)
                    else:
                        self.v.chamarche=2 # on demande l'arret au simul
                        self.ui.checkBox.setChecked(False)    
                 self.v.reportlog.info(" Charge ou decharge arrêtée")
                 util.message(" Charge ou decharge arrêtée")
             else:
                util.message(" arret inutile: pas de charge ou decharge en preparation ou en cours")
                self.v.reportlog.info(" arret impossible: pas de charge ou decharge en cours" )

        except Exception as ee:
              self.v.reportlog.error(" monChaOff " + str(ee))
              raise Exception(" monChaOff " + str(ee))


    def chargeok(self):
        util.setValtobd(self.v.dbpath,self.v,"int_ChargeMarche","1",self.v.util.getTemps(self.v.tempssimule,self.v.mempart));
   
    def libercon(self):
 
        dicval=dict()
        dicval["time"]=str(self.v.util.getTemps(self.v.tempssimule,self.v.mempart))
        ok=True
        if(self.v.isRest):
            resul= self.monclient.postVals(prog="libcon",data=dicval,jeeson=True)
            if(resul["status"] != "ok"):
                self.v.reportlog.error(" deconnexions " + str(resul["status"]))
                
            else:
                 self.v.reportlog.info("connexions liberees ")
        elif(self.v.isSerial):
                payload={"id":self.transacID,"verb":"libcon","params":dicval}
                resul=self.monuart.post(payload)
                self.transacID=self.transacID+1
                if(resul[0] != "ok"):
                     self.v.reportlog.error(" deconnexions " + str(resul[1]))
                    
                else:
                     self.v.reportlog.info("connexions liberees ")
        elif(self.v.isFilaire):  # il faut remettre le gpio à 0
                self.v.cabl.resetGPIO()
                self.v.cabl.setEtat(3)

    def monforcer(self):
          util.message(" non implémenté")

    def monarr_ter(self):
          #util.message(" non implémenté")
          self.v.reportlog.info(" arret demande")
          #cable.resetGPIO()
          self.v.sim.termine()
          self.v.app.quit()
          #raise Exception(" arret volontaire")

    def moneloigne(self):
          #util.message(" non implémenté")
          self.ui.checkBox_4.setText("wifi")
          self.ui.checkBox_4.setCheckState(QtCore.Qt.Unchecked) 
          if(not platform.platform().__contains__('Linux')):
            self.v.util.saveToutVal(self.v.dbpath,self.v,("int_adresseCS","",self.v.util.getTemps(self.v.tempssimule,self.v.mempart)))
          else:
            self.v.mempart.dicwrite("int_adresseCS","")
          self.v.sim.activite=False # on arrete udp

    def notImplemented(self):
            util.message("Pas encore implémenté")

    def changeItem(self):
     selectmdel=QtCore.QitemSelectionModel(don.model) # pour suivre les selections des items
     qselindex=selectmodel.currentindex()
     citem=open.model(qselindex)

    def CBwifi(self):
        #pdb.set_trace()
        if(self.ui.checkBox_4.isChecked()): # le vehicule est proche d'une borne
                if(not platform.platform().__contains__('Linux')):
                     self.v.mybas.savenom({"int_wifi":("1",self.v.util.getTemps(self.v.tempssimule,self.v.mempart))})
                else:
                     self.v.mempart.dicwrite("int_wifi",True)
                     if(self.v.reportlog != None):
                       memwifi=self.v.mempart.dicread("int_wifi")
                       self.v.reportlog.info("CBWifi en memoire partage int_wifi= "  + str(memwifi))
                       if(memwifi==True):          
                            self.v.reportlog.info( "ev met wifi a True")
                fois=5
                time.sleep(1)
                if(not platform.platform().__contains__('Linux')):
                    poursel=" SELECT value FROM nodes WHERE  name=\"int_adresseCS\""
                    valeur=(self.v.mybas.fetchone(poursel))[0]
                else:
                    valeur=self.v.mempart.dicread("int_adresseCS")
                    
                while(valeur.strip(" ")==""):
                    fois=fois-1
                    
                    if(fois<0):
                        if(self.v.reportlog != None):self.v.reportlog.info("CBWifi mauvaise  int_adressseCS arret  ")
                        self.v.sim.termine()
                        self.v.app.quit()
                    time.sleep(2)
                    if(not platform.platform().__contains__('Linux')):
                        valeur=(self.v.mybas.fetchone(poursel))[0]
                    else:
                        valeur=self.v.mempart.dicread("int_adresseCS")
                        #valeur="192.168.1.41"
                        
                if(self.v.reportlog != None):self.v.reportlog.info("CBWifi  int_adressseCS {0}  isRest {1}  ".format(valeur,self.v.isRest))   
                    #print("lecture base EV adresse =" +str(valeur))
                if(self.v.isRest):     
                    self.monclient=myclient(adresse=valeur + ":" + str(self.v.servport))
                elif(self.v.isSerial):
                    self.monuart=echuart.uart()
                elif(self.v.isFilaire):
                    self.v.reportlog.info("CBWIFI appel creation du cable")                 
                    self.v.cabl=cable.cable(self.v,type="ev")
                    time.sleep(.2)
                    self.v.cabl.prepareGPIO(self.v.typCon)             
                    self.ui.checkBox_5.setChecked(True)
                    time.sleep(.2)
                    self.v.ui.checkBox_3.setChecked(True)
               # if(self.v.reportlog != None):self.v.reportlog.info(" adresee serveur connexion " + valeur + ":" + str(self.v.servport))
               # if(self.v.reportlog != None):self.v.reportlog.info("demande connexion voiture")
                #self.ui.checkBox_5.setChecked(True)
        else:  # le vehicule s'éloigne
                if(not platform.platform().__contains__('Linux')):
                  self.v.mybas.savenom({"int_wifi":("0",self.v.util.getTemps(self.v.tempssimule,self.v.mempart))})
                  self.ui.LAlerte.setText("")
                else:
                  self.v.mempart.dicwrite("int_wifi",False)
                  self.ui.LAlerte.setText("")

                self.v.reportlog.info( "ev met wifi a 0")
                if(platform.platform().__contains__('Linux')):
                   if(True):
                       if(self.v.sim.ttemps.is_alive()):
                           self.v.sim.stoptempo()

    def CBSimulationChanged(self):
         if(self.ui.checkBox_4.isChecked()):
            texte=self.ui.checkBox_4.text()
            if(self.v.isRest):
                if(self.v.isautonome): self.ui.checkBox_3.show() # chckbox 'connecter' visible si autonome
            poursel=" UPDATE gestion  SET valeur=" +  "'OUI'" + "  WHERE nom=" + "'simulation'"
            self.v.mybas.executer(poursel,commit=True)
            #util.message("lancement des taches cycliques liees aux simulation de mesure")
            self.v.reportlog.info("lancement des taches cycliques liees aux simulation de mesure")
            #if(not platform.platform().__contains__('Linux')):
            self.v.horloge=self.v.util.monohrloge(name="horl",bdd=None,variables=self.v)
            self.v.horloge.start()
            self.v.pasrel.evpasrel.set()
            #self.v.sim.start()
         else: # on arrete 
                 poursel=" UPDATE gestion  SET valeur=" +  "'NON'" + "  WHERE nom=" + "'simulation'"
                 self.v.mybas.executer(poursel,commit=True)
            #util.message("fin de la simulation")
                 self.v.pasrel.evpasrel.clear()
                 self.ui.checkBox_5.setChecked(False)    

    def CBChargerChanged(self):
        try:
             
             if(self.v.ui.checkBox_2.isChecked()):
                 if(not platform.platform().__contains__('Linux')):
                     poursel="SELECT value FROM nodes WHERE id=" +  str(self.v.NomVersIdDic["can_ConnSt"]) # on regarde si on est connecté
                     # am 20 connecte=self.v.mybas.fetchone(poursel) 
                     connecte=self.v.mybas.fetchone(poursel)[0]
                 else:
                     connecte=str(self.v.mempart.dicread("can_ConnSt"))

                #am 20 if(connecte[0]=='1'): #oui on est connecte
                 if(connecte=='1'): #oui on est connecte
                    if(not platform.platform().__contains__('Linux')):
                        poursel="SELECT value FROM nodes WHERE name=\"" + "LDEV1_DEEV1_EVSt_stVal" + "\""
                        val=self.v.util.fetchindb(self.v.dbpath,self.v,poursel)[0]
                    else:
                        val=str(self.v.mempart.dicread("IEDEVLDEV1/DEEV1.EVSt.stVal"))
                    if(val == '1'): # est ce que la voiture est au repos
                            #pdb.set_trace()
                            if(not platform.platform().__contains__('Linux')):
                                poursel="SELECT value FROM nodes WHERE id=" + str(self.v.NomVersIdDic["SOC"])
                                soc=(self.v.mybas.fetchone(poursel))[0]
                            else:
                                soc=str(self.v.mempart.dicread("IEDEVLDEV1/DEEV1.Soc.mag.f"))

                            if(soc.__contains__('')):
                                soc=soc.replace("'","")
                            self.v.initsoc=float(soc) # soc initiale
                            self.v.ui.label_2.setText( "CC")
                            self.v.ui.label_2.hide() # pas de suivi autonome
                            self.v.ui.comboBox_3.hide() # pas de suivi autonome
                            self.v.ui.LChargt.setText( "Charging")
                            self.v.ui.label.setText( "Required Soc")
                            self.v.ui.label_5.setText("Current Soc")
                            self.v.ui.LChaMaxPosVal.setText(str(self.v.initsoc))
                            #self.v.ui.LChaMaxPosVal.show()
                            self.v.ui.LHeureDpt.setText("departure hour")
                            self.v.ui.LHBegin.setText("beginning hour")
                            self.v.ui.frame_3.show() # dialogue de charge montre
                            self.chaSaisie[0]=0
                            self.chaSaisie[1]=0
                            self.chaSaisie[2]=0
                            self.chaSaisie[3]=0
                    else: # non on ne peut donc commence un chargement
                           #self.v.ui.frame_3.hide()
                           util.message("moving car")
                           self.ui.checkBox_2.setChecked(False)    
                 else:
                    #self.v.ui.frame_3.hide()
                    util.message("car not connected")
                    self.ui.checkBox_2.setChecked(False)
             else:
                 self.v.ui.frame_3.hide()
                 
        except Exception as ee:
            raise Exception("erreur CBChargerChanged " + str(ee))

    def  CBDechargerChanged(self):
         if(self.ui.checkBox.isChecked()):
             if(not platform.platform().__contains__('Linux')):
                 poursel="SELECT value FROM nodes WHERE id=" +  str(self.v.NomVersIdDic["can_ConnSt"]) # on regarde si on est connecté
                 connecte=self.v.mybas.fetchone(poursel)[0]   
             else:
                 connecte=str(self.v.mempart.dicread("can_ConnSt"))
             if(connecte=='1'): #oui on est connecte
                  if(not platform.platform().__contains__('Linux')):
                     poursel="SELECT value FROM nodes WHERE name=\"" + "LDEV1_DEEV1_EVSt_stVal" + "\""
                     val=util.fetchindb(self.v.dbpath,self.v,poursel)[0]
                  else:
                     val=str(self.v.mempart.dicread("IEDEVLDEV1/DEEV1.EVSt.stVal"))
                  if(val == '1'): # est ce que la voiture est au repos 
                         if(not platform.platform().__contains__('Linux')):
                            poursel="SELECT value FROM nodes WHERE id=" + str(self.v.NomVersIdDic["SOC"])
                            soc=(self.v.mybas.fetchone(poursel))[0]
                         else:
                            soc=str(self.v.mempart.dicread("IEDEVLDEV1/DEEV1.Soc.mag.f"))
                            if(self.v.chaAlerte==0): # ne mettre socDemandee à 0 que en manuel
                                self.v.socDemandee=float(0)
                         #print("ds action ligne 363  " + str(self.v.socDemandee))
                         if(soc.__contains__('')):
                            soc=soc.replace("'","")
                         self.v.initsoc=float(soc) # soc initiale
                         self.v.ui.LChaMaxPosVal.setText(str(self.v.initsoc))
                         self.v.ui.LChaMaxPosVal.show()
                         self.v.ui.label_2.setText( "CD")
                         #self.v.util.setComboBox(self.v,self.v.ui.comboBox_3,table="BatCourbes",type="negatif3seg")
                         self.v.ui.comboBox_3.hide() # on est dans une demarche de suivi opportuniste (pas de courbe de charge)
                         self.v.ui.label.hide()
                         self.v.ui.LChargt.setText( "Discharging")
                         self.v.ui.label_5.setText("Current Soc")
                         self.v.ui.LChaMaxPosVal.setText(str(self.v.initsoc))
                         self.v.ui.LHeureDpt.setText("beginning hour")
                         self.v.ui.LHBegin.setText("end hour")
                         self.v.ui.frame_3.show() # dialogue de charge montre   
                         self.chaSaisie=[0,0,0,0]
                         self.v.ui.frame_3.show()
                  else: #  en charge ou decharge
                      self.v.ui.frame_3.hide()
                      util.message("moving car")
                      self.ui.checkBox.setChecked(False)
             else:
                self.v.ui.frame_3.hide()
                util.message("car not connected")
                self.ui.checkBox.setChecked(False)
         else:
            self.v.ui.frame_3.hide()
           


    def CBConnChanged(self):
        try:  
           #  monplot=cihm.myplot(self.v,test="legend")
            if(self.v.ui.checkBox_3.isChecked()): # demande de connexion
                if(self.v.mempart.dicread("can_ConnSt")==1):return #deja connecte
                selection=self.v.typCon
                self.v.reportlog.info("CBConnChanged  self.v.typCon {0}".format(selection))
            #rep=util.message( "confirmer? " + selection ,type="Question",boutons=["Yes","No"])
                self.v.reportlog.info( "connexion confirmée " + selection )
                rep = QtWidgets.QMessageBox.Yes
                if(rep!=QtWidgets.QMessageBox.Yes): #non
                    return
                # mise a jour du typpe de charge demandee
                dicval=dict()
                if(selection.startswith("vac")): 
                    typ="1" # Single Phase
                if(selection.startswith("vtri")): 
                    typ="2" # Tri Phase"
                if(selection.startswith("vdc")): typ="5" # DC"
                self.v.reportlog.info("selection cable selection={0}".format(selection))
                if(not platform.platform().__contains__('Linux')):
                     dicval["LDEV1_DEEV1_ConnTypSel_stVal"]=[typ,str(self.v.util.getTemps(self.v.tempssimule,self.v.mempart))] 
                     self.v.mybas.savenom(dicval)
                else:
                     self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.ConnTypSel.stVal",int(typ))

                # il faut indique à la charging station la demande de connexion 
                dicval=dict()
            
                poursel="SELECT type,EVId,capaciteCableAC,capaciteCableDC from voitures WHERE type="+"\""+selection+"\""
                        #vals=self.v.simulbas.fetchmany(poursel)
                vals=self.v.mybas.fetchmany(poursel)  
                dicval["EVId"]=vals[0][1]
                dicval["capac"]=vals[0][2]
                dicval["capdc"]=vals[0][3]
                dicval["type"]=typ
                dicval["time"]=str(self.v.util.getTemps(self.v.tempssimule,self.v.mempart))
                if(not self.v.isFilaire):
                    bon=True
                    
                    if(self.v.isRest):
                        #self.v.reportlog.info("CDConnChanged avant {0}".format(self.monclient.postVals))
                        resul= self.monclient.postVals(prog="cs",data=dicval,jeeson=True)
                        self.v.reportlog.info("CDConnChanged après {0}".format(self.monclient.postVals))
                        if(resul["status"] != "ok"):
                            self.v.reportlog.error(" selection voiture sur station " + resul["status"])
                            self.v.util.message("erreur serveur selection voiture ="+ resul["status"] ,type=="Warning")
                            bon=False
                        else:
                            adborne=resul["prefixevse"][0]
                            self.v.monclientborne=myclient(adresse=adborne + ":" + str(self.v.servport))
                            res=self.v.monclientborne.postVals(prog="cs",data=dicval,jeeson=True)
                            if(res["status"] != "ok"): 
                                self.v.reportlog.error(" selection  voiture sur borne  " + res["status"])
                                self.v.util.message("erreur serveur selection voiture ="+ resul["status"] ,type=="Warning")
                                bon=False
                            self.v.reportlog.error(" selection  voiture sur borne  " + res["status"])
                    elif(self.v.isSerial):
                        payload={"id":self.transacID,"verb":"connecte","params":dicval}
                        resul=self.monuart.post(payload)
                        self.transacID=self.transacID+1
                        if(resul[0] != "ok"):
                            self.v.reportlog.error(" selection voiture " + resul[1])
                            self.v.util.message("erreur serveur selection voiture ="+ resul[1] ,type=="Warning")
                            bon=False

                   # if(len(resul==0)): ATTENTION test faux sur bool
                       # self.v.reportlog.error(" selection voiture le CR de la connexion avec Zamserver est vide")
                       # self.v.util.message(" le CR de la connexion avec Zamserver est vide")
                       # return
                    #if(len(resul) != len(ret)): raise Exception( "Erreur connexion retour serveur rest")
                else: # cable en pur filaire
                    
                  
                    # on attend la connexion du cable en faisant bagoter le fil ad-hoc ( AC ou DC)

                    j=6
                    bon=True
                    if(self.isRest): #rest
                       pass
                    else:
                        while(j > 0):
                            ad=self.v.mempart.dicread("int_adresseSE")
                            self.v.reportlog.info("CBConnChanged int_adresseSE {0}".format(ad))
                            if(ad !=""):
                                self.v.evseref=ad
                                bon=True
                                self.v.ui.checkBox_3.show()
                                break
                            time.sleep(2)
                            j=j-1
                            if(j <=0 ):
                                     repons=util.message("you can connect",type="Question",boutons=["Yes","No"])
                                     if(repons==QtWidgets.QMessageBox.Yes):
                                        j=6
                                        self.v.reportlog.info("force connection cable  selection={0} ".format(selection))
                                        if(self.v.typCon.startswith("vac") or self.v.typCon.startswith("vtri") ): # connexion en AC
                                            #self.v.etatgpio=3
                                            #while(self.v.etatgpio==3):
                                              #  time.sleep(.1)
                                            #self.cabl.setetat(3)
                                            self.v.cabl.connect(kind="AC")
                                            bon=True
                                        elif(self.v.typCon.startswith("vdc")):
                                            #self.v.etatgpio=3
                                            #while(self.v.etatgpio==3):
                                               # time.sleep(.1)
                                            self.v.cabl.connect(kind="DC")
                                            bon=True
                                    
                                     elif(repons==QtWidgets.QMessageBox.No):
                                        bon=False    
                                        if(self.v.typCon.startswith("vac") or self.v.typCon.startswith("vtri") ): # deconnexion en AC
                                            self.v.cabl.deconnect(kind="AC")
                                        elif(self.v.typCon.startswith("vdc")):
                                            self.v.cabl.deconnect(kind="DC")
                                     #if(not bon): j=j-1
                                            #fin de boucle
                    if(not bon): raise Exception(" mauvaise connexion vehicule ")
                if(bon):
                    dicval=dict()
                    dicval["LDEV1_DEEV1_EVSt_stVal"]=["\"1\"",str(self.v.util.getTemps(self.v.tempssimule,self.v.mempart))]
                    #self.v.util.message(" voiture connectee a la CS")
                    if(self.v.isRest):
                        self.v.reportlog.info(" voiture connectee a la borne " + str(adborne))
                    else:
                        self.v.reportlog.info(" voiture connectee a la borne " + ad)
                    
                    if(not platform.platform().__contains__('Linux')):
                        self.v.mybas.savenom({"can_ConnSt":("1",self.v.util.getTemps(self.v.tempssimule,self.v.mempart))})
                        self.v.mybas.savenom({"int_LDEVSE":(resul["prefixevse"],self.v.util.getTemps(self.v.tempssimule,self.v.mempart))})
                        self.savenom({"LDEV1_DEEV1_EVSt_stVal":("1", self.v.util.getTemps(self.v.tempssimule,self.v.mempart))})
                    else:
                        self.v.mempart.dicwrite("can_ConnSt",1)
                        self.v.reportlog.info("CBConnChanged can_Connst=1")
                        if((self.v.chaAlerte==0) or (self.v.chaAlerte==1)):
                           self.v.ui.checkBox_2.setChecked(True)
                        else:
                            self.v.ui.checkBox.setChecked(True)
                        #self.v.mempart.dicwrite("IEDLDEV1/DEEV1.EVSt.stVal",1)
                        if(not self.v.isFilaire):
                             if(self.v.isFilaire):
                                self.v.mempart.dicwrite("int_LDEVSE",resul["prefixevse"])
                             

                    if(not platform.platform().__contains__('Linux')):
                        poursel="SELECT value FROM nodes WHERE id=" + str(self.v.NomVersIdDic["can_ConnSt"]) # on regarde si on est connecté
                        connecte=self.v.mybas.fetchone(poursel)[0]
                    else:
                        connecte=str(self.v.mempart.dicread("can_ConnSt"))
                        if(connecte!='1'):
                            self.v.ui.checkBox_3.setChecked(False)
                            self.monChaInterrupt()
                            #self.v.ui.frame_5.hide()
                    #self.v.ui.frame_5.show()           # apparition du dialogue de selection du cable
                
            else: # demande de deconnexion 
                 # on regarde l'etat reel connecte de la voiture
                 if((self.v.mempart.dicread("can_ConnSt")==1) and (self.v.evseref !="")):
                    #la deconnexion est demandée manuellement
                    return
                 else:
                    # la deconnexion est demandee par la passerelle on continue
                    pass
                 if(not platform.platform().__contains__('Linux')):
                  etat=util.getValfromdb(self.v.dbpath,self.v,"LDEV1_DEEV1_EVSt_stVal")
                 else:
                  etat=str(self.v.mempart.dicread("IEDEVLDEV1/DEEV1.EVSt.stVal"))
                 if(etat != '1'):# pour se déconnecter la voiture doit être au repos 
                     self.v.ui.PBChaOff.click()   # simule un click sur arret
                     while(self.v.mempart.dicread("IEDEVLDEV1/DEEV1.EVSt.stVal"!=1)):
                           time.sleep(0.3) #on n'est pas au boucle tant que la voiture
                 if(not platform.platform().__contains__('Linux')):
                    poursel="SELECT value FROM nodes WHERE name=\"int_LDEVSE\""  # on regarde a quoi on est connecté
                    prefixevse=self.v.mybas.fetchone(poursel)[0]
                 else:
                    prefixevse=self.v.mempart.dicread("int_LDEVSE")
                    dicval=dict()
                    if(not platform.platform().__contains__('Linux')):
                        poursel=" SELECT value FROM nodes WHERE name=\"LDEV1_DEEV1_EVId_setVal\""
                        val=self.v.mybas.fetchone(poursel)[0]  
                    else:
                        val=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.EVId.setVal")
                    dicval["EVId"]=val
                    dicval["prefixevse"]=prefixevse
                    #dicval["time"]=str(self.v.tempssimule)
                    dicval["time"]=str(self.v.util.getTemps(self.v.tempssimule,self.v.mempart))
                    bon=True
                    if(not self.v.isFilaire):
                        if(self.v.isRest):
                            resul= self.monclientborne.postVals(prog="dc",data=dicval,jeeson=True)
                            if(resul["status"] != "ok"):
                                bon=False
                                self.v.reportlog.error(" deconnexion voiture " + resul["status"])
                                
                        else:
                                payload={"id":self.transacID,"verb":"deconnecte","params":dicval}
                                resul=self.monuart.post(payload)
                                self.transacID=self.transacID+1
                                if(resul[0] != "ok"):
                                    self.v.reportlog.error(" deconnexion " + str(resul[1]))
                                    
                    else: # pure filaire
                        #pdb.set_trace()
                        self.v.reportlog.info("CBConnChanged etat={0}".format(self.v.cabl.etatconnect))
                        if(self.v.cabl.etatconnect==1): # connectAC
                            self.v.cabl.deconnect(kind="AC")    #kind="AC") 
                            self.v.cabl.setEtat(3)
                        elif(self.v.cabl.etatconnect==2):
                            self.v.cabl.deconnect(kind="DC")
                            self.v.cabl.setEtat(3)
                        j=5
                        bon=False
                        while(j >0):
                            ad=self.v.mempart.dicread("int_adresseSE")
                            if(ad != ""):
                                bon=True
                                break
                            else:
                                time.sleep(0.3)
                                j=j-1
                            if(bon):
                               # la voiture s'est correctement deconnectee au niveau de EVSE
                                temps=self.v.util.getTemps(self.v.tempssimule,self.v.mempart)
                             #self.v.simulbas.save({str(self.v.NomVersIdDic["can_ConnSt"]):("0",temps),str(id):("1",temps)})
                             #self.v.util.saveNodeindb(self.v.dbpath,self.v,{"can_ConnSt":("0",temps)})
                             #poursel=" UPDATE nodes  SET value=\"" +  "0" +  "\", timestamp=" + str(temps) + "valueNmoins1=\"" +  "0" +  "\", timestampNmoins1=" + str(temps) +"  WHERE name=\""  + "can_ConnSt" +"\""
                             #self.v.util.execindb(self.v.dbpath,self.v,poursel);
                                dicval=dict()
                                dicval["int_LDEVSE"]=("_",temps)
                                if(not platform.platform().__contains__('Linux')):
                                    dicval["can_ConnSt"]=("0",temps)
                                    self.v.util.saveOptindb(self.v.dbpath,self.v,dicval)
                                else:
                                    self.v.mempart.dicwrite("can_ConnSt",0)
                                    
                                #self.v.mybas.savenom({"int_LDEVSE":("_",self.v.util.getTemps(self.v.tempssimule,self.v.mempart))})

                                self.v.ui.checkBox_3.setCheckState(QtCore.Qt.Unchecked)
                                self.v.ui.checkBox_3.hide()
                                #self.v.util.message(" voiture deconnectée")
                                self.v.reportlog.info("CBConnChanged disconnected vehicle")
                                
                                #self.monChaInterrupt()
                         
                
        except Exception as ee:
            self.v.reportlog.error( " CBConnChanged  " + str(ee))
            
            

    def monChaInterrupt(self):
        """ interruption de charge"""
        if(self.v.ui.checkBox_2.isChecked()):
             util.message(" not yet implemented")
       
 
    def cb4selectionchange(self):
        try:
            #pdb.set_trace()
            self.v.typCon=self.v.ui.comboBox_4.currentText()
            poursel=" UPDATE gestion  SET valeur=\"" + self.v.typCon + "\"  WHERE nom=" + "'typCon'"
            self.v.mybas.executer(poursel,commit=True)
            self.v.ui.LTypeCon.setText(self.v.typCon)
            
        except Exception as ee:
             pee=str(ee)
             self.v.reportlog.error("typ connection error " + str())
             
             #raise Exception(" erreur dans selection vehicule " + pee)

    def cbselectionchange(self):
        selection=self.ui.comboBox.currentText()
        #util.message( "heure depart selectionnee: " + self.ui.comboBox.currentText() )
        self.chaSaisie[0]=1

    def cb3selectionchange(self):
        selection=self.ui.comboBox.currentText()
        #self.v.tsuivi=util.suiviprofil(name="tsuivi",bdd=self.v.simdbpath,variables=self.v,nom="LDEV1_FCSH1",nodeprofil=1) #,(),{"bdd":self.v.simulbas,"variables":self.v,"nom":"LDEV1_FCSH1","nodeprofil":0})
        #util.message( "profile  selectionne : " + self.ui.comboBox_3.currentText() )
        self.v.reportlog.info("profil  selectionne : " + self.ui.comboBox_3.currentText())
        if((selection==self.ui.comboBox.currentText().__contains__("--")) or (selection=='0')):
            return
        else:
            self.chaSaisie[3]=1
        #self.v.tsuivi.start()
    
    def cb2selectionchange(self):
        try:
            selection=self.ui.comboBox.currentText()
            #util.message( "heure depart selectionnee: " + self.ui.comboBox.currentText() +" H " + self.ui.comboBox_2.currentText() + "MN" )
            hour=hour=int(self.ui.comboBox.currentText())
            minute=int(self.ui.comboBox_2.currentText())
            self.v.timedepart=  (hour*60 + minute)*60000
            self.chaSaisie[1]=1
            self.chaSaisie[0]=1

        except Exception as ee:
            self.v.reportlog.error( "selection heure 1ere ligne "+ str(ee) )
            raise Exception("selection heure 1ere ligne "+ str(ee))

    def cb7selectionchange(self):
        try:
            selection=self.ui.comboBox_6.currentText()
            #util.message( "heure 2eme ligne selectionnee: " + self.ui.comboBox_6.currentText() +" H " + self.ui.comboBox_7.currentText() + "MN" )
            hour=int(self.ui.comboBox_6.currentText())
            minute=int(self.ui.comboBox_7.currentText())
            self.v.timestart=  (hour*60 + minute)*60000
            self.chaSaisie[4]=1
            self.chaSaisie[3]=1
            
        except Exception as ee:
            self.v.reportlog.error( "selection heure 2ieme ligne "+ str(ee) )
            raise Exception("selection heure 2ieme ligne "+ str(ee))
    
    def CBModifCon(self):
       
        if(self.v.ui.CBModifCon.checkState==QtCore.Qt.Unchecked):
            self.v.ui.comboBox_4.hide()
        else:
            self.v.ui.comboBox_4.show()

    def lineEditEdited(self):
        try:
            self.chaSaisie[2]=1
            texte=self.v.ui.lineEdit.text()
            self.v.socDemandee=float(texte)
            #print("ds action ligne 547  " + str(self.v.socDemandee))
            if((self.v.socDemandee < self.v.socmin) or (self.v.socDemandee > self.v.socmax)):
                self.v.util.message("value " + self.v.ui.lineEdit.text() + " outside   socmin socmax interval")
                return
            
            self.chamaxpos=(self.v.socmax -self.v.initsoc)*self.v.chaMax/100 # en cas de chargement dynamique on vise 90% de chargement
            
            if(((self.v.socDemandee*self.v.chaMax/100) < self.v.chaMin) or ((self.v.socDemandee*self.v.chaMax/100) > self.v.chaMax)):
                     self.chaSaisie[2]=0
                     self.v.util.message(" value "+ self.v.ui.lineEdit.text() + " outside  chamin chamax interval")
                     self.v.reportlog.error(" value "+ self.v.ui.lineEdit.text() + " outside  chamin chamax interval ")
                     return
            if(self.v.ui.checkBox_2.isChecked()): # sens positif
              
                if(self.v.socDemandee < self.v.initsoc):
                     self.chaSaisie[2]=0
                     util.message(" value "+ self.v.ui.lineEdit.text() + " non corresponding to the initial soc ")
                     self.v.reportlog.error(" entrée "+ self.v.ui.lineEdit.text() + " non corresponding to the initial soc ")
                else:
                    self.chamaxpos=(self.v.socDemandee -self.v.initsoc)*self.v.chaMax/100
                    self.ui.label_5.setText("charge possible")
                    self.ui.LChaMaxPosVal.setText(str(self.chamaxpos))
                    self.ui.LChaMaxPosVal.show()
                    self.chaSaisie[2]=1
            elif(self.v.ui.checkBox.isChecked()): #sens negatif
                if( self.v.socDemandee > self.v.initsoc):
                     self.chaSaisie[2]=0    
                     util.message(" value "+ self.v.ui.lineEdit.text() + " not correct ")
                     self.v.reportlog.error(" entrée "+ self.v.ui.lineEdit.text() + " not correct ")
                else:
                    self.ui.label_5.setText("possible discharge")
                    self.chamaxpos=((float(self.v.initsoc)-float(self.v.socDemandee))*self.v.chaMax)/100
                    self.ui.LChaMaxPosVal.setText(str(self.chamaxpos))
                    self.ui.LChaMaxPosVal.show()
                    self.chaSaisie[2]=1
        except Exception as ee:
            raise Exception("saisie lineedit" + str(ee))
    
    

  

    
