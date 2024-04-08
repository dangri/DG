#/usr/bib/python3
# -*- coding: utf-8 -*-
#  IHM de base de conf=duite de la charge côté EV
# nous avons de base 2 colonnes
#   - une colonne pour afficher les valeurs de mesure
#   - une colonne pour afficher les données et les dialogues ^du tableau de bord
#
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v1.2.6  | 01/23   |  - chamin = chamax*socminnnnnn                                 | AM   #
#############################################################################################
#------------------------------------------------
#            paramètres             
#-----------------------------------------------
TrialogRest="0.0.0.0:5001" # adresse de la tâche rest trialog
ZamirenRest="127.0.0.1:5000" # adresse de la tâche rest zamiren

#-----------------------------------------------
import pdb
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
#from myclienthttp import myclient as myclient
import sqlite3
import platform
from simulEV import simulEV as simulEV
from ZamPasserelleEV import passerelle as passerelle
from actions import mesactions as mesactions
import struct





class variables:
    def __init__(self):
        """ constructeur des variables """
        try:
         self.isautonome=False
         self.curDir=os.getcwd() # repertoire courant
         if(platform.platform().__contains__('Linux')):
            self.loghandler=logging.FileHandler("/home/pi/logs/ZamreportEV.log",mode='w',encoding=None,delay=False)
         else:
            self.loghandler=logging.FileHandler(self.curDir + "/" + "reportServicesZamEV.txt",mode='w',encoding=None,delay=False)
            print(" logging dans : " + self.curDir + "/" + "reportServicesZamEV.txt")
         self.loghandlerconsole=logging.StreamHandler(stream=sys.stderr)
         self.formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
         self.loghandler.setFormatter(self.formatter)   
         self.loghandlerconsole.setLevel(logging.ERROR)
         self.reportlog=logging.getLogger('reportlog')
         self.reportlog.setLevel(logging.INFO)
         self.reportlog.addHandler(self.loghandler)
         self.reportlog.addHandler(self.loghandlerconsole)
         self.curDir=None # la fenetre centrale
         self.mesacts=None # pointe sur les actions
         self.tempssimule=-1 # temps pendant une simulation (peut etre accelere)
         self.previousalerte="" # alerte precedente
         #donnees=None # les donnees 
         self.mytabl=mytable(self.reportlog) # une tab
         self.tableborne=mytable(self.reportlog) # table pour afficher les variables de la borne
         self.mempart=None
         self.dicvars=None
         self.timestart=0
         self.timedepart=0
         self.etatgpio=0 # si vaut 3 force la passerelle à positionner l'etat GPIO à 3
         if(platform.platform().__contains__('Linux')):
             self.dbpath='/home/zamam/ZamEVBase.db'

             from mempart import MemPart as MemPart
             self.mempart=MemPart(adresse="/ZAMVAREVE")
             self.dicvars=dict()  # le dictionaire des variables partagees
                                  # nomvar ArrayList(type,adresse,nboctet)
             import posix_ipc
             self.sem=posix_ipc.Semaphore("/sem.ZAMSEMAL")
         else:
             self.dbpath='.\\ZamEVBase.db'
             self.simdbpath='.\\ZamSimulEVBase.db'
             self.sem=None
         #self.mybas=mybase(self.dbpath,self.reportlog)
         #self.simulbas=mybase(self.simdbpath,self.reportlog)
         self.mybas=None
         self.ll=threading.RLock()
         self.sessionCom={"zamkern":False}
         self.verrouCom={"zamkern":threading.Lock()}
                    # self.dicFenetres=dict()
         self.ui= None # fenêtre principale
         self.util=util
         self.cyclique=False
         self.initsoc=0 # soc initiale
         self.whreserve=0 # energie réservée mise ajour par trait alerte 
         self.chamarche=0 # reflète l'état de charge en reel ou en simulation (piloté pas mesures.db)
         self.sim=None #  thread de simulation
         self.pause=threading.Event()
         self.pasrel=None # thread de la passerelle entre la bd de simulation et la bdd
         self.evsuivi=threading.Event()
         self.adservercs="localhost"  # valeur par defaut
         self.servport="4000"
         self.Wpertes=0.0 # perte etre la batterie et la puissance mesuree dans EV 
         self.socDemandee=0 # le soc demande
         self.isFilaire=False # la communication est-elle filaire?
         self.isSerial=False  # la communication est-elle serial?
         self.cabl=None  # le cable de connexion
         self.capacites=dict() # le choix des capacités de cable 
         self.codesCC=dict() # codage des capacités
         self.isRest=False
         self.typCon="vac20" # type de connexion courante
         self.evseref="" # adresse de la borne quand connecté
         self.chaSaisie=[0,0,0,0,0]
         self.chaAlerte=0 # positionne par passerelle si alerte chage ou decharge

         
        

        except Exception as ee:
            self.reportlog.error(" variables " +str(ee))
            #print(" erreur variables " + str(ee))

    def initdicvars(self):
        if(self.dicvars != None):
             varlist=[
             ("temps","ull",8,0), \
             ("can_ConnSt","int",4,8), \
             ("int_chaMax","float",4,12), \
             ("int_chaMin","float",4,16), \
             ("int_ChargeMarche","int",4,20), \
             ("int_adresseCS","ip",20,24), \
             ("int_adresseEV","ip",20,44), \
             ("int_adresseSE","ip",20,64), \
             ("int_wifi","bool",1,84), \
             ("IEDEVLDEV1/DEEV1.ConnTypSel.stVal","int",4,88), \
             ("IEDEVLDEV1/DEEV1.DptTm.setTm","ull",8,92), \
             ("IEDEVLDEV1/DEEV1.EVId.setVal","string",256,100), \
             ("IEDEVLDEV1/DEEV1.EVSt.stVal","int",4,356), \
             ("IEDEVLDEV1/DEEV1.Soc.mag.f","float",4,360), \
             ("IEDEVLDEV1/DEEV1.WChaTgt.setMag.f","float",4,364), \
             ("IEDEVLDEV1/DEEV1.WCurrent.mag.f","float",4,368), \
             ("IEDEVLDEV1/DEEV1.WhTgt.setMag.f","float",4,372), \
             ("IEDEVLDEV1/DEEV1.EMAId.setVal","string",256,376), \
             ("IEDEVLDEV1/DEEV1.AlmMsg.setVal","string",256,632), \
             ("int_PID","int",4,888), \
             ("int_Wmax","float",4,892), \
             ("IEDEVLDEV1/DEEV1.StartTm.setTm","ull",8,896), \
             ("int_connected","int",4,900), \
             ("IEDEVLDEV1/DSTO1.WhMaxRtg.setMag.f","float",4,908), \
             ("IEDEVLDEV1/DSTO1.WhMinRtg.setMag.f","float",4,912), \
             #("IEDEVLDEV1/DSTO.DERUnit.seSrcRef","string",256,916), \
             #("IEDEVLDEV1/DSTO1.DschWMax.setMag.f","float",4,1172), \
             #("IEDEVLDEV1/DSTO1.DschWMaxRtg.setMag.f","float",4,1176), \
             #("IEDEVLDEV1/DSTO1.InWh.mag.f","float",4,1180), \
             #("IEDEVLDEV1/DSTO1.InclSto1.setSrcRef","string",256,1184), \
             #("IEDEVLDEV1/DSTO1.InclSto2.setSrcRef","string",256,1440), \
             #("IEDEVLDEV1/DSTO1.InclSto3.setSrcRef","string",256,1696), \
             #("IEDEVLDEV1/DSTO1.OutWh.mag.f","float",4,1952), \
             ("can_AmpCurrent","float",4,1500), \
             ("can_Vcurrent","float",4,1504), \
             ("int_LDEVSE","string",256,1760), \
             ("int_WPertes","float",4,1764), \
             ("int_socmax","float",4,1768), \
             ("int_socmin","float",4,1772) \
             
              ]
             for var in varlist:
                 self.mempart.dicvarsadd(var[0],typ=var[1],lo=var[2],ad=var[3])

             
    def initIds(self):
        try:
             self.idsaffich=""
             self.withids=False # si TRue n'affiche que les variable ids sélectionnées
             self.NomVersIdDic=dict() 

             poursel=" SELECT id FROM nodes WHERE (SUBSTR(name,1,5)='LDEV1') AND (name LIKE '%Soc_mag_f')"
             id=(self.mybas.fetchone(poursel))[0]
             self.NomVersIdDic["SOC"]=id
             self.idsaffich=self.idsaffich + " id=" + str(id) + " or "
             poursel=" SELECT id FROM nodes WHERE (SUBSTR(name,1,5)='LDEV1') AND (name LIKE '%ConnTypSel_stVal')"
             id=(self.mybas.fetchone(poursel))[0]
             self.NomVersIdDic["ConnTypSel"]=id
             self.idsaffich=self.idsaffich  + " id="+ str(id) + " or "
             poursel=" SELECT id FROM nodes WHERE (SUBSTR(name,1,5)='LDEV1') AND (name LIKE '%DptTm_setTm')"
             id=(self.mybas.fetchone(poursel))[0]
             self.NomVersIdDic["DptTm"]=id
             self.idsaffich=self.idsaffich  + " id="+ str(id) + " or "
             poursel=" SELECT id FROM nodes WHERE (SUBSTR(name,1,5)='LDEV1') AND (name LIKE '%WhTgt_setmag_f')"
             id=(self.mybas.fetchone(poursel))[0]
             self.NomVersIdDic["WhTgt"]=id
             self.idsaffich=self.idsaffich + " id=" +  str(id) + " or "
             poursel=" SELECT id FROM nodes WHERE (SUBSTR(name,1,5)='LDEV1') AND (name LIKE '%WChaTgt_setmag_f')"
             id=(self.mybas.fetchone(poursel))[0]
             self.NomVersIdDic["WChaTgt"]=id
             self.idsaffich=self.idsaffich + " id=" + str(id) + " or "
             poursel=" SELECT id FROM nodes WHERE (name LIKE '%ChargeMarche')"
             id=(self.mybas.fetchone(poursel))[0]
             self.NomVersIdDic["int_ChargeMarche"]=id
             self.idsaffich=self.idsaffich + " id=" + str(id) + " or "
             poursel=" SELECT id FROM nodes WHERE (name LIKE '%VCurrent')"
             id=(self.mybas.fetchone(poursel))[0]
             self.NomVersIdDic["can_Vcurrent"]=id
             self.idsaffich=self.idsaffich + " id=" + str(id) + " or "
             poursel=" SELECT id FROM nodes WHERE (name LIKE '%AmpCurrent')"
             id=(self.mybas.fetchone(poursel))[0]
             self.idsaffich=self.idsaffich + " id=" + str(id) + " or "
             self.NomVersIdDic["can_AmpCurrent"]=id
             self.idsaffich=self.idsaffich + " id=" + str(id) + " or "
             poursel=" SELECT id FROM nodes WHERE  (name = 'can_ConnSt')"
             id=(self.mybas.fetchone(poursel))[0]
             self.NomVersIdDic["can_ConnSt"]=id
             self.idsaffich=self.idsaffich + " id=" + str(id) 


         
        except Exception as ee:
            self.reportlog.error(" variables initIds " + str(ee))
            raise Exception (" variables initIds " + str(ee))


def pourtest(vv):
    """ initialisations pour les tests"""
    try:
        vv.mempart.dicwrite("int_adresseCS", "10.3.141.116")
    except Exception as ee:
        vv.reportlog.error("pourtest " + str(ee))
        #print("erreur pourtest " + str(ee))
 

#------------------- programe racine --------------------------------

if __name__ == "__main__":
    import sys
    try:
        import pdb
        #pdb.set_trace()
        vv=variables()
        if(platform.platform().__contains__('Linux')):
          vv.mempart.setVars(vv)
          vv.dicvars=dict()
          vv.initdicvars()
        isautonome=True # a priori la simulation met en jeux l'IED mais sinon on peut tester cette IHM sans lui de maniere autonome
        args=sys.argv # liste des parametre d'appel
        if(len(args)<=1):
            if(platform.platform().__contains__('Linux')):
                             bddrep='/home/pi/zamam'
                             vv.dbpath=bddrep+"/ZamEVBase.db"
                             vv.mybas=mybase(vv.dbpath,vv.reportlog,variables=vv)
                             isautonome=True
                            # vv.isFilaire=True
                             valeur=vv.mempart.dicread("int_adresseCS")
                             
            else:
                             bddrep='C:\\Users\\andell3\\Documents\\Maquette_Vedecom\\tests'
                             vv.dbpath=bddrep+"\\ZamEVBase.db"
                             vv.mybas=mybase(vv.dbpath,vv.reportlog,variables=vv)
                             poursel=" SELECT value FROM nodes WHERE  name=\"int_adresseCS\""
                             valeur=utilitaires.fetchindb(vs.dbpath,vs,poursel)[0]
        else:
            
            ri=range(1,len(args))
            dicargs=dict()
            for i in ri:
                ll=args[i].split('=')
                dicargs[ll[0]]=ll[1]
                EVid="EV" # nom par defaut
                for param in dicargs.keys():
                            if(param=="mode"): 
                                if(dicargs[param]=="autonome"):
                                   isautonome=True
                                   vv.isRest=True
                                elif(dicargs[param]=="serial"):
                                    isautonome=True
                                    vv.isSerial=True
                                elif(dicargs[param]=="filaire"):
                                    isautonome=True
                                    vv.isFilaire=True
                            elif(param=="bddrep"):
                                bddrep=dicargs["bddrep"]
                                if(platform.platform().__contains__('Linux')):
                                    vv.dbpath=bddrep +"/ZamEVBase.db"
                                else:
                                    vv.dbpath=bddrep  +"\\ZamEVBase.db"
                                vv.mybas=mybase(vv.dbpath,vv.reportlog,variables=vv)
                            elif(param=="servport"):
                                vv.servport=int(dicargs["servport"])
        vv.isautonome=isautonome
        if(platform.platform().__contains__('Linux')):     
            vv.adservercs=vv.mempart.dicread("int_adresseCS")
            vv.WPertes=vv.mempart.dicread("int_WPertes")
        else:
            vv.initIds()
            vv.WPertes=vv.util.getValfromdb(vv.dbpath,vv,"int_WPertes")
        app = QtWidgets.QApplication(sys.argv)
        vv.app=app
        MainWindow = QtWidgets.QMainWindow()
        
        #------------- ajustements graphiques ----------------------------
        ui = Ui_MainWindow()
        ui.setupUi(MainWindow)
        #ui.PBSave.hide() # fait disparaitre le bouton de sauvegarde
        #ui.verticalLayout_2.setAlignment(Qt.AlignTop)
        ui.comboBox.addItems(["00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24"])
        lh=list() #"minutes"
        ri=range(0,61)
        for i in ri:
            lh.append(str(i))
        ui.comboBox_2.addItems(lh)
        ui.comboBox_6.addItems(["00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24"])
        lh=list() #"minutes"
        ri=range(0,61)
        for i in ri:
            lh.append(str(i))
        ui.comboBox_7.addItems(lh)
       #--------------- partie profil de charge si autonome----------------
        if(not platform.platform().__contains__('Linux')):
                poursel="SELECT id from fsch"
                ids=vv.mybas.fetchall(poursel)
                profils=list()
                profils.append("---")
                ri=range(0,len(ids))
                for i in ri:
                    profils.append(str(ids[i][0]))
                    if(isautonome):
                       ui.label_2.show();
                       ui.comboBox_3.addItems(profils)
                       ui.comboBox_3.show()
                    else:
                       ui.label_2.hide();
                       ui.comboBox_3.hide()
        #----------------------------------------------

        #vv=variables(autonomie=isautonome)
        vv.ui=ui
        
        #----------  partie actions ------------------------
        mesacts=mesactions(vv)
        vv.mesacts=mesacts
        mesacts.ui=ui
       
        ui.actionafficher.triggered.connect(mesacts.afficOneShot)
        ui.actionafficher_cycliquement.triggered.connect(mesacts.monafficher_cycliquement)
        ui.actionmise_jour.triggered.connect(mesacts.monmise_jour)
        ui.actionAfficheVarsBorne.setVisible(False)
        ui.actionAfficheVarsBorne.triggered.connect(mesacts.afficVarsBorne)
        ui.actionAfficherVarsBorneCycliquement.setVisible(False)
        ui.actionAfficherVarsBorneCycliquement.triggered.connect(mesacts.afficVarsBorneCycliquement)
        ui.actionChargeok.setVisible(False)
        
        if(not platform.platform().__contains__('Linux')):
            ui.actionChargeok.setVisible(True)
            ui.actionChargeok.triggered.connect(mesacts.chargeok)
        if(not vv.isFilaire):
             ui.actionLib_rer_les_connexions.triggered.connect(mesacts.libercon)
        else: # on remet le GPIO à zero
             ui.actionLib_rer_les_connexions.triggered.connect(mesacts.libercon)
       # ui.actionforcer.triggered.connect(mesacts.monforcer)
        #ui.PBSave.clicked.connect(mesacts.monsave)
        ui.actionarr_ter.triggered.connect(mesacts.monarr_ter)
        MainWindow.destroyed.connect(mesacts.monarr_ter)
        ui.actioneloigner_EV.triggered.connect(mesacts.moneloigne)
       #affiche le nom de lautomobile
        #pdb.set_trace()          
        if(not platform.platform().__contains__('Linux')):
            poursel="SELECT value FROM nodes WHERE name=\"" +  "LDEV1_DEEV1_EVId_setVal" +"\""
            val=util.fetchindb(vv.dbpath,vv,poursel)[0]
            ui.groupBox.setTitle("vehicule automobile " + val)
            poursel="SELECT value FROM nodes WHERE name=\"" +  "int_wifi" +"\""
            val=util.fetchindb(vv.dbpath,vv,poursel)[0]
        else:
            #pdb.set_trace()
            #val=vv.mempart.dicread("IEDEVLDEV1/DEEV1.EVId.setVal")
            #ui.groupBox.setTitle("vehicule automobile " + val)
            ui.groupBox.setTitle("")
            val=str(vv.mempart.dicread("int_wifi"))
        if((val != None) and (val=="1")):
           ui.checkBox_4.setChecked(True)   
        ui.checkBox_4.stateChanged.connect(mesacts.CBwifi)
        ui.checkBox_5.stateChanged.connect(mesacts.CBSimulationChanged)
        ui.checkBox_2.stateChanged.connect(mesacts.CBChargerChanged)
        ui.checkBox.stateChanged.connect(mesacts.CBDechargerChanged)
        ui.CBModifCon.stateChanged.connect(mesacts.CBModifCon)
        if(vv.isautonome):
            ui.checkBox_3.stateChanged.connect(mesacts.CBConnChanged)
            ui.comboBox_3.currentIndexChanged.connect(mesacts.cb3selectionchange)
        else:
            ui.checkBox_3.hide()
        
        ui.comboBox_2.currentIndexChanged.connect(mesacts.cb2selectionchange)
        ui.comboBox_7.currentIndexChanged.connect(mesacts.cb7selectionchange)
        ui.comboBox_5.hide()
        ui.label_7.setText("CD")
        ui.checkBox_2.hide()
        ui.checkBox.hide()
        ui.PBChaOn.clicked.connect(mesacts.monChaOn)
        ui.PBChaOff.clicked.connect(mesacts.monChaOff)
        ui.frame_3.hide()  
        ui.lineEdit.setInputMask("999")
        ui.lineEdit.setText("000")
        ui.lineEdit.editingFinished.connect(mesacts.lineEditEdited)


        
         # ---- preparation selection du type de connexion  -------------------
        #ui.frame_5.hide()
        ui.comboBox_4.hide()
        ui.checkBox_3.hide()
        poursel=" SELECT  valeur  FROM gestion WHERE nom=\"socinit\""
        res=vv.mybas.fetchone(poursel)
        if (res==None): 
            vv.reportlog.error(" socinit non connu dans table gestion")
        else:
            socinit=float(res[0])
            vv.socinit=socinit
        poursel=" SELECT  value  FROM nodes WHERE name=\"int_socmin\""
        res=vv.mybas.fetchone(poursel)
        if (res==None): 
            vv.reportlog.error(" int_socmin non connu dans table nodes")
        else:
            socmin=float(res[0])
            vv.socmin=socmin
        poursel=" SELECT  value  FROM nodes WHERE name=\"int_socmax\""
        res=vv.mybas.fetchone(poursel)
        if (res==None): 
            vv.reportlog.error(" int_socmax non connu dans table nodes")
        else:
            socmax=float(res[0])
            vv.socmax=socmax
        poursel=" SELECT  valeur  FROM gestion WHERE nom=\"chaMax\""
        res=vv.mybas.fetchone(poursel)
        if (res==None): 
            vv.reportlog.error(" chamax non connu dans table gestion")
        else:
            chamax=float(res[0])
        #poursel=" SELECT  valeur  FROM gestion WHERE nom=\"chaMin\""
        #res=vv.mybas.fetchone(poursel)
        #if (res==None): 
            #vv.reportlog.error(" chemin non connu dans table gestion")
        #else:
            #chamin=float(res[0])
        chamin=chamax*vv.socmin*0.01
        poursel=" UPDATE gestion  SET valeur=\"" + str(chamin) + "\"  WHERE nom=\"chaMin\""      
        vv.util.execindb(vv.dbpath,vv,poursel)
        poursel=" SELECT  valeur  FROM gestion WHERE nom=\"wpertes\""
        res=vv.mybas.fetchone(poursel)
        if (res==None): 
            vv.reportlog.error(" wpertes non connu dans table gestion")
        else:
            wpertes=float(res[0])
        #ui.frame_4.hide() # cache la visualisation des variables de la borne
        #pdb.set_trace()
        poursel="SELECT type,capaciteCableAC,capaciteCableDC from voitures"
        voits=vv.mybas.fetchall(poursel)
        types=list()
        types.append("---")
        ri=range(0,len(voits))
        for i in ri:
           voit=str(voits[i][0])
           types.append(voit)
           if(vv.isFilaire): # cas filaire renseigner les capacites
                if((voit.__contains__("vac")) or (voit.__contains__("tri"))):
                    vv.capacites[voit]=vv.util.getCodeFromCapacite(int(voits[i][1]))
                else:
                    vv.capacites[voit]=vv.util.getCodeFromCapacite(int(voits[i][2]))
        ui.comboBox_4.addItems(types)
        ui.comboBox_4.activated.connect(mesacts.cb4selectionchange)
        poursel="SELECT  valeur from gestion WHERE nom=\"typCon\""
        typcon=vv.mybas.fetchone(poursel)
        if(typcon==None):raise("typCon is not found in table gestion")
        ui.LTypeCon.setText(typcon[0])
        vv.typCon=typcon[0]
        vv.reportlog.info("Main init vv.typCon {0}".format(vv.typCon))
        #-------------------------------------------------------
        MainWindow.show()
       # win()
        
        time.sleep(1)
        vv.reportlog.warning("debut")
        #--- lancement de la passerelle 
        vv.pasrel=passerelle(vv)
        #-- simulation ----
        # reset des données initiales (can_ConnSt,int_Marche)
        if(not platform.platform().__contains__('Linux')):
            
            poursel=" UPDATE nodes  SET value=" + '0' +  ", timestamp=" + str(0.0) +" , valueNmoins1=\"" + '0' +  "\", timestampNmoins1=" + str(0.0) + "  WHERE name=\"can_ConnSt\""      
            vv.util.execindb(vv.dbpath,vv,poursel)
            poursel=" UPDATE nodes  SET value=" + '1' +  ", timestamp=" + str(0.0) +" , valueNmoins1=\"" + '1' +  "\", timestampNmoins1=" + str(0.0) + "  WHERE name=\"LDEV1_DEEV1_EVSt_stVal\""      
            vv.util.execindb(vv.dbpath,vv,poursel)
            poursel=" UPDATE nodes  SET value=" + '3' +  ", timestamp=" + str(0.0) +" , valueNmoins1=\"" + '3' +  "\", timestampNmoins1=" + str(0.0) + "  WHERE name=\"LDEV1_DEEV1_ConnTypSel_stVal\""      
            vv.util.execindb(vv.dbpath,vv,poursel)
            poursel=" UPDATE nodes  SET value=" + '40' +  ", timestamp=" + str(0.0) +" , valueNmoins1=\"" + '40' +  "\", timestampNmoins1=" + str(0.0) + "  WHERE name=\"LDEV1_DEEV1_Soc_mag_f\""      
            vv.util.execindb(vv.dbpath,vv,poursel)
            vv.util.saveNodeindb(vv.dbpath,vv,{"LDEV1_DEEV1_WChaTgt_setMag_f":("0",0)})
            vv.util.saveNodeindb(vv.dbpath,vv,{"LDEV1_DEEV1_WCurrent_mag_f":("0",0)})
            util.saveToutVal(vv.dbpath,vv,("int_ChargeMarche",0,0))
            util.saveToutVal(vv.dbpath,vv,("int_wifi",0,0))
            util.saveToutVal(vv.dbpath,vv,("int_adresseCS","127.0.0.1",0.0))
        else:
            resul=vv.mempart.dicread("IEDEVLDEV1/DEEV1.EVId.setVal")
            if(resul==None): raise Exception(" pb dicread")
            vv.ui.LNom.setText(resul)
            resul=vv.mempart.dicwrite("int_PID",os.getpid())
            if(not resul): raise Exception(" pb dicwrite")
            resul=vv.mempart.dicwrite("int_ChargeMarche",0)
            if(not resul): raise Exception(" pb dicwrite")
            vv.mempart.dicwrite("can_ConnSt",0)
            if(not resul): raise Exception(" pb dicwrite")
            vv.mempart.dicwrite("IEDEVLDEV1/DEEV1.EVSt.stVal",1)
            if(not resul): raise Exception(" pb dicwrite")
            vv.mempart.dicwrite("IEDEVLDEV1/DEEV1.ConnTypSel.stVal",3)
            if(not resul): raise Exception(" pb dicwrite")
            vv.mempart.dicwrite("IEDEVLDEV1/DEEV1.Soc.mag.f",socinit)
            if(not resul): raise Exception(" pb dicwrite")
            vv.mempart.dicwrite("IEDEVLDEV1/DEEV1.WChaTgt.setMag.f",0)
            if(not resul): raise Exception(" pb dicwrite")
            vv.mempart.dicwrite("IEDEVLDEV1/DEEV1.WCurrent.mag.f",0)
            if(not resul): raise Exception(" pb dicwrite")
            vv.mempart.dicwrite("int_wifi",False)
            if(not resul): raise Exception(" pb dicwrite")
            vv.mempart.dicwrite("int_socmax",vv.socmax)
            if(not resul): raise Exception(" pb dicwrite")
            vv.mempart.dicwrite("int_socmin",vv.socmin)
            if(not resul): raise Exception(" pb dicwrite")
            vv.mempart.dicwrite("int_chaMax",chamax)
            if(not resul): raise Exception(" pb dicwrite")
            vv.chaMax=vv.mempart.dicread("int_chaMax")
            vv.mempart.dicwrite("int_chaMin",chamin)
            vv.reportlog.info("ZamihmEV ecriture de int_chaMin {0}".format(chamin))
            if(not resul): raise Exception(" pb dicwrite")
            vv.chaMin=vv.mempart.dicread("int_chaMin")
            vv.mempart.dicwrite("int_WPertes",wpertes)
            if(not resul): raise Exception(" pb dicwrite")
            vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.WhMaxRtg.setMag.f",vv.chaMax)
            vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.WhMinRtg.setMag.f",vv.chaMin)
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.DERUnit.seSrcRef","")
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.DschWMax.setMag.f",vv.chaMax*3/10)
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.DschWMaxRtg.setMag.f",vv.chaMax*3/10)
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.InWh.mag.f",0)
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.InclSto1.setSrcRef","IEDEVLDEV1")
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.InclSto2.setSrcRef","")
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.InclSto3.setSrcRef","")
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.OutWh.mag.f",0)
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.DERUnit.seSrcRef","")
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.DschWMax.setMag.f",vv.chaMax*3/10)
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.DschWMaxRtg.setMag.f",vv.chaMax*3/10)
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.InWh.mag.f",0)
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.InclSto1.setSrcRef","IEDEVLDEV1")
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.InclSto2.setSrcRef","")
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.InclSto3.setSrcRef","")
            #vv.mempart.dicwrite("IEDEVLDEV1/DSTO1.OutWh.mag.f",0)
            if(not vv.isFilaire):
                """
            #--- pour test -----------------
                vv.mempart.dicwrite("int_adresseCS","10.3.141.116") #pour test uniquement
                if(not resul): raise Exception(" pb dicwrite")
                print(" j'ecris dans wifi")
                vv.mempart.dicwrite("int_wifi",True)
                memwifi=self.v.mempart.dicread("int_wifi")
                if(memwifi==True):          
                     print( "j'ai bien mis met wifi a True")
                time.sleep(1)
            #------------------------------------------------------------------------
                """

            vv.reportlog.info("restihmEV vv.chaMin= vv.chaMax".format(vv.chaMin,vv.chaMax))
            
        vv.sim=simulEV(vv)
        prov=1

        #-------------------
       
        sys.exit(app.exec_())
    except Exception as ee:
        vv.reportlog.error(" Main erreur:" + str(ee))
    finally:
        app.closeAllWindows()
        app.closingDown()  
        vv.reportlog.warning(" finally")
        os.abort()
