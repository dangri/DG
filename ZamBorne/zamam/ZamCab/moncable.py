import sys
import os
import io
import logging
import platform
import time
import lxml
import threading
import utilitaires
from cable import cable as cable
if(platform.platform().__contains__('Linux')):
             import posix_ipc


"""
cette application traite les échanges de simulation du cable de connexion EV
Version 1.1.2    10/05/21
"""

# -------------  variables  ------------------------
vs = None


# -----------
class XMLP:
    """lecture des fichiers XML de configuration """

    def __init__(self):
        """constructeur de XmlParser"""
        try:
            # pdb.set_trace();
            curdir = os.getcwd()

            confpath = os.getcwd() + "/ZamEVSE-Config.xml"
            if (os.path.exists(confpath)):
                self.activeConfigPath = confpath;
            else:
                self.activeConfigPath = ""
            self.tree = lxml.etree.parse(self.activeConfigPath)
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

    def getProfDataValue(self, select):
        """ recupere la valeur de la profdata de nom name """
        try:
            express = "./dataProfile/profdata[@name=\"" + str(select) + "\"]"
            self.e = self.root.find(express)
            if (self.e != None):
                return self.e.attrib["value"]
            else:
                return ""
        except Exception as ee:
            print("XMLP.getConfigValue " + str(ee))

    def getJsonDataValue(self, select):
        """ recupere la valeur de la profdata de nom name """
        try:
            express = "./dataProfile/jsondata[@name=\"" + str(select) + "\"]"
            self.e = self.root.find(express)
            if (self.e != None):
                return self.e.attrib["value"]
            else:
                return ""
        except Exception as ee:
            print("XMLP.getConfigValue " + str(ee))


# ------

class Variables:

    def __init__(self):
        """ constructeur des variables """
        #xmlp = XMLP()
        try:
            # pdb.set_trace();
            #self.xmlp = xmlp
            self.tempo= 1 # temporisation de polling
            self.iedevse = "IEDSE"
            self.ldevse = "LDEVSE/"
            self.curDir = os.getcwd()  # repertoire courant
            self.loghandler = logging.FileHandler("/home/pi/logs" + "/zamreportbornepy.log", mode='w', encoding=None, delay=False)
            self.loghandlerconsole = logging.StreamHandler(stream=sys.stderr)
            self.formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
            self.loghandler.setFormatter(self.formatter)
            self.loghandlerconsole.setLevel(logging.ERROR)
            self.reportlog = logging.getLogger('reportlog')
            self.reportlog.setLevel(logging.INFO)
            self.reportlog.addHandler(self.loghandler)
            self.reportlog.addHandler(self.loghandlerconsole)
            self.mempart = None
            self.dicvars = None
            self.ll = threading.RLock()
            self.sem = None
            self.cabl = None  # le cable
            self.util = utilitaires

            if (platform.platform().__contains__('Linux')):
                import posix_ipc
                self.sem = posix_ipc.Semaphore("/sem.ZAMSEMAL")
                import mempart
                # pdb.set_trace()
                self.mempart = mempart.MemPart(adresse="/ZAMVARCST", vs=self)
                self.dicvars = dict()

            else:
                self.sem = None
                self.dicvars = None
        except Exception as ee:
            # pdb.set_trace()
            print("erreur init  variables " + str(ee))
            a = input()

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
            raise Exception("initdicvars " + str(ee))

def pourtest(vv):
    """ initialisations pour les tests"""
    try:
        vv.mempart.dicwrite("int_adresseSE", "192.168.1.34")
    except Exception as ee:
        vv.reportlog.error("pourtest " + str(ee))
        print("erreur pourtest " + str(ee))


def traitconnecte(vv,type=""):
    """ traite la connection """
    try:
        if(vv.cabl.etatconnect != 0):
            vv.reportlog.warning(" deja connecte")
        else:
            code = vv.cabl.getCodeSelonPins()
            vv.reportlog.info(" VE se connecte avec code {0}".format(code))
            if(code=="0"):
                vv.reporlog.warning (" etat incoherent de connexion signal= HIGH code= 0")
                print(" etat incoherent de connexion signal= HIGH code= 0")
                return
            if(type=="AC"):
                vv.cabl.etatconnect=1
                vv.mempart.dicwrite(vv.iedevse + vv.ldevse + "DEAO1.CabRtgAC.stVal", code)
                vv.mempart.dicwrite(vv.iedevse + vv.ldevse + "DEAO1.PlgStAC.stVal", 4)
            else:
                vv.cabl.etatconnect=2
                vv.mempart.dicwrite(vv.iedevse + vv.ldevse + "DEDO1.CabRtgDC.stVal", code)
                vv.mempart.dicwrite(vv.iedevse + vv.ldevse + "DEDO1.PlgStDC.stVal", 4)
            time.sleep(0.3)
            print(" message connexion type={0} code={1}".format(type, code))


    except Exception as e:
        raise Exception("connecte "+str(e))

def traitdeconnecte(vv,type=None):
    """ traite la connection """
    try:
        if(vv.cabl.etatconnect==0):
            vv.reportlog.warning(" cable deja deconnecte")
           # print(" warning cable deja deconnecte")
            return
        vv.cabl.etatconnect=0
        print(" message deconnexion type={0} ".format(type))
        if(type=="AC"):
            vv.mempart.dicwrite(vv.iedevse + vv.ldevse + "DEAO1.PlgStAC.stVal", 1)
            vv.mempart.dicwrite(vv.iedevse + vv.ldevse + "DEAO1.CabRtgAC.stVal", 98)
            vv.reportlog.info(" deconnecte {0}/DEAO1_PlgStAC_stVal=1".format(vv.ldevse))
            print(" deconnecte " + vv.ldevse + "DEAO1_PlgStAC_stVal ")
        else:
            vv.reportlog.info(" dicwrite {0}/DEDO1.PlgStAC_stVal=1".format(vv.ldevse1))
            vv.mempart.dicwrite(vv.iedevse + vv.ldevse + "DEDO1.PlgStDC.stVal", 1)
            vv.mempart.dicwrite(vv.iedevse + vv.ldevse + "DEDO1.CabRtgDC.stVal", 98)
            time.sleep(0.3)
            print(" deconnecte " + vv.ldevse + "DEDO1_PlgStDC_stVal ")
    except Exception as e:
        raise Exception("deconnecte " + str(e))


if __name__ == "__main__":
    try:
        vs = Variables()
        vs.initdicvars()
        # vs.ll=threading.RLock()
        # vs.ll.acquire(blocking=False)
        args = sys.argv
        if (len(args) <= 1):
            if (platform.platform().__contains__('Linux')):
                bddrep = '/home/pi/zamam'
                vs.dbpath = bddrep + "/ZamEVSEBase.db"
                vs.mempart.setVars(vs)
                vs.dicvars = dict()
                vs.initdicvars()
                pourtest(vs)
                val=vs.mempart.dicread("int_adresseSE")
                if(val==""):
                    raise Exception("int_adesseSE non initialisee ")
                valeur = [0]
            else:
                bddrep = 'C:\\Users\\andell3\\Documents\\Maquette_Vedecom\\tests'
                vs.dbpath = bddrep + "\\ZamEVSEBase.db"
                poursel = " SELECT value FROM nodes WHERE  name=\"int_adresseCS\""
                valeur = utilitaires.fetchindb(vs.dbpath, vs, poursel)[0]
        else:
            ri = range(1, len(args))
            for i in ri:
                ll = args[i].split('=')
                if (ll[0] == "bddrep"):
                    bddrep = ll[1]
                    if (platform.platform().__contains__('Linux')):
                        vs.dbpath = bddrep + "/ZamEVSEBase.db"
                        vs.mempart.setVars(vs)
                        vs.dicvars = dict()
                        vs.initdicvars()
                    else:
                        vs.dbpath = bddrep + "\\ZamEVSEBase.db"
                elif (ll[0] == "iedevse"):
                    vs.iedevse = ll[1]

        valeur = vs.mempart.dicread("int_adresseSE")

        vs.cabl=cable(vs, type="borne")

        print("rapport dans "+ os.getcwd() + "/reportzcab.txt")
        vs.reportlog.info("---------  moncable ------------ ")
        vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEAO1.PlgStAC.stVal", 1)
        vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEDO1.PlgStDC.stVal", 1)
        vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEAO1.CabRtgAC.stVal", 98)
        vs.mempart.dicwrite(vs.iedevse +  vs.ldevse + "DEDO1.CabRtgDC.stVal", 98)
        pid = os.getpid();
        vs.mempart.dicwrite("int_PID", pid)
        #print("pid={0} Attente message vehicule".format(pid))
        fin="nok"
        while(fin == "nok"):  #on va lire GPIO de manière cyclique
            # on lit le GPIO  de la borne pour décider de ce qu'on fait
            # pour l'instant on fait du polling mais plus tard on fera de l'évenements
             #on surveille les connection AC et DC
            #vs.reportlog.info("moncable surveillance {0}".format(vs.cabl.getEtat()))
            vs.cabl.getPins()
            if(vs.cabl.getEtat()==3):
                ok=False
                while(not ok):
                    vs.reportlog.info("moncable surveillance apres etat3 {0}".format(vs.cabl.etat))
                    cac = vs.cabl.getconnect(kind="AC")
                    cdc = vs.cabl.getconnect(kind="DC")
                    if(cac != cdc):
                        if(vs.cabl.etatconnect==0):
                            if(cac):
                                traitconnecte(vs, type="AC")
                            elif(cdc):
                                traitconnecte(vs, type="DC")
                        ok=True
                    time.sleep(0.2)
            if(not vs.cabl.keepAlive(kind="borne")):
                    #on est mort on force une deconnexion
                    if (cac):
                        traitdeconnecte(vs, type="AC")
                    elif (cdc):
                        traitdeconnecte(vs, type="DC")
            time.sleep(vs.tempo)

    except Exception as ee:
             vs.reportlog.error("moncable " + str(ee))
             print("erreur moncable " +str(ee))
             a=input()







