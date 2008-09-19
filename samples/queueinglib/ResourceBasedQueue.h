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

#ifndef __RESOURCE_BASED_QUEUE_H
#define __RESOURCE_BASED_QUEUE_H

#include <omnetpp.h>
#include "Resource.h"

namespace queueing {

class Job;

/**
 * Queue that reserves a resource for processing each job.
 */
class ResourceBasedQueue : public cSimpleModule, public IResourceAllocator
{
    private:
    	// statistics
        cOutVector droppedStats;
        cOutVector lengthStats;
        cOutVector queueingTimeStats;
        cWeightedStdDev scalarWeightedLengthStats;
        cStdDev scalarLengthStats;
        cWeightedStdDev scalarUtilizationStats;
        int droppedJobs;

        // state
        Job *jobServiced;
        cMessage *endServiceMsg;
        cQueue queue;
        bool resourceAllocated;

        // parameters
        int capacity;
        bool fifo;
        IResourcePool *resourcePool;
        int resourceAmount;
        int resourcePriority;

        Job *getFromQueue();
        Job *peek();

        simtime_t prevQueueEventTimeStamp;   // the time stamp of the last queue length change
        simtime_t prevServiceEventTimeStamp; // the time stamp of the last service state change
        void queueLengthWillChange();
        void processorStateWillChange();

        // resource allocation helpers
        bool allocateResource(Job *job);
        void releaseResource();
        bool isResourceAllocated();
    public:
        ResourceBasedQueue();
        virtual ~ResourceBasedQueue();
        int length();

        // call-back method called from any associated resource
    	virtual void resourceGranted(IResourcePool *provider);
    	virtual std::string getFullPath() const;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

        // hook functions to (re)define behavior
        virtual void arrival(Job *job);
        virtual simtime_t startService(Job *job);
        virtual void endService(Job *job);
};

}; //namespace

#endif
