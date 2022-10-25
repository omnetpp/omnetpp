#
# This file is part of an OMNeT++/OMNEST simulation example.
#
# Copyright (C) 1992-2015 Andras Varga
#
# This file is distributed WITHOUT ANY WARRANTY. See the file
# `license' for details on this and other legal matters.
#

from omnetpp.runtime import *

class Sink(omnetpp.cSimpleModule):
    """
    Packet sink; see NED file for more info.
    """

    def initialize(self, stage : int):
        self.lifetimeSignal = type(self).registerSignal("lifetime")

    def handleMessage(self, msg : omnetpp.cMessage):
        lifetime = omnetpp.simTime() - msg.getCreationTime()
        EV(f"Received {msg.getName()}, lifetime: {lifetime}s")
        self.emit(self.lifetimeSignal, lifetime)

