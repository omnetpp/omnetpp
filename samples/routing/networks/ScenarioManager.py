#
# This file is part of an OMNeT++/OMNEST simulation example.
#
# Copyright (C) 1992-2015 Andras Varga
#
# This file is distributed WITHOUT ANY WARRANTY. See the file
# `license' for details on this and other legal matters.
#

import cppyy

cppyy.include("omnetpp.h")

from cppyy.gbl import omnetpp

class ScenarioManager(omnetpp.cSimpleModule):
    def initialize(self, stage : int):
        self.targetNode = self.getSystemModule().getSubmodule("switchL")
        self.portIndex = 0
        self.event1 = self.createAndSchedule("break link", t=1)
        self.event2 = self.createAndSchedule("restore link", t=2)

    def handleMessage(self, msg : omnetpp.cMessage):
        msg.__python_owns__ = True
        if msg == self.event1:
            self.setChannelEnabled(self.targetNode, self.portIndex, False)
            self.event1 = None
        elif msg == self.event2:
            self.setChannelEnabled(self.targetNode, self.portIndex, True)
            self.event2 = None
        else:
            raise RuntimeError("Unknown timer")

    def createAndSchedule(self, name, t):
        msg = omnetpp.cMessage(name)
        self.scheduleAt(t, msg)
        return msg

    def setChannelEnabled(self, node, portIndex, enabled):
        gate = node.gate("port$o", portIndex)
        channel = gate.getChannel()
        channel.setEnabled(enabled)
        gate2 = gate.getOtherHalf()
        channel2 = gate2.getPreviousGate().getChannel()
        channel2.setEnabled(enabled)
        color = "" if enabled else "red"
        channel.getDisplayString().setTagArg("ls", 0, color)
        channel2.getDisplayString().setTagArg("ls", 0, color)

