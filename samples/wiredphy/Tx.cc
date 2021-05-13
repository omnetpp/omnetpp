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

#include "Tx.h"
#include "ProgressInfo_m.h"

namespace phy {

Define_Module(Tx);

static std::ostream& operator<<(std::ostream& os, cPacket *pk)
{
    os << pk->getName() << " (" << pk->str() << ")";
    ProgressInfo *progressInfo = dynamic_cast<ProgressInfo*>(pk->getControlInfo());
    if (progressInfo) {
        int64_t availableBits = progressInfo->getAvailableBitLength();
        bool isEnd = progressInfo->isEnd();
        os << ", @ " << availableBits << " bits" << (isEnd ? ", end" : "");
    }
    return os;
}

void Tx::initialize()
{
    bitrate = par("bitrate");
    preemptionEnabled = par("preemptionEnabled");
    txEndTimer = new cMessage("TxEndTimer");
    queueingEnabled = par("queueingEnabled");
}

Tx::~Tx()
{
    cancelAndDelete(txEndTimer);
    delete txPacket;
}

void Tx::handleMessage(cMessage *message)
{
    if (message->arrivedOn("upperLayerIn"))
        receiveFromUpperLayer(check_and_cast<cPacket *>(message));
    else if (message->arrivedOn("cutthroughIn"))
        receiveFromCutthrough(check_and_cast<cPacket *>(message));
    else if (message == txEndTimer)
        endTx();
    else
        throw cRuntimeError("Unknown message");
}

bool Tx::isAcceptingCutthrough() const
{
    Enter_Method_Silent();
    bool result = !isTransmitting();
    EV << "isAcceptingCutthrough: " << result << endl;
    return result;
}

void Tx::receiveFromUpperLayer(cPacket *packet)
{
    EV << "receiveFromUpperLayer, pk=" << packet << endl;
    if (isTransmitting()) {
        if (preemptionEnabled)
            abortTx();
        else if (queueingEnabled) {
            EV << "tx busy, enqueueing packet\n";
            txQueue.insert(packet);
            return;
        }
        // else let startTx fail
    }

    startTx(packet);
}

void Tx::receiveFromCutthrough(cPacket *packet)
{
    EV << "receiveFromCutthrough, pk=" << packet << endl;
    if (!isTransmitting()) {
        cutthroughInProgress = true;
        startTx(packet);
    }
    else {
        if (!cutthroughInProgress)
            throw cRuntimeError("Received cutthrough packet while another packet is being transmitted");
        updateTx(packet);
    }
}

void Tx::sendToMedium(cPacket *packet, simtime_t duration)
{
    EV << "sendToMedium, pk=" << packet << endl;
    send(packet, SendOptions().transmissionId(transmissionId).duration(duration), gate("mediumOut"));
}

void Tx::sendTxUpdateToMedium(cPacket *packet, simtime_t duration)
{
    EV << "sendTxUpdateToMedium, pk=" << packet << endl;
    send(packet, SendOptions().updateTx(transmissionId).duration(duration), gate("mediumOut"));
}

void Tx::startTx(cPacket *packet)
{
    EV << "startTx, pk=" << packet << endl;
    if (isTransmitting())
        throw cRuntimeError("Another packet is already being transmitted");
    transmissionId = packet->getId();
    txPacket = packet->dup();
    simtime_t duration = calculateDuration(packet);
    scheduleTxEnd(duration);
    sendToMedium(packet, duration);
}

void Tx::updateTx(cPacket *packet)
{
    EV << "updateTx, pk=" << packet << endl;
    cancelEvent(txEndTimer);

    ProgressInfo *progressInfo = check_and_cast<ProgressInfo*>(packet->removeControlInfo());
    int64_t availableBits = progressInfo->getAvailableBitLength();
    bool isEnd = progressInfo->isEnd();

    if (isEnd) {
        packet->setBitLength(availableBits);
        delete txPacket;
        txPacket = packet->dup();
        simtime_t duration = calculateDuration(packet);
        sendTxUpdateToMedium(packet, duration);
        scheduleTxEnd(gate("mediumOut")->getTransmissionChannel()->getTransmissionFinishTime() - simTime());
    }
}

void Tx::endTx()
{
    EV << "endTx, pk=" << txPacket << endl;
    delete txPacket;
    txPacket = nullptr;
    transmissionId = -1;
    cutthroughInProgress = false;

    if (queueingEnabled && !txQueue.isEmpty())
        startTx(txQueue.pop());
}

void Tx::abortTx()
{
    EV << "abortTx, pk=" << txPacket << endl;
    cancelEvent(txEndTimer);
    simtime_t txDurationUntilNow = simTime() - txEndTimer->getSendingTime();
    int64_t truncatedLength = (int64_t)std::floor(bitrate * txDurationUntilNow.dbl());
    txPacket->setBitLength(truncatedLength);
    simtime_t duration = calculateDuration(txPacket);
    sendTxUpdateToMedium(txPacket, duration);
    txPacket = nullptr;
    transmissionId = -1;
}

simtime_t Tx::calculateDuration(cPacket *packet)
{
    EV << "calculateDuration, pk=" << packet << endl;
    return packet->getBitLength() / bitrate;
}

void Tx::scheduleTxEnd(simtime_t duration)
{
    EV << "scheduleTxEnd, t+" << duration << " = " << (simTime() + duration) << endl;
    scheduleAt(simTime() + duration, txEndTimer);
}

}  // namespace phy
