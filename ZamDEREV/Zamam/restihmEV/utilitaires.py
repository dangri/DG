#!/usr/bin/python
""" ensembles de fonctions utilitaires
message(texte,type="Information",boutons=["Ok"]) # montre un dialogue de message 'message' avec des boutons
"""
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v0.0.1  | 03/03/19   |  version initiale avec bogues corrigées                     |  AM  #
# v0.1.1  | 01/02/21   |  - echanges  memoire partagee                               | AM   #
#                                                                                           #
#############################################################################################
from PyQt5 import QtCore, QtGui, QtWidgets
import threading
import time
from Mybase import mybase as mybase
import platform
import struct
import sqlite3
import random
import pdb
if(platform.platform().__contains__('Linux')):
  #  import iec61850
  pass

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
    

def oldgetTemps(vars):
    try:
         if(platform.platform().__contains__('Linux')):
             tt=vars.mempart.read8()
             ttt=struct.unpack('d',tt)
             t=ttt[0]
         else:
             t=vars.tempssimule
         return t

    except Exception as ee:
        vars.reportlog.error("getTemps " + str(ee))
        raise Exception("getTemps " + str(ee))


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
    def __init__(self,points=[],vars=None):
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
        #print(str(self.points))
        if(vars!=None):
            vars.reportlog.info("mycourbe calculee {0}".format(self.yvsx)) 

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


def calculCourbeCharge(min=(0,180),lineaire=(0,180),intermed=(70,280),max=(100,284),pas=5,offset=0.5,sens="positif"):
     """ calcul grossier d'une courbe de charge donnant lea tension/soc
     L'axe des x est gradué de 0 à 100
    La courbe est divisée en 3 parties
     - premire partie de charge lineaire jusqu'à une valeur de tension
     - partie intermediaire jusqu'à une valeur de tension
     - une partie de saturation
     le pas de calcul est la distance entre 2 points valués de soc
     le résultat est une liste de mypoint contenant (x ,y)
       """
     #min=min
     #lineaire=lineaire
     #intermed=intermedmax=max
     #pas=pas
     #offset=offset
     #print("INFO calculCourbeCharge min={0} lineaire={1} offset={2}".format(min,lineaire,offset))
     try:
         
         points=list()
         if(min[0]==lineaire[0]):
            # la ligne initiale est reduite à un point
            points.append(min)
         else:
             #partie lineaire
             if((sens=="positif") or (sens=="negatif")):
                pi=range(min[0],lineaire[0]+pas,pas)
                a=(lineaire[1]-min[1])/(lineaire[0]-min[0])
                for i in pi:
                    points.append((i,min[1]+i*a))
             else: #sens negatif
                pass
          #premire partie de courbe
         if ((sens=="positif") or (sens=="negatif")):
                    #calcul des points de la partie indermediaire
                   # on suit l'equation:
                   # Yn=Yn-1 + D -(n-1)d
                   #ce qui donne Yn=(n-1)*D - n*(n-1)d/2

                   # on connait Yn 
                   # on fixe arbitrairement D => on en déduit d et la possibilité de calculer chaque point
                   #d=2*((n-1)*D -Yn)/n*(n-1)
                   #
                  Yn=intermed[1]-lineaire[1] # le delta de progression de la grandeur
                   # # il faut que (n-1)*D soit plus grand que Yn donc on fixe D à Yn/n-1 + offset
                  pp=range(lineaire[0],intermed[0]+pas,pas) # nb de pas 
                  n=len(pp)
                  #print("calculCourbeCharge avant Z pp={0} n={1} offset={2} pas={3}".format(pp,n,offset,pas))
                  D=Yn/n+offset
                  d=2*(n*D -Yn)/(n*(n-1))     
                  N=0
                  Y=lineaire[1]
                  for p in pp:
                   Y=Y+D-N*d
                   points.append((p,Y))
                   N=N+1
         else: # sens negatif
                  Yn=lineaire[1]-intermed[1] # le delta de progression de la grandeur vers le bas
                   # # il faut que (n-1)*D soit plus grand que Yn donc on fixe D à Yn/n-1 + offset
                  pp=range(lineaire[0],intermed[0]+pas,pas) # nb de pas 
                  n=len(pp)
                  D=Yn/n+offset
                  d=2*(n*D -Yn)/(n*(   n-1))
                  N=0 
                  Y=lineaire[1]
                  for p in pp:
                   Y=Y-D+N*d
                   points.append((p,Y))
                   N=N+1

                   #deuxieme partie de courbe
                   
         if ((sens=="positif") or (sens=="negatif")):    
                  Yn=max[1]-intermed[1] # le delta de progression de la grandeur
                  pp=range(intermed[0],max[0]+pas,pas) # nb de pas 
                  n=len(pp)
                  D=Yn/n+(offset/2)
                  d=2*(n*D -Yn)/(n*(n-1))
                  N=0
                  Y=intermed[1]
                  for p in pp:
                   Y=Y+D-N*d
                   points.append((p,Y))
                   N=N+1
         else: # sens negatif
                  Yn=intermed[1]-max[1] # le delta de progression de la grandeur vers le bas
                   # # il faut que (n-1)*D soit plus grand que Yn donc on fixe D à Yn/n-1 + offset
                  pp=range(intermed[0],max[0]+pas,pas) # nb de pas 
                  n=len(pp)
                  D=Yn/n+offset
                  d=2*(n*D -Yn)/(n*(n-1))
                  N=0 
                  Y=intermed[1]
                  for p in pp:
                   Y=Y-D+N*d
                   points.append((p,Y))
                   N=N+1 
         return points
     except Exception as ee:
           raise Exception("   calcul courbecharge  " +str(ee))
     

def mydeltaPuissance(self,soc,deltat):
        """ fournit un tuple (detacha,deltasoc)
        à partir de la currentSOC
        on suppose le pas de temps assez petit pour  considérer la tension et l'intensite constante 
        et à la soc d'origine donnée
        le deltat est donné en timestamp"""
        try:
            if(not platform.platform().__contains__('Linux')):
              poursel="SELECT value FROM nodes WHERE id=" + str(self.v.NomVersIdDic["WChaTgt"])
              val=self.mybas.fetchone(poursel)[0]
            else:
              val=self.v.mempart.dicread("IEDEVLDEV1/DEEV1.WChaTgt.setMag.f")
            pmax=eval(val)
            deltah=deltat/3600  # on ramène le deltat en heure
            U=self.v.courbeV.Y(soc)
            I=min(self.v.courbeI.Y(soc),pmax/U)
            deltapwr=U*I*deltah
            deltasoc=100*deltapwr/self.v.chaMax
            return (deltasoc,U,I)
        except Exception as ee:
            raise Exception (" erreur calcul deltapuissance " + str(ee))
        
def calculProfilCharge(timedepart,WhTgt,pmaxmin,pmaxmax,nbinterval=100):
    """ calcul un profil charge optimal
       la courbe est calculée pour optimiser la charge demandée 
       en fonction de l'heure de départ choisie
       enenrée on donne les Pmaxmin et Pmaxmax souhaités aux extrémité de la fonction
       on approxime le fonction par une fonction polynomiale f(x)=a-bx+cx2
       Pour calculer les coefficients a , b et c de la fonction , nous avons besoin de 3 equation
       f(0)=a=pmaxmin
       f(timedepart)=pmaxmax=a-b*timedepart+ctimedepart**2
       b=(-pmaxmax+a+c*timedepart**2)/timedepart
        duree=timedepart-timenow
        f(duree)=a+b*duree+c*duree**2
        WhTgt=intgr(f(x))

       Pour approximer l'integrale de la fonction f(x) entre 0 et duree nous utilisons 
       la methode des trapèzes
        rappel pour le trapèze (Xa,Ya),(Xb,Yb) l'aire est (Xb-Xa)*(Yb+Ya)/2

        On suppose que l'on decoupe la duree en k interval de duree égaledonc la largeur d'un trapeze est 
        d=duree/nbinterval

        pour un trapèze de coordonnée Xi,Xi+1 on trouve:
        Yi=a-b*d*i+c*d**2*i**2
        donc 
        Ai=d*(Yi+Yi+1)/2=d*((a-b*d*(i+1)+c*d**2*(i+1)**2)+(a-b*d*i+c*d**2*i**2)/2
        
        2Ai/d=2a-bd(2i+1)+cd**2(2i**2+2i+1)
        2A/d=2na-bdn(n+2)+cd**2(2SC+n(n+2))

        K=dn(n+2)
        T=timedepart-timenow (on raisonne en temps relatif
        P=Pmaxmax
        D=2SC+n(n+2) avec SC=n(n+1)(2n+1)/6
        D=n(n+1)(2n+1)/3+n(n+2)
        D=(n(n+1)(2n+1)+3n(n+2))/3
        D=n((n+1)(2n+1)+3(n+1)+3)/3
        D=n((n+1)(2n+4)+3))/3
       2A/d=2na-Kb+cd**2D
       P=a-bT+cT**2

       2AT/d=2naT-aK+c(d**2DT-KT**2)
       c=(2AT-PKd-2nadT+aKd)/d**2DT-KT**2
       b=(a+cT**2)/T

   """
    try:
     n=nbinterval         #nb de trapèzes (intervales)
     timenow=time.time()
     T=timedepart-timenow # temps relatif
     d=T/n                # temps ehtre 2 points
     K=d*n*(n+2)
     P=pmaxmax
     A=WhTgt              # surfac sous la courbe


     #------------- calcul de coef de l'equation f(x)=a-bt+ct**2
     a=pmaxmin            # on démarre au minima
     hh=(n+1)*(2*n+4) + 3
     D=n*hh/3
     c=(2*A*T-P*K*d-2*n*a*d*T+a*K*d)/(d**2*D*T-K*T**2)
     b=(a+c*T**2)/T
    except Exception as ee:
        print("suite")

class suiviprofil(threading.Thread): 
    """ classe de creation et de demarrage d'un thread pour le suivi d'une courbe de charge"""
    def __init__(self,name=None,bdd=None,variables=None,nom="LDEV1_FCSH1",nodeprofil=0):
        self.bd=bdd
        self.vv=variables
        self.myname=nom
        self.profilno=nodeprofil
        self.fini=False
        #self.simbas=mybase(self.bd,self.vv.reportlog,variables=self.vv) # acces a la base de simulation
        self.mybas=mybase(vv.bdpath,vv.reportlog,variables=self.vv)
        self.courbe=self.setcourbe(self.proFileNotFoundError) # courbe suivie
        threading.Thread.__init__(self,name=name,target=self.suivideprofil)


    def suivideprofil(self):
            """ cette fonction est pour suivre un profil de charge
            stocké dans la base de donnée sous le nom name
               Dans le cas de fonctionnement autonome 
              nodeproffil > 0 un no de  profil est saisi et stocke le profil ad'hoc
              sinon on suppose le profil prêt
              """
            try:
                if((self.bd==None) or (self.vv==None) or (self.myname==None )): raise (" paramètres bdd ou variables non renseigné")
                
               
                dicval=dict()

                while(True):
                    vv.evsuivi.wait()
                    print(" suivi " + self.myname )
                    #valpoint=self.courbe.Y(vv.tempssimule)
                    valpoint=self.courbe.Y(vv.util.getTemps(vv.tempssimule,vv.mempart))
                    #dicval[str(self.v.NomVersIdDic["\""+ self.valeurpilotee + "\""])]=["\""+ str(valpoint) + "\"",str(vv.tempssimule)] 
                    dicval[str(self.v.NomVersIdDic["\""+ self.valeurpilotee + "\""])]=["\""+ str(valpoint) + "\"",str(vv.util.geetTemps())] 
                    #self.simbas.save(dicval)
                    self.mybas.save(dicval)
                    time.sleep(.2)
            except Exception as ee:
                print(" pb dans suivide profil " + str(ee))

   


    def setcourbe(self,profilno):
        try:
          if(self.profilno > 0): # il s'agit d'un suivi de profil autonome
                                           # on commence par charger le schedule de nom self.myname
                    poursel="SELECT NumEntr_setVal,SchdIntv_setVal,StrTm0_setTime,StrTm1_setCal_hr, \
                              StrTm1_setCal_mn,StrTm1_setCal_occType,StrTm1_setCal_occPer,EvtTrg_setVal,valeurs,LDEV1_DEEV1_WChaTgt_setMag_f \
                       FROM fsh WHERE id=" + str(self.profilno)
                    fschvals=self.v.mybas.fetchmany(poursel)
                    NumEntr_setVal=fschvals[0][0]
                    SchdIntv_stVal=fschvals[0][1]
                    StrTm0_setTime=fschvals[0][2]
                    StrTm1_setCal_hr=fschvals[0][3]
                    StrTm1_setCal_mn=fschvals[0][4]
                    StrTm1_setCal_occType=fschvals[0][5]
                    StrTm1_setCal_occPer=fschvals[0][6]
                    EvtTrg_setVal=fschvals[0][7]
                    valeurs=fschvals[0][8]
                    valeurpilotee=fschvals[0][9]
                    self.valeurpilotee=valeurpilotee
                    if(StrTm1_setCal_occPer=="Day"): # traitement des schedule periodique journalier (type jour/njit)
                        values=valeurs.split(";")
                        thistime=time.gmtime()
                        thistime[3]=0
                        thistime[4]=0
                        thistime[5]=0
                        today=mktime(thistime)
                        points=list()
                        ri=range(0,len(values))
                        for i in ri:
                            x=today+i*(SchdIntv_stVal/1000) # heure en s apres epoch
                            y=values[i]
                            points.append((x,y))
                        self.courbe=mycourbe(points)
                    else:
                        message(" ce cas de schedule no" + str(self.profil) + " non encore traite")
                        raise Exception (" cas de type profil non traite " + arg_str(profilno))
        except Exception as ee:
                raise Exception(" pb dans setcourbe")

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
                         #self.vv.ui.label_3.setText(time.strftime("%H:%M:%S", time.gmtime(self.vv.util.getTemps((self.vv.tempssimule)/1000,self.vv.mempart))))
                         #self.vv.ui.label_3.show()
                         pass
                   else:
                       self.repet=self.repet-1
                   time.sleep(1/self.K)  # lorsque K=60 1 seconde reelle = 1s de temps simulee
                   #if(self.vv.util.getTemps(self.vv.tempssimule,self.vv.mempart)==tempscourrant):  #le temps ne bouge plus il faut s'arrêter
                     #  self.vv.mesacts.monarr_ter()
            except Exception as ee:
                self.vv.reportlog.error(" pb ds majtime " + str(ee))
   
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
        
                    bon=True
                if(not self.vv.isFilaire):
                    if(self.vv.isRest):
                        pass
                        """
                        resul= self.vv.mesacts.monclient.postVals(prog="Calc",data=dicval,jeeson=True)
                        if(resul["status"] != "ok"):
                            self.vv.reportlog.error(" CalculsCycliques " + resul["status"])
                            bon=False
                            raise Exception(" echanges serveur ")
                        """
                    else:
                        payload={"id":self.transacID,"verb":"libcon","params":dicval}
                        resul=self.mesacts.monuart.post(payload)
                        self.transacID=self.transacID+1
                        if(resul[0] != "ok"):
                            self.vv.reportlog.error(" CalculsCycliques " + resul[1])
                            bon=False
                            raise Exception(" echanges serveur " +str(ee))
            if("mesureWCurrent"):
                # on suppose une perte de P % "
                if(not platform.platform().__contains__('Linux')):
                    poursel="SELECT name,value,timestamp FROM nodes WHERE (name=\"can_Vcurrent\") or (name=\"can_AmpCurrent\")"
                    vals=self.mybas.fetchmany(poursel)
                    if(len(vals)==0): raise Exception("  donnees non trouvee")
                    if(self.vv.chamarche != 0):
                        wcurrent=float(vals[0][1])*float(vals[1][1])
                        wperte=float(getValfromdb(self.vv.dbpath,self.vv,"int_WPertes"))
                        Wcurrent=("LDEV1_DEEV1_WCurrent_mag_f",str(wcurrent-(random.random()*wperte*wcurrent)/1000),str(getTemps(self.vv.tempssimule,self.vv.mempart)))    
                        saveToutVal(self.vv.dbpath,self.vv,Wcurrent)
                        #self.vv.reportlog.info("calculs cycliques av db WCurrent=" + str(Wcurrent))
                else:
                    if(self.vv.chamarche != 0):
                        vcurrent=self.vv.mempart.dicread("can_Vcurrent")
                        acurrent=self.vv.mempart.dicread("can_AmpCurrent")
                        wcurrent=float(vcurrent)*float(acurrent)
                        wperte=float(self.vv.mempart.dicread("int_WPertes"))
                        wc=self.vv.mempart.dicread("IEDEVLDEV1/DEEV1.WCurrent.mag.f")
                        #self.vv.reportlog.info("calculs cycliques  AVANT WCurrent=" + str(wc))
                        pert=(random.random()*wperte*wcurrent)/1000
                        if(self.vv.pause.is_set()):
                            self.vv.mempart.dicwrite("IEDEVLDEV1/DEEV1.WCurrent.mag.f",wcurrent-pert)   
                            #self.vv.reportlog.info("calcul avec perte wcbrut={0} perte={1}".format(wcurrent,pert ))
                            wc=self.vv.mempart.dicread("IEDEVLDEV1/DEEV1.WCurrent.mag.f")
                            #self.vv.reportlog.info("calculs  WCurrent=" + str(wc))
                        else:
                            self.vv.mempart.dicwrite("IEDEVLDEV1/DEEV1.WCurrent.mag.f",0.0) 
                            wc=self.vv.mempart.dicread("IEDEVLDEV1/DEEV1.WCurrent.mag.f")
                            #self.vv.reportlog.info("calculs  WCurrent=" + str(wc))
                    else:
                        self.vv.mempart.dicwrite("IEDEVLDEV1/DEEV1.WCurrent.mag.f",0.0) 
                       
                
        except Exception as ee:
            raise Exception("calculsCycliques " + str(ee))


    def maj(self):
        """incremente le temps"""
        #self.vv.tempssimule=self.vv.tempssimule+1
        #self.vv.ui.label_3.setText(time.strftime("%H:%M:%S", time.gmtime(self.vv.tempssimule)))
        #self.vv.ui.label_3.show()
#-------------------------------------------------------------------
def setComboBox(vv,combo,table="BatCourbes",type="all"):
        """ ajoute a un combo (ds ui de vv) box la colonne type de la table de la base de donnee bdd 
          le premier champ de la table est obligatoirement id(integer)
          en commençan pas reinitialiser le combo
          type = positif | negatif | all"""
        try:   
            prov=combo.count()
            ri=range(0,combo.count())
            for i in ri:
                prov=combo.itemText(0)
                combo.removeItem(0) ## on vide le combo
            prov=combo.count()
            prov=combo.itemText(0)
            profils=list()  # on va le charger selon le type
            profils.append("---")
            if(table=="BatCourbes"):
                if(type.__contains__("positif") or type.__contains__("negatif")):
                 poursel="SELECT id FROM "+ table +" WHERE type=\"" + type + "\""
            else:
                 poursel="SELECT id FROM " + table
            #ids=vv.simulbas.fetchmany(poursel)
            ids=vv.mybas.fetchmany(poursel)
            if(ids==None): raise Exception(" erreur setCombpBox pas de courbe trouvee" )
            ri=range(0,len(ids))
            for i in ri:
               profils.append(str(ids[i][0]))
            if(vv.isautonome):
               vv.ui.label_2.show();
               prov=combo.count()
               combo.addItems(profils)
               prov=combo.count()
               prov=combo.isEditable()
               prov=combo.currentIndex()
               prov=combo.currentText()
               combo.setCurrentIndex(len(profils)-1)
               prov=combo.currentIndex()
               prov=combo.currentText()
               combo.show()
            else:
               vv.ui.label_2.hide();
               combo.hide()
        except Exception as ee:
           vv.reportlog.error("erreur setCombo " + str(ee))

           raise Exception("erreur setCombo " + str(ee))		   
def setInitCourbe(bd=None,frombd=None,courbeno=1,FSCHName="LDEV1_FSCH1"):
            """ cree un ou des  schedule dans la base de donnée  a partir de donnees prealables
               en general on trouve ces donnees dans la base de simulation fromdb
               mais on peut aussi la trouver dans la base elle meme si fomdb=None
               bd= base de donée à mettre à jour
               frombd base de donnée ou sont stockées les schedules
               courbeno=num deu schedule
               FSCHName="""
            try:
                 if(frombd==None):
                     pourbase=bd
                     frombase=bd
                 else:
                     pourbase=bd
                     frombase=frombd

                 if(courbeno > 0): # il s'agit d'un suivi de profil 
                                               # on commence par charger le schedule de nom self.myname
                        poursel="SELECT NumEntr_setVal,SchdPrio_stVal,SchdIntv_setVal,StrTm0_setTime,StrTm1_setCal_hr,\
                                  StrTm1_setCal_mn,StrTm1_setCal_occType,StrTm1_setCal_occPer,EvtTrg_setVal,InSyn_intAddr,valeurs,valeurmaj \
                           FROM fsch WHERE id="  + str(courbeno)
                        fschvals=frombase.fetchmany(poursel) #,arg=(str(courbeno),))
                        if ((fschvals==None) or (len(fschvals)==0)): raise Exception("setInitCourbe pas de donnees" )
                        NumEntr_setVal=fschvals[0][0]
                        SchdPrio_setVal=fschvals[0][1]
                        SchdIntv_setVal=fschvals[0][2]
                        StrTm0_setTm=fschvals[0][3]
                        StrTm1_setCal_hr=fschvals[0][4]
                        StrTm1_setCal_mn=fschvals[0][5]
                        StrTm1_setCal_occType=fschvals[0][6]
                        StrTm1_setCal_occPer=fschvals[0][7]
                        EvtTrg_setVal=fschvals[0][8]
                        InSyn_intAddr=fschvals[0][9]
                        valeurs=fschvals[0][10]
                        valeurpilotee=fschvals[0][11]
                        dicvals=dict()
                        dicvals[FSCHName + "_NumEntr_setVal"]=(NumEntr_setVal,0)
                        dicvals[FSCHName + "_SchdPrio_setVal"]=(SchdPrio_setVal,0)
                        dicvals[FSCHName + "_SchdIntv_setVal"]=(SchdIntv_setVal,0)
                        dicvals[FSCHName + "_StrTm0_setTm"]=(StrTm0_setTm,0)
                        dicvals[FSCHName + "_StrTm1_setCal_hr"]=(StrTm1_setCal_hr,0)
                        dicvals[FSCHName + "_StrTm1_setCal_mn"]=(StrTm1_setCal_mn,0)
                        dicvals[FSCHName + "_StrTm1_setCal_occType"]=(StrTm1_setCal_occType,0)
                        dicvals[FSCHName + "_StrTm1_setCal_occPer"]=(StrTm1_setCal_occPer,0)
                        dicvals[FSCHName + "_EvtTrg_setVal"]=(EvtTrg_setVal,0)
                        dicvals[FSCHName + "_InSyn_intAddr"]=(InSyn_intAddr,0)
                        dicvals[FSCHName + "_NumEntr_setVal"]=(NumEntr_setVal,0)
                        valeurslist=valeurs.split(';')
                        ri=range(0,len(valeurslist))
                        for i in ri:
                            b=(valeurslist[i],0.0)
                            dicvals[FSCHName + "_ValASG"+ str(i+1) +"_setMag_f"]=b
                        pourbase.savenom(dicvals)
                        


            except Exception as ee: 
                raise Exception(" setInitCourbe " + str(ee))
def setNewInitCourbe(vv,bd=None,frombd=None,courbeno=1,FSCHName="LDEV1_FSCH1"):
            """ cree un ou des  schedule dans la base de donnée  a partir de donnees prealables
               en general on trouve ces donnees dans la base de simulation fromdb
               mais on peut aussi la trouver dans la base elle meme si fomdb=None
               bd= base de donée à mettre à jour
               frombd base de donnée ou sont stockées les schedules
               courbeno=num deu schedule
               FSCHName="""
            try:
                 if(frombd==None):
                     pourbase=bd
                     frombase=bd
                 else:
                     pourbase=bd
                     frombase=frombd

                 if(courbeno > 0): # il s'agit d'un suivi de profil 
                                               # on commence par charger le schedule de nom self.myname
                        poursel="SELECT NumEntr_setVal,SchdPrio_stVal,SchdIntv_setVal,StrTm0_setTime,StrTm1_setCal_hr,\
                                  StrTm1_setCal_mn,StrTm1_setCal_occType,StrTm1_setCal_occPer,EvtTrg_setVal,InSyn_intAddr,valeurs,valeurmaj \
                           FROM fsch WHERE id="  + str(courbeno)
                        fschvals=frombase.fetchmany(poursel) #,arg=(str(courbeno),))
                        if ((fschvals==None) or (len(fschvals)==0)): raise Exception("setInitCourbe pas de donnees" )
                        NumEntr_setVal=fschvals[0][0]
                        SchdPrio_setVal=fschvals[0][1]
                        SchdIntv_setVal=fschvals[0][2]
                        StrTm0_setTm=fschvals[0][3]
                        StrTm1_setCal_hr=fschvals[0][4]
                        StrTm1_setCal_mn=fschvals[0][5]
                        StrTm1_setCal_occType=fschvals[0][6]
                        StrTm1_setCal_occPer=fschvals[0][7]
                        EvtTrg_setVal=fschvals[0][8]
                        InSyn_intAddr=fschvals[0][9]
                        valeurs=fschvals[0][10]
                        valeurpilotee=fschvals[0][11]
                        dicvals=dict()
                        dicvals[FSCHName + "_NumEntr_setVal"]=(NumEntr_setVal,0)
                        dicvals[FSCHName + "_SchdPrio_setVal"]=(SchdPrio_setVal,0)
                        dicvals[FSCHName + "_SchdIntv_setVal"]=(SchdIntv_setVal,0)
                        dicvals[FSCHName + "_StrTm0_setTm"]=(StrTm0_setTm,0)
                        dicvals[FSCHName + "_StrTm1_setCal_hr"]=(StrTm1_setCal_hr,0)
                        dicvals[FSCHName + "_StrTm1_setCal_mn"]=(StrTm1_setCal_mn,0)
                        dicvals[FSCHName + "_StrTm1_setCal_occType"]=(StrTm1_setCal_occType,0)
                        dicvals[FSCHName + "_StrTm1_setCal_occPer"]=(StrTm1_setCal_occPer,0)
                        dicvals[FSCHName + "_EvtTrg_setVal"]=(EvtTrg_setVal,0)
                        dicvals[FSCHName + "_InSyn_intAddr"]=(InSyn_intAddr,0)
                        dicvals[FSCHName + "_NumEntr_setVal"]=(NumEntr_setVal,0)
                        valeurslist=valeurs.split(';')
                        ri=range(0,len(valeurslist))
                        for i in ri:
                            b=(valeurslist[i],0.0)
                            dicvals[FSCHName + "_ValASG"+ str(i+1) +"_setMag_f"]=b
                        #pourbase.savenom(dicvals)
                        saveNodeindb(vv.dbpath,vv,dicvals)


            except Exception as ee: 
                raise Exception(" setInitCourbe " + str(ee))

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

def getValeur(self, adresse,adserveur="localhost"):
            """recupère la valeur d'une variable 61850
         adresse sous la forme [ adresse6850,fc,type] 
         type=BOOLEAN|INTEGER|FLOAT|STRING"""
        
            res = "";
            ad = adresse[0];
            fc = adresse[1];
            type = adresse[2];
           # MessageBox.Show(" recuperation de valeur avec l'adresse: " + ad + "," + fc + "," + type);
           # ici recuperationn des donnee sivant le fc et le type de da
            
            
            try:
                FunctionalConstraint=iec61850.FunctionalConstraint_fromString(fc) 
                con = iec61850.IedConnection_create()
                error = iec61850.IedConnection_connect(con, adserver, 102) # ATTENTION recuperer l'adresse serveur
                if (error != iec61850.IED_ERROR_OK): raise Exception(" erreur connexion")
                #con.Connect(this.adServeur, 102);
                if(type== "BOOLEAN"):
                   #res = con.ReadBooleanValue(ad, getFC(fc)).ToString();
                   [value, error] = iec61850.IedConnection_readBooleanValue(con,ad, FunctionalConstraint)
                elif(type=="INTEGER"):
                   #res = con.ReadIntegerValue(ad, getFC(fc)).ToString();
                   [value, error] = iec61850.IedConnection_readIntegerValue(con,ad, FunctionalConstraint)
                elif(type=="FLOAT"):
                   #res = con.ReadFloatValue(ad, getFC(fc)).ToString();
                   [value, error] = iec61850.IedConnection_readFloatValue(con,ad, FunctionalConstraint)
                elif(type=="STRING"):
                   #res = con.ReadStringValue(ad, getFC(fc)).ToString();
                    [value, error] = iec61850.IedConnection_readStringValue(con,ad, FunctionalConstraint)     
                iec61850.IedConnection_close(con)
                iec61850.IedConnection_destroy(con)  


            except Exception as ee:
                raise Exception("ER: getValeur " + ad  +" " + str(ee))
            return res
            # ff = con.ReadFloatValue("IEDEVLDEV1/FSCH1.ValASG1.setMag.f", FunctionalConstraint.SP);

def traitAlerte(vv,message):
    """traite les messagde d'alerte"""
    
    try:
        mes=message.split('!')
        vv.reportlog.info("traiAlerte mes ={0} len={1}".format(mes,len(mes)))
        for sousmes in mes:
            oo=sousmes.split(':')
            ordre=oo[0]
            if(len(oo)<2):
                valeur=""
            else:
                valeur=oo[1]
            if(ordre=="action"):   #connexion avec reservation
                if((valeur=="load") or (valeur=="unload")):
                    if((len(mes) != 5) and (len(mes) !=2)): 
                        raise Exception("traitAlerte message  mal forme len={0}".format(len(mes)))
                    if((len(mes) == 5)):
                        """
                        les champs suivants du message sont HBegin ,HEnd et Wh
                         on doit remplir les champs du dialogue
                          1ligne: comboBox , comboBox_2
                          2ligne: comboBox_6 , comboBox_7
                        """
                        #hbegin
                        hbegin=mes[2].split(":")[1]
                        hend=mes[3].split(":")[1]
                        wh=float(mes[4].split(":")[1])
                    
                        if (valeur=="unload"):
                            h1=hbegin.split("h")[0]
                            mn1=hbegin.split("h")[1]
                            hour=hour=int(h1)
                            minute=int(mn1)
                            vv.timestart=  (hour*60 + minute)*60000
                            h2=hend.split("h")[0]
                            mn2=hend.split("h")[1]
                            hour=hour=int(h2)
                            minute=int(mn2)
                            vv.timedepart=  (hour*60 + minute)*60000
                            vv.reportlog.info("traitAlerte h1={0} mn1={1} h2={2} mn2={3}".format(h1,mn1,h2,mn2))

                        elif(valeur=="load"):
                            h2=hbegin.split("h")[0]
                            mn2=hbegin.split("h")[1]
                            hour=hour=int(h2)
                            minute=int(mn2)
                            vv.timestart=  (hour*60 + minute)*60000
                            h1=hend.split("h")[0]
                            mn1=hend.split("h")[1]
                            hour=hour=int(h1)
                            minute=int(mn1)
                            vv.timedepart=  (hour*60 + minute)*60000
                        vv.ui.comboBox.setCurrentText(h1) 
                        if(mn1[0]=='0'):
                            mn1=mn1[1]
                        vv.ui.comboBox_2.setCurrentText(mn1)                     
                        vv.ui.comboBox_6.setCurrentText(h2)
                        if(mn2[0]=='0'):
                            mn2=mn2[1]
                        vv.ui.comboBox_7.setCurrentText(mn2)
                                       
                        poursel=" SELECT  valeur  FROM gestion WHERE nom=\"chaMax\""
                        chamax=float(vv.mybas.fetchone(poursel)[0])
                        poursel=" SELECT  valeur  FROM gestion WHERE nom=\"chaMin\""
                        chamin=float(vv.mybas.fetchone(poursel)[0])
                        chainit=vv.socinit*chamax/100
                        if(valeur=="unload"):
                            if(chamin<(chainit-wh)):
                                socpossible=((chainit-wh)/chamax)*100
                                vv.whreserve=chainit-wh
                            else:
                                socpossible=(chamin/chamax)*100
                                vv.whreserve=chainit-chamin
                        elif(valeur=="load"):
                            if(chamax<chainit+wh):
                                socpossible=100
                            else:
                                socpossible=100*(chainit+wh)/chamax
                        vv.socDemandee=socpossible
                        vv.reportlog.info("traitalerte chainit={0} chamax={1} wh={2}".format(chainit,chamax,wh))
                               
                     
                        vv.ui.lineEdit.setText(str(int(socpossible)))
                        vv.reportlog.info("traitAlerte ecriture lineEdit socpossible={0} vv.socdemandee={1}".format(socpossible,vv.socDemandee))
                        if(valeur=="load"):
                            vv.chaAlerte=1
                        elif(valeur=="unload"):
                            vv.chaAlerte=2
                        if(valeur=="unload"):
                            #vv.ui.checkBox.show();
                            #vv.ui.checkBox.setChecked(True)
                            pass
                        elif(valeur=="load"):
                            #vv.ui.checkBox_2.show();
                            #vv.ui.checkBox_2.setChecked(True)
                            pass
                    elif((len(mes)==2)): #connexion sans reservation
                        vv.socDemandee=0;
                        vv.chaAlerte=0
                        #self.v.ui.checkBox_2.setChecked(True)
                    vv.reportlog.info("traitAlerte chaSaisie={0}".format(vv.chaSaisie))
                        

    except Exception as ee:
        #print("traitAlerte message={0} err={1}".format(message,ee))
        vv.reportlog.error("traitAlerte message={0} err={1}".format(message,ee))
