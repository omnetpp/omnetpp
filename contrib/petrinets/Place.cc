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

#include "Place.h"

Define_Module(Place);

void Place::initialize(int stage)
{
    if (stage==0) {
        numTokens = par("numInitialTokens");
        WATCH(numTokens);
    }
    else if (stage==1) {
        numTokensChanged();
    }
}

void Place::handleMessage(cMessage *msg)
{
    error("this module does not process messages");
}

int Place::getNumTokens()
{
    return numTokens;
}

void Place::addTokens(int n)
{
    Enter_Method_Silent();
    ASSERT(n>0);
    numTokens += n;
    numTokensChanged();
}

void Place::removeTokens(int n)
{
    Enter_Method_Silent();
    ASSERT(n>0);
    ASSERT(numTokens>=n);
    numTokens -= n;
    numTokensChanged();
}

void Place::numTokensChanged()
{
    if (ev.isGUI())
        getDisplayString().setTagArg("t", 0, numTokens);

    // notify output transitions
    int n = gateSize("out");
    for (int i=0; i<n; i++) {
        ITransition *transition = getOutputTransition(i);
        if (transition)
            transition->numTokensChanged(this);
    }
}

ITransition *Place::getOutputTransition(int i)
{
    cGate *g = gate("out", i);
    if (g->getNextGate() == NULL)
        return NULL; // not connected
    return check_and_cast<ITransition *>(g->getPathEndGate()->getOwnerModule());
}
