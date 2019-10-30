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

#ifndef __FIGUREAPI_TESTBASE_H_
#define __FIGUREAPI_TESTBASE_H_

#include <omnetpp.h>

using namespace omnetpp;


class TestBase : public cSimpleModule, public cIListener {
protected:
    cLabelFigure *descr;
    cLabelFigure *state;
    intval_t lastTick = 0;

    virtual void initialize();
    virtual void refreshDisplay() const;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, intval_t l, cObject *details = 0);

    // dummies
    virtual void handleMessage(cMessage *msg) {}

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b, cObject *details) {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, uintval_t l, cObject *details) {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d, cObject *details) {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t, cObject *details) {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s, cObject *details) {}
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {}

    virtual std::string init() = 0;
    virtual std::string onTick(int tick) = 0;
    virtual std::string onFrame(int tick, double t) const { return ""; }
};

#endif
