#!/usr/bin/python
""" gestion d'une table de données pour interfacer une table """

from PyQt5 import QtCore, QtGui, QtWidgets


class mytable :
    """ les données sont données optionnellement sous forme de listes
    de lignes """
    v=None # variables
    don = None
    def __init__(self,report=None,donnees=list()):
        try:
            self.reportlog=report
            self.don=donnees 
            if(len(donnees)==0):
             self.rowCount=0
             self.colCount=0
            else:
             self.rowCount=len(self.don[0][0])
             self.colCount= len(self.don[0])
        except Exception as ee:
            if(not self.reportlog==None): self.reportlog.error(" my table creation table")
            #print(" Erreur creation table")

    def reset(self):
        try:
            self.rowCount=0
            self.colCount=0
            self.don=list()
        except Exception as ee:
            if(not self.reportlog==None):  self.reportlog.error(" my table reset table")
            #print ("mytable reset table")
           
    def setCell(self,row,col,val):
        try:
         if(row+1> self.rowCount): raise Exception(" mytable ligne inexistante ")
         if(col+1 > self.colCount): raise Exception(" mytable colonne inexistante")
         self.don[row][col]=val
        except Exception as ee:
         if(not self.reportlog==None):  self.reportlog.error(" my table " + str(ee))
         #print(" Erreur setCell:" + str(ee))

    def getCell(self,row,col):
         try:
             if(row+1> self.rowCount): raise Exception(" mytable ligne inexistante ")
             if(col+1 > self.colCount): raise Exception(" mytable colonne inexistante")
             return self.don[row][col]
         except Exception as ee:
             if(not self.reportlog==None):  self.reportlog.error(" my table " + str(ee))
             #print(" Erreur getCell:" + str(ee))
             return(None)

    def getRow(self,row):
        if(row+1> self.rowCount): raise Exception(" mytable ligne inexistante ")
        return self.don[row]

    def getCol(self,col):
        if(col+1 > self.colCount): raise Exception(" mytable colonne inexistante")
        ret=list()
        ri=range(0,len(self.don[0]))
        for i in ri:
            ret.append(self.don[i][col])
        return ret

    def setRow(self,row,rowval):
        """ si le rg de la ligne excede le nombre de ligne on ajoute  une ligne
        sinon on remplace la ligne"""
        try:
            if((self.rowCount != 0) and (len(rowval) != self.colCount)):
                raise Exception(" longueur de ligne incoherente")
            if(row+1> self.rowCount):
                self.don.append(rowval)
                self.rowCount=self.rowCount+1
                if(self.colCount==0): self.colCount=len(rowval)
            else:  
                self.don[row]=rowval
        except Exception as ee:
            if(not self.reportlog==None):  self.reportlog.error(" my table setRow " +str(ee))


    def initTableWidget(self,tablewidget):
        """ remplit une table widget avec les donnéés d'une table"""
        try:
            self.tablew=tablewidget
            self.tablew.setRowCount(self.rowCount)
            self.tablew.setColumnCount(self.colCount)
            ri=range(0,self.rowCount)
            rj=range(0,self.colCount)
            for i in ri:
                for j in rj:
                     self.tablew.setItem (i,j, QtWidgets.QTableWidgetItem (str(self.getCell(i,j))))
            prov=1
        except Exception as ee:
            if(not self.reportlog==None):  self.reportlog.error(" my table initTable" + str(ee))
            #print(" Erreur initTablewidget:" + str(ee))


    def setCol(self,col,colval):
        """ si le rg de la colonne excede le nombre de ligne on ajoute  une ligne
        sinon on remplace la colonne"""
        try:
            if((self.colCount != 0) and (len(colval) != self.rowCount)):
                raise Exception(" longueur de colonne incoherente")
            if(col+1> self.colCount):
                ri=range(0,self.rowCount)
                for i in ri :
                   self.don[i].append(colval[i])
                self.colCount=self.colCount+1
            else:
                for i in ri :
                  self.don[i][col]=colval[i]

        except Exception as ee:
         if(not self.reportlog==None):  self.reportlog.error(" my table setCol" + str(ee))
         #print(" Erreur setCol:" + str(ee))

    def autotest (self):
        testtable=[["a1","a2","a3"],["b1","b2","b3"],["c1","c2","c3"],["d1","d2","d3"]]
        soustesttable=["a4","b4","c4","d4"]
        ri=range(0,len(testtable))
        for i in ri:
         self.setRow(i,testtable[i])
        self.setCol(3,soustesttable)