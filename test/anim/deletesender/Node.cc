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

using namespace omnetpp;

namespace deletesender {

Define_Module(Node);

void Node::initialize()
{
    if (strcmp(getName(), "a") == 0) {
        scheduleAt(simTime()+1, new cMessage("send", 1));
        scheduleAt(simTime()+2, new cMessage("die", 2));
    }
}

void Node::handleMessage(cMessage *msg)
{
    switch (msg->getKind()) {
        case 1: // send
            send(new cMessage("hello"), "g$o");
            break;
        case 2: // die
            deleteModule();
            break;
    }

    delete msg;
}

} //namespace
