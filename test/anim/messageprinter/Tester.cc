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

#include <iomanip>
#include "Tester.h"

Define_Module(Tester);

class CustomPrinter : public cMessagePrinter
{
   virtual int getScoreFor(cMessage *msg) const override { return 50; }
   std::vector<std::string> getColumnNames(const Options *options) const override { return {"Column 1", "Column 2", "Kind"}; }
   virtual void printMessage(std::ostream& os, cMessage *msg, const Options *options) const override { os << "HEY\tHO\tKind = " << std::to_string(msg->getKind()); }
};

Register_MessagePrinter(CustomPrinter);

void Tester::initialize()
{
    i = 0;
    msg = new cMessage("selfMsg");
    send(msg, "out");
}

void Tester::handleMessage(cMessage *msg)
{
    ++i;
    msg->setKind(i);
    sendDelayed(msg, SimTime(33, SIMTIME_MS), "out");
}
