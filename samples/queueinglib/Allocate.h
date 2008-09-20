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

#ifndef __ALLOCATE_H
#define __ALLOCATE_H

#include <omnetpp.h>
#include "ResourcePool.h"

namespace queueing {

class Job;

/**
 * Module that reserves resources from a ResourcePool block or any module
 * that implements the IResourcePool interface. See NED file for more info.
 */
class Allocate : public cSimpleModule, public IResourceAllocator
{
    private:
        // state
        cQueue queue;

        // parameters
        int capacity;
        bool fifo;
        IResourcePool *resourcePool;
        int resourceAmount;
        int resourcePriority;

        // statistics
        long numDropped;
        cOutVector droppedVector;
        cOutVector lengthVector;
        cOutVector queueingTimeVector;

    public:
        Allocate();
        virtual ~Allocate();
        int length();

        // call-back method called from any associated resource
        virtual void resourceGranted(IResourcePool *provider);
        virtual std::string getFullPath() const;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

        virtual Job *dequeue();
        virtual Job *peek();
        virtual void enqueueOrDrop(Job *job);
        virtual bool allocateResource(Job *job);
};


}; //namespace

#endif
