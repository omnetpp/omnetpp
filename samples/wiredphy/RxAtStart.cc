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

#include "RxAtStart.h"
#include "ProgressInfo_m.h"
#include "Tx.h"

namespace phy {

Define_Module(RxAtStart);

static std::ostream& operator<<(std::ostream& os, cPacket *pk)
{
    os << pk->getName() << " (" << pk->str() << ")";
    return os;
}

void RxAtStart::initialize()
{
    cutthroughEnabled = par("cutthrough");
    if (cutthroughEnabled) {
        cutthroughLatency = par("cutthroughLatency");
        cutthroughOutGate = gate("cutthroughOut");
        cutthroughTargetModule = check_and_cast_nullable<Tx *>(cutthroughOutGate->getPathEndGate()->getOwnerModule());
        cutthroughStartTimer = new cMessage("CutthroughStartTimer");
    }

    gate("mediumIn")->setDeliverImmediately(true);
    setTxUpdateSupport(true);

    rxEndTimer = new cMessage("RxEndTimer");
    rxEndTimer->setSchedulingPriority(-1);
}

RxAtStart::~RxAtStart()
{
    cancelAndDelete(rxEndTimer);
    cancelAndDelete(cutthroughStartTimer);
    delete rxPacket;
}

void RxAtStart::handleMessage(cMessage *message)
{
    if (message->arrivedOn("mediumIn"))
        receivePacketFromMedium(check_and_cast<cPacket *>(message));
    else if (message == rxEndTimer)
        endRx();
    else if (cutthroughEnabled && message == cutthroughStartTimer)
        tryCutthrough();
    else
        throw cRuntimeError("Unknown message");
}

void RxAtStart::sendToUpperLayer(cPacket *packet)
{
    send(packet, gate("upperLayerOut"));
}

void RxAtStart::receivePacketFromMedium(cPacket *packet)
{
    EV << "receivePacketFromMedium, pk=" << packet << endl;
    if (!packet->isUpdate())
        startRx(packet);
    else
        updateRx(packet);
}

void RxAtStart::startRx(cPacket *packet)
{
    EV << "startRx, pk=" << packet << endl;
    rxPacket = packet;
    cutthroughInProgress = false;
    if (cutthroughEnabled && rxPacket->getDuration() > cutthroughLatency)
        scheduleCutthroughStart(cutthroughLatency);
    scheduleRxEnd(rxPacket->getRemainingDuration());
}

void RxAtStart::updateRx(cPacket *packet)
{
    EV << "updateRx, pk=" << packet << endl;

    delete rxPacket;
    rxPacket = packet;

    if (cutthroughEnabled) {
        if (cutthroughStartTimer->isScheduled()) {
            if (rxPacket->getDuration() <= cutthroughLatency) // packet too small
                cancelEvent(cutthroughStartTimer);
        }
    }

    cancelEvent(rxEndTimer);
    if (rxPacket->getRemainingDuration() == 0)
        endRx();
    else
        scheduleRxEnd(rxPacket->getRemainingDuration());
}

void RxAtStart::endRx()
{
    EV << "endRx, pk=" << rxPacket << endl;
    if (!cutthroughInProgress)
        sendToUpperLayer(rxPacket);
    else
        sendCutthroughProgress(rxPacket, rxPacket->getBitLength(), true);
    rxPacket = nullptr;
    cutthroughInProgress = false;
}

void RxAtStart::tryCutthrough()
{
    EV << "tryCutthrough\n";
    if (cutthroughTargetModule->isAcceptingCutthrough()) {
        cutthroughInProgress = true;
        EV << "Starting cutthrough, pk=" << rxPacket << std::endl;
        sendCutthroughProgress(rxPacket->dup(), computeNumBitsReceived(rxPacket, cutthroughLatency), false);
    }
}

void RxAtStart::scheduleRxEnd(simtime_t duration)
{
    simtime_t t = simTime() + duration;
    EV << "scheduleRxEnd, for t=" << t.ustr() << endl;
    scheduleAt(t, rxEndTimer);
}

void RxAtStart::scheduleCutthroughStart(simtime_t latency)
{
    simtime_t t = simTime() + latency;
    EV << "scheduleCutthroughStart, for t=" << t.ustr() << endl;
    scheduleAt(t, cutthroughStartTimer);
}

void RxAtStart::sendCutthroughProgress(cPacket *packet, int64_t bitsReceived, bool isEnd)
{
    EV << "sendCutthroughProgress, pk=" << packet << ", @ " << bitsReceived << " bits" << (isEnd ? ", end" : "") << endl;
    ProgressInfo *progressInfo = new ProgressInfo();
    progressInfo->setAvailableBitLength(bitsReceived);
    progressInfo->setIsEnd(isEnd);
    packet->setControlInfo(progressInfo);
    send(packet, cutthroughOutGate);
}

int64_t RxAtStart::computeNumBitsReceived(cPacket *packet, simtime_t receptionTime)
{
    if (receptionTime == packet->getDuration())
        return packet->getBitLength();
    else {
        double percentageReceived = receptionTime / packet->getDuration(); // quite a simplification
        return (int64_t)(packet->getBitLength() * percentageReceived);
    }
}

}  // namespace phy
