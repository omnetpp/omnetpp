#
# This file is part of an OMNeT++/OMNEST simulation example.
#
# Copyright (C) 1992-2015 Andras Varga
#
# This file is distributed WITHOUT ANY WARRANTY. See the file
# `license' for details on this and other legal matters.
#

from omnetpp.runtime import *

class AbstractFifo(omnetpp.cSimpleModule):
    """
    Abstract base class for single-server queues. Subclasses should
    define startService() and endService(), and may override other
    virtual functions.
    """

    def __init__(self):
        super().__init__()
        self.queue = omnetpp.cQueue()
        self.msgServiced = None

    def initialize(self, stage : int):
        self.endServiceMsg = omnetpp.cMessage("end-service")
        self.queue.setName("queue")

        self.qlenSignal = type(self).registerSignal("qlen")
        self.busySignal = type(self).registerSignal("busy")
        self.queueingTimeSignal = type(self).registerSignal("queueingTime")
        self.emit(self.qlenSignal, self.queue.getLength())
        self.emit(self.busySignal, False)

    def handleMessage(self, msg : omnetpp.cMessage):
        msg.__python_owns__ = True # TODO: should be automatic
        if msg == self.endServiceMsg:
            self.endService(self.msgServiced)
            if self.queue.isEmpty():
                self.msgServiced = None
                self.emit(self.busySignal, False)
            else:
                self.msgServiced = self.queue.pop()
                self.emit(self.qlenSignal, self.queue.getLength())
                self.emit(self.queueingTimeSignal, omnetpp.simTime() - self.msgServiced.getTimestamp())
                serviceTime = self.startService(self.msgServiced)
                self.scheduleAt(omnetpp.simTime() + serviceTime, self.endServiceMsg)
        elif not self.msgServiced:
            self.arrival(msg)
            self.msgServiced = msg
            self.emit(self.queueingTimeSignal, omnetpp.SimTime.ZERO)
            serviceTime = self.startService(self.msgServiced)
            self.scheduleAt(omnetpp.simTime() + serviceTime, self.endServiceMsg)
            self.emit(self.busySignal, True)
        else:
            self.arrival(msg)
            self.queue.insert(msg)
            msg.setTimestamp()
            self.emit(self.qlenSignal, self.queue.getLength())

    # hook functions to (re)define behavior
    def arrival(self, msg : omnetpp.cMessage):
        pass

    def startService(self, msg : omnetpp.cMessage) -> omnetpp.SimTime :
        raise NotImplementedError("startService() must be implemented in subclasses")

    def endService(self, msg : omnetpp.cMessage):
        raise NotImplementedError("endService() must be implemented in subclasses")
