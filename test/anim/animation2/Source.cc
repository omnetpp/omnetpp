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
    cancelAndDelete(textMessage);
    cancelAndDelete(operationMessage);
}

void Source::initialize()
{
    ASSERT(explanations.size() == numOperations);

    cCanvas *canv = getParentModule()->getCanvas();
    canv->setAnimationSpeed(1, this);

    text = check_and_cast<cTextFigure*>(canv->getFigure("operation"));
    check_and_cast<cTextFigure*>(canv->getFigure("title"))->setText(title.c_str());

    textMessage = new cMessage("text");
    operationMessage = new cMessage("operation");

    scheduleAt(simTime(), textMessage);
}

void Source::handleMessage(cMessage *msg)
{
    if (msg == textMessage) {
        text->setText(("Operation " + std::to_string(operation) + " in [0.." + std::to_string(numOperations-1) + "]:\n"
            + explanations[operation]).c_str());

        scheduleAt(simTime(), operationMessage);

        return;
    }

    int packetByteLength = intuniform(4, 65536);

    EV << "Performing operation " << operation << " out of [0.." << numOperations-1 << "]...\n";

    Node *nodeA = static_cast<Node*>(getParentModule()->getSubmodule("nodeA"));
    Node *nodeB = static_cast<Node*>(getParentModule()->getSubmodule("nodeB"));
    Sink *sinkA = static_cast<Sink*>(getParentModule()->getSubmodule("sinkA"));
    Sink *sinkB = static_cast<Sink*>(getParentModule()->getSubmodule("sinkB"));

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

            sinkA->bar();

            cPacket *jobB = new cPacket("jobB");
            jobB->setByteLength(packetByteLength);
            send(jobB, "outSB");

            sinkB->bar();

            break;
        }
        case 3: nodeA->foo(); break;
        case 4: nodeB->foo(); break;
        case 5: nodeA->baz(); break;
        case 6: nodeB->baz(); break;

        default: ASSERT(false);
    }

    ++operation;

    operation %= numOperations;

    scheduleAt(simTime() + 1, textMessage);
}

}; // namespace
