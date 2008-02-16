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

#include "Sink.h"
#include "Job_m.h"

namespace queueing {

Define_Module(Sink);

void Sink::initialize()
{
    lifeTimeStats.setName("total lifetime");
    queueingTimeStats.setName("total queueing time");
    queueStats.setName("number of queue nodes visited");
    serviceTimeStats.setName("total service time");
    delayTimeStats.setName("total delay");
    delayStats.setName("number of delay nodes visited");
    generationStats.setName("generation");
    keepJobs = par("keepJobs");
}

void Sink::handleMessage(cMessage *msg) {
    Job *job = check_and_cast<Job *>(msg);

    // gather statistics
    lifeTimeStats.record(simTime()- job->creationTime());
    queueingTimeStats.record(job->getTotalQueueingTime());
    queueStats.record(job->getQueueCount());
    serviceTimeStats.record(job->getTotalServiceTime());
    delayTimeStats.record(job->getTotalDelayTime());
    delayStats.record(job->getDelayCount());
    generationStats.record(job->getGeneration());

    if (!keepJobs)
        delete msg;
}

void Sink::finish()
{
    // TODO missing scalar statistics
}

}; //namespace

