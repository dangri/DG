
import utilitaires as util
import threading
import time
import platform
from Mybase import mybase as mybase
import socket
import struct
import pdb

# from ComplementsIHM import myspline as myspline
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v0.0.1  | 03/03/19   |  version initiale avec bogues corrigées                     |  AM  #
#############################################################################################

class simulEV(object):
    """cette classe décrit les attributs et methode slies à la simulation du chargeur/dechargeur EV
    le simulateur dans un thread met à jour les données de la Bdd  ZamiEVBase.db
     les données de charge sont données par la courbe type que l'on a entrée 
     deplus il suit les consgne données par le straton en ce qui concerne le PMAX
     il calcule l'énergie produite et arrete la charge lorsque celle-ci atteint les limites fixées
     le thread est en attente d'un verrou pilotée par restihmEV
    """
    def __init__(self,vars):
        try:
            
            self.v=vars
            #pdb.set_trace()
            if(not platform.platform().__contains__('Linux')):
                poursel="SELECT value FROM nodes WHERE name='int_chaMax' OR name='int_chaMin' ORDER BY name ASC "
                chaM=self.v.mybas.fetchmany(poursel)
                chaMax=chaM[0][0]
                chaMin=chaM[1][0]
                self.v.chaMax=float(chaMax)
                self.v.chaMin=float(chaMin)
                poursel="SELECT value FROM nodes WHERE name='int_socmax' OR name='int_socmin' ORDER BY name ASC "
                socs=self.v.mybas.fetchmany(poursel)
                socmax=socs[0][0]
                socmin=socs[1][0]
                self.v.socmax=float(socmax)
                self.v.socmin=float(socmin)
            else:
                self.v.chaMax=self.v.mempart.dicread("int_chaMax")
                self.v.chaMin=self.v.mempart.dicread("int_chaMin")
                self.v.socmax=self.v.mempart.dicread("int_socmax")
                self.v.socmin=self.v.mempart.dicread("int_socmin")
            self.v.reportlog.info("init simul chaMax={0} chaMin={1} socmax={2} socmin={3}".format(  \
                                self.v.chaMax, \
                                self.v.chaMin, \
                                self.v.socmax, \
                                self.v.socmin  ))
                
            self.evsim=threading.Event() # free l'evènement 
            self.ticksim=threading.Event() # synchro sur le temps
            self.fin=threading.Event() # signifie la terminaison du thread
            #self.pause=threading.Event() # signifie la pause du thread
            self.v.pause.clear();
            self.evsim.clear()
            self.ticksim.clear() # on bloque la synchro au depard
            self.adrudp="192.168.1.102" # adresse borne
            self.sock=None # connection udp a la boorne
            self.activite=False # pour arreter le thread de tempo
            self.fin.clear()
            self.K=1 # le coef d'acceleration
            #self.starttime=0.0 # heure début de la tache en timestamp posix
            #self.endtime=0.0
            self.wh=0 # sert à compter le cumul de charge depuis le debut de la simulation
            self.whinitial=0
            self.socinitial=0
            self.deltacha=0 #charge supplementaire 
            self.currentSOC=0
            self.pastSOC=0
            self.marche=0
            self.heuredepart=0
            self.t =threading.Thread(name="tsimule",target=self.simule)
            self.t.start()
            if(platform.platform().__contains__('Linux')):
             if(True):
                # il faut un thread de synchro du temps
                self.ttemps =threading.Thread(name="temps",target=self.tempo)
                #self.ttemps.start()
                self.starttempo()
                self.sock=None
                self.adrudp=None # adresse de la borne

        except Exception as ee:
            raise Exception(" Erreur dans init simule" + str(ee))

    def start(self):
        """demarre la simulation""" 
  
        self.starttime=self.v.util.getTemps(self.v.tempssimule,self.v.mempart)
        self.currenttime=self.starttime
        self.pasttime=self.starttime
        self.v.reportlog.info("SIMUL start ")
        self.evsim.set()
        self.v.pause.set()# on n'est pas en pause

    def stop(self):
        """arrete la simulation """
        self.v.reportlog.info("SIMUL stop ")
        self.evsim.clear()

    def termine(self):
        """arrete la simulation """
        self.v.reportlog.info("SIMUL termine ")
        self.activite=False
        time.sleep(1)
        self.fin.set()

    def setTest(self,cond):
        self.test=cond # true ou false

    def starttempo(self):
        self.v.reportlog.info("SIMUL starttempo ")
        self.activite=True
        self.ttemps.start()

    def stoptempo(self):
        self.activite=False
        self.v.reportlog.info("SIMUL stoptempo ")

    def tempo(self):
        """ thread pour synchroniser le temps sur le straton"""
        
        try:
            while(True ):
                if(not platform.platform().__contains__('Linux')):
                    self.adrudp=util.getValfromdb(self.v.dbpath,self.v,"int_adresseCS")
                    #print(" adrudp= " + self.adrudp)
                else:
                    self.adrudp=self.v.mempart.dicread("int_adresseCS")
                    #if(self.v.reportlog != None): self.v.reportlog.info(" initialisation adrudp=|" + self.adrudp + "|")
                    #print(" adrudp=int_adresse " + self.adrudp)
                if((self.adrudp != None) and (self.adrudp.__contains__("."))):
                     #print(" adrudp=int_adresse lue" + self.adrudp)
                     if(self.v.reportlog != None): self.v.reportlog.info(" adrudp=int_adresse " + self.adrudp)
                     break
                time.sleep(1)
            #print("contact wifi et adresse CS recuperee")
            # on se connecte sur le socket en udp
            if(self.sock==None):
                    self.sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM) #,socket.IPPROTO_UDP)
                    self.sock.setblocking(True)
                    self.sock.settimeout(5)
                    self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                    #self.sock.bind(('', 1313))
                    self.sock.bind(('<broadcast>', 1313))
                    activite=True
                    sanssock=False
           
            
            while(True and (self.activite==True)):
                    try:
                         #print("avant rcv")
                         a=self.sock.recvfrom(8)
                         #print("a" + str(a) + " " + str(a[1]))
                         #if(self.v.reportlog != None): 
                             #self.v.reportlog.info( "a" + str(a) + " " + str(a[0]))
                         #if(False):# test de bonne reception de l'adresse
                                   # ATTENTION erreur conversion b to string
                             #if(a[0]==self.adrudp):
                                 #self.ticksim.set() # relache simule
                                 #print("*************")
                             #else:
                                # print ("apres recv " + a[0])
                                # pass
                         self.ticksim.set() # relache simule
                         #print("*************")
                    except Exception as ff:
                        #print("rcvudp  " + " "  + self.adrudp + " " + str(ff))
                        if(self.v.reportlog != None): self.v.reportlog.error(" simule.tempo exception  {0}".format(ff))
                        pass
                     

        except Exception as ee:
              self.v.reportlog.error("erreur   connexion ou reception  udp " + str(ee)) 
        
    def simule(self):
        """ est lance par le thread de simulation
        Ce module de simulation met a jour periodiw=quement dans la base simulemesuresEV.bd les variables suivantes
        SOC   calcule a chaque pas d'iteration en fonction
                               - de la tension donnee par la courbe de charhe en fonction du SOC de debut
                               - de la puissance de regulation donnée soit
                                          - en mode calculée par une courbe de charge type
                                          - en mode normal par la variable WChaTgStp mise a jour par straton (FSCC)

        VCurrent donné par la courbe de charge 
        ACurrent calculée à partir de la puissance et de VCurrent
       """
        try:
          self.v.reportlog.info("SIMUL lancement")
          self.dbpath=self.v.dbpath  #'./ZamEVBase.db'
          self.mybas=mybase(self.dbpath,self.v.reportlog,variables=self.v)
          poursel=" SELECT valeur FROM gestion WHERE nom=\"K\""
          #self.v.ui.LAlerte.setText("")
          #self.v.previousalerte=""
          self.K=int(self.mybas.fetchone(poursel)[0])
          self.currenttime=self.v.util.getTemps(self.v.tempssimule,self.v.mempart)
          self.heuredepart=0
          self.v.reportlog.info("TEST ligne simul.89") #test
          self.evsim.wait() # on attend d'être lance
          self.currenttime=util.getTemps(self.v.tempssimule,self.v.mempart)
          self.v.reportlog.info("TEST ligne simul.91") #test
          ok=True
          j=0
          #self.evsim.wait() # on attend d'être lance
          while(ok): # boucle infinie si pas fin
              #self.v.reportlog.info("SIMUL boucle")
              j=j+1
              #self.v.reportlog.info(" avant wait   pause")
              self.v.pause.wait() # on se met en pause si l'evenement n'es pas positionné
              #self.v.reportlog.info(" apres  wait pause")
              if(self.fin.is_set()):
                 self.fin.clear() # on reset l'evenement 
                 self.evsim.clear()
                 #self.v.evsuivi.clear() # pour l'instant on ne s'occupe pas du suivi
                 #self.v.ui.label_3.hide()
                  
          #    self.maspline=myspline(self.v) # affichage de al courbe
                 ok=False# on peut etre arrete par fin

              """ 
              tt=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.AlmMsg.setVal")
              self.v.reportlog.info(" SIMUL message adresse= {0} alerte={1}".format(self.v.dicvars["IEDEVLDEV1/DEEV1.AlmMsg.setVal"],tt))
              if( tt != self.v.previousalerte):
                self.v.ui.LAlerte.setText(tt)
                self.v.previousalerte=tt
              """
              if(self.heuredepart==0): # on n'a pas encore recupere l'heure de depart
                  if(not platform.platform().__contains__('Linux')):
                     poursel=" SELECT value FROM nodes WHERE id=" + str(self.v.NomVersIdDic["DptTm"])
                     dpttm=self.mybas.fetchone(poursel)[0]
                  else:
                     dpttm=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.DptTm.setTm")
                  dpttm=str(dpttm)
                  if(dpttm=='0'):
                      #19 self.heuredepart=86400 +self.currenttime# on met 1j max
                      self.heuredepart=86400000 +self.currenttime# on met 1j max
                  else:
                      #am 04 self.heuredepart= float(dpttm)
                      self.heuredepart= int(dpttm)
                 # self.v.reportlog.info("TEST ligne simul.113 heure depart=" + str(self.heuredepart))
                  self.maxsoc=self.v.socDemandee
                  if(not platform.platform().__contains__('Linux')):
                    poursel=" SELECT value FROM nodes WHERE id=" + str(self.v.NomVersIdDic["WhTgt"])
                    whtgt=self.mybas.fetchone(poursel)[0]
                    whtgt=float(whtgt) 
                    poursel="SELECT value FROM nodes WHERE name=\"int_socmin\""
                    self.socmin=float(util.fetchindb(self.v.dbpath,self.v,poursel)[0])
                    self.v.wchaTgt=eval(self.v.util.getValfromdb(self.v.dbpath,self.v,"LDEV1_DEEV1_WChaTgt_setMag_f"))
                  else:
                    whtgt=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.WhTgt.setMag.f")
                    self.socmin=self.v.mempart.dicread("int_socmin") 
                    self.v.wchaTgt=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.WChaTgt.setMag.f")
                  self.v.currentSOC=self.v.initsoc
              
              
              deltad=float(self.heuredepart)-self.currenttime
              tempscourant=self.currenttime/1000 # cuurenttime est prime en ms tempscourant est exprme en s
              
             # print( "TOUR de simulation (K=" +str(self.K) +") tcour=" + time.strftime("%H:%M:%S", time.gmtime(tempscourant)) \
              #     +  " delta=" + str(self.currenttime - self.pasttime) +  " soc= " + str(self.v.currentSOC))     
             # self.v.reportlog.info("TOUR de simulation (K=" +str(self.K) +") tcour=" + time.strftime("%H:%M:%S", time.gmtime(tempscourant)) \
              #     +  " delta=" + str(self.currenttime - self.pasttime) + "soc=" + str(self.v.currentSOC))
              #self.currenttime=time.time()
              #chamarche=self.v.chamarche
              
              if(self.v.chamarche==1):# on vient de demander la transition 
                                     # on fait evoluer l'etat du vehicule de ready a encharge ou decharge  #
                      if(self.v.isRest):
                          if(self.v.typCon.startswith("vac") or self.v.typCon.startswith("tri")):
                             td='4'
                             tc='2'
                          else:
                             td='5'
                             tc='3'
                      if(self.v.isFilaire):self.v.reportlog.info("demarrage typcable={0}".format(self.v.cabl.typ))
                      if((self.v.isFilaire) and (self.v.cabl.etatconnect==1)):
                          td='4'
                          tc='2'
                      else:
                          td='5'
                          tc='3'
                      if(self.v.ui.checkBox.isChecked()): # sens negatif
                          if(not platform.platform().__contains__('Linux')):                           
                               util.saveNodeindb(self.v.dbpath,self.v,{"LDEV1_DEEV1_EVSt_stVal":(td,self.currenttime)}) # decharge
                          else:
                               self.v.reportlog.info(" simul ecriture EVSt.stVal={0} whtgt={1}".format( td,whtgt))
                               self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.EVSt.stVal", int(td)) # decharge
                               
                      else: # sens positif
                          if(not platform.platform().__contains__('Linux')):
                               util.saveNodeindb(self.v.dbpath,self.v,{"LDEV1_DEEV1_EVSt_stVal":(tc,self.currenttime)}) # charge        
                          else:
                              self.v.reportlog.info(" simul ecriture EVSt.stVal={0} whtgt={1}".format( tc,whtgt))
                              self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.EVSt.stVal", int(tc))
                     
                          
                      self.v.chamarche=3
              if((self.v.chamarche==3) and (ok==True)):# self.v.marche prend les valeurs de transition passant de simul  a pas de simul
                                                       # self.v.chamarche = 0              pas de charge ou decharge demandee
                                                       # self.v.charge =1                  straton demande la simulation
                                                       # self.v.charge =2                  demande d'arret de la simulation
                                                       # self.v.charge =3                  en charge ou decharge

                    #time.sleep(1);
                    self.currenttime=self.v.util.getTemps(self.v.tempssimule,self.v.mempart)
                    self.v.reportlog.info("Simul TIME mempart={0} currentime={1} pastime={2}".format(self.v.mempart,self.currenttime,self.pasttime))
                   # self.pasttime=self.currenttime
                    
                   
                    if(False):# avant reglage tick du temps
                         #---------------------------------------
                        if(platform.platform().__contains__('Linux')):
                             courtime=self.v.util.getTemps(self.v.tempssimule,self.v.mempart)
                             while( self.pasttime==courtime ) :
                                 time.sleep(0.1)
                                 courtime=self.v.util.getTemps(self.v.tempssimule,self.v.mempart)
                             self.currenttime=courtime
                        else:
                            self.currenttime=self.v.util.getTemps(self.v.tempssimule,self.v.mempart)
                         #---------------------------------------
                    #self.currenttime=self.v.util.getTemps(self.v.tempssimule,self.v.mempart)
                    #self.v.reportlog.info(" simul  pour calcul delta self.v.wchaTgt="  + str(self.v.wchaTgt))
                    self.v.wchaTgt=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.WChaTgt.setMag.f")
                    delta=self.deltaPuissance(self.v.currentSOC,((self.currenttime- self.pasttime)/1000),self.v.wchaTgt) # le calcul de puissance suppose des secondes
                    self.v.reportlog.info("simul delta={0} deltatime={1}".format(delta,(self.currenttime- self.pasttime)/1000 )) #test
                    
                    """
                    if(self.v.ui.checkBox.isChecked()): # sens negatif
                        newsoc=self.v.currentSOC-delta[0]
                    else:
                        newsoc=self.v.currentSOC+delta[0]
                    """
                    if(self.v.wchaTgt<0):
                        newsoc=self.v.currentSOC-abs(delta[0])
                    else:
                        newsoc=self.v.currentSOC+abs(delta[0])
                    self.v.currentSOC=newsoc  
                    self.currentSOC=newsoc
                   # print("tour de simulation " + str(j) + " newsoc=" +str(newsoc) + " currenttime=" + str(self.currenttime) + " pastime=" + str(self.pasttime))
                    u=delta[1]
                    i=delta[2]
                    if(j> 2*self.K): # on va sauvegarder le soc tous les nxtours
                        j=0
                        #print(" tour de simulation avant ecriture base")
                        self.v.reportlog.info("simul impression calcul")
                        tempscourant=self.currenttime/1000
                        affsoc='{:3.6}'.format(str(self.v.currentSOC)) + " à " + time.strftime("%H:%M:%S", time.gmtime(tempscourant))
                        self.v.ui.LChaMaxPosVal.setText(affsoc)
                        if(not platform.platform().__contains__('Linux')):
                             self.v.wchaTgt=eval(self.v.util.getValfromdb(self.v.dbpath,self.v,"LDEV1_DEEV1_WChaTgt_setMag_f"))
                        else:
                             self.v.wchaTgt=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.WChaTgt.setMag.f")
                             #self.v.reportlog.info("simul : self.v.wchaTgt=" + str(self.v.wchaTgt))
                        if(not platform.platform().__contains__('Linux')):
                            poursave=dict()
                            poursave["LDEV1_DEEV1_Soc_mag_f"]=(str(newsoc),self.currenttime)
                            
                            poursave["can_AmpCurrent"]=(str(i) ,self.currenttime)
                            self.v.util.saveOptindb(self.v.dbpath,self.v,poursave)
                            poursave=dict()
                            poursave["can_Vcurrent"]=( str(u)  ,self.currenttime)
                            self.v.util.saveNodeindb(self.v.dbpath,self.v,poursave)
                        else:  
                            self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.Soc.mag.f",newsoc)
                            self.v.mempart.dicwrite("can_AmpCurrent",i)
                            self.v.mempart.dicwrite("can_Vcurrent",u)
                            self.v.reportlog.info("simul impression soc={0} I={1} V={2}".format(newsoc,i,u))
                            self.v.reportlog.info("simul : ecriture IEDEVLDEV1/DEEV1.Soc.mag.f, can_AmpCurrent, can_Vcurrent")
                      
                    self.deltatime=self.currenttime-self.starttime # temps
                    prov= time.strftime("%H:%M:%S", time.gmtime(self.currenttime)) + " || " + time.strftime("%H:%M:%S", time.gmtime(float(self.heuredepart)))
                    arret=False
                    #self.v.reportlog.info("TEST ligne simul.61") #test
                    if(self.v.ui.checkBox.isChecked()): # sens negatif
                         if(
                            ((self.v.socDemandee != 0) and (newsoc < self.v.socDemandee))
                            or 
                            (newsoc < self.v.socmin)):
                             self.v.reportlog.info(" soc minimale  atteinte")
                             arret=True
                    else: # sens positif
                         if(
                             (self.currenttime > float(self.heuredepart))
                             or
                             (newsoc > self.maxsoc)

                          ):
                             arret=True
                             self.v.reportlog.info(" heure de depart ou chrge attendue atteinte")
                             #self.v.reportlog.info("TEST ligne 178") #test
              if(self.v.chamarche==2):
                        # l'interfce demande l'arret
                        arret=True
              if(arret):
                            self.v.reportlog.info("SIMUL arret")
                            self.marche=0;
                            #self.fin.set() # fin de la simulation
                            #self.v.evsuivi.clear()
                       #---------- on arrete la charge ----------------
                            if(not platform.platform().__contains__('Linux')):
                                dicval=dict()
                                dicval[str(self.v.NomVersIdDic["int_ChargeMarche"])]=["'0'",str(self.v.util.getTemps(self.v.tempssimule,self.v.mempart))]
                                self.mybas.save(dicval)
                                dicval=dict()
                                dicval["LDEV1_DEEV1_EVSt_stVal"]=('1',str(self.v.util.getTemps(self.v.tempssimule,self.v.mempart)))
                                self.v.util.saveNodeindb(self.v.dbpath,self.v,dicval) # on previent straton de l'arret
                            else:
                                self.v.mempart.dicwrite("int_ChargeMarche",0)
                                self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.EVSt.stVal",1)
                                self.v.reportlog.info(" simul passage de EVSt.stVal à 1")
                                self.v.mempart.dicwrite("IEDEVLDEV1/DEEV1.WCurrent.mag.f",0.0) 
                            self.v.mesacts.chaSaisie=[0,0,0]
                            self.v.ui.lineEdit.setText="000000.00"
                            self.v.ui.lineEdit.setReadOnly(False)
                            self.chamaxpos=0.0
                            self.v.ui.LChaMaxPosVal.text=str(self.chamaxpos)
                            self.v.ui.PBChaOn.setStyleSheet("color: rgb(0, 0, 0);")
                            self.v.ui.checkBox_2.setChecked(False)
                            self.v.chamarche=0 
                            #self.evsim.clear()
                            #self.v.ui.label_3.hide()
                            
                       
              else:
                   
                  #self.currenttime=time.time()
                  self.currenttime=util.getTemps(self.v.tempssimule,self.v.mempart)
              # autres variations de grandeurs que la charge
              # par ex fluctuation de la tension
              #self.evsim.wait() # on attend d'être lance
              self.pasttime=self.currenttime
              if(platform.platform().__contains__('Linux')):
                    # sur la plateforme linux on attend la synchro du temps pour le récupérer
                    #self.v.reportlog.info("SIMUL avant   tick")
                    self.ticksim.wait() #
                    #print(" apres tick")
                    #self.v.reportlog.info("SIMUL apres  tick")
                    self.ticksim.clear()
              else:
                    #print(" nouveau tour de simu")
                    time.sleep(1/self.K) # sur windows on attends la seconde simulee suivante
          prov=1 # pour marquer la fin de la boucle  
        except Exception as ee:
                self.v.reportlog.error("SIMUL error={0}".format(str(ee)))
                #print( "erreur simule  " +str(ee))
                self.fin.set() # fin de la simulation
                self.v.evsuivi.clear()  

    def deltaPuissance(self,soc,deltat,wchaTgt):
        #self.v.reportlog.info(" deltaPuissance  soc={0} deltat={1} wchaTgt={2}".format(soc,deltat,wchaTgt))
        """ fournit un tuple (detacha,deltasoc)
        à partir de la currentSOC
        on suppose le pas de temps assez petit pour  considérer la tension et l'intensite constante 
        et à la soc d'origine donnée
        le deltat est donné en timestamp"""
        try:
            #poursel="SELECT value FROM nodes WHERE id=" + str(self.v.NomVersIdDic["WChaTgt"])
            #val=self.mybas.fetchone(poursel)[0]
            #pdb.set_trace()
            val=wchaTgt
            #perte=util.getValfromdb(self.v.dbpath,self.v,"int_W")
            #pmax=eval(wchaTgt)*(100-perte)/100
            #pmax=eval(wchaTgt)
            pmax=wchaTgt
            deltah=deltat/3600  # on ramène le deltat en heure
            U=self.v.courbeV.Y(soc)
            icourbe=self.v.courbeI.Y(soc)
            if(wchaTgt<0): #oct
                I=-min(self.v.courbeI.Y(soc),abs(pmax)/U) #oct
            else:
                I=min(self.v.courbeI.Y(soc),pmax/U)
            deltapwr=U*I*deltah
            deltasoc=100*deltapwr/self.v.chaMax
            #self.v.reportlog.info(" deltapuissance pmax={2} soc={3} U={4} I={5} deltapwr={0} deltasoc={1}".format(deltapwr,deltasoc,pmax,soc,U,I))
            return (deltasoc,U,I)
        except Exception as ee:
            raise Exception (" erreur calcul deltapuissance " + str(ee))
        

