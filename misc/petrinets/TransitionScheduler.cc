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
#include "TransitionScheduler.h"

Define_Module(TransitionScheduler);

void TransitionScheduler::registerTransition(ITransition *t)
{
    transitions.push_back(t);
}

void TransitionScheduler::deregisterTransition(ITransition *t)
{
    std::vector<ITransition*>::iterator it = std::find(transitions.begin(), transitions.end(), t);
    ASSERT(it != transitions.end());
    transitions.erase(it);
}

void TransitionScheduler::initialize(int stage)
{
    if (stage==2)
        scheduleNextFiring();
}

bool TransitionScheduler::scheduleNextFiring()
{
    Enter_Method_Silent("scheduleNextFiring");

    // choose one randomly among the ones that can fire
    std::vector<ITransition*> armedList;
    int n = transitions.size();
    for (int i=0; i<n; i++)
        if (transitions[i]->canFire())
            armedList.push_back(transitions[i]);

    if (armedList.empty()) {
        EV << "No transition can fire\n";
        return false;
    }

    int k = intrand(armedList.size());
    armedList[k]->scheduleFiring();
    EV << "Selected transition X\n"; //TODO
    return true;
}
