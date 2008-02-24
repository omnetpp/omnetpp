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
// A module that generates messages. You can set the number of messages to be genereated,
// the starting and ending time and the message interArrivalTime distribution.
// Message generation stops if numMessage msg was sent out OR stopTime was reached,
// whichever occures first. You can set the genereated message type and priority too.
//
simple Source
{
    parameters:
        @group(Queueing);
        @display("i=block/source");
        double startTime @unit(s) = default(0);  // when the module sends out the first job
        double stopTime @unit(s) = default(-1s); // when the module stops the job generation (-1 means no limit)
        int numJobs = default(-1);               // number of jobs to be generated (-1 means no limit)
        volatile double interArrivalTime @unit(s); // time between generated jobs
        volatile int jobType = default(0);       // the type attribute of the created job (used by classifers and other modules)
        volatile int jobPriority = default(0);   // priority of the job
        string jobName = default("");            // the base name of the generated job (will be the source name if left empty)
    gates:
        output out;
}
