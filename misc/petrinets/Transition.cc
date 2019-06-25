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

#include <algorithm>
#include "Transition.h"
#include "IPlace.h"

Define_Module(Transition);

Transition::Transition()
{
    fireEvent = endTransitionEvent = NULL;
}

Transition::~Transition()
{
    transitionScheduler = dynamic_cast<TransitionScheduler*>(getModuleByPath(par("transitionSchedulerModule").stringValue()));
    if (transitionScheduler) // still exists
        transitionScheduler->deregisterTransition(this);
    cancelAndDelete(fireEvent);
    cancelAndDelete(endTransitionEvent);
}

void Transition::initialize()
{
    priority = par("priority");
    if (priority < 0 || priority >= 32)
        error("priority=%d is out of range, must be 0..32", priority);

    transitionTimePar = &par("transitionTime");

    fireEvent = new cMessage("fire");
    endTransitionEvent = new cMessage("endTransition");

    discoverNeighbours();

    transitionScheduler = check_and_cast<TransitionScheduler*>(getModuleByPath(par("transitionSchedulerModule").stringValue()));
    transitionScheduler->registerTransition(this);
}

void Transition::discoverNeighbours()
{
    inputPlaces.clear();
    outputPlaces.clear();

    int numInputs = gateSize("in");
    inputPlaces.reserve(numInputs);
    for (int i = 0; i < numInputs; i++) {
        cGate *g = gate("in", i);
        if (g->getPreviousGate() != nullptr) {
            Neighbour neighbour;
            neighbour.place = check_and_cast<IPlace *>(g->getPathStartGate()->getOwnerModule());
            cChannel *arc = g->getPreviousGate()->getChannel();
            neighbour.multiplicity = arc->par("multiplicity");
            if (std::find_if(inputPlaces.begin(), inputPlaces.end(), [&] (const Neighbour& j) { return j.place == neighbour.place; }) != inputPlaces.end())
                throw cRuntimeError("Multiple input arcs from place '%s'", g->getPathStartGate()->getOwnerModule()->getFullPath().c_str());
            inputPlaces.push_back(neighbour);
        }
    }

    int numOutputs = gateSize("out");
    outputPlaces.reserve(numOutputs);
    for (int i = 0; i < numOutputs; i++) {
        cGate *g = gate("out", i);
        if (g->getNextGate() != nullptr) {
            Neighbour neighbour;
            neighbour.place = check_and_cast<IPlace *>(g->getPathEndGate()->getOwnerModule());
            cChannel *arc = g->getChannel();
            neighbour.multiplicity = arc->par("multiplicity");
            outputPlaces.push_back(neighbour);
        }
    }

    EV << inputPlaces.size() << " input places, " << outputPlaces.size() << " output places\n";
}

void Transition::handleMessage(cMessage *msg)
{
    if (msg == fireEvent)
        startFire();
    else if (msg == endTransitionEvent)
        endFire();
    else
        error("Unexpected message received");
}

void Transition::refreshDisplay() const
{
    if (endTransitionEvent->isScheduled())
        getDisplayString().setTagArg("b", 3, "yellow");  // firing
    else if (fireEvent->isScheduled())
        getDisplayString().setTagArg("b", 3, "lightblue"); // armed
    else
        getDisplayString().setTagArg("b", 3, "grey");  // disabled
}

void Transition::numTokensChanged(IPlace *)
{
    Enter_Method_Silent("numTokensChanged");
    // currently ignored -- may be used for caching
}

bool Transition::canFire()
{
    Enter_Method_Silent("canFire");

    // check if all inputs have enough tokens
    for (auto& inputPlace : inputPlaces)
        if (inputPlace.multiplicity > 0 ?
            inputPlace.place->getNumTokens() < inputPlace.multiplicity : // normal arc
            inputPlace.place->getNumTokens() > -inputPlace.multiplicity) // inhibitor arc
            return false;
    return evaluateGuardCondition();
}

void Transition::scheduleFiring()
{
    Enter_Method_Silent("scheduleFiring");
    scheduleAt(simTime(), fireEvent);
}

void Transition::startFire()
{
    ASSERT(canFire());

    EV << "startFire: removing tokens from input places\n";

    for (auto& inputPlace : inputPlaces)
        if (inputPlace.multiplicity > 0)
            inputPlace.place->removeTokens(inputPlace.multiplicity);

    // do or schedule endFire()
    simtime_t transitionTime = transitionTimePar->doubleValue();
    if (transitionTime == 0)
        endFire();
    else {
        transitionScheduler->scheduleNextFiring();
        scheduleAt(simTime()+transitionTime, endTransitionEvent); // with strongest priority, i.e. zero
    }
}

void Transition::endFire()
{
    EV << "endFire: adding tokens to output places\n";

    // add tokens to output places
    for (auto& outputPlace : outputPlaces) {
        ASSERT(outputPlace.multiplicity > 0); // outgoing inhibitor arcs don't exist
        outputPlace.place->addTokens(outputPlace.multiplicity);
    }

    transitionScheduler->scheduleNextFiring();
}
