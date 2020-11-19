//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

using namespace omnetpp;

/**
 * Abstract base class for single-server queues. Subclasses are
 * expected to redefine the arrival(), startService() and endService()
 * methods.
 */
class AbstractQueue : public cSimpleModule
{
  protected:
    short int priority;
    cMessage *msgServiced = nullptr;
    cMessage *endServiceMsg = nullptr;
    cQueue queue;
    cOutVector queueLength;

  public:
    virtual ~AbstractQueue();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    // hook functions to (re)define behaviour
    virtual void arrival(cMessage *msg) {}
    virtual simtime_t startService(cMessage *msg) = 0;
    virtual void endService(cMessage *msg) = 0;
    virtual void refreshDisplay() const override;
};

AbstractQueue::~AbstractQueue()
{
    delete msgServiced;
    cancelAndDelete(endServiceMsg);
}

void AbstractQueue::initialize()
{
    priority = par("priority");
    endServiceMsg = new cMessage("end-service");
    queue.setName("queue");
    queueLength.setName("queueLength");
}

void AbstractQueue::handleMessage(cMessage *msg)
{
    if (msg == endServiceMsg) {
        endService(msgServiced);
        if (queue.isEmpty()) {
            msgServiced = nullptr;
        }
        else {
            msgServiced = (cMessage *)queue.pop();
            queueLength.record(queue.getLength());
            simtime_t serviceTime = startService(msgServiced);
            endServiceMsg->setSchedulingPriority(priority);
            scheduleAt(simTime()+serviceTime, endServiceMsg);
        }
    }
    else if (!msgServiced) {
        arrival(msg);
        msgServiced = msg;
        simtime_t serviceTime = startService(msgServiced);
        endServiceMsg->setSchedulingPriority(priority);
        scheduleAt(simTime()+serviceTime, endServiceMsg);
    }
    else {
        arrival(msg);
        queue.insert(msg);
        queueLength.record(queue.getLength());
    }
}

void AbstractQueue::refreshDisplay() const
{
    getDisplayString().setTagArg("i2", 0, msgServiced ? "status/execute" : "");
}

//------------------------------------------------

/**
 * Queue model, with service time as parameter; see NED file for more info.
 */
class Queue : public AbstractQueue
{
  public:
    virtual void initialize() override;

    virtual simtime_t startService(cMessage *msg) override;
    virtual void endService(cMessage *msg) override;
};

Define_Module(Queue);

void Queue::initialize()
{
    AbstractQueue::initialize();

    long numInitialJobs = par("numInitialJobs");
    for (long i = 0; i < numInitialJobs; i++) {
        cMessage *job = new cMessage("job");
        queue.insert(job);
        queueLength.record(queue.getLength());
    }

    if (!queue.isEmpty()) {
        msgServiced = (cMessage *)queue.pop();
        queueLength.record(queue.getLength());
        simtime_t serviceTime = startService(msgServiced);
        scheduleAt(simTime()+serviceTime, endServiceMsg);
    }
}

simtime_t Queue::startService(cMessage *msg)
{
    EV << "Starting service of " << msg->getName() << endl;
    return par("serviceTime").doubleValue();
}

void Queue::endService(cMessage *msg)
{
    EV << "Completed service of " << msg->getName() << endl;
    msg->setSchedulingPriority(priority);
    send(msg, "out");
}

