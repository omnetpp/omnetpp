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

#ifndef __UPDATEPACKET_SOURCE_H_
#define __UPDATEPACKET_SOURCE_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace updatepacket {

class SourceBase : public cSimpleModule
{
    cMessage *nextTxTimer = nullptr;
    cMessage *nextUpdateTimer = nullptr;

    struct TxInfo {
        std::string description;
        SimTime initialDuration;
        std::vector<std::pair<SimTime, SimTime>> updates; // each pair is: <preceding delay, new remainingduration>

        explicit TxInfo(std::string line);
    };

    std::vector<TxInfo> txInfos;
    TxInfo *curTxInfo = nullptr;
    int nextTxIndex = 0;
    int nextUpdateIndex = 0;

    long transmissionId = -1;
    SimTime lastTxStartTime;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    virtual void outputPacket(cPacket *packet, const SendOptions& options) = 0;

    virtual ~SourceBase();
};

class Source : public SourceBase {
    virtual void outputPacket(cPacket *packet, const SendOptions& options) override {
        send(packet, options, "out");
    }
};


class DirectSource : public SourceBase {
    virtual void outputPacket(cPacket *packet, const SendOptions& options) override {
        sendDirect(packet, SendOptions(options).propagationDelay(0.03), getSystemModule()->getModuleByPath(par("destination").stringValue())->gate("in"));
    }
};


} //namespace

#endif
