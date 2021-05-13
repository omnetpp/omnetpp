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

#ifndef __PHY_RXATSTART_H
#define __PHY_RXATSTART_H

#include <omnetpp.h>

using namespace omnetpp;

namespace phy {

class Tx;

class RxAtStart : public cSimpleModule
{
  protected:
    bool cutthroughEnabled = false;
    simtime_t cutthroughLatency = 0;
    Tx *cutthroughTargetModule = nullptr;
    cGate *cutthroughOutGate = nullptr;
    cMessage *cutthroughStartTimer = nullptr;
    bool cutthroughInProgress = false;
    cMessage *rxEndTimer = nullptr;
    cPacket *rxPacket = nullptr;

  public:
    virtual ~RxAtStart();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *message) override;

    virtual void receivePacketFromMedium(cPacket *packet);
    virtual void sendToUpperLayer(cPacket *packet);

    virtual void startRx(cPacket *packet);
    virtual void updateRx(cPacket *packet);
    virtual void endRx();

    virtual void scheduleRxEnd(simtime_t duration);

    virtual void tryCutthrough();
    virtual void scheduleCutthroughStart(simtime_t latency);
    virtual void sendCutthroughProgress(cPacket *packet, int64_t bitsReceived, bool isEnd);
    int64_t computeNumBitsReceived(cPacket *packet, simtime_t receptionTime);
};

}  // namespace phy

#endif

