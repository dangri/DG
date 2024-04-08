#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v0.0.1  | 03/03/19   |  version initiale avec bogues corrigées                     |  AM  #
#############################################################################################
class valeurinits(object):
    
    def __init__(self):
        try:
            self.valeurs=dict()
            self.valeurs["LDEV1-DEEV1.Beh.stVal"]=1
            self.valeurs["LDEV1-DEEV1.EVSt.stVal"]=1
            self.valeurs["LDEV1-DEEV1.WhTgt.setMag.f"]=0
            self.valeurs["LDEV1-DEEV1.EVId.setVal"]="VE1"
            self.valeurs["LDEV1-DEEV1.WChaTgt.setMag.f"]=0
            self.valeurs["LDEV1-DEEV1.Soc.mag.f"]=40.0
            self.valeurs["LDEV1-DEEV1.DptTm.setTm"]=0.0
            self.valeurs["LDEV1-DEEV1.WCurrent.mag.f"]=0.0
            self.valeurs["LDEV1-FSCH1.Beh.stVal"]=1
            self.valeurs["LDEV1-FSCH1.SchdEntr.stVal"]=1
            self.valeurs["LDEV1-FSCH1.ValMV.mag.f"]=0.0
            self.valeurs["LDEV1-FSCH1.NumEntr.setVal"]=1

            #self.valeursAffichées=

        except Exception as ee:
            print(" erreur dans init valeurinits " + ee.Message)


