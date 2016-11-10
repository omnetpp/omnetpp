//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Server.h"

namespace aloha {

Define_Module(Server);

Server::Server()
{
    endRxEvent = nullptr;
}

Server::~Server()
{
    cancelAndDelete(endRxEvent);
}

void Server::initialize()
{
    channelStateSignal = registerSignal("channelState");
    endRxEvent = new cMessage("end-reception");
    channelBusy = false;
    emit(channelStateSignal, IDLE);

    gate("in")->setDeliverOnReceptionStart(true);

    currentCollisionNumFrames = 0;
    receiveCounter = 0;
    WATCH(currentCollisionNumFrames);

    receiveBeginSignal = registerSignal("receiveBegin");
    receiveSignal = registerSignal("receive");
    collisionSignal = registerSignal("collision");
    collisionLengthSignal = registerSignal("collisionLength");

    emit(receiveSignal, 0L);
    emit(receiveBeginSignal, 0L);

    getDisplayString().setTagArg("p", 0, par("x").doubleValue());
    getDisplayString().setTagArg("p", 1, par("y").doubleValue());
}

void Server::handleMessage(cMessage *msg)
{
    if (msg == endRxEvent) {
        EV << "reception finished\n";
        channelBusy = false;
        emit(channelStateSignal, IDLE);

        // update statistics
        simtime_t dt = simTime() - recvStartTime;
        if (currentCollisionNumFrames == 0) {
            // start of reception at recvStartTime
            cTimestampedValue tmp(recvStartTime, 1l);
            emit(receiveSignal, &tmp);
            // end of reception now
            emit(receiveSignal, 0);
        }
        else {
            // start of collision at recvStartTime
            cTimestampedValue tmp(recvStartTime, currentCollisionNumFrames);
            emit(collisionSignal, &tmp);

            emit(collisionLengthSignal, dt);
        }

        currentCollisionNumFrames = 0;
        receiveCounter = 0;
        emit(receiveBeginSignal, receiveCounter);
    }
    else {
        cPacket *pkt = check_and_cast<cPacket *>(msg);

        ASSERT(pkt->isReceptionStart());
        simtime_t endReceptionTime = simTime() + pkt->getDuration();

        emit(receiveBeginSignal, ++receiveCounter);

        if (!channelBusy) {
            EV << "started receiving\n";
            recvStartTime = simTime();
            channelBusy = true;
            emit(channelStateSignal, TRANSMISSION);
            scheduleAt(endReceptionTime, endRxEvent);
        }
        else {
            EV << "another frame arrived while receiving -- collision!\n";
            emit(channelStateSignal, COLLISION);

            if (currentCollisionNumFrames == 0)
                currentCollisionNumFrames = 2;
            else
                currentCollisionNumFrames++;

            if (endReceptionTime > endRxEvent->getArrivalTime()) {
                cancelEvent(endRxEvent);
                scheduleAt(endReceptionTime, endRxEvent);
            }

            // update network graphics
            if (hasGUI()) {
                char buf[32];
                sprintf(buf, "Collision! (%ld frames)", currentCollisionNumFrames);
                bubble(buf);
                getParentModule()->getCanvas()->holdSimulationFor(par("animationHoldTimeOnCollision"));
            }
        }
        channelBusy = true;
        delete pkt;
    }
}

void Server::refreshDisplay() const
{
    if (!channelBusy) {
        getDisplayString().setTagArg("i2", 0, "status/off");
        getDisplayString().setTagArg("t", 0, "");
    }
    else if (currentCollisionNumFrames == 0) {
        getDisplayString().setTagArg("i2", 0, "status/yellow");
        getDisplayString().setTagArg("t", 0, "RECEIVE");
        getDisplayString().setTagArg("t", 2, "#808000");
    }
    else {
        getDisplayString().setTagArg("i2", 0, "status/red");
        getDisplayString().setTagArg("t", 0, "COLLISION");
        getDisplayString().setTagArg("t", 2, "#800000");
    }
}

void Server::finish()
{
    EV << "duration: " << simTime() << endl;

    recordScalar("duration", simTime());
}

}; //namespace
