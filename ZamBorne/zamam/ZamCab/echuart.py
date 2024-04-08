

import serial #Importation de la bibliothèque “pySerial”
import json
import echuart

class uart:
    ser=None # port serie à ouvrir ou fermer
    def __init__(self,port="/dev/ttyAMA0",baudrate=115200,timeout=1):
        """initialise la communication entre vehicule et borne par cable"""
        try:
            self.ser = serial.Serial(port=port, baudrate=baudrate,timeout=timeout) #Création du port lié au port a une vitesse de baudrate bauds et un timout d’une seconde
            self.ser.close() #Cloture du port pour le cas ou il serait déjà ouvert ailleurs
            self.ser.open() #Ouverture du port

        except Exception as ee:
            print(" Erreur init mycable " + str(ee))

    def close(self):
        try:
            self.ser.close()
        except Exception as ee:
            print(" Erreur close mycable " + str(ee))

    def open(self):
        try:
            self.ser.open()
        except Exception as ee:
            print(" Erreur open mycable " + str(ee))

    def destroy(self):
        try:
            self.ser.__del__()
        except Exception as ee:
            print(" Erreur destroy mycable " + str(ee))

    def post(self,payload):
        """ envoi un ordre sur le port serie
        :param la payload est un schema json
        {
            id:yyyy,     # identificateur de cette transaction
            verb:xxxx,   # un ordre à executer
            params:{param1:val1,param2:val2} # un repertoire de parametre dont les valeurs sont des chaines
                                             # La semantique de la payload depend du verb
        }
        """
        try:
            if (not self.ser.isOpen()): raise Exception("port non ouvert")
            spayload=json.dumps(payload)
           #print("spayload="+spayload)
            bpayload=spayload.encode()
            lg=len(bpayload)
            sentete=(str(lg))+"\n"
            entete=sentete.encode()
            nb = self.ser.write(entete + bpayload)
            nok=True
            while(nok):
                try:
                    r = self.ser.readline().strip(b'\n')
                    nok=False
                    return r
                except TimeoutError as to:
                     nok=True
            print("postvals " + json.dumps(payload) + " cr=" + r.decode())

        except Exception as ee:
            print(" Erreur post mycable {0} erreur={1}".format( payload, ee))
            r="[nok,"+str(ee)+"]"
        return json.load(r)

    def get(self,id):
        """ recoit une reponse suite à un ordre ou une interrogation sur le port serie
                la reponse est de la forme
                {
                    "id":yyyy,     # identificateur de cette transaction
                    "verb":zzz,     # en gral repond
                    "params":{param1:val1,param2:val2} # un repertoire de parametre dont les valeurs sont des chaines
                                                     # La semantique de la payload depend du verb
                }
                """
        try:
            if(not self.ser.isOpen()): raise Exception("port non ouvert")
            nok=True
            while(nok):
                try:
                    rr = self.ser.readline()
                except TimeoutError as t:
                    nok = True
            nb=int(rr.decode().strip("\n"))
            print(" lecture get nb=" + str(nb))
            rr=self.ser.read(nb)
            rrs=rr.decode()
            print(" lecture get message=" + rrs)
            r=json.loads(rrs)
            return r
        except Exception as ee:
            print(" Erreur get mycable  erreur={0}".format( ee))
            return(["nok", str(ee)])
