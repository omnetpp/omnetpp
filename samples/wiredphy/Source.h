//
// Copyright (C) OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see http://www.gnu.org/licenses/.
//

#ifndef __PHY_SOURCE_H
#define __PHY_SOURCE_H

#include <omnetpp.h>

using namespace omnetpp;

namespace phy {

class Source : public cSimpleModule
{
  protected:
    cMessage *timer = nullptr;

    int seqNum = 0;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *message) override;

    virtual void scheduleTimer();

  public:
    virtual ~Source();
};

}  // namespace phy

#endif

