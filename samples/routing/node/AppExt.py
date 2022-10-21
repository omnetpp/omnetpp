import os
import cppyy
from omnetpp.runtime import *

cppyy.cppdef("#define protected public")
cppyy.include(os.path.join(os.path.dirname(__file__), "App.h"))

App = cppyy.gbl.App

class AppExt(App):
    def initialize(self):
        App.initialize(self)
        self.timer = omnetpp.cMessage("timer")
        self.scheduleAt(10, self.timer)
        self.stop()
        EV("hello")

    def handleMessage(self, msg):
        if msg == self.timer:
            self.startStop()
        else:
            App.handleMessage(self, msg)

    def startStop(self):
        if self.isRunning():
            EV(self.getFullPath() + "stopping")
            self.stop()
        else:
            EV(self.getFullPath() + "starting")
            self.start()
        self.scheduleAfter(10, self.timer)

