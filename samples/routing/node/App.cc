//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "App.h"

using namespace omnetpp;

Define_Module(App);

App::~App()
{
    cancelAndDelete(generatePacketTimer);
}

void App::initialize()
{
    myAddress = par("address");
    packetLengthBytes = &par("packetLength");
    sendIATime = &par("sendIaTime");  // volatile parameter
    pkCounter = 0;

    WATCH(pkCounter);
    WATCH(myAddress);

    destAddresses = cStringTokenizer(par("destAddresses").stdstringValue().c_str()).asIntVector();

    if (!destAddresses.empty()) {
        generatePacketTimer = new cMessage("nextPacket");
        scheduleAt(sendIATime->doubleValue(), generatePacketTimer);
    }

    endToEndDelaySignal = registerSignal("endToEndDelay");
    hopCountSignal = registerSignal("hopCount");
    sourceAddressSignal = registerSignal("sourceAddress");
}

bool App::isRunning()
{
    return generatePacketTimer->isScheduled();
}

void App::start()
{
    if (!isRunning())
        generatePacket();
}

void App::stop()
{
    if (isRunning() && !destAddresses.empty())
        cancelEvent(generatePacketTimer);
}

void App::handleMessage(cMessage *msg)
{
    if (msg == generatePacketTimer)
        generatePacket();
    else
        processReceivedPacket(check_and_cast<Packet *>(msg));
}

void App::generatePacket()
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

    scheduleAt(simTime() + sendIATime->doubleValue(), generatePacketTimer);

    // shout("Generating packet...");
}

void App::processReceivedPacket(Packet *pk)
{
    EV << "received packet " << pk->getName() << " after " << pk->getHopCount() << "hops" << endl;
    emit(endToEndDelaySignal, simTime() - pk->getCreationTime());
    emit(hopCountSignal, pk->getHopCount());
    emit(sourceAddressSignal, pk->getSrcAddr());
    delete pk;

    // shout("Arrived!");
}

void App::shout(const char *s)
{
    getParentModule()->bubble(s);
}
