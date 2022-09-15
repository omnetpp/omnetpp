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


class Sink(omnetpp.cSimpleModule):
    """
    Packet sink; see NED file for more info.
    """

    def initialize(self, stage : int):
        self.lifetimeSignal = type(self).registerSignal("lifetime")

    def handleMessage(self, msg : omnetpp.cMessage):
        msg.__python_owns__ = True # TODO: should be automatic
        lifetime = omnetpp.simTime() - msg.getCreationTime()
        # EV << "Received " << msg->getName() << ", lifetime: " << lifetime << "s" << endl;
        self.emit(self.lifetimeSignal, lifetime)

