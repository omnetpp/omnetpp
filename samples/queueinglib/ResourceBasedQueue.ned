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

package omnetpp.queueing;

//
// Queue that allocates a resource for processing each job.
// If the resource is not available, the queue is idle. 
//
simple ResourceBasedQueue
{
    parameters:
        @group(Queueing);
        @display("i=block/queue;q=queue");
        int capacity = default(-1);    // negative capacity means unlimited queue
        bool fifo = default(true);     // whether the module works as a queue (fifo=true) or a stack (fifo=false)
        volatile double serviceTime @unit(s);
        
        string resourceModuleName = default("");  // the name of the ResourcePool block to provide the resources
        int resourceAmount = default(1);		  // the amount of resource needed for servicing a job
        int resourcePriority = default(0);		  // the priority used durning resource allocation (smaller priorities get resources easier)
        
    gates:
        input in[];
        output out;
}
