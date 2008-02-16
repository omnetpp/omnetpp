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

#ifndef __ROUTER_H
#define __ROUTER_H

#include <omnetpp.h>

namespace queueing {

// routing algorithms
enum {
     ALG_RANDOM,
     ALG_ROUND_ROBIN,
     ALG_MIN_QUEUE_LENGTH,
     ALG_MIN_DELAY,
     ALG_MIN_SERVICE_TIME
};

/**
 * Sends the messages to different outputs depending on a set algorithm.
 */
class Router : public cSimpleModule
{
    private:
        int routingAlgorithm;  // the algorithm we are using for routing
        int rrCounter;         // msgCounter for round robin routing
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

}; //namespace

#endif
