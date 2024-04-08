import platform
if(platform.platform().__contains__("Linux")):
	import RPi.GPIO as GPIO
else:
	import RPi.GPIO2 as GPIO
import time


def resetGPIO():
	try:
		GPIO.setmode(GPIO.BOARD)
		GPIO.setup(35, GPIO.OUT)  # GPIO19  35
		GPIO.output(35, GPIO.LOW)
		GPIO.setup(36, GPIO.OUT)  # GPIO26  36
		GPIO.output(36, GPIO.LOW)
		GPIO.setup(29, GPIO.OUT)  # GPIO5    29
		GPIO.output(29, GPIO.LOW)
		GPIO.setup(31, GPIO.OUT)  # GPIO6    31
		GPIO.output(31, GPIO.LOW)
		GPIO.setup(33, GPIO.OUT)  # GPIO13    13
		GPIO.output(33, GPIO.LOW)
		GPIO.setup(38, GPIO.OUT)  # GPIO20    38
		GPIO.output(38, GPIO.LOW)
		GPIO.setup(40, GPIO.OUT)  # GPIO21     40
		GPIO.output(40, GPIO.LOW)
	except Exception as ee:
		raise Exception(" pb restGPIO " + str(ee))


class cable(object):
	"""cette classe interface le gpio pour les bornes et les vehicules"""
	vv=None
	typ=None
	cc=dict() #capacite des cables
	pin29=0
	oldpin29=0
	nbkeepalive=6
	etatconnect=None # etat de la connexion 0 deconnecte
				     #                      1 connecte AC
					 #                      2 connecte DC
	def __init__(self,variables,type=None):
		""" l'initialisation positionne les pins selon le type
         type= ev | borne
         	
			Fil 0 :  masse	 position 39
			Fil 1 :  Connexion AC 	GPIO 19
			Fil 2 :  connexion DC  	GPIO 16
			Fil 3 :  calibre CA 1        : calibre CC 1	GPIO 5
			Fil 4 :  calibre CA 2        : calibre CC 2	GPIO 6
			Fil 5 :  connexion CA1  : calibre CC 3	GPIO 13
			Fil 6 :  connexion CA2  : calibre CC 4	GPIO 20
			Fil 7 :  connexion CA3	GPIO 21
		"""
		self.vv=variables
		self.typ=type
		self.etatconnect=0
		try:
			GPIO.setwarnings(False)
			GPIO.setmode(GPIO.BOARD)
			self.vv.reportlog.info("creation cable type= {0} ".format(type))
			if(type=="ev"):
				GPIO.setup(35, GPIO.OUT) # GPIO19  35
				GPIO.output(35, GPIO.HIGH)
				GPIO.setup(36, GPIO.OUT) # GPIO16  36
				GPIO.output(36, GPIO.HIGH)
				GPIO.setup(29, GPIO.OUT) # GPIO5    29
				GPIO.output(29, GPIO.LOW)
				GPIO.setup(31, GPIO.OUT) # GPIO6    31
				GPIO.output(31, GPIO.LOW)
				GPIO.setup(33, GPIO.OUT) # GPIO13    13
				GPIO.output(33, GPIO.LOW)
				GPIO.setup(38, GPIO.OUT) # GPIO20    38
				GPIO.output(38, GPIO.LOW)
				GPIO.setup(40, GPIO.OUT) # GPIO21     40
				GPIO.output(40, GPIO.LOW)
			elif(type=="borne"):
				GPIO.setup(35,GPIO.IN) # GPIO19
				GPIO.setup(36,GPIO.IN) # GPIO16
				GPIO.setup(29,GPIO.IN) # GPIO5
				GPIO.setup(31,GPIO.IN) # GPIO6
				GPIO.setup(33,GPIO.IN) # GPIO13
				GPIO.setup(38,GPIO.IN) # GPIO20
				GPIO.setup(40,GPIO.IN) # GPIO21	
			else:
				self.vv.reportlog.error("cable type non connu {0} ".format(type))
				raise Exception("cable type non connu {0} ".format(type))
			self.vv.reportlog.info("creation cable de kind={0}".format(self.typ))

		except Exception as ee:
			self.vv.reportlog.error("creation cable {0}".format(ee))
			

	def connect(self, kind="AC"):
		""" set la connexion AC"""
		if(self.typ=="borne"):
			raise Exception(" une borne ne se connecte pas au vehicule")
		if(kind=="AC"):
			GPIO.output(35,GPIO.HIGH)
			GPIO.output(36, GPIO.LOW)
			self.etatconnect=1
			self.vv.reportlog.info("cable connecte AC (pin35)")
		else: #DC
			GPIO.output(36, GPIO.HIGH)
			GPIO.output(35, GPIO.LOW)
			self.vv.reportlog.info("cable connecte AC (pinO36)")
			etatconnect = 2



	def getconnect(self,kind="AC"):
		" lit l'etat du signal de connection"
		if(self.typ=="ev"):
			raise Exception( " un vehicule ne peut tester l'état de ses connexions")
		ret=False
		if(kind=="AC"):
			if(GPIO.input(35)==GPIO.HIGH):
				ret= True

		elif(kind=="DC"):
			if (GPIO.input(36) == GPIO.HIGH):
				ret = True
		return ret

	def deconnect(self, kind="AC"):
		""" get,set la connexion AC"""
		if(self.typ=="borne"):
			raise Exception (" une borne se peut se deconnecter")
		if(kind=="AC"):
			#GPIO.output(35,GPIO.LOW)
			#GPIO.output(36,GPIO.HIGH) # on met l'etat gpio a 3
			self.setEtat(3)
			self.vv.reportlog.info("cable deconnecte AC (GPIO35)")
			self.setPinsSelonCode(0)
			self.etatconnect=0
		if (kind == "DC"):
			#GPIO.output(36, GPIO.LOW)
			#GPIO.output(35,GPIO.HIGH) # on met l'etat GPIO à 3
			self.setEtat(3)
			self.vv.reportlog.info("cable deconnecte DC (GPIO36)")
			self.setPinsSelonCode(0)
			self.etatconnect = 0


	def setPinsSelonCode(self,code):
		try:
			""" positionne les bits selon le code fourni
				codage binaire sur 5 bits:
				GPIO.setup(29,GPIO.OUT) # GPIO5
				GPIO.setup(31,GPIO.OUT) # GPIO6
				GPIO.setup(33,GPIO.OUT) # GPIO13
				GPIO.setup(38,GPIO.OUT) # GPIO20
				GPIO.setup(40,GPIO.OUT) # GPIO21
			"""
			pins=[29,31,33,38,40]
			if(self.typ != "ev"):
				raise Exception ("cable.setSelonCable  vehicule seul autorise")
			codage='{0:0>5b}'.format(code)
			ri=range(0,5)
			for i in ri:
				if(codage[i]=='1'):
					GPIO.output(pins[i],GPIO.HIGH)
				else:
					GPIO.output(pins[i],GPIO.LOW)
		except Exception as ee:
			raise Exception("cable.setPinsSelonCode " +str(ee))

	def getCodeSelonPins(self):
		try:
			""" recupere le code genere par ev
				codage binaire sur 5 bits:
				GPIO.setup(29,GPIO.IN) # GPIO5
				GPIO.setup(31,GPIO.IN) # GPIO6
				GPIO.setup(33,GPIO.IN) # GPIO13
				GPIO.setup(38,GPIO.IN) # GPIO20
				GPIO.setup(40,GPIO.IN) # GPIO21
			"""

			pins=[40,38,33,31,29]
			if(self.typ != "borne"):
				raise Exception ("cable.getCodeSelonPins  borne seule autorise")
			ri=range(0,5)
			res = 0
			k=0
			for i in ri:
				k=i*2
				if(GPIO.input(pins[i])==GPIO.HIGH):
					if(i==0):
						res=1
					else:
						res=res+k

			return res
		except Exception as ee:
			raise Exception("getCodeSelonPins " +str(ee))
			
	def setType(self,type):
		"""affect un type au cable"""
		if((type != "ev") and (type != "borne")):
			raise (" cable.setType type inconnu")
		self.typ=type
		
	def prepareGPIO(self,selection):
		"""prepare le GPIO a une connexion"""
		try:
			dicval=dict()
			if(selection.startswith("vac")): typ="1"  # Single Phase
			if(selection.startswith("vtri")): typ="2" # Tri Phase"
			if(selection.startswith("vdc")): typ="5" # DC"
			poursel="SELECT type,EVId,capaciteCableAC,capaciteCableDC from voitures WHERE type="+"\""+selection+"\""
				 #vals=self.v.simulbas.fetchmany(poursel)
			vals=self.vv.mybas.fetchmany(poursel)  
			dicval["EVId"]=vals[0][1]
			dicval["capac"]=vals[0][2]
			dicval["capdc"]=vals[0][3]
			dicval["type"]=typ
			dicval["time"]=str(self.vv.util.getTemps(self.vv.tempssimule,self.vv.mempart))
			code=self.vv.capacites[selection]
			self.vv.reportlog.info("Preparation connection cable  selection={0} code={1}".format(selection,code))
			self.vv.cabl.setPinsSelonCode(code)
			"""
			self.vv.cabl.setEtat(3)	
			time.sleep(0.1)
			if(code < 8): # connexion en AC
				self.vv.cabl.connect(kind="AC")
			else:
				self.vv.cabl.connect(kind="DC")
			"""
		except Exception as ee:
			raise Exception("prepareGPIO " +str(ee))

	def keepAlive(self,kind=""):
		""" maintient la liaison en vie

		pour cela,si le cable est connecte, on fait battre le  GPIO5 pin 29
		côté vehicule on change de valeur tous les cycles de surveillance
		côté borne on lit la valeur tous les cycles
		si la valeur reste fixe plus de 6 fois
		on considere que le cable est deconnecte
		on declanche une deconnexion
		"""
		ret=True
		if(kind=="ev"):
			if (self.etatconnect >0 ):
					self.vv.reportlog.info("keepAlive pin29={0}".format(self.oldpin29))
					if(self.oldpin29==GPIO.LOW):
						self.oldpin29=GPIO.HIGH
						GPIO.output(29, GPIO.HIGH)
					elif (self.oldpin29 == GPIO.HIGH):
						self.oldpin29 = GPIO.LOW
						GPIO.output(29, GPIO.LOW)

		elif(kind=="borne"):
			if (self.etatconnect < 1):return False
			self.pin29=GPIO.input(29)
			if(self.oldpin29 != self.pin29):
				self.oldpin29=self.pin29  #on est vivant
				self.nbkeepalive = 6
			else:
				self.nbkeepalive=self.nbkeepalive-1
				if(self.nbkeepalive<0):
					ret=False # on est mort
			return ret
	
	def setEtat(self,etat):
		ret=0
		ac=0
		dc=0
		try:
			" force  l'etat du cable "
			if (self.typ == "borne"):
				raise Exception(" une borne  ne peut forcer l'état de ses connexions")
			if(etat==3):
				GPIO.output(36,GPIO.HIGH)
				GPIO.output(35,GPIO.HIGH)
				self.vv.reportlog.info("cable setEtat {0}".format(etat))
			elif(etat==2):
				GPIO.output(35,GPIO.HIGH)
				GPIO.output(36,GPIO.LOW)
				self.vv.reportlog.info("cable setEtat {0}".format(etat))
			elif(etat==1):
				GPIO.output(35,GPIO.LOW)
				GPIO.output(36,GPIO.HIGH)
			elif(etat==0):
				GPIO.output(35,GPIO.LOW)
				GPIO.output(36,GPIO.LOW)
				self.vv.reportlog.info("cable setEtat {0}".format(etat))
			self.etat=etat
			return dc+(2*ac)
		except Exception as ee:
			self.vv.reportlog.error(" cable getetat une borne  ne peut pas forcer l'état de ses connexions")
			return 0

	def getEtat(self,kind="AC"):
		ret = 0
		ac=0
		dc=0
		try:
			" lit l'etat du cable "
			if (self.typ == "ev"):
				raise Exception(" un vehicule ne peut tester l'état de ses connexions")
			if (GPIO.input(35) == GPIO.HIGH):
				ac = 1
			if (GPIO.input(36) == GPIO.HIGH):
				dc = 1
			ret = dc+(ac*2)
		except Exception as ee:
			self.vv.reportlog.error(" cable getetat un vehicule ne peut tester l'état de ses connexions")
		return ret

