import serial
import sys
import echuart
import os
import io
import logging
import platform
import lxml
import threading
import utilitaires
import json
if(platform.platform().__contains__('Linux')):
             import posix_ipc


"""
cette application traite les échanges de simulation du cable de connexion EV"""
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
            self.iedevse = "IEDSE"
            self.ldevse = "LDEVSE/"
            self.winport="COM1"
            self.curDir = os.getcwd()  # repertoire courant
            self.loghandler = logging.FileHandler(self.curDir + "/reportzcab.txt", mode='w', encoding=None, delay=False)
            self.formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
            self.loghandler.setFormatter(self.formatter)
            self.reportlog = logging.getLogger('reportlog')
            self.reportlog.setLevel(logging.INFO)
            self.reportlog.addHandler(self.loghandler)
            self.mempart = None
            self.dicvars = None
            self.ll = threading.RLock()
            self.sem = None
            self.vuart = None
            self.util = utilitaires
            if (platform.platform().__contains__('Linux')):
                import posix_ipc
                self.sem = posix_ipc.Semaphore("/sem.ZAMSEMAL")
                import mempart
                # pdb.set_trace()
                self.mempart = mempart.MemPart(adresse="/ZAMVARCST", vs=self)
                self.dicvars = dict()
                self.vuart = echuart.uart()
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

def traitPort(vv):
    """ lit le port d'entree et traite les ordres reçus
       :vv   variables

       les ordres recus sont des schema json
{
            id:yyyy,     # identificateur de cette transaction
            verb:xxxx,   # un ordre à executer
            params:{param1:val1,param2:val2} # un repertoire de parametre dont les valeurs sont des chaines
                                             # La semantique de la payload depend du verb
        }
       ordres traites:

    """
    def ok():
        return "[\"ok\"]".encode()

    def nok(erreur):
        return ("[\"nok\"," + erreur + "]").encode()

    def traitconnecte(vv, params):
            """ traite la connection """
            try:
                #prefixEVSE = "LDEVSE1_"

                dicval = dict()
                cap = 2
                bb = params
                if (bb["capac"] == 13):
                    cap = 1
                elif (bb["capac"] == 20):
                    cap = 2
                elif (bb["capac"] == 32):
                    cap = 3
                elif (bb["capac"] == 63):
                    cap = 4

                if (bb["type"] == str(5)):
                    if (bb["capdc"] == 70):
                        cap = 5
                    elif (bb["capdc"] == 80):
                        cap = 6
                    elif (bb["capdc"] == 125):
                        cap = 7
                    elif (bb["capdc"] == 200):
                        cap = 8
                    elif (bb["capdc"] == 250):
                        cap = 9
                    elif (bb["capdc"] == 400):
                        cap = 10
                    # poursel=" SELECT id FROM nodes WHERE name=\"" + prefixEVSE + "DEDO1_PlgStDC_setVal\""

                    if (not platform.platform().__contains__('Linux')):
                        dicval[vv.iedevse + prefixEVSE + "/DEDO1_CabRtgDC_stVal"] = (cap, bb["time"])
                        dicval[ vv.iedevse + prefixEVSE+ "DEDO1_PlgStDC_stVal"] = ("4", bb["time"])
                        utilitaires.saveOptindb(vs.dbpath, vv, dicval)
                    else:
                        vs.mempart.dicwrite(vv.iedevse + vv.ldevse  + "/DEDO1.CabRtgDC.stVal", cap)
                        vs.mempart.dicwrite(vv.iedevse + vv.ldevse  + "/DEDO1.PlgStDC.stVal", 4)
                        vs.vuart.ser.write(ok())
                else:
                    if (not platform.platform().__contains__('Linux')):
                        dicval[prefixEVSE + "DEAO1_CabRtgAC_stVal"] = (cap, bb["time"])
                        dicval[prefixEVSE + "DEAO1_PlgStAC_stVal"] = ("4", bb["time"])
                        utilitaires.saveOptindb(vs.dbpath, vv, dicval)
                    else:
                        vs.mempart.dicwrite(vv.iedevse + vv.ldevse + "/DEAO1.CabRtgAC.stVal", cap)
                        vs.mempart.dicwrite(vv.iedevse + vv.ldevse + "/DEDO1.PlgStDC.stVal", 4)
                        vs.vuart.ser.write(ok())
                    """
                    else:  attente de clarification
                    print("erreur connecte la chaine transmise par EV est nulle")
                    vv.reportlog.error("erreur connecte la chaine transmise par EV est nulle")
                    vv.vuart.ser.write(["nok","chaine transmise incorrecte"])
                     """
            except Exception as e:
                    raise Exception("connacte "+str(e))

    def traitdeconnecte(vv, params):
        """ traite la connection """
        try:
            dicval = dict()
            bb = params
            temps = bb["time"]
            prefixEVSE="LDEVSE1_"
            if (not platform.platform().__contains__('Linux')):
                utilitaires.saveNodeindb(vv.dbpath, vv, {prefixEVSE + "DEAO1_PlgStAC_stVal": ("1", temps)})
                utilitaires.saveNodeindb(vv.dbpath, vv, {prefixEVSE + "DEAO1_CabRtgAC_stVal": ("98", temps)})
                utilitaires.saveNodeindb(vv.dbpath, vv, {prefixEVSE + "DEDO1_PlgStDC_stVal": ("1", temps)})
                utilitaires.saveNodeindb(vv.dbpath, vv, {prefixEVSE + "DEDO1_CabRtgDC_stVal": ("98", temps)})
            else:
                vv.mempart.dicwrite(vv.iedevse + vv.ldevse + "DEAO1.PlgStAC.stVal", 1)
                vv.mempart.dicwrite(vv.iedevse + vv.ldevse + "DEDO1.PlgStDC.stVal", 1)
                vv.mempart.dicwrite(vv.iedevse + vv.ldevse + "DEAO1.CabRtgAC.stVal", 98)
                vv.mempart.dicwrite(vv.iedevse + vv.ldevse + "DEDO1.CabRtgDC.stVal", 98)
                print(" deconnecte " + vv.ldevse + "DEAO1_PlgStAC_stVal et  DEDO1_PlgStDC_stVal ")
                vv.vuart.ser.write(ok())
                """
                 else:
                    print("post/dc pb connexion")
                    return {'status': 'failure'}
                """

        except Exception as e:
            raise Exception("deconnecte " + str(e))


    def traitlibcon(vv, params):
        """ traite la connection """
        try:
            prefixEVSE = "LDEVSE1_"
            bb=params
            time = bb["time"]
            ok = False
            try:
                if (not platform.platform().__contains__('Linux')):
                    dicval = dict()
                    dicval["mod_MA"] = ("0", time)
                    dicval["LDEVSE1_DEAO1_PlgStAC_stVal"] = ("1", time)
                    dicval["LDEVSE1_DEDO1_PlgStDC_stVal"] = ("1", time)
                    utilitaires.saveOptindb(vs.dbpath, vv, dicval)
                else:
                    vs.mempart.dicwrite("mod_MA", 1)
                    vs.mempart.dicwrite(vv.iedevse+vv.ldevse+"DEAO1.PlgStAC.stVal", 1)
                    vs.mempart.dicwrite(vv.iedevse+vv.ldevse+"DEDO1.PlgStDC.stVal", 1)
                ook = True
            except Exception as ee:
                ook = False
            if (ook):
                vv.vuart.ser.write(ok())
            else:
                raise Exception("   " + str(ee))
        except Exception as e:
            raise Exception("traitlibcon " + str(e))

    def traitcalc(vv, params):
        try:
            dicval = dict()
            bb = params
            vamp = bb["can_AmpCurrent"]
            volt = bb["can_Vcurrent"]
            time = bb["time"]
            # print(" can_AmpCurrent =" + str(vamp) + " can_VCurrent=" + str(vv))
            ok = False
            try:
                if (not platform.platform().__contains__('Linux')):
                    dicval = dict()
                    dicval["int_" + vv.ldevse + "AmpCurrent"] = (vamp[0], vamp[1])
                    dicval["int_" + vv.ldevse + "VCurrent"] = (volt[0], volt[1])
                    utilitaires.saveOptindb(vs.dbpath, vv, dicval)
                else:
                    vv.reportlog.info("CALC AmpCurrent={0} VCurrent={1}".format(float(vamp[0]), float(vv[0])))
                    vv.mempart.dicwrite("int_" + vv.ldevse + "AmpCurrent", float(vamp[0]))
                    vv.mempart.dicwrite("int_" + vv.ldevse + "VCurrent", float(volt[0]))
                    ook = True
            except Exception as ee:
                ook = False
            if (ook):
                vv.vuart.ser.write(ok())
            else:
                print(" traitcalcalc pb bdd " + str(ee))
                raise Exception(" " + str(ee))
            """
            else:
                print("post/Calc pb connexion")
                return {'status': 'pb connexion'}
        """
        except Exception as e:
            raise Exception("traitcalc " + str(e))
    try:
        if (not vs.vuart.ser.isOpen()):
            if (platform.platform().__contains__('Linux')):
                vs.vuart = echuart.uart()
            else:
                vs.vuart = echuart.uart(port=vs.winport)
        id=1
        ordre = vs.vuart.get(id)  # on recupere le message
        print("traitPort " + json.dumps(ordre))
        id = ordre["id"]
        verb = ordre["verb"]
        params = ordre["params"]
        if (verb == "connecte"):
            traitconnecte(vs, params)
        elif (verb == "deconnecte"):
            traitdeconnecte(vv, params)
        elif (verb == "libcon"):
            traitlibcon(vv, params)
        elif (verb == "calc"):
            traitcalc(vv, params)
        else:
            vs.reportlog.warning("uart.get verb inconnu " + verb)
            print("uart.get verb inconnu " + verb)

    except Exception as ee:
         vs.reportlog.error("traitport " + str(ee))
         vs.vuart.ser.write(nok(" traitport  " + str(ee)))
         print("erreur traitport" + str(ee))
         raise Exception(" traitPort " + str(ee))





if __name__ == "__main__":
    try:
        vs = Variables()

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
        if (not platform.platform().__contains__('Linux')):
            poursel = " SELECT value FROM nodes WHERE  name=\"int_adresseCS\""
            valeur = utilitaires.fetchindb(vs.dbpath, vs, poursel)[0]
        else:
            valeur = vs.mempart.dicread("int_adresseSE")
        prefixEVSEs = ["LDEVSE1", "LDEVSE2"]
        ri = range(0, 1)
        for i in ri:
            if (not platform.platform().__contains__('Linux')):
                prefixEVSE = prefixEVSEs[i]
                utilitaires.saveNodeindb(vs.dbpath, vs, {prefixEVSE + "_DEAO1_PlgStAC_stVal": ("1", 0)})
                utilitaires.saveNodeindb(vs.dbpath, vs, {prefixEVSE + "_DEAO1_CabRtgAC_stVal": ("98", 0)})
                utilitaires.saveNodeindb(vs.dbpath, vs, {prefixEVSE + "_DEDO1_PlgStDC_stVal": ("1", 0)})
                utilitaires.saveNodeindb(vs.dbpath, vs, {prefixEVSE + "_DEDO1_CabRtgDC_stVal": ("98", 0)})
            else:
                prefixevse = prefixEVSEs[i].strip("_")
                if (vs.reportlog != None): vs.reportlog.info(" zamserver prefixevse=" + prefixevse)
                vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEAO1.PlgStAC.stVal", 1)
                vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEDO1.PlgStDC.stVal", 1)
                vs.mempart.dicwrite(vs.iedevse + vs.ldevse + "DEAO1.CabRtgAC.stVal", 98)
                vs.mempart.dicwrite(vs.iedevse +  vs.ldevse + "DEDO1.CabRtgDC.stVal", 98)
        pid = os.getpid();
        if (not platform.platform().__contains__('Linux')):
            poursel = " UPDATE nodes  SET value=" + str(pid) + ", timestamp=" + str(
                0.0) + " , valueNmoins1=\"" + '0' + "\", timestampNmoins1=" + str(
                0.0) + "  WHERE name=\"" + "int_PID" + "\""
            utilitaires.execindb(vs.dbpath, vs, poursel)
        else:
            vs.mempart.dicwrite("int_PID", pid)
            print("pid={0} Attente message vehicule".format(pid))

        fin="nok"
        while(fin == "nok"):  #on va se bloquer sur le port d'entree'
            try:
                traitPort(vs)
            except TimeoutError as te:
                fin="nok"
    except Exception as ee:
             print("erreur mycable " +str(ee))
             a=input()







