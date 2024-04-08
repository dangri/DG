#!/usr/bin/python3
# -*- coding: utf-8 -*-
""" ce serveur simule le serveur necessaire à dot.vision et sert aux communications "cable" de EV en simulation
il est aussi le serveur du client dotvision
               Les uri qui sont  proposées  et leurs déclinaisons get et post

               /CS
               Post:  
                      connexion d'un vehicule à la CS

              /dc
               Post:
                      deconnexion d'un vehicule à la CS

              Les uri qui seront a l'avenir  proposées  et leurs déclinaisons get et post
                         /setNode
                                 Post : 
                                              paramètres renseignés du node
                    /getNodes
                                 Get :
                                         Envoie les données de tous les nodes  node en json 
                  /getNodes/<no>
                                 Get :
                                        Envoie les données d’un node d’identificateur <no>
                /getNodeValues
                                   Get :
                                         Envoie toutes les nodeValues et leursdonnées
                  /getNodes/<no>
                                    Get :
                                            Envoie une nodeValue et ses données
               /setNodeValues
                                 Post :
                                      Positionne  l’ensemble des données qui sont envoyées
                                         Si une  n’est pas de  type ou W  la valeur correspondante est ignorée, le compte rendu est ok
             

"""
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v0.0.1  | 03/03/19   |  version initiale avec bogues corrigées                     |  AM  #
#############################################################################################
#
# exemples de ligne de commande de lancement
# python Zamserver.py bddrep=" C:\Users\andell3\Documents\Maquette_Vedecom\tests"

from flask import Flask, request, jsonify
from flask_restful import Resource, Api 
#from sqlalchemy import create_engine
from json import dumps
#from Mybase import mybase as mybase
import sqlite3
import os
import collections
import logging
import threading
import platform
import sys
import mempart
 
size=20 #taille des requetes sqlite
app = Flask(__name__)

api = Api(app)

##---------------------------------------------------------------------------------------------------
class utilitaires:
    def __init__(self):
        pass
    def gettyp(self,val):
        """ donne une chaine du type d'une variable"""
        tt=str(type(val))
        tt=tt.strip('<>')
        valtyp=tt.split(' ')[1]
        valtyp=eval(valtyp)
        return valtyp

def execindb(poursel):
        """
        effectue une action sur la base de donnee db

        """
        global vs
        try:
         # vv.reportlog.info("avant acquire execind poursel=" + pourcel)
          conn=sqlite3.connect(vs.dbpath) 
          conn.execute(poursel)
          conn.commit()
          conn.close()   
         # vv.reportlog.info("avant release execind poursel=" + pourcel)
        except Exception as ee:
             raise Exception("execindb " + str(ee))
def getValfromdb(nomval):
        """ recupere une valeur d'une  base """
        global vs
        poursel="SELECT value FROM nodes WHERE name=\""+ nomval + "\""
        val=fetchindb(poursel,mode="one")[0]
        return val

def setValtobd(nomval,value,time):
        """ met a jour  une valeur d'une  base """
        global vs
        try:
            poursel=" UPDATE nodes SET value=\"" + value + "\", timestamp=" + str(time) + " WHERE name=\"" + nomval + "\""
            execindb(poursel)
        except Exception as ee:
            vv.reportlog.error(" errer setval " + poursel + " " + str(ee))

def fetchindb(poursel,mode="one",size=500):
      global vs
      try:
         #vv.reportlog.info("avant acquire fetchinbd poursel=" + pourcel)
         conn=sqlite3.connect(vs.dbpath) 
         c=conn.execute(poursel)
         if(mode=="one"):
            res = c.fetchone()
         else:
            res= c.fetchmany(size)
         conn.close() 
         #vv.reportlog.info("avant release execind poursel=" + pourcel)
         return res
      except Exception as ee:
         raise Exception("fetchindb " + str(ee))

##-----------------------------------------------------------------------------------

class variables:
    def __init__(self):
        """ constructeur des variables """
        try:
         self.curDir=os.getcwd() # repertoire courant
         self.loghandler=logging.FileHandler("/home/pi/logs/reportServicesZamrest.txt",mode='w',encoding=None,delay=False)
         self.formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
         self.loghandler.setFormatter(self.formatter)
         self.reportlog=logging.getLogger('reportlog')
         self.reportlog.setLevel(logging.INFO)
         self.reportlog.addHandler(self.loghandler)
         self.iedevse = "IEDSE"
         self.ldevse = "LDEVSE?/"
         self.mode=None
         self.dbpath=None
         self.dicvars=dict()
         self.util=utilitaires()
         
      
        except Exception as ee:
            print(" erreur variables " + str(ee))
    def initdicvars(self):
        try:
            if (self.dicvars != None):
                varlist = [ \
                    ("Temps", "ull", 8, 0), \
                    ("int_adresseCS", "ip", 20, 8), \
                    ("int_adresseEV", "ip", 20, 28), \
                    ("int_adresseSE", "ip", 20, 48), \
                    ("mod_MA", "int", 4, 68), \
                    ( self.iedevse + self.ldevse +"DEAO1.CabRtgAC.stVal", "int", 4, 72), \
                    ( self.iedevse + self.ldevse +"DEAO1.ChaARtg.setVal", "int", 4, 76), \
                    ( self.iedevse + self.ldevse +"DEAO1.ConnSt.stVal", "int", 4, 80), \
                    ( self.iedevse + self.ldevse +"DEAO1.ConnTypDC.setVal", "bool", 4, 84), \
                    ( self.iedevse + self.ldevse +"DEAO1.ConnTypPhs1.setVal", "bool", 4, 88), \
                    ( self.iedevse + self.ldevse +"DEAO1.ConnTypPhs3.setVal", "bool", 4, 92), \
                    ( self.iedevse + self.ldevse +"DEAO1.PlgStAC.stVal", "int", 4, 96), \
                    ( self.iedevse + self.ldevse + "DESE1.ChaAMaxg.setVal", "float", 4, 100), \
                    ( self.iedevse + self.ldevse + "DESE1.ChaA.mag.f", "float", 4, 104), \
                    ( self.iedevse + self.ldevse +"DESE1.ChaV.mag.f", "float", 4, 108), \
                    ("int_PID", "int", 4, 112), \
                    ( self.iedevse + self.ldevse +"DEDO1.CabRtgDC.stVal", "int", 4, 116), \
                    ( self.iedevse + self.ldevse +"DEDO1.ChaARtg.setVal", "int", 4, 120), \
                    ( self.iedevse + self.ldevse +"DEDO1.ConnStA.stVal", "int", 4, 124), \
                    ( self.iedevse + self.ldevse +"DEDO1.ConnStC.stVal", "int", 4, 128), \
                    ( self.iedevse + self.ldevse +"DEDO1.PlgStDC.stVal", "int", 4, 132), \
                    ( self.iedevse + self.ldevse + "DEDO1.PlgStDC.stVal", "int", 4, 132), \
                    ("int_LDEVSE1_Vcurrent", "float", 4, 136), \
                    ("int_LDEVSE1_AmpCurrent", "float", 4, 140), \
                    ]
                for var in varlist:
                    # pdb.set_trace()
                    cc = var[0]
                    tt = var[1]
                    los = var[2]
                    aa = var[3]
                    self.mempart.dicvarsadd(cc, typ=tt, lo=los, ad=aa)
                self.reportlog.info("dicvars:\n\r" + str(self.dicvars))
        except Exception as ee:
            vs.reportlog.error("initvars {0}".format(ee))
            raise Exception("initdicvars " + str(ee))




class CS(Resource):
    def get(self):
        global vs
        try:
            pass
        except Exception as ee:
            vs.reportlog.error(" cs/get " +str(ee))
            print(" erreur cs/get " + str(ee))

    def post(self):
        global vs
        try:
                #simbas=mybase('../ZamStation.db',vs.reportlog) # connect to database
                #mybas=mybase(vs.dbpath,vs.reportlog) # connect to database
                vs.reportlog.info("CS  debut traitement du post {0}".format(vs.mode))
                dicval=dict()
                r=request
                d=dict(r.form)
                b=request.json
                bb=eval(str(b))
                vs.reportlog.info("CS data= bb={0} d={1}".format(bb,d))
                if(len(d) !=0):
                    bb=d
                else:
                    pass

                if(vs.mode=="Station"):
                        # recherche d'un EVSE libre
                       poursel="SELECT name,state FROM bornes"
                       vals=fetchindb(poursel,mode="many",size=20)
                       ret=list()
                       ri=range(0,len(vals))
                       for i in ri:
                           if(vals[i][1]=="F"):
                              ret.append(vals[i][0])
                       if(len(ret)==0):
                            # tous les sockets sont occupés
                          return{'status': 'toutes les prises CS sont occupées'}   
                       prefixEVSE="LD"+ ret[0] + "_"
                       prefixEV="LDEV"+str(str(prefixEVSE[6]))+"_"
                       vs.reportlog.info("CS evse={0}".format(prefixEVSE))
                       poursel="SELECT AdBorne FROM bornes WHERE name=\"" + ret[0] + "\""
                       val=fetchindb(poursel)
                       return{'status': 'ok','prefixevse' :val}             
                elif(vs.mode.__contains__("EVSE")):
                       capac=bb["capac"]
                       capdc=bb["capdc"]
                       vs.reportlog.info("CD EVSE ecritre dans le LD {0}".format(vs.iedevse + vs.ldevse))
                       if(bb["type"]==str(5)):
                            vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEDO1.CabRtgDC.stVal", capdc)
                            vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEDO1.PlgStDC.stVal", 4)
                       else:
                            vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEAO1.CabRtgAC.stVal", capac)
                            vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEAO1.PlgStAC.stVal", 4)
                       return{'status': 'ok','prefixevse' :vs.ldevse} 
                else:
                    vs.reportlog.info("CS failure")
                    return{'status': 'failure'}         

        except Exception as ee:
            vs.reportlog.error(" cs/post " +str(ee))
            return{'status': 'failure'} 
            print(" erreur cs/post " + str(ee))
class Calc(Resource):
    """
    cette classe reçoit les donnees des calculs cycliques des EV
    (name=\"can_Vcurrent\")or (name=\"can_AmpCurrent\")or (name=\"int_LDEVSE\")"
   """

    def post(self):
        try:
            #simbas=mybase(vs.simdbpath,vs.reportlog) # connect to database
            #mybas=mybase(vs.dbpath,vs.reportlog) # connect to database
            dicval=dict()
            r=request
            d=dict(r.form)
            b=request.json
            bb=eval(str(b))
   
            if((len(d)) !=0):
               bb=d
            elif(len(b) != 0):
               pass
            else:
                return{'status': 'failure'}     
            ldevse=bb["int_LDEVSE"]
            vamp=bb["can_AmpCurrent"]
            vv=bb["can_VCurrent"]
            return{'status': 'ok'}     
            
        except Exception as ee:
            vs.reportlog.error(" cs/post " +str(ee))
            return{'status': 'failure'} 
            print(" erreur cs/post " + str(ee))

class Deconnecte(Resource):
    def get(self):
        global vs
        try:
            pass
        except Exception as ee:
            vs.reportlog.error(" mesures/get " +str(ee))
            print(" erreur mesures/get " + str(ee))

    def post(self):
        try:
            #mybas=mybase('../ZamStation.db',vs.reportlog) # connect to database
            dicval=dict()
            r=request
            d=dict(r.form)
            b=request.json
            bb=eval(str(b))     
            if((len(d)) !=0):
                bb=d
            else:
                pass
            if(vs.mode.__contains__("EVSE")):
                    evid=bb["EVId"]
                    temps=bb["time"]
                    #prefixEVSE=bb["prefixevse"]
                    #prefixEV="LDEV"+str(str(prefixEVSE[6]))+"_"
                    vs.reportlog.info("appel a dicwrite")
                    vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEAO1.PlgStAC.stVal", 1)
                    vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEAO1.CabRtgAC.stVal", 98)
                    vs.reportlog.info(" deconnecte {0}/DEAO1_PlgStAC_stVal=1".format(vs.ldevse))
                    vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEDO1.PlgStDC.stVal", 1)
                    vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEDO1.CabRtgDC.stVal", 98)
                    vs.reportlog.info(" deconnecte dicwrite {0}/DEDO1.PlgStDC_stVal=1".format(vs.ldevse))
                    return{'status': 'ok','prefixevse' : vs.ldevse}     
            else:
                    print( "DC erreur  deconnexion ")
                    return{'status': 'failure'} 
                

        except Exception as ee:
            vs.reportlog.error(" cs/post " +str(ee))
            print(" erreur dc/post " + str(ee))
            return{'status': 'failure'} 
            
class Mg(Resource):
    """ pour de futurs services"""
    def get(self):
        pass
    def post(self):
        try:

            dicval=dict()
            d=dict(request.form)
            b=None
            b=request.get_json()
            #vs.reportlog.info("MG b={0}".format(b)) 
            if((len(d)) !=0):
                pass
            elif(len(b) != 0):
                pass
            return {'status':'ok={0} {1} '.format(b,d)}   
        except Exception as ee:
            return {'status':'failure {0}'.format(ee)}
            print("fin Zamserver")

class Test(Resource):
        
        def post(self,param):
            vs.reportlog.info("Test {0}".format(param))
            print( " test liaison ")
            return {'status':'ok '}
#--------   le routage du serveur ---------------------------------

#api.add_resource(Mesures, '/mesures') # Route_1
api.add_resource(Mg,'/mg')
api.add_resource(CS, '/cs') # Route_1
api.add_resource(Deconnecte,'/dc') #Route-2
api.add_resource(Calc,'/Calc') #Route-3



#-------------------------------------------------------------------

if __name__ == '__main__':
	#app.run()
    #import pdb
    #pdb.set_trace()
    try:
        vs=variables()
       

        args=sys.argv
        ri=range(1,len(args))
        for i in ri:
                ll=args[i].split('=')
                if(ll[0]=="mode"):
                    vs.mode=ll[1]
                    if((not vs.mode.__contains__("EVSE")) and (vs.mode != "Station")):
                        raise Exception( "mode doit être de la forme EVSEx ou Station")
                    else:
                        if(vs.mode != "Station"):
                            vs.ldevse="LD"+vs.mode+"/"
                    vs.reportlog.info(" ZamServermode={0}".format(vs.mode))
                elif(ll[0]=="bddrep"):
                    if(vs.mode==None):
                       print( "mode doit etre soit station ou borne")
                       raise Exception("")
                    bddrep=ll[1]
                    if(platform.platform().__contains__('Linux')):
                        if(vs.mode.__contains__("EVSE")):
                            vs.dbpath=bddrep+"/ZamEVSEBase.db"
                        else:
                            vs.dbpath=bddrep+"/ZamStation.db"
                    else:
                        if(vs.mode.__contains__("EVSE")):
                             vs.dbpath=bddrep+"\\ZamEVSEBase.db"  
                        else:
                            vs.dbpath=bddrep+"\\ZamStation.db"
                    #vs.mybas=mybase(vs.dbpath,vs.reportlog)
                   
        if(vs.mode != "Station"):
            vs.ldevse = "LD" + "EVSE" + "/"
            vs.mempart = mempart.MemPart(adresse="/ZAMVARCST", vs=vs)
            vs.initdicvars()
        print("ZamServer mode={0}".format(vs.mode))
        app.run("0.0.0.0","4000") # 0.0.0.0 pour être à l'écoute de tous les ports
    except Exception as ee:
        print( "lancement zamRestServer {0}".format(ee))