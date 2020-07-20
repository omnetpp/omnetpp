//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "SimpleSource.h"

namespace animation2 {

Define_Module(SimpleSource);

void SimpleSource::initialize()
{
    scheduleAt(simTime() + 1, new cMessage("tick"));
}

void SimpleSource::handleMessage(cMessage *msg)
{
    delete msg;

    cPacket *jobA = new cPacket("cake");
    jobA->setByteLength(intuniform(4, 65536));
    send(jobA, "out");

    scheduleAt(simTime() + 1, new cMessage("tick"));
}

} //namespace
