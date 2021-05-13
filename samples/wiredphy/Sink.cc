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

#include "Sink.h"

namespace phy {

Define_Module(Sink);

void Sink::initialize()
{
    WATCH(numPacket);
}

void Sink::handleMessage(cMessage *message)
{
    numPacket++;
    std::string text = "received " + std::to_string(numPacket);
    getDisplayString().setTagArg("t", 0, text.c_str());
    delete message;
}

}  // namespace phy

