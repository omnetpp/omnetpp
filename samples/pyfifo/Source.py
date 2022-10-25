#
# This file is part of an OMNeT++/OMNEST simulation example.
#
# Copyright (C) 1992-2015 Andras Varga
#
# This file is distributed WITHOUT ANY WARRANTY. See the file
# `license' for details on this and other legal matters.
#

from omnetpp.runtime import *

class Source(omnetpp.cSimpleModule):
    """
    Generates messages or jobs; see NED file for more info.
    """

    def initialize(self, stage : int):
        self.sendMessageEvent = omnetpp.cMessage("sendMessageEvent")
        self.scheduleAt(omnetpp.simTime(), self.sendMessageEvent)

    def handleMessage(self, msg : omnetpp.cMessage):
        assert msg == self.sendMessageEvent

        job = omnetpp.cMessage("job")
        self.send(job, "out")
        p = self.par("interarrivalTime")
        self.scheduleAt(omnetpp.simTime() + p.doubleValue(), self.sendMessageEvent)
