#!/usr/bin/python
import sys
#import time
import platform
import mmap
if(platform.platform().__contains__('Linux')):
    import posix_ipc
#import hashlib
import os
import pdb
import struct
import utilitaires

class MemPart(object):
    """acces a la memoire partagee
    pour gestion du temps
    vars variables globales
    adresse nom deu segment de memoire partagee
    """
    vv=None # ls variables
    
    NULL_CHAR = '\0'
    dicvars=None
    def __init__(self,adresse="",vs=None):
     try:
        self.vv=vs
        self.memory=posix_ipc.SharedMemory(adresse)
        self.mapfile=mmap.mmap(self.memory.fd,self.memory.size)
        if(vs != None):
          self.vv=vs
          vs.dicvars=dict()
        #os.close(self.memory.fd) # il parait qu'on peut fermer le descripteur apres l'avoir maape
     except Exception as ee:
        # vv.reportlog.error(" MemPart ")
       print(" error,Mempart adresse={0} erreur={1} ".format(adresse,ee))

    def setVars(self,vars):
       self.vv=vars
       
       

    def write_to_memory(self,mapfile, s):
        """Writes the string s to the mapfile"""
        #say("writing %s " % s)
        try:
            mapfile.seek(0)
             # I append a trailing NULL in case I'm communicating with a C program.
            s += '\0'
            s = s.encode()
            mapfile.write(s)
        except Exception as ee:
            print(" error,write_to_memory mapfile={0} erreur={1} ".format(mapfile,ee))

    def write(self,adresse,bytes):
        """ ecrit 8 octets a partir de l'adresse """
        try:
            self.mapfile.seek(adresse)
            self.mapfile.write(bytes)
        except Exception as ee:
            print(" error,write  adresse={0} erreur={1} ".format(adresse,ee))

    def dicwrite(self, nom, val):
         """ ecriture d'une valeur au nom indique"""
         ret=True
        # if(self.vv.sem != None): self.vv.sem.acquire() #(timeout=float(2))
         if(self.vv.ll != None): self.vv.ll.acquire(blocking=True,timeout=2)
        
         #pdb.set_trace() 
         try:
        # il faut convertir val en fonction du type de dic
           if(len(self.vv.dicvars)==0): 
              self.vv.reportlog.error("Mempart dicwrite dicvars est vide")
              return False
           l=self.vv.dicvars[nom]
           typ=l[0]
           ad=l[2]
           lo=l[1]
           valtyp=utilitaires.gettyp(val)
           self.vv.reportlog.info("dicwrite ad={2} nom={0} val={1} ".format(nom,val,ad))
           #ull | int | float | | ip | bool | string 
           if(typ=="ull"):
               if(valtyp=='float'): 
                    val=int(val)
                    valtyp=utilitaires.gettyp(val)
               if(valtyp != 'int'):
                  raise Exception("mauvais type de valeur "  + str(nom)+" " + str(val))
               valb=struct.pack('Q',val)
           elif(typ=="int"):
               if(valtyp=='float'): 
                   val=int(val)
                   valtyp=utilitaires.gettyp(val)
               if(valtyp != 'int'): 
                   raise Exception("mauvais type de valeur "  + str(nom)+" " + str(val))
               valb=struct.pack('i', val)
           elif(typ=="double"):
               if(valtyp=='int'): 
                   val=float(val)
                   valtyp=utilitaires.gettyp(val)
               if(valtyp != 'float'): 
                   raise Exception("mauvais type de valeur "  + str(nom)+" " + str(val))
               valb=struct.pack('d', val)
           elif(typ=="float"):
               if(valtyp=='int'): 
                    val=float(val)
                    valtyp=utilitaires.gettyp(val)
               if(valtyp != 'float'): 
                   raise Exception("mauvais type de valeur "  + str(nom)+" " + str(val))
               valb=struct.pack('f',val)
               #self.vv.reportlog.info("dicwrite float nom={0} valtyp={1} val= {2} valb={3} deval={4}".format(nom,valtyp,val,valb,struct.unpack('d',valb)))
           elif(typ=="ip"):
               if((valtyp != 'str') and (not val.__contains__("."))): 
                   raise Exception("mauvais type de valeur "  + str(nom)+" " + str(val))
               valb=val.encode(encoding="utf-8")
           elif(typ=="bool"):
               if(valtyp != 'bool'): 
                   raise Exception("mauvais type de valeur "  + str(nom)+" " + str(val))
               valb=struct.pack('?',val)
           elif(typ=="string"):
               if(valtyp != 'str'): 
                   raise Exception("mauvais type de valeur "  + str(nom)+" " + str(val))
               valb=(val+"\0").encode(encoding="utf-8")
           else:
               raise Exception(" type de valeur inconnue " + str(nom)+" " + str(val) +" "+ str(type))
           self.mapfile.seek(ad)
           self.mapfile.write(valb)
           #self.vv.reportlog.info("ecriture memoire  ad={0} val={1}".format(ad,val))
           
         except Exception as ee:
             ret=False
             #print(" erreur dicwrite nom= {0} val={1} err={2}".format(nom,val,ee))
             if((self.vv !=None) and (self.vv.reportlog != None)): 
                #self.vv.reportlog.error(" MemPart  dicwrite nom={n0} typ={1} val= {2} valb={3} deval={4} ee={5}".format(nom,typ,val,valb,struct.unpack('d',valb),str(ee)))
                self.vv.reportlog.error(" MemPart  dicwrite nom={0} val={1} erreur={2}".format(nom,val,str(ee)))
         if(self.vv.ll!=None) :self.vv.ll.release()
        # if(self.vv.sem != None): self.vv.sem.release()
         return ret

    def dicread(self,nom):
        """lit la valeur de la variable nom (None si erreur) """
        #pdb.set_trace()
        ret=True
       # if(self.vv.sem != None): self.vv.sem.acquire() #(timeout=float(2))
        if(self.vv.ll!=None): self.vv.ll.acquire(blocking=True,timeout=2)
        try:
           lc=self.vv.dicvars[nom]
           typ=lc[0]
           ad=lc[2]
           lo=lc[1]
           self.mapfile.seek(ad)
           valb=self.mapfile.read(lo)
           if(typ=="ull"):
                ret=struct.unpack('Q',valb)[0]
           elif(typ=="int"):
                ret=struct.unpack('i',valb)[0]
           elif(typ=="double"):
                ret=struct.unpack('d',valb)[0]
           elif(typ=="float"):
                ret=struct.unpack('f',valb)[0]
                #self.vv.reportlog.info("dicread float nom={0} valb={1} deval={2}".format(nom,valb,ret))
           elif(typ=="ip"):
                if(valb[0]==0):
                   ret=""
                else:
                   ret=(valb.strip(b'\x00')).decode("utf-8")
           elif(typ=="bool"):        
                   ret=struct.unpack('?',valb)[0]
           elif(typ=="string"):
               if(valb[0]==0):
                   ret=""
               else:
                   ret=(valb.strip(b'\x00')).decode("utf_8")
           
        except Exception as ee:
            ret=None
            #print(" erreur dicread   "   + str(nom) + " " + str(ee) )
            if((self.vv !=None) and (self.vv.reportlog != None)): 
                  self.vv.reportlog.error("restihm MemPart dicread " + str(nom) + " " + str(ee))
        if(self.vv.ll!=None) :self.vv.ll.release()
        #if(self.vv.sem != None): self.vv.sem.release()
        return ret

    def read8(self):
        """Reads 8 octets from the mapfile and returns en tant que bytes """
        self.mapfile.seek(0)
        tt=self.mapfile.read(8)
        return tt
        
      

    def read(self):
        """Reads 8 octets from the mapfile and returns that """
        self.mapfile.seek(0)
        s = []
        c = self.mapfile.read_byte()
        
       # while c != NULL_CHAR:
        ri=range(0,8)
        for i in ri:
          s.append(c)
          c = self.mapfile.read_byte()
          #s = [chr(c) for c in s]
        #s = ''.join(s)
        return s
        
   
                    
    

    
            
         
