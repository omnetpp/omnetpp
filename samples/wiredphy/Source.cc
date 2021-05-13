//
// Copyright (C) OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see http://www.gnu.org/licenses/.
//

#include "Source.h"

namespace phy {

Define_Module(Source);

Source::~Source()
{
    cancelAndDelete(timer);
}

void Source::initialize()
{
    if ((simtime_t)par("interval") < 0)
        return;
    timer = new cMessage("Timer");
    scheduleTimer();
    WATCH(seqNum);
}

void Source::handleMessage(cMessage *message)
{
    if (message == timer) {
        std::string name = "data-" + std::to_string(seqNum);
        cPacket *packet = new cPacket(name.c_str(), seqNum % 8);
        packet->setBitLength(par("length"));
        packet->setKind(intuniform(0, 7));
        send(packet, gate("out"));
        scheduleTimer();
        seqNum++;
        std::string text = "sent " + std::to_string(seqNum);
        getDisplayString().setTagArg("t", 0, text.c_str());
    }
    else
        throw cRuntimeError("Unknown message");
}

void Source::scheduleTimer()
{
    scheduleAt(simTime() + par("interval"), timer);
}

}  // namespace phy

