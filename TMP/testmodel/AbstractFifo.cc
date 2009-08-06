//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "AbstractFifo.h"

void emitD(simsignal_t signal, double d) { }

//#define emit(x,y)    // disable signals
//#define emit(x,y)    emitD(x,y) // empty version

void dumpSignalTable(cComponent *component)
{
    ev << component->getFullPath() << " signals:\n";
    std::vector<simsignal_t> signals = component->getLocalListenedSignals();
    if (signals.empty())
        ev << "  empty\n";
    else {
        for (int i=0; i<signals.size(); i++) {
            simsignal_t signalID = signals[i];
            std::vector<cIListener*> listeners = component->getLocalSignalListeners(signalID);
            ev << "  \"" << component->getSignalName(signalID) << "\" (signalID=" << signalID << ") "
               << " listeners=" << listeners.size() << "\n";
        }
    }
}

void dumpSignalTableRec(cComponent *component)
{
    dumpSignalTable(component);
    if (dynamic_cast<cModule *>(component)) {
        for (cModule::SubmoduleIterator submod((cModule *)component); !submod.end(); submod++)
            dumpSignalTableRec(submod());
        for (cModule::ChannelIterator chan((cModule *)component); !chan.end(); chan++)
            dumpSignalTableRec(chan());
    }
}

namespace fifo {

class DummyListener : public cIListener {
  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d) {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t) {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s) {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) {}
};

AbstractFifo::AbstractFifo()
{
    msgServiced = endServiceMsg = NULL;
}

AbstractFifo::~AbstractFifo()
{
    delete msgServiced;
    cancelAndDelete(endServiceMsg);
}

void AbstractFifo::initialize()
{
    endServiceMsg = new cMessage("end-service");
    queue.setName("queue");

    dropSignal = registerSignal("drop");
    preemptedSignal = registerSignal("preempted");
    resumedSignal = registerSignal("resumed");
    queueLengthSignal = registerSignal("queueLength");
    serverBusySignal = registerSignal("serverBusy");
    someStringSignal = registerSignal("someStringSignal");

    emit(queueLengthSignal, (double)0);
    emit(serverBusySignal, (double)false);

    ev << "----------\n"; dumpSignalTableRec(simulation.getSystemModule());
    simulation.getSystemModule()->checkConsistencyRec();

    subscribe(queueLengthSignal, new DummyListener());

    ev << "----------\n"; dumpSignalTableRec(simulation.getSystemModule());
    simulation.getSystemModule()->checkConsistencyRec();

    subscribe(serverBusySignal, new DummyListener());

    ev << "----------\n"; dumpSignalTableRec(simulation.getSystemModule());
    simulation.getSystemModule()->checkConsistencyRec();

    subscribe(serverBusySignal, new DummyListener());

    ev << "----------\n"; dumpSignalTableRec(simulation.getSystemModule());
    simulation.getSystemModule()->checkConsistencyRec();

    ev << "----------\n"; dumpSignalTableRec(simulation.getSystemModule());
    simulation.getSystemModule()->checkConsistencyRec();

    getParentModule()->subscribe(queueLengthSignal, new DummyListener());

    ev << "----------\n"; dumpSignalTableRec(simulation.getSystemModule());
    simulation.getSystemModule()->checkConsistencyRec();

    getParentModule()->subscribe(serverBusySignal, new DummyListener());

    emit(someStringSignal, "ssdsd sad sada asda");
}

void AbstractFifo::handleMessage(cMessage *msg)
{
    if (msg==endServiceMsg)
    {
        endService( msgServiced );
        if (queue.empty())
        {
            msgServiced = NULL;
            emit(serverBusySignal, (double)false);
        }
        else
        {
            msgServiced = (cMessage *) queue.pop();
            emit(queueLengthSignal, (double)queue.length());
            simtime_t serviceTime = startService( msgServiced );
            scheduleAt( simTime()+serviceTime, endServiceMsg );
        }
    }
    else if (!msgServiced)
    {
        arrival( msg );
        msgServiced = msg;
        simtime_t serviceTime = startService( msgServiced );
        scheduleAt( simTime()+serviceTime, endServiceMsg );
        emit(serverBusySignal, (double)true);
    }
    else
    {
        arrival( msg );
        queue.insert( msg );
        emit(queueLengthSignal, (double)queue.length());
    }
}

}; //namespace

