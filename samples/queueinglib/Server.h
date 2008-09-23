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

#ifndef __SERVER_H
#define __SERVER_H

#include <omnetpp.h>
#include "SelectionStrategies.h"

namespace queueing {

class Job;
class SelectionStrategy;

/**
 * An interface that must be implemented by blocks that are capable
 * of processing a job. The block implementing IPassiveQueue is calling
 * these methods to communicate with the block to decide whether
 * a new job can be sent to a server
 */
class IServer
{
	public:
		virtual ~IServer() { };
		// indicate whether the service is currently idle (ie. no processing is done currently)
		virtual bool isIdle() = 0;
};

/**
 * The queue server. It cooperates with several Queues that which queue up
 * the jobs, and send them to Server on request.
 *
 * @see PassiveQueue
 */
class Server : public cSimpleModule, public IServer
{
    private:
        cWeightedStdDev scalarUtilizationStats;

        int numQueues;
        SelectionStrategy *selectionStrategy;

        simtime_t prevEventTimeStamp;
        Job *jobServiced;
        cMessage *endServiceMsg;

    public:
        Server();
        virtual ~Server();

    protected:
        virtual void initialize();
        virtual int numInitStages() const {return 2;}
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

    public:
        virtual bool isIdle();
};

}; //namespace

#endif


