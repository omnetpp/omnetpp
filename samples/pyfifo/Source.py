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


"""
Generates messages or jobs; see NED file for more info.
"""
class Source(omnetpp.cSimpleModule):

    def initialize(self, stage : int):
        self.sendMessageEvent = omnetpp.cMessage("sendMessageEvent")
        self.scheduleAt(omnetpp.simTime(), self.sendMessageEvent)

    def handleMessage(self, msg : omnetpp.cMessage):
        msg.__python_owns__ = True # TODO: should be automatic
        # ASSERT(msg == sendMessageEvent); # TODO: implement ASSERT

        job = omnetpp.cMessage("job")
        self.send(job, "out")
        p = self.par("interarrivalTime")
        self.scheduleAt(omnetpp.simTime() + p.doubleValue(), self.sendMessageEvent)

    def __del__(self):
        self.cancel(self.sendMessageEvent) # TODO: is this optional?
