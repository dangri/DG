#/usr/bib/python3
# Premiere version de CMCS
#
#
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v1.1.0 | mai  2021  |                                                             | AM   #
#                                                                                           #
#                                                                                           #
#-------------------------------------------------------------------------------------------#
#                                                                                           #
#############################################################################################
#------------------------------------------------
#            paramètres
#-----------------------------------------------

#-----------------------------------------------
import pdb
from PyQt5 import  QtWidgets, QtCore, QtGui
from PyQt5.QtCore import Qt
from lxml import etree
import sqlite3
import os
import time
import datetime
import threading
import logging
import platform
import os.path
import sqlite3
import pdb
import struct

class XMLP:
    """lecture des fichiers XML de configuration """

    def __init__(self, dirname, surappdata=False):
        """constructeur de XmlParser"""
        self.activeConfigPath=dirname+"\\ZamCsms-Config.xml"
        try:
            # pdb.set_trace()
            curdir = os.getcwd()
            """
            basename = os.path.basename(curdir)
            if (basename == "ZamLEMSpy"):
                self.activeConfigPath = os.getcwd() + "\\ZamCsms-Config.xml"
            elif (basename == "python375-64"):
                    self.activeConfigPath = "..\ZamLEMSpy" + "\\ZamCsms-Config.xml"
            elif (basename=="ZamClients"):
                self.activeConfigPath = ".\ZamLEMSpy" + "\\ZamCsms-Config.xml"
            """
            # pdb.set_trace()
            self.tree = etree.parse(self.activeConfigPath)

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


    def setConfigValue(self, select, value):
        """ sauvegarde la valeur de la config de nom name """
        try:
            express = "./appSettings/configuration[@name=\"" + str(select) + "\"]"
            self.e = self.root.find(express)
            if (self.e != None):
                self.e.attrib["value"] = str(value)
                self.tree.write("prov.txt")
                with open("prov.txt",mode='r') as f:
                    a = f.read()
                entete="<?xml version=\"1.0\"?>\n"
                b=entete+a
                with open(self.activeConfigPath, mode='w') as ff:
                    ff.write(b)
            else:
                return ""
        except Exception as ee:
            print("XMLP.setConfigValue " + str(ee))


    def getClxmppValue(self, select):
        """ recupere la valeur le clxmpp de nom name id et passwd """
        try:
            express = "./appSettings/clxmpp[@name=\"" + str(select) + "\"]"
            self.e = self.root.find(express)
            if (self.e != None):
                id=self.e.attrib["id"]
                passwd=self.e.attrib["passwd"]
                return (id,passwd)
            else:
                return ()
        except Exception as ee:
            print("XMLP.getClxmppValue " + str(ee))




    def getClxmppValues(self):
        """ recupere les clxmpp de nom name """
        ret = ()
        try:
            express = "./stations/clxmpp"
            self.e = self.root.findall(express)
            if (len(self.e) != 0):
                for cl in self.e:
                    id = cl.attrib["id"]
                    passwd = cl.attrib[passwd]
                    ret.append((id, passwd))
            else:
                return ()
        except Exception as ee:
            print("XMLP.getClxmppValue " + str(ee))

    def getBorneValue(self, station,borne):
        """ recupere la valeur le clxmpp de nom name """
        try:
            ret=dict()
            express = "./stations/borne[(@name=\"" + str(borne) + "\") and (@station=\"" + str(station) + "\")]"
            self.e = self.root.find(express)
            if (self.e != None):
                ret["type"]=self.e.attrib["type"]
                ret["status"]=self.e.attrib["status"]
                return ret
            else:
                return ()
        except Exception as ee:
            print("XMLP.getBorneValue " + str(ee))




    def getBorneValues(self):
        """ recupere les clxmpp de nom name """
        ret = list()
        try:
            express = "./appSettings/clxmpp"
            self.e = self.root.findall(express)
            if (len(self.e) != 0):
                for cl in self.e:
                    res=dict()
                    res["name"]=cl.attrib["name"]
                    res["station"] = cl.attrib["station"]
                    res["type"] = cl.attrib["type"]
                    res["status"]=cl.attrib["status"]
                    ret.append(res)
            else:
                return ()
        except Exception as ee:
            print("XMLP.getBorneValues " + str(ee))


    def setBorneValue(self, station, borne,type=None,status=None):
        """ sauvegarde la valeur de la config de nom name """
        try:
            express = "./stations[(@name=\"" + str(borne) + "\") and (@station=\"" + str(station) + "\")]"
            self.e = self.root.find(express)
            if (self.e != None):
                if(type !=None):
                    self.e.attrib["type"] = str(type)
                if (status !=None):
                    self.e.attrib["status"] = str(status)
                self.tree.write("prov.txt")
                with open("prov.txt",mode='r') as f:
                    a = f.read()
                entete="<?xml version=\"1.0\"?>\n"
                b=entete+a
                with open(self.activeConfigPath, mode='w') as ff:
                    ff.write(b)
            else:
                return ""
        except Exception as ee:
            print("XMLP.setConfigValue " + str(ee))

# ------

