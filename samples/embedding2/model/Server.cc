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

Server::~Server()
{
    cancelAndDelete(endRxEvent);
}

void Server::initialize()
{
    endRxEvent = new cMessage("end-reception");
    channelBusy = false;

    gate("in")->setDeliverImmediately(true);

    currentCollisionNumFrames = 0;
    totalFrames = 0;
    collidedFrames = 0;
    WATCH(currentCollisionNumFrames);
    WATCH(totalFrames);
    WATCH(collidedFrames);

    totalReceiveTime = 0.0;
    totalCollisionTime = 0.0;
    currentChannelUtilization = 0;
    WATCH(totalReceiveTime);
    WATCH(totalCollisionTime);
    WATCH(currentChannelUtilization);

    collisionMultiplicityVector.setName("collision multiplicity");
    collisionMultiplicityVector.setType(cOutVector::TYPE_INT);
    collisionMultiplicityVector.setInterpolationMode(cOutVector::NONE);

    collisionLengthVector.setName("collision length");
    collisionLengthVector.setUnit("s");
    collisionLengthVector.setInterpolationMode(cOutVector::NONE);

    channelUtilizationVector.setName("channel utilization");
    channelUtilizationVector.setType(cOutVector::TYPE_DOUBLE);
    channelUtilizationVector.setInterpolationMode(cOutVector::LINEAR);

    collisionMultiplicityHistogram.setName("collision multiplicity");
    collisionLengthHistogram.setName("collision length");

}

void Server::handleMessage(cMessage *msg)
{
    if (msg == endRxEvent) {
        EV << "reception finished\n";
        channelBusy = false;

        // update statistics
        simtime_t dt = simTime() - recvStartTime;
        if (currentCollisionNumFrames == 0) {
            totalReceiveTime += dt;
        }
        else {
            totalCollisionTime += dt;
            collisionMultiplicityVector.record(currentCollisionNumFrames);
            collisionMultiplicityHistogram.collect(currentCollisionNumFrames);
            collisionLengthVector.record(dt);
            collisionLengthHistogram.collect(dt);
        }
        currentChannelUtilization = totalReceiveTime/simTime();
        channelUtilizationVector.record(currentChannelUtilization);

        currentCollisionNumFrames = 0;
    }
    else {
        cPacket *pkt = check_and_cast<cPacket *>(msg);

        totalFrames++;
        ASSERT(pkt->isReceptionStart());
        simtime_t endReceptionTime = simTime() + pkt->getDuration();
        if (!channelBusy) {
            EV << "started receiving\n";
            recvStartTime = simTime();
            channelBusy = true;
            scheduleAt(endReceptionTime, endRxEvent);
        }
        else {
            EV << "another frame arrived while receiving -- collision!\n";

            collidedFrames++;
            if (currentCollisionNumFrames == 0)
                currentCollisionNumFrames = 2;
            else
                currentCollisionNumFrames++;

            if (endReceptionTime > endRxEvent->getArrivalTime()) {
                cancelEvent(endRxEvent);
                scheduleAt(endReceptionTime, endRxEvent);
            }
        }
        channelBusy = true;
        delete pkt;
    }
}

void Server::finish()
{
    EV << "duration: " << simTime() << endl;
    EV << "total frames: " << totalFrames << endl;
    EV << "collided frames: " << collidedFrames << endl;
    EV << "total receive time: " << totalReceiveTime << endl;
    EV << "total collision time: " << totalCollisionTime << endl;
    EV << "channel utilization: " << currentChannelUtilization << endl;

    recordScalar("duration", simTime());
    recordScalar("total frames", totalFrames);
    recordScalar("collided frames", collidedFrames);
    recordScalar("total receive time", totalReceiveTime);
    recordScalar("total collision time", totalCollisionTime);
    recordScalar("channel utilization", currentChannelUtilization);
    recordStatistic(&collisionMultiplicityHistogram, "packets");
    recordStatistic(&collisionLengthHistogram, "s");
}

}; //namespace
