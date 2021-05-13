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

#include "RxAtEnd.h"

namespace phy {

Define_Module(RxAtEnd);

static std::ostream& operator<<(std::ostream& os, cPacket *pk)
{
    os << pk->getName() << " (" << pk->str() << ")";
    return os;
}

void RxAtEnd::initialize()
{
    gate("mediumIn")->setDeliverImmediately(false); // no-op since this is the default; put here for emphasis
}

void RxAtEnd::handleMessage(cMessage *message)
{
    if (message->arrivedOn("mediumIn"))
        receiveFromMedium(check_and_cast<cPacket *>(message));
    else
        throw cRuntimeError("Unknown message");
}

void RxAtEnd::receiveFromMedium(cPacket *packet)
{
    EV << "receiveFromMedium, pk=" << packet << endl;
    endRx(packet);
}

void RxAtEnd::endRx(cPacket *packet)
{
    EV << "endRx, pk=" << packet << endl;
    sendToUpperLayer(packet);
}

void RxAtEnd::sendToUpperLayer(cPacket *packet)
{
    EV << "sendToUpperLayer, pk=" << packet << endl;
    send(packet, gate("upperLayerOut"));
}

}  // namespace phy
