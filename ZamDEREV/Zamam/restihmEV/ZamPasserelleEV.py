#/usr/bin/python
# -*- coding: utf-8 -*-
"""

Cette tache explore périodiquement les données 
-de simulation pour maj la base de donnée (note: on pourra etudier un fonctionnement en evènement
-les donnees de bdd pour faire les actions de commandes simulées necessaires
elle est lancée en tant que thread de la tache principale

"""
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v0.0.1  | 03/03/19   |  version initiale avec bogues corrigées                     |  AM  #
#############################################################################################
#   
#------------------------------------------------
#            paramètres             
#-----------------------------------------------


#-----------------------------------------------

import os
import time
import threading
import logging
import utilitaires as util
from Mybase import mybase as mybase
import sqlite3
import sys
import collections
import platform
import pdb

class passerelle:
         
    def __init__(self,vars):
        """ constructeur des variables """
        try:

          self.v=vars
          self.size=20
          self.cyclemesure=0.1 # cycle par defaut 100ms
          self.util=self.v.util
          self.evpasrel=threading.Event() # free l'evènement 
          self.fin=threading.Event() # signifie la terminaison du thread
          self.evpasrel.clear()
          self.fin.clear()
          poursel="SELECT nom,valeur FROM gestion WHERE nom='cyclemesure'" 
          r=vars.mybas.fetchone(poursel)
          self.cyclemesure=float(r[1])
          self.currenttime=0
          self.pasttime=0
          self.t =threading.Thread(name="tpasrel",target=self.marche)
          #util.message("lancement du thread de la passerelle entre les bdd")
          self.t.start()
          self.starttime=0.0 # heure début de la tacheen timestamp posix
          self.endtime=0.0
       
        except Exception as ee:
            self.vv.reportlog.error(" variables " +str(ee))
            #print(" erreur variables " + str(ee))
    def start(self):
        """demarre la passerelle""" 
        #self.starttime=time.time()
        #self.starttime=self.v.tempssimule
        self.starttime=self.v.util.getTemps(self.v.tempssimule,self.v.mempart)
        self.currenttime=self.starttime
        self.pasttime=self.starttime

    def stop(self):
        """arrete la simulation """
        self.evsim.clear()

    def termine(self):
        """arrete la simulation """
        self.fin.set()

    def setTest(self,cond):
        self.test=cond # true ou false

    def marche(self):
      try:
         prov=1
         #self.mybas=mybase(self.v.dbpath,self.v.reportlog,variables=self.v)
         #self.simulbaspas=mybase(self.v.simdbpath,self.v.reportlog,variables=self.v)
     #--------------------  lecture des mesures --------------------------------------
         pas=0
         self.evpasrel.wait()
         self.v.reportlog.info("test passerelle lancée")
         poursel=" SELECT valeur FROM gestion WHERE nom=\"K\""
   
         k=self.util.fetchindb(self.v.dbpath,self.v,poursel)[0]
         self.K=int(k)
              #self.v.ui.checkBox_4.setStyleSheet("color: rgb(153,255,51);")
         self.pasttime=self.currenttime
         #self.currenttime=time.time()
         #self.v.tempssimule=self.currenttime
         ri=range(0,self.K)
         cptsimul=0  # compteur de ticks pour recuperer les grandeur de simulation
         cptbd=0  # compteur de ticks pour repuperer des grandeur de la bd
         self.v.ui.LAlerte.setText("")
          #self.v.previousalerte=""
         while(True):
              if(self.fin.is_set()):
                 util.message(" interruption passerelle")
                 break
              #if(self.v.etatgpio==3):
                  #self.v.cabl.setEtat(3)
                  #time.sleep(.5)
                  #self.v.etatgpio=0
              tt=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.AlmMsg.setVal")
              #self.v.reportlog.info(" SIMUL message adresse= {0} alerte={1}".format(self.v.dicvars["IEDEVLDEV1/DEEV1.AlmMsg.setVal"],tt))
              if( tt != self.v.previousalerte):
                self.v.ui.LAlerte.setText(tt)
                self.v.previousalerte=tt
                self.v.util.traitAlerte(self.v,tt)
              self.v.reportlog.info("passerelle can_ConnST={0} int_adresse={1}".format(self.v.mempart.dicread("can_ConnSt"),self.v.mempart.dicread("int_adresseSE")))
              if(self.v.isFilaire):
                  if((self.v.mempart.dicread("can_ConnSt")==1) and (self.v.mempart.dicread("int_adresseSE")=="")):
                      # le cable s'est deconnecte
                      self.v.evseref=""
                      self.v.ui.checkBox_3.setChecked(False)
              for i in ri:   # on tourne K fois plus vite
                  #self.v.ui.label_3.show()
                  if(cptbd > 1*self.K): # on ne recupere les ordres stratons que toutes les ns
                      cptbd=0
                      if(not platform.platform().__contains__('Linux')):
                          poursel="SELECT value,timestamp,valueNmoins1 FROM nodes WHERE (value != valueNmoins1) and (name=\"int_ChargeMarche\") " 
                          res=util.fetchindb(self.v.dbpath,self.v,poursel)
                          if(res != None): res=res[0]
                      else:
                          res=str(self.v.mempart.dicread("int_ChargeMarche")) 
                          #self.v.reportlog.info("passerelle ping-pong  etatconnect={0} oldpin29={1} ".format(self.v.cabl.etatconnect,self.v.cabl.oldpin29)      
                          if(self.v.isFilaire):
                              if((self.v.mempart.dicread("can_ConnSt")==1) and (self.v.evseref != "")): # est-on vivant?
                                  wc=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.WCurrent.mag.f")
                                  self.v.reportlog.info("Passerelle en vie wc={0} ".format(wc))
                                  self.v.cabl.keepAlive(kind="ev")
                          else:
                              wc=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.WCurrent.mag.f")
                      if(res!=None): # il y a un resultat
                              
                              if(res=='1'): # starton autorise la charge : on peut y aller
                                  if(not platform.platform().__contains__('Linux')):
                                        util.saveToutVal(self.v.dbpath,self.v,("int_ChargeMarche",0,0))
                                  else:
                                        self.v.reportlog.info("reecriture de  int_ChargeMarche o ")
                                        self.v.mempart.dicwrite("int_ChargeMarche",0)
                      
                               # il va falloir verifier quelle est l'ordre a mettre 
                              # on effectue  l'ordre demande demarrer la charge ou la decharge
                                  self.v.chamarche=1 # on marque l'etat marche
                                  self.v.reportlog.info(" passage de v.chmarche 1")
                                  self.v.sim.start() # demarrage de la simulation
                                  self.v.reportlog.info(" int_chargemarche declanchee ")
                               
                  else:
                      cptbd=cptbd+1
                  if(cptsimul>1*self.K): # on n'affiche l'heure que toutes  les ns
                    ss=self.v.tempssimule
                    dd=self.v.mempart
                    temp=self.v.util.getTemps(self.v.tempssimule,self.v.mempart,reportlog=self.v.reportlog)
                    if(platform.platform().__contains__('Linux')):
                       temps=temp/1000
                    else:
                       #19 temps=temp
                       temps=temp/1000
                    #pdb.set_trace()
                    #self.v.ui.LHeure.setText(time.strftime("%H:%M:%S", time.gmtime(temps)))  
                    #self.v.ui.label_6.setText(time.strftime("%H:%M:%S", time.gmtime(temps)))  
                    tt=self.v.util.getTemps(self.v.tempssimule,self.v.mempart)/1000
                    #self.v.reportlog.info("Test passerelle temps={0}".format(tt))
                    self.v.ui.label_6.setText(time.strftime("%H:%M:%S", time.gmtime(tt)))  
                    #self.v.reportlog.info("Test passerelle temps={0}".format(self.v.util.getTemps(self.v.tempssimule,self.v.mempart)/1000))
                   # self.v.reportlog.info("Test passerelle " + time.strftime("%H:%M:%S", time.gmtime(temp)) +"," + time.strftime("%H:%M:%S", time.gmtime(temps)))
                    cptsimul=0
   
                  else:
                    cptsimul=cptsimul+1
                  time.sleep(1/self.K)
             # time.sleep(1) # un petit retard a l'allumage dont on peut discuter l'utilité
      except Exception as ee:
        if(not self.v.reportlog==None): self.v.reportlog.error("passerelle " + str(ee))
        #print(" erreur passerelle:" + str(ee))