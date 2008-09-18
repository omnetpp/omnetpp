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

#include "Job.h"
#include "JobTable.h"

namespace queueing {

Define_Module(JobTable);

JobTable *JobTable::defaultInstance = NULL;


JobTable::JobTable() {
    if (defaultInstance == NULL)
        defaultInstance = this;
}

JobTable::~JobTable() {
    if (defaultInstance == this)
        defaultInstance = NULL;
    for (std::set<Job*>::iterator it=jobs.begin(); it!=jobs.end(); ++it)
        (*it)->table = NULL;
}

void JobTable::initialize() {
    WATCH_PTRSET(jobs);
}

void JobTable::handleMessage(cMessage *msg) {
    throw cRuntimeError("this modules does not process messages");
}

void JobTable::registerJob(Job *job) {
    jobs.insert(job);
}

void JobTable::deregisterJob(Job *job) {
    std::set<Job*>::iterator it = jobs.find(job);
    ASSERT(it != jobs.end());
    jobs.erase(it);
}

JobTable *JobTable::getDefaultInstance() {
    return defaultInstance;
}

const std::set<Job*> JobTable::getJobs() {
    return jobs;
}

}; // namespace
