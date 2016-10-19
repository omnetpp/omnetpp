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

#include "Sink.h"

namespace animation2 {

Define_Module(Sink);

void Sink::handleMessage(cMessage *msg)
{
    EV << "Received " << msg->getName() << endl;
    delete msg;
}

void Sink::bar() {
    Enter_Method("bar");
}

void Sink::baz() {
    Enter_Method("baz");
}

}; // namespace
