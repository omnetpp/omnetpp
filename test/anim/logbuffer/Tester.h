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

#ifndef __LOGBUFFER_TESTER_H_
#define __LOGBUFFER_TESTER_H_

#include <omnetpp.h>

using namespace omnetpp;

class MyEvent : public cEvent {
    cModule *target;

public:
    MyEvent(cModule *target);

    virtual MyEvent *dup() const override;
    virtual void execute() override;
    cObject *getTargetObject() const override;
};

class Tester : public cSimpleModule
{
  public:
    Tester();

  protected:
    void initialize() override;

    void handleMessage(cMessage *msg) override;
};

class Dummy : public cSimpleModule
{
  public:
    Dummy();

  protected:
    void initialize() override;

    void handleMessage(cMessage *msg) override;
};

#endif
