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

TransitionRegistry *TransitionRegistry::instance = NULL;

//FIXME: store only armed transitions!! and: armed(t), disarmed(t)!!!
TransitionRegistry *TransitionRegistry::getInstance()
{
    if (!instance)
        instance = new TransitionRegistry();
    return instance;
}

void TransitionRegistry::registerTransition(ITransition *t)
{
    transitions.push_back(t);
}

void TransitionRegistry::deregisterTransition(ITransition *t)
{
    std::vector<ITransition*>::iterator it = transitions.find(t);
    ASSERT(it != transitions.end());
    transitions.erase(it);
}

void TransitionRegistry::scheduleNextFiring()
{
    // choose one randomly among the ones that can fire
    std::vector<ITransition*> armedList;
    int n = transitions.size();
    for (int i=0; i<n; i++)
        if (transitions[i]->canFire())
            armedList.push_back(transitions[i]);
    if (!armedList.empty()) {
        int k = intrand(armedList.size());
        armedList[k]->scheduleFiring();
    }
}

//----------


Transition::Transition()
{
    fireEvent = endTransitionEvent = NULL;
    TransitionRegistry::getInstance()->registerTransition(this);
}

Transition::~Transition()
{
    TransitionRegistry::getInstance()->deregisterTransition(this);
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
    //TODO inputsChanged = true;
}

bool Transition::canFire()
{
    //TODO cache state

    // check if all inputs have enough tokens
    int n = inputPlaces.size();
    for (int i = 0; i < n; i++)
        if (inputPlaces[i].multiplicity > 0 ?
            inputPlaces[i].place->getNumTokens() < inputPlaces[i].multiplicity : // normal arc
            inputPlaces[i].place->getNumTokens() > -inputPlaces[i].multiplicity) // inhibitor arc
            return false;
    return evaluateGuardCondition();
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
    else {
        TransitionRegistry->getInstance()->scheduleNextFiring();
        scheduleAt(simTime()+transitionTime, endTransitionEvent); // with strongest priority, i.e. zero
    }
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
    TransitionRegistry->getInstance()->scheduleNextFiring();
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



