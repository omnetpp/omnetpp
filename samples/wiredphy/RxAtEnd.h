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

#ifndef __PHY_RXATEND_H
#define __PHY_RXATEND_H

#include <omnetpp.h>

using namespace omnetpp;

namespace phy {

class RxAtEnd : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *message) override;
    virtual void receiveFromMedium(cPacket *packet);
    virtual void endRx(cPacket *packet);
    virtual void sendToUpperLayer(cPacket *packet);
};

}  // namespace phy

#endif

