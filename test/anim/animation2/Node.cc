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

#include "Node.h"
#include "Sink.h"

namespace animation2 {

Define_Module(Node);

void Node::handleMessage(cMessage *msg)
{
    delete msg;

    int packetByteLength = 4096;

    cPacket *jobB = new cPacket("jobB");
    jobB->setByteLength(packetByteLength);
    send(jobB, "outB");

    cPacket *jobA = new cPacket("jobA");
    jobA->setByteLength(packetByteLength);
    send(jobA, "outA");
}

void Node::foo() {
    Enter_Method("foo");
    static_cast<Sink*>(gate("outA")->getNextGate()->getOwnerModule())->bar();
    static_cast<Sink*>(gate("outB")->getNextGate()->getOwnerModule())->bar();
}

void Node::baz() {
    Enter_Method("baz");

    int packetByteLength = 4096;

    static_cast<Sink*>(gate("outA")->getNextGate()->getOwnerModule())->baz();
    auto dummyA = new cPacket("dummyA");
    dummyA->setByteLength(packetByteLength);
    send(dummyA, "outA");

    static_cast<Sink*>(gate("outB")->getNextGate()->getOwnerModule())->baz();
    auto dummyB = new cPacket("dummyB");
    dummyB->setByteLength(packetByteLength);
    send(dummyB, "outB");
}

} //namespace
