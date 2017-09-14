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

#include "Animator.h"

Define_Module(Animator);

void Animator::initialize()
{
    playgroundWidth = strtod(getParentModule()->getDisplayString().getTagArg("bgb", 0), nullptr);
    playgroundHeight = strtod(getParentModule()->getDisplayString().getTagArg("bgb", 1), nullptr);
    if (playgroundWidth <=0 || playgroundHeight <=0)
        throw cRuntimeError("Specify playground size in the 'bgb' display string tag");

    int population = par("population");
    std::string prototypeName = par("figurePrototype");
    cFigure *prototype = getCanvas()->getFigure(prototypeName.c_str());

    cCanvas *canvas = getParentModule()->getCanvas();
    for (int i = 0; i < population; i++) {
        cFigure *bee = prototype->dup();
        canvas->addFigure(bee);
        bees.push_back(bee);
    }

    canvas->setAnimationSpeed(1, nullptr);
    scheduleAt(0, new cMessage("dummy"));
}

void Animator::refreshDisplay() const
{
    int population = (int)bees.size();
    double now = simTime().dbl();
    const double a = 10, b = 7; // parameters of the Lissajous curve

    for (int i = 0; i < population; i++) {
        // Lissajous
        double t = now/20 + i/(double)population * 10;
        double x = sin(a*t);
        double y = sin(b*t);

        // scale to playground size
        x = (x+1) * playgroundWidth/2;
        y = (y+1) * playgroundHeight/2;
        setPosition(bees[i], x, y);
    }
}

void Animator::setPosition(cFigure *figure, double x, double y) const
{
    if (auto rect = dynamic_cast<cRectangleFigure*>(figure))
        rect->setPosition({x, y}, cFigure::ANCHOR_CENTER);
    else if (auto rect = dynamic_cast<cLineFigure*>(figure)) {
        double w = rect->getLineWidth();
        rect->setStart({x, y});
        rect->setEnd({x+w, y});
    }
    else if (auto img = dynamic_cast<cAbstractImageFigure*>(figure))
        img->setPosition({x, y});
    else if (auto txt = dynamic_cast<cAbstractTextFigure*>(figure))
        txt->setPosition({x, y});
    else
        throw cRuntimeError("Cannot set node position: unsupported figure type");
}

void Animator::handleMessage(cMessage *msg)
{
    scheduleAt(simTime()+1, msg);
}
