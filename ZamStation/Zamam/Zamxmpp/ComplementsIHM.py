#!/usr/bin/pyhon

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import Qt
import matplotlib
import numpy as np
#import PyQt5.QtChart
import matplotlib.pyplot as plt
matplotlib.use("qt5agg")
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas  
from matplotlib.figure import Figure
import random
import json
import sys
#############################################################################################
# version #   date     #              modifications                                  # auth #
#############################################################################################
# v0.0.1  | 03/03/19   |  version initiale avec bogues corrigées                     |  AM  #
#############################################################################################
"""
class myspline:
    def __init__(self,variables,items=[(0,6),(2,4),(3,8),(7,4),(10,5),(11,1),(13,3),(17,6),(18,3),(20,2)]):
        self.v=variables    
        self.v.ui.chartview=QChart.QChartView(self.v.ui.verticalLayoutWidget_3)
        self.v.ui.chartview.setObjectName("mychartview")

        self.series=QChart.QSplineSeries()
        for item in items:
            self.series.append(item[0],item[1])

        qc=QChart.Qchart()
        #pc.legend().hide()
        qc.addSeries(self.series)
        qc.title(" simple spline chart")
        qc.createDefaultAxes()
        #chart.axisY().seRange(0,10)

        #self.v.ui.chartview.setChart(qc)
        #self.v.ui.chartview.show()
 """

class myplot(object): #(object,x =[1,2,3],y=[1,2,3]):
    """ exemple de plot """
    def __init__(self,variables,test="charge"):
        try:  
            self.v=variables
            if(test=="simpleplot"):
                plt.plot([1, 2, 3, 4])
                plt.ylabel('some numbers')
                plt.show()
            elif(test=="agg_oo_sgskip"):
                from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
                from matplotlib.figure import Figure

                fig = Figure()
                # A canvas must be manually attached to the figure (pyplot would automatically
                # do it).  This is done by instantiating the canvas with the figure as
                # argument.
                #FigureCanvas(fig)
                ax = fig.add_subplot(111)
                ax.plot([1, 2, 3])
                ax.set_title('hi mom')
                ax.grid(True)
                ax.set_xlabel('time')
                ax.set_ylabel('volts')
                scene=QtWidgets.QGraphicsScene(fig)
                self.v.ui.graphicsView.setScene(scene)
                self.v.ui.graphicsView.show()
                #fig.savefig('test')
            elif(test=="essaiIntegration"):
               from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas   
               self.figure = plt.figure()
                # this is the Canvas Widget that displays the `figure`
                # it takes the `figure` instance as a parameter to __init__
               self.v.ui.canvas = FigureCanvas(self.figure)
                # this is the Navigation widget
                # it takes the Canvas widget and a parent
                # set the layout
               self.v.ui.verticalLayout_3.addWidget(self.v.ui.canvas)             
               #-------------------------------------
               # plot some random stuff 
                # random data
               data = [random.random() for i in range(10)]
               # instead of ax.hold(False)
               self.figure.clear()
               # create an axis
               ax = self.figure.add_subplot(111)
               # discards the old graph
               # ax.hold(False) # deprecated, see above
               # plot data
               ax.plot(data, '*-')
               # refresh canvas
               self.v.ui.canvas.draw()
               self.v.ui.frame_4.show()
            elif(test=="oo"):
                #A pure OO (look Ma, no pyplot!) example using the agg backend.
                from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas  
                from matplotlib.figure import Figure
                fig = Figure()
                # A canvas must be manually attached to the figure (pyplot would automatically
                # do it).  This is done by instantiating the canvas with the figure as
                # argument.
                self.v.ui. canvas=FigureCanvas(fig)
                ax = fig.add_subplot(111)
                ax.plot([1, 2, 3])
                ax.set_title('hi mom')
                ax.grid(True)
                ax.set_xlabel('time')
                ax.set_ylabel('volts')
                self.v.ui.verticalLayout_3.addWidget(self.v.ui.canvas) 
                self.v.ui.canvas.draw()
                self.v.ui.frame_4.show()
            elif(test=="legend"):
                from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas  
                from matplotlib.figure import Figure
                fig=Figure()
                self.v.ui. canvas=FigureCanvas(fig)
                # exemple
                x=list(range(0,10))
                y=list(range(0,10))
                ax = fig.add_subplot(111)
                ax.set_title('courbe de charge')
                ax.grid(True,which='minor')
                ax.set_xlabel('time')
                ax.set_ylabel('soc')
                ax.set_xlim(0,50,auto=False)
                ax.plot(x,y,'b.')
                #legend = ax.legend(loc='upper center', shadow=True, fontsize='x-large')
                #legend = ax.legend(loc='upper center', shadow=True, fontsize='x-large')
                self.v.ui.verticalLayout.addWidget(self.v.ui.canvas) 
                self.v.ui.canvas.draw()
                
            elif(test=="charge"):
                from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas  
                from matplotlib.figure import Figure
                fig=Figure()
                self.v.ui. canvas=FigureCanvas(fig)
                # exemple
                x=list(range(0,10))
                y=list(range(0,10))
                ax = fig.add_subplot(111)
                ax.set_title('courbe de charge')
                ax.grid(True,which='minor')
                ax.set_xlabel('time')
                ax.set_ylabel('soc')
                ax.set_xlim(0,50,auto=False)
                ax.plot(x,y,'b.')
                #legend = ax.legend(loc='upper center', shadow=True, fontsize='x-large')
                #legend = ax.legend(loc='upper center', shadow=True, fontsize='x-large')
                self.v.ui.verticalLayout.addWidget(self.v.ui.canvas) 
                self.v.ui.canvas.draw()
            elif(test=="tension"):
                from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas  
                from matplotlib.figure import Figure
                fig=Figure()
                self.v.ui.canvas=FigureCanvas(fig)
                # exemple
                x=list(range(0,10))
                y=list(range(0,10))
                ax = fig.add_subplot(111)
                ax.set_title('voltage curve')
                ax.grid(True,which='minor')
                ax.set_xlabel('time')
                ax.set_ylabel('voltage')
                settings=self.v.xmlp.getConfigValue("SimulSettings")
                jsettings=json.loads(settings)
                ax.set_xlim(jsettings["minx"],jsttings["maxx"],auto=False)
                ax.set_ylim(jsettings["miny"],jsttings["maxy"],auto=False)
                ax.plot(x,y)
                #legend = ax.legend(loc='upper center', shadow=True, fontsize='x-large')
                #legend = ax.legend(loc='upper center', shadow=True, fontsize='x-large')
                self.v.ui.verticalLayout.addWidget(self.v.ui.canvas) 
                self.v.ui.canvas.draw()
        except Exception as ee:
            self.v.reportlog.error("problem myplot test=" + test + " " + str(ee))
            raise Exception(" problem myplot test=" + test + " " + str(ee))
   
class vplot(object):
    """ trace la tension """
    #fig=None # canvas de la figure
    #ax=None # Axes de la figure
    def __init__(self,variables):
        try:  
            self.v=variables
            self.fig=Figure()
            self.v.ui.canvas=FigureCanvas(self.fig)
            self.ax = self.fig.add_subplot(111)
            self.ax.set_title('voltage curve')
            self.ax.grid(True,which='minor')
            self.ax.set_xlabel('time')
            self.ax.set_ylabel('voltage')
            settings=self.v.xmlp.getConfigValue("SimulSettings")
            self.jsettings=json.loads(settings)
            if(sys.platform == "linux"):
                t=int(self.v.mempart.dicread("temps")*6/1000/60)
                self.jsettings["minx"]=t
            self.jsettings["maxx"]=self.jsettings["minx"] + self.jsettings["deltax"]
            self.ax.set_xlim(self.jsettings["minx"],self.jsettings["maxx"],auto=False)
            self.ax.set_ylim(self.jsettings["miny"],self.jsettings["maxy"],auto=False)
            #legend = ax.legend(loc='upper center', shadow=True, fontsize='x-large')
            #legend = ax.legend(loc='upper center', shadow=True, fontsize='x-large')
            self.v.ui.verticalLayout.addWidget(self.v.ui.canvas) 
            self.v.ui.canvas.draw()
        except Exception as ee:
            self.v.reportlog.error("problem vplot init " + str(ee))
            raise Exception(" problem vplot init " + str(ee))

    def trace(self,x,y):
        try: 
            self.ax.plot(x,y)
        except Exception as ee:
            self.v.reportlog.error(" vplot trace " + str(ee))
            raise Exception(" vplot trace " + str(ee))

    def resize(self,newmaxx=None,newminx=None,newmaxy=None,newminy=None):
        try:
            minx,maxx=self.ax.get_xlim()
            miny,maxy=self.ax.get_ylim()
            if(newmaxx != None):
                maxx=newmaxx
            if(newminx != None):
                minx=newminx
            if(newmaxy != None):
                maxy=newmaxy
            if(newminy != None):
                miny=newminy
            self.ax.set_xlim(minx,maxx)
            self.ax.set_ylim(miny,maxy)
        except Exception as ee:
            self.v.reportlog.error(" vplot resize " + str(ee))
            raise Exception("  vplot resize " + str(ee))
        
            


       