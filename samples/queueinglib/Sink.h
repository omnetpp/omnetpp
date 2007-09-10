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

#ifndef __SINK_H
#define __SINK_H

#include <omnetpp.h>

/**
 * Consumes jobs; see NED file for more info.
 */
class Sink : public cSimpleModule
{
  private:
    cOutVector lifeTimeStats;
    cOutVector queueingTimeStats;
    cOutVector queueStats;
    cOutVector serviceTimeStats;
    cOutVector delayTimeStats;
    cOutVector delayStats;
    cOutVector generationStats;
    bool keepJobs;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif













