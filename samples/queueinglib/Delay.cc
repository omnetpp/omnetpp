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

#include "Delay.h"
#include "Job_m.h"

namespace queueing {

Define_Module(Delay);

void Delay::initialize()
{
    currentlyStored = 0;
    sizeStats.setName("delayed jobs");
    sizeStats.record(currentlyStored);
    WATCH(currentlyStored);
}

void Delay::handleMessage(cMessage *msg)
{
    Job *job = check_and_cast<Job *>(job);

    if (!job->isSelfMessage())
    {
        // if it is not a self-message, send it to ourselves with a delay
        currentlyStored++;
        double delay = par("delay");
        scheduleAt(simTime() + delay, job);
    }
    else
    {
        job->setDelayCount(job->getDelayCount()+1);
        simtime_t d = simTime() - job->getSendingTime();
        job->setTotalDelayTime(job->getTotalDelayTime() + d);

        // if it was a self message (ie. we have already delayed) so we send it out
        currentlyStored--;
        send(job, "out");
    }

    sizeStats.record(currentlyStored);

    if (ev.isGUI())
        getDisplayString().setTagArg("i",1, currentlyStored==0 ? "" : "cyan4");
}

}; //namespace

