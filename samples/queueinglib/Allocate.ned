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
// A module that allocates a resource for each job. If no resource is 
// available, jobs are queued. 
//
simple Allocate
{
    parameters:
        @group(Queueing);
        @display("i=block/arrival;q=queue");
        int capacity = default(-1);    // negative capacity means unlimited queue
        bool fifo = default(true);     // whether the module works as a queue (fifo=true) or a stack (fifo=false)

        string resourceModuleName;         // the name of the ResourcePool block to allocate from
        int resourceAmount = default(1);   // the amount of resource allocated
        int resourcePriority = default(0); // the priority used during resource allocation (smaller number means higher priority)

    gates:
        input in[];
        output out;
}
