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

#include "Split.h"
#include "Job.h"

namespace queueing {

Define_Module(Split);

void Split::initialize()
{
}

void Split::handleMessage(cMessage *msg)
{
    Job *job = check_and_cast<Job *>(msg);
    int n = gateSize("out");

    // for each output gate
    for (int i=0; i<n; i++)
    {
    	// generate a child job
        Job *child = job->dup();
        child->makeChildOf(job);

        // give it a better name
        char buf[256];
        sprintf(buf, "%s-%d", job->getName(), i);
        child->setName(buf);

        // then send it out
        send(child, "out", i);
    }

    // Note: we cannot delete the parent job, as it will be needed
    // in the Join module. It'll stay pending in this module until
    // Join takes it from us.
}

}; // namespace
