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

#include "Source.h"
#include "omnetpp/cpacket.h"
#include "Node.h"
#include "Sink.h"

namespace animation2 {

Define_Module(Source);

Source::~Source()
{
    cancelAndDelete(timerMessage);
}

void Source::initialize()
{
    timerMessage = new cMessage("timer");
    scheduleAt(simTime(), timerMessage);
}

void Source::handleMessage(cMessage *msg)
{
    ASSERT(msg == timerMessage);

    int packetByteLength = 4096;

    EV << "Performing operation " << operation;

    switch (operation) {
    case 0: {
        cPacket *jobA = new cPacket("jobA");
        jobA->setByteLength(packetByteLength);
        send(jobA, "outNA");
        break;
    }
    case 1: {
        cPacket *jobB = new cPacket("jobB");
        jobB->setByteLength(packetByteLength);
        send(jobB, "outNB");
        break;
    }
    case 2: {
        cPacket *jobA = new cPacket("jobA");
        jobA->setByteLength(packetByteLength);
        send(jobA, "outSA");
        static_cast<Sink*>(gate("outSA")->getNextGate()->getOwnerModule())->bar();

        cPacket *jobB = new cPacket("jobB");
        jobB->setByteLength(packetByteLength);
        send(jobB, "outSB");
        static_cast<Sink*>(gate("outSB")->getNextGate()->getOwnerModule())->bar();

        break;
    }
    case 3:
        static_cast<Node*>(gate("outNA")->getNextGate()->getOwnerModule())->foo();
        break;
    case 4:
        static_cast<Node*>(gate("outNB")->getNextGate()->getOwnerModule())->foo();
        break;
    case 5:
        static_cast<Node*>(gate("outNA")->getNextGate()->getOwnerModule())->baz();
        break;
    case 6:
        static_cast<Node*>(gate("outNB")->getNextGate()->getOwnerModule())->baz();
        break;
    }

    ++operation;
    operation %= 7;

    scheduleAt(simTime()+par("sendInterval").doubleValue(), timerMessage);
}

}; // namespace
