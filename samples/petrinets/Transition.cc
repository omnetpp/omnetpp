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

#include "Transition.h"
#include "IPlace.h"

Define_Module(Transition);

#define SHIFT 1000  /* 32*1000 < 32767 (max of cMessage's schedulingPriority) */


Transition::Transition()
{
    fireEvent = endTransitionEvent = NULL;
}

Transition::~Transition()
{
    cancelAndDelete(fireEvent);
    cancelAndDelete(endTransitionEvent);
}

void Transition::initialize()
{
    priority = par("priority");
    if (priority<0 || priority>=32)
        error("priority=%d is out of range, must be 0..32", priority);

    transitionTimePar = &par("transitionTime");

    fireEvent = new cMessage("fire");
    endTransitionEvent = new cMessage("endTransition");

    discoverNeighbours(); // to be called on every topology change
}

void Transition::discoverNeighbours()
{
    inputPlaces.clear();
    outputPlaces.clear();

    // TODO merge multiple arcs to the same place
    int ni = gateSize("in");
    inputPlaces.reserve(ni);
    for (int i=0; i<ni; i++) {
        cGate *g = gate("in", i);
        if (g->getPreviousGate() != NULL) {
            Neighbour neighbour;
            neighbour.place = check_and_cast<IPlace *>(g->getPathStartGate()->getOwnerModule());
            neighbour.multiplicity = 1; //FIXME read arc's parameter
            inputPlaces.push_back(neighbour);
        }
    }

    int no = gateSize("out");
    outputPlaces.reserve(no);
    for (int i=0; i<no; i++) {
        cGate *g = gate("out", i);
        if (g->getNextGate() != NULL) {
            Neighbour neighbour;
            neighbour.place = check_and_cast<IPlace *>(g->getPathEndGate()->getOwnerModule());
            neighbour.multiplicity = 1; //FIXME read arc's parameter
            outputPlaces.push_back(neighbour);
        }
    }

    EV << inputPlaces.size() << " input places, " << outputPlaces.size() << " output places\n";
}

void Transition::handleMessage(cMessage *msg)
{
    if (msg==fireEvent)
        startFire();
    else if (msg==endTransitionEvent)
        endFire();
    else
        error("unexpected message received");

    updateGUI();
}

void Transition::numTokensChanged(IPlace *)
{
    Enter_Method_Silent();
    if (endTransitionEvent->isScheduled()) {
        // not interested in changes -- we are currently firing
    }
    else {
        if (!fireEvent->isScheduled()) {
            if (canFire())
                arm();
        }
        else {
            if (!canFire())
                disarm();
        }
    }
}

bool Transition::canFire()
{
    // check if all inputs have enough tokens
    int n = inputPlaces.size();
    for (int i = 0; i < n; i++)
        if (inputPlaces[i].multiplicity > 0 ?
            inputPlaces[i].place->getNumTokens() < inputPlaces[i].multiplicity : // normal arc
            inputPlaces[i].place->getNumTokens() > -inputPlaces[i].multiplicity) // inhibitor arc
            return false;
    return evaluateGuardCondition();
}

void Transition::arm()
{
    EV << "We can fire: arming transition\n";
    // set event prio to ensure that transitions of the same priority fire in random order
    fireEvent->setSchedulingPriority(priority*SHIFT + intuniform(0,SHIFT-1));
    scheduleAt(simTime(), fireEvent);
    updateGUI();
}

void Transition::disarm()
{
    EV << "Disarming transition\n";
    cancelEvent(fireEvent);
    updateGUI();
}

void Transition::startFire()
{
    ASSERT(canFire());

    EV << "startFire: removing tokens from input places\n";

    int n = inputPlaces.size();
    for (int i = 0; i < n; i++)
        if (inputPlaces[i].multiplicity > 0)
            inputPlaces[i].place->removeTokens(inputPlaces[i].multiplicity);

    // do or schedule endFire()
    simtime_t transitionTime = transitionTimePar->doubleValue();
    if (transitionTime==0)
        endFire();
    else
        scheduleAt(simTime()+transitionTime, endTransitionEvent); // with strongest priority, i.e. zero
}

void Transition::endFire()
{
    EV << "endFire: adding tokens to output places\n";

    // add tokens to output places
    int n = outputPlaces.size();
    for (int i = 0; i < n; i++) {
        ASSERT(outputPlaces[i].multiplicity > 0); // outgoing inhibitor arcs don't exist
        outputPlaces[i].place->addTokens(outputPlaces[i].multiplicity);
    }

    // during firing we didn't listen to notifications, so we need to check again
    if (canFire())
        arm();
}

void Transition::updateGUI()
{
    if (ev.isGUI()) {
        if (endTransitionEvent->isScheduled())
            getDisplayString().setTagArg("b", 3, "yellow");  // firing
        else if (fireEvent->isScheduled())
            getDisplayString().setTagArg("b", 3, "lightblue"); // armed
        else
            getDisplayString().setTagArg("b", 3, "grey");  // disabled
    }
}



