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

#include "Timer.h"

Define_Module(Timer);

int Timer::numInitStages() const
{
    return 2;
}

void Timer::initialize(int stage)
{
    if (stage == 0) {
        tick = registerSignal("tick");
        i = 0;
        nextTickMsg = new cMessage("nextTick");
        scheduleAt(0, nextTickMsg);
        frameMsg = new cMessage("frame");

        if (par("addFrameEvents").boolValue())
            scheduleAt(0, frameMsg);
    } else {
        auto net = getSystemModule();
        net->getCanvas()->setAnimationSpeed(1, this);
        for (cModule::SubmoduleIterator it(net); !it.end(); ++it)
            if (*it != this) {
                (*it)->getCanvas()->setAnimationSpeed(1, this);
                if (auto l = dynamic_cast<cIListener *>(*it))
                    net->subscribe("tick", l);
            }
    }
}

void Timer::handleMessage(cMessage *msg)
{
    if (msg == frameMsg) {
        scheduleAt(simTime() + SimTime(33, SIMTIME_MS), msg);
    }
    if (msg == nextTickMsg) {
        emit(tick, i);
        i++;
        scheduleAt(simTime() + 1, msg);
    }
}
