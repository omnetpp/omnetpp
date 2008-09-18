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

#ifndef __JOBTABLE_H__
#define __JOBTABLE_H__

#include <omnetpp.h>
#include <set>

namespace queueing {

/**
 * Makes it possible to iterate over all Job messages in the system.
 */
class JobTable : public cSimpleModule
{
    friend class Job;
    protected:
        std::set<Job*> jobs;
        static JobTable *defaultInstance;
    public:
        JobTable();
        ~JobTable();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        void registerJob(Job *job);
        void deregisterJob(Job *job);
    public:
        static JobTable *getDefaultInstance(); // may return NULL
        const std::set<Job*> getJobs();
};

}; // namespace

#endif
