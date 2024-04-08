#!/usr/bin/python
"""  connexion au serveur serveur.py
http """
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v0.0.1  | 03/03/19   |  version initiale avec bogues corrigées                     |  AM  #
############################################################################################
import json
import requests


class myclient:
    """ client rest """
    def __init__ (self,adresse="127.0.0.1:5001",report=""):
        try:
            self.reportlog=report
            self.basurl='http://' + adresse 
        except Exception as ee:
            if(self.reportlog != ''): self.reportlog.error("init client http adresse=" + adresse + " "  +str(ee))
            #print("Erreur init client http adresse=" + " " + adresse )

    def getVals(self,prog=''):
        """recupere les donnees du programme prog 
        selon l'adresse baseurl"""
        try:
          r = requests.get(self.basurl + '/' + prog)
          tt=type(r)
          e =r.encoding
          t=r.text
          c=r.content
          prov=1
          return r
        except Exception as ee:
           if(self.reportlog != ''):  self.reportlog.error("init client http adresse=" + str(prog)+ " " +str(ee))
           #print("Erreur init client http adresse=" + str(adresse)+ " " + str(ee ))
   
       

    def postVals(self,prog="",data="",jeeson=False):
        """ va envoyer un post 
             data est soit soue la forme d'un dictonnaire {id1:val1,...idn:valn}
             ou une chaine de caractere
          """
        try:
            payload = data
            if(jeeson):
                r = requests.post(self.basurl + '/' + prog, json=payload)
                c=r.json()
            else:
              pdb.set_trace()  
              r = requests.post(self.basurl + '/' + prog, data=payload)
              c=r.text
            a=r.text
            b=r.content
            return c
        except Exception as ee:
            if(self.reportlog != ''): self.reportlog.error("init client http post adresse=" + str(prog)+ " " +str(ee))
            pee=str(ee)
            print("Erreur clienthttp postvals prog={0} {1}".format(prog,ee))
          
            
   