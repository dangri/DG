#/usr/bin/python
import sqlite3
from valeurinits import valeurinits as valeurinits
import utilitaires
""" acces à une base de donnee sqlite
permet de fixer l'adresse de la base
on suppose connue la structure des tables
"""
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v0.0.1  | 03/03/19   |  version initiale avec bogues corrigées                     |  AM  #
#############################################################################################
class mybase:
    db=None
    def __init__(self,db,reportlog,lock=None,variables=None):
        try:
            self.reportlog=reportlog
            self.db=db
            self.reportlog.info("db=" + db)
            #19 self.conn=sqlite3.connect(db)
            self.dblock=lock
            self.v=variables # emplacement pour les variables positonnes par setVars
           
              
           
        except Exception as ee:
            if(self.reportlog != None):self.reportlog.error(" acces BDD "  + str(ee))
            print(" ERREUR dans acces BDD " + str(ee))
    
    def setVars(self,variables):
        """positionne la valeur de la valeur globale v """
        self.v=variables
   
    def setTable(self,table,action="all",ids="",size=20):
        """ lire les données de la table bdtable de la  base de donnee 
        size lignes ou 20 lignes par defaut
        """
        try:
            # aiguillage des traitements en fonction des tables
             table.reset() # on remet la table à 0
             if(action=="pourModValue"):
                #odr="SELECT id,name,value,timestamp FROM nodes" # INNER JOIN  nodes ON nodes.id=nodValues.idNod" #idNod
                odr="SELECT * FROM nodValues ORDER BY timestamp DESC"
                rows=utilitaires.fetchindb(self.db,self.v,odr,mode="many")
                ri = range(0,len(rows))
                for i in ri:
                   table.setRow(i,rows[i])
             if(action=="afficsomeids"):
                 odr="SELECT id,name,value,timestamp FROM nodes" + " WHERE " + ids
                 rows=utilitaires.fetchindb(self.db,self.v,odr,mode="many")
                 ri = range(0,len(rows))
                 for i in ri:
                   table.setRow(i,rows[i])

             if(action=="all"):
                 #odr="SELECT id,name,value,timestamp FROM nodes" 
                 odr="SELECT id,name,value,timestamp FROM nodes" 
                 rows=utilitaires.fetchindb(self.db,self.v,odr,mode="many")
                 ri = range(0,len(rows))
                 for i in ri:
                   table.setRow(i,rows[i])

        except Exception as ee:
           if(self.reportlog != None):self.reportlog.error(" setTable "  + str(ee))
           print(" ERREUR dans maj table " + str(ee))

    def executer(self,poursel,commit=False):
        """ pour l'instant on ne traite pas le commit false"""
        try:
         #if(self.dblock!=None): self.dblock.acquire(blocking=True,timeout=2)
         #cur = self.conn.cursor()
         #cur.execute(poursel)
         #if(commit==True):
        #     self.conn.commit()
         #cur.close()
         #if(self.dblock!=None) :self.dblock.release()
         utilitaires.execindb(self.db,self.v,poursel)
        except Exception as ee:
             if(self.reportlog != None):self.reportlog.error(" bdd executer "  + str(ee))
             raise Exception(" bdd executer "  + poursel +" "+ str(ee))


    def commit(self):
        if(self.dblock!=None): self.ll.acquire(blocking=True,timeout=2)
        self.conn.commit()
        if(self.dblock!=None) :self.ll.release()
        
    def fetchone(self,poursel,arg=None):## arg est un tuple de valeurs avec poursel utilisant ?
        try:
            #if(self.dblock!=None): self.dblock.acquire(blocking=True,timeout=2)
            #cur = self.conn.cursor()
            #if(arg==None):
             #cur.execute(poursel)
            #else:
             #cur.execute(poursel,arg)
            #res=cur.fetchone()
            #cur.close()
            #if(self.dblock!=None) :self.dblock.release()
            res=utilitaires.fetchindb(self.db,self.v,poursel)
            return res
        except Exception as ee:
             self.reportlog.error(" bdd fetchone "  + str(ee))
             raise Exception(" bdd fetchone "  + str(ee))

    def fetchmany(self,poursel,size=20,arg=None): # arg est un tuple de valeurs avec poursel utilisant ?
        try:
            #if(self.dblock!=None): self.dblock.acquire(blocking=True,timeout=2)
            #cur = self.conn.cursor()
            #if(arg==None):
            # cur.execute(poursel)
            #else:
            # cur.execute(poursel,arg)
            #res=cur.fetchmany(size)
            #cur.close()
            #if(self.dblock!=None) :self.dblock.release()
            res=utilitaires.fetchindb(self.db,self.v,poursel,mode="many")
            return res
        except Exception as ee:
             if(self.reportlog != None): self.reportlog.error(" bdd fetchmany "  + str(ee))
             raise Exception(" bdd fetchmany "  + str(ee))

    def fetchall(self,poursel,arg=None): # arg est un tuple de valeurs avec poursel utilisant ?
        try:
           # if(self.dblock!=None): self.dblock.acquire(blocking=True,timeout=2)
           # cur = self.conn.cursor()
            #if(arg==None):
            # cur.execute(poursel)
            #else:
            # cur.execute(poursel,arg)
            #res=cur.fetchall()
           # cur.close()
           # if(self.dblock!=None) :self.dblock.release()
            res=utilitaires.fetchindb(self.db,self.v,poursel,mode="many") # ATTENTION creer le concept de all
            return res
        except Exception as ee:
             if(self.reportlog != None): self.reportlog.error(" bdd fetchall "  + str(ee))
             raise Exception(" bdd fetchall "  + str(ee))

    def save(self,nodvaluedict):
        """ sauvegarde dans la base nodes
        un dictionnaire dont les éléments sont
         id: (value,timestamp)
         les element valuesN-1 , et timestepmonisun sont remplacé par les valeurs existantes
         """
        try:
            d=nodvaluedict
            keys=d.keys()
            for k in keys:
                poursel=" SELECT value,timestamp FROM nodes WHERE id=" + str(k) 
                #val=self.fetchone(poursel) 
                #self.v.reportlog.info(" debut save fetch id=" + str(k))
                val=utilitaires.fetchindb(self.db,self.v,poursel)
                
                poursel=" UPDATE nodes SET valueNmoins1=\"" + str(val[0]) + "\", timestampNmoins1=" + str(val[1]) + " WHERE id=" + str(k)
                #self.reportlog.info(" mybase.SAVE poursel=" + poursel )
                #self.executer(poursel,commit=True)
                #self.v.reportlog.info(" save update N-1 id=" + str(k))
                utilitaires.execindb(self.db,self.v,poursel)
                poursel=" UPDATE nodes  SET value=\"" +  str(d[k][0]) +  "\", timestamp=" + str(d[k][1]) + "  WHERE id=" + str(k)
                #self.reportlog.info(" avant savee poursel= " + poursel)
                #self.executer(poursel,commit=True)
                #self.v.reportlog.info(" save update N id=" + str(k))
                utilitaires.execindb(self.db,self.v,poursel)
                #self.v.reportlog.info(" fin save  id=" + str(k))
            #self.commit()
              #util.message(" non implémenté")
                #self.monaffichage()
        except Exception as ee:
              if(self.reportlog != None): self.reportlog.error(" bdd save " +poursel + " " + str(ee))
              print(" erreur bdd save " + poursel +" "+str(ee))


    def savenom(self,nodvaluedict):
        """ sauvegarde dans la base nodes
        un dictionnaire dont les éléments sont
         name: (value,timestamp)
         les element valuesN-1 , et timestepmonisun sont remplacé par les valeurs existantes
         """
        try:
            d=nodvaluedict
            keys=d.keys()
            for k in keys:
                poursel=" SELECT value,timestamp FROM nodes WHERE name=\"" + str(k) +"\""
                #val=self.fetchone(poursel) 
                val=utilitaires.fetchindb(self.db,self.v,poursel)
                poursel=" UPDATE nodes SET valueNmoins1=\"" + str(val[0]) + "\", timestampNmoins1=" + str(val[1]) + " WHERE name=\""  + str(k) +"\""
                #self.reportlog.info(" mybase.SAVE poursel=" + poursel )
                #self.executer(poursel,commit=True)
                utilitaires.execindb(self.db,self.v,poursel)
                poursel=" UPDATE nodes  SET value=\"" +  str(d[k][0]) +  "\", timestamp=" + str(d[k][1]) + "  WHERE name=\""  + str(k) +"\""
                #self.reportlog.info(" avant savee poursel= " + poursel)
                c=self.executer(poursel,commit=True)
                utilitaires.execindb(self.db,self.v,poursel)
            #19 self.commit()

              #util.message(" non implémenté")
                #self.monaffichage()
        except Exception as ee:
              if(self.reportlog != None): self.reportlog.error(" bdd savenom " +poursel + " " + str(ee))
              print(" erreur bdd savenom " + poursel +" "+str(ee))



    def initvaleurs(self):
        try:
            inits=valeurinits()
            keys=inits.valeurs.keys()
            for kk in keys:  # on met toutes les c=valeurs initiales dans la base 
                poursel=" SELECT id FROM nodes WHERE name=" + "\"" +str(kk)+"\""
                id=self.fetchone(poursel) 
                val=str(inits.valeurs[kk])
                poursel=" UPDATE nodes  SET valeurinit=" + "\""+ val + "\"" + " WHERE id=" + str(id[0]) 
                #self.reportlog.info(" mybase.SAVE poursel=" + poursel )
                self.executer(poursel,commit=True)
        except Exception as ee:
           print(" ERREUR dans init valeurs " + str(ee.Message))


    def setInitCourbe(self,frombd=None,courbeno=1,FSCHName="LDEV1_FSCH1"):
            """ cree un courbe a partir de donnees prealables
               en general on trouve ces donnees dans la base de simulation fromdb
               mais on peut aussi la trouver dans la base elle meme si fromdb=None"""
            try:
                 if(frombd==None):
                     frombase=self
                 else:
                    
                     frombase=mybase(frombd,self.reportlog)
                     frombase.setVars(self.v) ## on suppose que v est prealablement positionne 

                 if(courbeno > 0): # il s'agit d'un suivi de profil autonome
                                               # on commence par charger le schedule de nom self.myname

                        poursel="SELECT NumEntr_setVal,SchdPrio_stVal,SchdIntv_setVal,StrTm0_setTime,StrTm1_setCal_hr, \
                                  StrTm1_setCal_mn,StrTm1_setCal_occType,StrTm1_setCal_occPer,EvtTrg_setVal,InSyn_intAddr,valeurs,valeurmaj \
                          FROM fsch WHERE id=" + str(courbeno)
                        fschvals=frombase.fetchmany(poursel,arg=(str(courbeno,)))
                        
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
                        self.savenom(dicvals)


            except Exception as ee: 
                raise Exception(" setInitCourbe " + str(ee))
