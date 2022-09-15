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

from AbstractFifo import AbstractFifo

class Fifo(AbstractFifo):
    """
    Single-server queue with a given service time.
    """

    def startService(self, msg : omnetpp.cMessage):
        #EV << "Starting service of " << msg.getName() << endl
        return self.par("serviceTime")

    def endService(self, msg : omnetpp.cMessage):
        #EV << "Completed service of " << msg.getName() << endl
        self.send(msg, "out")

