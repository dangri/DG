import RPi.GPIO as GPIO
import time 

class cable(object):
	"""cette classe interface le gpio pour les bornes et les vehicules"""
	vv=None
	typ=None
	cc=dict() #capacite des cables
	etatconnect=None # etat de la connexion 0 deconnecte
				     #                      1 connecte AC
					 #                      2 connecte DC
	def __init__(self,variables,type=None):
		""" l'initialisation positionne les pins selon le type
         type= ev | borne
         	
			Fil 0 :  masse	 position 39
			Fil 1 :  Connexion AC 	GPIO 19
			Fil 2 :  connexion DC  	GPIO 26
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
			GPIO.setmode(GPIO.BOARD)
			self.vv.reportlog.info("creation cable type= {0} ".format(type))
			if(type=="ev"):
				GPIO.setup(35, GPIO.OUT) # GPIO19  35
				GPIO.output(35, GPIO.LOW)
				GPIO.setup(37, GPIO.OUT) # GPIO26  37
				GPIO.output(37, GPIO.LOW)
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
				GPIO.setup(37,GPIO.IN) # GPIO26
				GPIO.setup(29,GPIO.IN) # GPIO5
				GPIO.setup(31,GPIO.IN) # GPIO6
				GPIO.setup(33,GPIO.IN) # GPIO13
				GPIO.setup(38,GPIO.IN) # GPIO20
				GPIO.setup(40,GPIO.IN) # GPIO21
			else:
				self.vv.reportlog.error("cable type non connu {0} ".format(type))
				raise Exception("cable type non connu {0} ".format(type))
		except Exception as ee:
			print("problème cable " + str(ee))

	def connect(self, kind="AC"):
		""" set la connexion AC"""
		if(self.typ=="borne"):
			raise Exception(" une borne ne se connecte pas au vehicule")
		if(kind=="AC"):
			GPIO.output(35,GPIO.HIGH)
			self.etatconnect=1
		else: #DC
			GPIO.output(37, GPIO.HIGH)
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
			if (GPIO.input(37) == GPIO.HIGH):
				ret = True
		return ret

	def deconnect(self, kind="AC"):
		""" get,set la connexion AC"""
		if(self.typ=="borne"):
			raise Exception (" une borne se peut se deconnecter")
		if(self.kind=="AC"):
			GPIO.output(35,GPIO.LOW)
			self.setPinsSelonCode(0)
			self.etatconnect=0
		if (self.kind == "DC"):
			GPIO.output(37, GPIO.LOW)
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
					