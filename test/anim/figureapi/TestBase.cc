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

#include "TestBase.h"

void TestBase::initialize()
{
    descr = new cLabelFigure("description");
    descr->setPosition(cFigure::Point(20, 20));

    state = new cLabelFigure("state");
    state->setPosition(cFigure::Point(20, 60));

    getCanvas()->addFigure(descr);
    getCanvas()->addFigure(state);

    std::string descrText = init();
    descr->setText(descrText.c_str());
}

void TestBase::refreshDisplay() const
{
    Enter_Method_Silent(); // just to set the context object

    double t = (simTime() - simTime().inUnit(SIMTIME_S)).dbl();

    std::string stateText = onFrame(lastTick, t);
    if (!stateText.empty())
        state->setText(stateText.c_str());
}

void TestBase::receiveSignal(cComponent *source, simsignal_t signalID, intval_t l, cObject *details)
{
    Enter_Method_Silent(); // just to set the context object

    lastTick = l;

    std::string stateText = onTick(l);
    state->setText(stateText.c_str());
}
