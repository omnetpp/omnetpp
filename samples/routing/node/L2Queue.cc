//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <cstdio>
#include <cstring>
#include <omnetpp.h>

using namespace omnetpp;

/**
 * Point-to-point interface module. While one frame is transmitted,
 * additional frames get queued up; see NED file for more info.
 */
class L2Queue : public cSimpleModule, public cListener
{
  private:
    intval_t frameCapacity;

    cPacketQueue queue;
    cChannel *transmissionChannel = nullptr;
    cMessage *endTransmissionEvent = nullptr;
    bool isBusy; // i.e. transmitting

    simsignal_t qlenSignal;
    simsignal_t busySignal;
    simsignal_t queueingTimeSignal;
    simsignal_t dropSignal;
    simsignal_t txBytesSignal;
    simsignal_t rxBytesSignal;

  public:
    virtual ~L2Queue();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual bool isInterfaceUp();
    virtual void startTransmitting(cPacket *packet);
    virtual void startTransmittingFromQueue();
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;
    virtual void refreshDisplay() const override;
};

Define_Module(L2Queue);

L2Queue::~L2Queue()
{
    cancelAndDelete(endTransmissionEvent);
}

void L2Queue::initialize()
{
    queue.setName("queue");
    endTransmissionEvent = new cMessage("endTxEvent");

    if (par("useCutThroughSwitching"))
        gate("line$i")->setDeliverImmediately(true);

    subscribe(POST_MODEL_CHANGE, this);

    transmissionChannel = gate("line$o")->findTransmissionChannel();

    frameCapacity = par("frameCapacity");

    qlenSignal = registerSignal("qlen");
    busySignal = registerSignal("busy");
    queueingTimeSignal = registerSignal("queueingTime");
    dropSignal = registerSignal("drop");
    txBytesSignal = registerSignal("txBytes");
    rxBytesSignal = registerSignal("rxBytes");

    emit(qlenSignal, queue.getLength());
    emit(busySignal, false);
    isBusy = false;
}

bool L2Queue::isInterfaceUp()
{
    return transmissionChannel && !transmissionChannel->isDisabled();
}

void L2Queue::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details)
{
    if (getSimulation()->getStage() == cSimulation::STAGE_CLEANUP)
        return;

    transmissionChannel = gate("line$o")->findTransmissionChannel();
    if (!isBusy && !queue.isEmpty() && isInterfaceUp())
        startTransmittingFromQueue();
}

void L2Queue::handleMessage(cMessage *msg)
{
    if (msg == endTransmissionEvent) {
        // transmission finished, we can start transmitting next one from the queue
        EV << "Transmission finished.\n";
        if (!queue.isEmpty() && isInterfaceUp()) {
            startTransmittingFromQueue();
        }
        else {
            isBusy = false;
            emit(busySignal, false);
        }
    }
    else if (msg->arrivedOn("line$i")) {
        // pass up
        cPacket *packet = check_and_cast<cPacket *>(msg);
        emit(rxBytesSignal, packet->getByteLength());
        send(packet, "out");
    }
    else {  // arrived on gate "in"
        cPacket *packet = check_and_cast<cPacket *>(msg);
        if (!isInterfaceUp()) {
            EV << "Received " << packet << " but interface is down\n";
            emit(dropSignal, (intval_t)packet->getByteLength());
            delete packet;
        }
        else if (isBusy) {
            // queue up the packet if there's space left in the queue
            if (frameCapacity && queue.getLength() >= frameCapacity) {
                EV << "Received " << packet << " but transmitter busy and queue full: discarding\n";
                emit(dropSignal, (intval_t)packet->getByteLength());
                delete packet;
            }
            else {
                EV << "Received " << packet << " but transmitter busy: queueing up\n";
                packet->setTimestamp();
                queue.insert(packet);
                emit(qlenSignal, queue.getLength());
            }
        }
        else {
            // We are idle, so we can start transmitting right away.
            EV << "Received " << packet << endl;
            emit(queueingTimeSignal, SIMTIME_ZERO);
            startTransmitting(packet);
        }
    }
}

void L2Queue::startTransmittingFromQueue()
{
    cPacket *packet = queue.pop();
    emit(queueingTimeSignal, simTime() - packet->getTimestamp());
    emit(qlenSignal, queue.getLength());
    startTransmitting(packet);
}

void L2Queue::startTransmitting(cPacket *packet)
{
    EV << "Starting transmission of " << packet << endl;

    if (!isBusy) {
        isBusy = true;
        emit(busySignal, true);
    }

    int64_t numBytes = packet->getByteLength();
    send(packet, "line$o");

    emit(txBytesSignal, numBytes);

    // Schedule an event for the time when last bit will leave the gate.
    simtime_t endTransmissionTime = gate("line$o")->getTransmissionChannel()->getTransmissionFinishTime();
    scheduleAt(endTransmissionTime, endTransmissionEvent);
}

void L2Queue::refreshDisplay() const
{
    getDisplayString().setTagArg("t", 0, isBusy ? "transmitting" : "idle");
    getDisplayString().setTagArg("i", 1, isBusy ? (queue.getLength() >= 3 ? "red" : "yellow") : "");
}

