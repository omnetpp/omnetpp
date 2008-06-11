//
// Copyright (C) 2006 Rudolf Hornig
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include <omnetpp.h>

#include "Server.h"
#include "PQueue.h"
#include "Job_m.h"
#include "SelectionStrategies.h"

namespace queueing {

Define_Module(Server);

Server::Server()
{
    selectionStrategy = NULL;
    jobServiced = NULL;
    endServiceMsg = NULL;
}

Server::~Server()
{
    delete selectionStrategy;
    delete jobServiced;
    cancelAndDelete(endServiceMsg);
}

void Server::initialize()
{
    scalarUtilizationStats.setName("utilization");
    prevEventTimeStamp = 0.0;

    endServiceMsg = new cMessage("end-service");
    jobServiced = NULL;
    selectionStrategy = SelectionStrategy::create(par("fetchingAlgorithm"), this, true);
    if (!selectionStrategy)
        error("invalid selection strategy");
}

void Server::handleMessage(cMessage *msg)
{
    scalarUtilizationStats.collect2((jobServiced != NULL ? 1 : 0), simTime()-prevEventTimeStamp);
    prevEventTimeStamp = simTime();

    if (msg==endServiceMsg)
    {
        ASSERT(jobServiced!=NULL);
        simtime_t d = simTime() - jobServiced->getTimestamp();
        jobServiced->setTotalServiceTime(jobServiced->getTotalServiceTime() + d);
        send(jobServiced, "out");
        jobServiced = NULL;

        if (ev.isGUI()) getDisplayString().setTagArg("i",1,"");

        // examine all input queues, and request a new job from a non empty queue
        int k = selectionStrategy->select();
        if (k >= 0)
        {
            ev << "requesting job from queue " << k << endl;
            cGate *gate = selectionStrategy->selectableGate(k);
            check_and_cast<PQueue *>(gate->getOwnerModule())->request(gate->getIndex());
        }
    }
    else
    {
        if (jobServiced)
            error("job arrived while already servicing one");

        jobServiced = check_and_cast<Job *>(msg);
        jobServiced->setTimestamp();

        simtime_t serviceTime = par("serviceTime");
        scheduleAt(simTime()+serviceTime, endServiceMsg);

        if (ev.isGUI()) getDisplayString().setTagArg("i",1,"cyan");
    }
}

void Server::finish()
{
    recordScalar("utilization", scalarUtilizationStats.getMean());
}

bool Server::isIdle()
{
    return jobServiced == NULL;
}

}; //namespace

