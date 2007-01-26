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

#ifndef __SOURCE_H
#define __SOURCE_H

#include <omnetpp.h>

/**
 * Generates packets; see NED file for more info.
 */
class Source : public cSimpleModule
{
    private:
        cMessage *selfMsg;
        simtime_t startTime;       // when the module sends out the first job
        simtime_t stopTime;        // when the module stops the job generation (-1 means no limit)
        int numJobs;               // number of jobs to be generated (-1 means no limit)
        int jobCounter;            // number of jobs generated since start
        const char *jobName;       // the name of the generated job (if "" the job name should be
                                   // generated from the module name)

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

#endif
