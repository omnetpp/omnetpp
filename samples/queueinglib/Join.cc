//
// Copyright (C) 2008 Andras Varga
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

#include "Join.h"

namespace queueing {

Define_Module(Join);

Join::Join()
{
}

Join::~Join()
{
    while (!jobsHeld.empty()) {
    	delete jobsHeld.front();
    	jobsHeld.pop_front();
    }
}

void Join::initialize()
{
    WATCH_PTRLIST(jobsHeld);
}

void Join::handleMessage(cMessage *msg)
{
    Job *job = check_and_cast<Job *>(msg);
    jobsHeld.push_back(job);

    Job *parent = job->getParent();
    ASSERT(parent != NULL); // still exists

    int subJobsHeld = 0;
    for (std::list<Job*>::iterator it=jobsHeld.begin(); it!=jobsHeld.end(); it++)
        if ((*it)->getParent() == parent)
            subJobsHeld++;

    if (subJobsHeld == parent->getNumChildren()) {
        take(parent);
        for (std::list<Job*>::iterator it=jobsHeld.begin(); it!=jobsHeld.end(); /*nop*/) {
            if ((*it)->getParent() != parent)
                ++it;
            else {
                std::list<Job*>::iterator tmp = it++;
                delete (*tmp);
                jobsHeld.erase(tmp);
            }
        }
        send(parent, "out");
    }
}

}; // namespace

