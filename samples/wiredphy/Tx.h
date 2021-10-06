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

#ifndef __PHY_TX_H
#define __PHY_TX_H

#include <omnetpp.h>

using namespace omnetpp;

namespace phy {

class Tx : public cSimpleModule
{
  protected:
    double bitrate = 0;
    bool preemptionEnabled = false;
    bool queueingEnabled = false;
    cPacketQueue txQueue;
    cMessage *txEndTimer = nullptr;
    txid_t transmissionId = -1;
    cPacket *txPacket = nullptr; // actually, a clone of it
    bool cutthroughInProgress = false;

  public:
    virtual ~Tx();
    virtual bool isAcceptingCutthrough() const;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *message) override;

    virtual void receiveFromUpperLayer(cPacket *packet);
    virtual void receiveFromCutthrough(cPacket *packet);
    virtual void sendToMedium(cPacket *packet, simtime_t duration);
    virtual void sendTxUpdateToMedium(cPacket *packet, simtime_t duration);

    virtual void startTx(cPacket *packet);
    virtual void updateTx(cPacket *packet);
    virtual void endTx();
    virtual void abortTx();

    virtual bool isTransmitting() const { return txPacket != nullptr; }
    virtual simtime_t calculateDuration(cPacket *packet);
    virtual void scheduleTxEnd(simtime_t duration);
};

}  // namespace phy

#endif

