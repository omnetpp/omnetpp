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

#include <fstream>

namespace updatepacket {

SourceBase::TxInfo::TxInfo(std::string line)
{
    int pipe = line.find("|");
    if (pipe != std::string::npos) {
        description = opp_trim(line.substr(pipe+1));
        line = line.substr(0, pipe);
    }

    line = opp_trim(line);

    cStringTokenizer tok(line.c_str(), ";");
    auto toks = tok.asVector();

    initialDuration = SimTime::parse(toks[0].c_str());

    for (int i = 1; i < toks.size(); ++i) {
        auto updateStr = opp_trim(toks[i]);

        if (updateStr.empty())
            continue;

        int colon = updateStr.find(":");
        ASSERT(colon != std::string::npos);
        std::string precDelay = opp_trim(updateStr.substr(0, colon));
        std::string newRemDuration = opp_trim(updateStr.substr(colon + 1));

        ASSERT(precDelay[0] == '+');
        ASSERT(newRemDuration[0] == '-');

        precDelay = precDelay.substr(1);
        newRemDuration = newRemDuration.substr(1);

        updates.push_back({SimTime::parse(precDelay.c_str()), SimTime::parse(newRemDuration.c_str())});
    }
}

Define_Module(Source);
Define_Module(DirectSource);

SourceBase::~SourceBase()
{
    cancelAndDelete(nextTxTimer);
    cancelAndDelete(nextUpdateTimer);
}

void SourceBase::initialize()
{
    nextTxTimer = new cMessage("nextTx");
    scheduleAt(simTime() + 1, nextTxTimer);

    nextUpdateTimer = new cMessage("nextUpdate");

    std::ifstream file("transmissions.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            // strip comments
            int hashmark = line.find("#");
            if (hashmark != std::string::npos)
                line = line.substr(0, hashmark);

            line = opp_trim(line);
            if (line.empty())
                continue;

            txInfos.push_back(TxInfo(line));
        }
        file.close();
    }
}

void SourceBase::handleMessage(cMessage *msg)
{
    if (msg == nextTxTimer) {
        curTxInfo = &txInfos[nextTxIndex];
        EV << "Initiating transmission " << nextTxIndex << " out of [0.." << txInfos.size()-1 << "]...\n";
        EV << curTxInfo->description << std::endl;

        cPacket *packet = new cPacket(("packet-" + std::to_string(nextTxIndex)).c_str());
        transmissionId = packet->getId();
        outputPacket(packet, SendOptions().transmissionId(transmissionId).duration(curTxInfo->initialDuration));
        lastTxStartTime = simTime();

        ++nextTxIndex;
        nextTxIndex %= txInfos.size();
        nextUpdateIndex = 0;

        if (curTxInfo->updates.empty())
            // no updates for this transmission, the next thing to do is to transmit the next packet
            scheduleAt(simTime() + 1, nextTxTimer);
        else
            // the next thing to do is to update this transmission
            scheduleAt(simTime() + curTxInfo->updates[0].first, nextUpdateTimer);
    }
    else if (msg == nextUpdateTimer) {
        cPacket *packet = new cPacket(("update-" + std::to_string(nextTxIndex-1) + "-" + std::to_string(nextUpdateIndex)).c_str());
        SimTime remainingDuration = curTxInfo->updates[nextUpdateIndex].second;
        outputPacket(packet, SendOptions()
            .updateTx(transmissionId, remainingDuration)
            .duration(simTime() - lastTxStartTime + remainingDuration) // This only needs to be here for the sendDirect() case
        );

        ++nextUpdateIndex;

        if (nextUpdateIndex == curTxInfo->updates.size())
            // we are done with this transmission, on to the next one
            scheduleAt(simTime() + 1, nextTxTimer);
        else
            // this transmission has some further update(s)
            scheduleAt(simTime() + curTxInfo->updates[nextUpdateIndex].first, nextUpdateTimer);
    }
    else {
        ASSERT(false);
    }

}

} //namespace
