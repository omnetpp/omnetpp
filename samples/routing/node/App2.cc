//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "App2.h"

using namespace omnetpp;

Define_Module(App2);

App2::~App2()
{
    cancelAndDelete(generatePacketTimer);
}

void App2::initialize()
{
    myAddress = par("address");
    packetLengthBytes = &par("packetLength");
    pkCounter = 0;

    WATCH(pkCounter);
    WATCH(myAddress);

    destAddresses = cStringTokenizer(par("destAddresses").stdstringValue().c_str()).asIntVector();

    if (!destAddresses.empty()) {
        generatePacketTimer = new cMessage("nextPacket");
        scheduleAt(getIaTime(), generatePacketTimer);
    }

    endToEndDelaySignal = registerSignal("endToEndDelay");
    hopCountSignal = registerSignal("hopCount");
    sourceAddressSignal = registerSignal("sourceAddress");
}

bool App2::isRunning()
{
    return generatePacketTimer->isScheduled();
}

void App2::start()
{
    if (!isRunning())
        generatePacket();
}

void App2::stop()
{
    if (isRunning() && !destAddresses.empty())
        cancelEvent(generatePacketTimer);
}

void App2::handleMessage(cMessage *msg)
{
    if (msg == generatePacketTimer)
        generatePacket();
    else
        processReceivedPacket(check_and_cast<Packet *>(msg));
}

double App2::getIaTime()
{
    while (nextIaTimeIndex >= (int)sendIATimes.size()) {
        EV << "rereading iaTimes parameter\n";
        sendIATimes =  check_and_cast<cValueArray*>(par("iaTimes").objectValue())->asDoubleVector();
        nextIaTimeIndex = 0;
    }
    double result = sendIATimes[nextIaTimeIndex++];
    EV << "iaTime = " << result << endl;
    return result;
}

void App2::generatePacket()
{
    int destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];

    std::string pkName = opp_stringf("pk-%d-to-%d-#%ld", myAddress, destAddress, pkCounter++);
    EV << "generating packet " << pkName << endl;

    Packet *pk = new Packet(pkName.c_str());
    pk->setByteLength(packetLengthBytes->intValue());
    pk->setKind(intuniform(0, 7));
    pk->setSrcAddr(myAddress);
    pk->setDestAddr(destAddress);
    send(pk, "out");

    scheduleAt(simTime() + getIaTime(), generatePacketTimer);

    // shout("Generating packet...");
}

void App2::processReceivedPacket(Packet *pk)
{
    EV << "received packet " << pk->getName() << " after " << pk->getHopCount() << "hops" << endl;
    emit(endToEndDelaySignal, simTime() - pk->getCreationTime());
    emit(hopCountSignal, pk->getHopCount());
    emit(sourceAddressSignal, pk->getSrcAddr());
    delete pk;

    // shout("Arrived!");
}

void App2::shout(const char *s)
{
    getParentModule()->bubble(s);
}
