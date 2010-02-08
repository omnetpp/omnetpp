//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "Server.h"

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
    endRxEvent = new cMessage("end-reception");
    channelBusy = false;

    gate("in")->setDeliverOnReceptionStart(true);

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

    beginRxSignal = registerSignal("beginRx");
    channelUtilizationSignal = registerSignal("channelUtilization");

    emit(channelUtilizationSignal, 0.0);

    if (ev.isGUI())
        getDisplayString().setTagArg("i2",0,"x_off");
}


void Server::handleMessage(cMessage *msg)
{
    if (msg==endRxEvent)
    {
        EV << "reception finished\n";
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
        }
        currentChannelUtilization = totalReceiveTime/simTime();
        emit(channelUtilizationSignal, currentChannelUtilization);

        currentCollisionNumFrames = 0;
        emit(beginRxSignal, (long)0);

        // update network graphics
        if (ev.isGUI())
        {
            getDisplayString().setTagArg("i2",0,"x_off");
            getDisplayString().setTagArg("t",0,"");
        }
    }
    else
    {
        cPacket *pkt = check_and_cast<cPacket *>(msg);

        totalFrames++;
        ASSERT(pkt->isReceptionStart());
        simtime_t endReceptionTime = simTime() + pkt->getDuration();
        if (!channelBusy)
        {
            EV << "started receiving\n";
            recvStartTime = simTime();
            channelBusy = true;
            scheduleAt(endReceptionTime, endRxEvent);
            emit(beginRxSignal, (long)1);

            if (ev.isGUI())
            {
                getDisplayString().setTagArg("i2",0,"x_yellow");
                getDisplayString().setTagArg("t",0,"RECEIVE");
                getDisplayString().setTagArg("t",2,"#808000");
            }
        }
        else
        {
            EV << "another frame arrived while receiving -- collision!\n";

            collidedFrames++;
            if (currentCollisionNumFrames==0)
                currentCollisionNumFrames = 2;
            else
                currentCollisionNumFrames++;

            if (endReceptionTime > endRxEvent->getArrivalTime())
            {
                cancelEvent(endRxEvent);
                scheduleAt(endReceptionTime, endRxEvent);
            }

            emit(beginRxSignal, (long)currentCollisionNumFrames);

            // update network graphics
            if (ev.isGUI())
            {
                getDisplayString().setTagArg("i2",0,"x_red");
                getDisplayString().setTagArg("t",0,"COLLISION");
                getDisplayString().setTagArg("t",2,"#800000");
                char buf[32];
                sprintf(buf, "Collision! (%ld frames)", currentCollisionNumFrames);
                bubble(buf);
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
}

}; //namespace
