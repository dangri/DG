import utilitaires
import json
import sys
import pdb
from ComplementsIHM import myplot as myplot

class actions(object):
    """actions à la souris"""
    vv=None
    def __init__(self,vv):
        self.vv=vv

    
    def bcourbes(self):
        monplot=myplot(self.vv,test="charge")
        

    def benvois(self):
        mes=self.vv.ui.CB_envois.currentText()
        if(not mes in self.vv.essaiEnvois): raise Exception("benvois nom fichier message non reconnu")
        fic=""
        if(sys.platform=="linux"):
            f=open("/home/pi/zamam/essais_envois/{0}.xml".format(mes))
        else:
            f=open("..\\essais_envois\\{0}.xml".format(mes))
        message=f.read()
        utilitaires.sendMessage(vers=self.vv.csms,message=message,variables=self.vv)
        f.close()

    def clickedCC(self,cc):
        try:
            tt=cc.isChecked()
            if(not cc.isChecked()): return
            name=cc.objectName()
            if(name=="CCRQST"):
               emaid=self.vv.ui.CBRQST.currentText()
               if(emaid=="---"):
                    utilitaires.message(" you must choose an EVU")
                    return
               utilitaires.writeMem(self.vv,"int_RQSTConnect",emaid)   
               h=self.vv.ui.CBRQSTH.currentText()
               if(h=="---"):
                    utilitaires.message(" you must choose an hour")
                    return
               vv.reportlog.info("action clicked name={0}".format(name))
               utilitaires.writeMem(self.vv,"temps",utilitaires.getConvertedH(h))  

               
            elif(name=="CCChangeChargeStatus"):
                #pdb.set_trace()

                wids=utilitaires.getWidsForEvse(self.vv,"ChangeChargeStatus")
                if(wids[0]==""):
                    return
                if(wids[3]==""):
                    utilitaires.message(" you must choose a target or total energy ")
                    return
                payload=wids[0]+","+wids[1] +","+ wids[2]+","+ wids[3]
                self.vv.reportlog.info("CCChangedChargeStatus payload = {0}".format(payload))
                utilitaires.writeMem(self.vv,"int_ChangeChargeStatus",payload)

            elif(name=="CCConnect"):
                wids=utilitaires.getWidsForEvse(self.vv,"Connect")
                if(wids[0]==""):
                    return
                payload=wids[0]+","+wids[1] +","+ wids[2]
                utilitaires.writeMem(self.vv,"int_Connect",payload)
                self.vv.reportlog.info("CCConnect readMem={0}".format(utilitaires.readMem(self.vv,"int_Connect")))

            elif(name=="CCDisconnect"):
                wids=utilitaires.getWidsForEvse(self.vv,"Disconnect")
                if(wids[0]==""):
                    return
                payload=wids[0]+","+wids[1] +","+ wids[2]
                utilitaires.writeMem(self.vv,"int_Disconnect",payload)

           
            elif(name=="CCHeuremax"):
                pass

        except Exception as ee:
            raise Exception("actions.clickedCC "+ str(ee))

