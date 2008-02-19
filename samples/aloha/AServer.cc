//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "AServer.h"

namespace aloha {

Define_Module(Server);


Server::Server()
{
    endRxEvent = NULL;
}


Server::~Server()
{
    cancelAndDelete(endRxEvent);
}


void Server::initialize()
{
    txRate = par("txRate");
    endRxEvent = new cMessage("end-reception");
    channelBusy = false;

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

    if (ev.isGUI())
        displayString().setTagArg("i2",0,"x_off");
}


void Server::handleMessage(cMessage *msg)
{
    if (msg==endRxEvent)
    {
        ev << "reception finished\n";
        channelBusy = false;

        // update statistics
        simtime_t dt = simTime() - recvStartTime;
        if (currentCollisionNumFrames==0)
        {
            totalReceiveTime += dt;
        }
        else
        {
            totalCollisionTime += dt;
            collisionMultiplicityVector.record(currentCollisionNumFrames);
            collisionLengthVector.record(dt);
        }
        currentChannelUtilization = totalReceiveTime/simTime();
        channelUtilizationVector.record(currentChannelUtilization);

        currentCollisionNumFrames = 0;

        // update network graphics
        if (ev.isGUI())
        {
            displayString().setTagArg("i2",0,"x_off");
            displayString().setTagArg("t",0,"");
        }
    }
    else
    {
        totalFrames++;
        simtime_t endReception = simTime() + msg->length() / txRate;
        if (!channelBusy)
        {
            ev << "started receiving\n";
            recvStartTime = simTime();
            channelBusy = true;
            scheduleAt(endReception, endRxEvent);
            if (ev.isGUI())
            {
                displayString().setTagArg("i2",0,"x_yellow");
                displayString().setTagArg("t",0,"RECEIVE");
                displayString().setTagArg("t",2,"#808000");
            }
        }
        else
        {
            ev << "another frame arrived while receiving -- collision!\n";

            collidedFrames++;
            if (currentCollisionNumFrames==0)
                currentCollisionNumFrames = 2;
            else
                currentCollisionNumFrames++;

            if (endReception > endRxEvent->arrivalTime())
            {
                cancelEvent(endRxEvent);
                scheduleAt(endReception, endRxEvent);
            }

            // update network graphics
            if (ev.isGUI())
            {
                displayString().setTagArg("i2",0,"x_red");
                displayString().setTagArg("t",0,"COLLISION");
                displayString().setTagArg("t",2,"#800000");
                char buf[32];
                sprintf(buf, "Collision! (%ld frames)", currentCollisionNumFrames);
                bubble(buf);
            }
        }
        channelBusy = true;
        delete msg;
    }
}


void Server::finish()
{
    ev << "duration: " << simTime() << endl;
    ev << "total frames: " << totalFrames << endl;
    ev << "collided frames: " << collidedFrames << endl;
    ev << "total receive time: " << totalReceiveTime << endl;
    ev << "total collision time: " << totalCollisionTime << endl;
    ev << "channel utilization: " << currentChannelUtilization << endl;

    recordScalar("duration", simTime());
    recordScalar("total frames", totalFrames);
    recordScalar("collided frames", collidedFrames);
    recordScalar("total receive time", totalReceiveTime);
    recordScalar("total collision time", totalCollisionTime);
    recordScalar("channel utilization", currentChannelUtilization);
}

}; //namespace
