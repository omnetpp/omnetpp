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

#include "Tester.h"

#include "omnetpp/cevent.h"

MyEvent::MyEvent(cModule *target) : cEvent("MyEvent"), target(target) {}

MyEvent *MyEvent::dup() const
{
    return new MyEvent(target);
}

void MyEvent::execute()
{
    EV_INFO << "MyEvent executed with target: " << (target ? target->getName() : "<nullptr>") << std::endl;
}

cObject *MyEvent::getTargetObject() const
{
    return target;
}

Define_Module(Tester);

Tester::Tester()
{
    EV_INFO << "Message from the Tester constructor." << std::endl;
}

void Tester::initialize()
{
    EV_INFO << "Message from the Tester initialize." << std::endl;

    cEvent *ev = new MyEvent(nullptr);
    ev->setArrivalTime(SimTime(10, SIMTIME_MS));
    getSimulation()->getFES()->insert(ev);

    ev = new MyEvent(this);
    ev->setArrivalTime(SimTime(15, SIMTIME_MS));
    getSimulation()->getFES()->insert(ev);

    scheduleAt(SimTime(20, SIMTIME_MS), new cMessage("createDummy"));
}

void Tester::handleMessage(cMessage *msg)
{
    EV_INFO << "Creating Dummy module." << std::endl;
    cModuleType *moduleType = cModuleType::get("Dummy");
    moduleType->createScheduleInit("dummy", this);
    EV_INFO << "Dummy module created." << std::endl;
    delete msg;
}

Define_Module(Dummy);

Dummy::Dummy()
{
    EV_INFO << "Message from the Dummy constructor." << std::endl;
}

void Dummy::initialize()
{
    EV_INFO << "Message from the Dummy initialize." << std::endl;
}

void Dummy::handleMessage(cMessage *msg)
{
    delete msg;
}