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

package org.omnetpp.samples.queueing;

//
// Queue server. It serves multiple input queues (PQueue), using a preset
// algorithm. Inputs must be connected to Passive Queues (PQueue)
//
simple Server
{
    parameters:
        @group(Queueing);
        @display("i=block/server");
        string fetchingAlgorithm @enum("priority","random","roundRobin","longestQueue") = default("priority");
             // how the next job will be choosen from the attached queues
        volatile double serviceTime @unit(s); // service time of a job
    gates:
        input in[];
        output out;
}


