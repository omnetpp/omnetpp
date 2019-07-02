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

#include "tester.h"

Define_Module(Tester);
Define_Module(Dummy);

void Tester::initialize()
{
    // None of the images are in the project directory, the images or the bitmaps subfolders, intentionally.
    // Adding the photos1 folder to the image path. This will make the images "lena" and "cgi/teapot" available.
    getEnvir()->appendToImagePath(resolveResourcePath("photos1").c_str());

    // Loading two images with default name, the image names will be "baboon" and "box".
    getEnvir()->loadImage(resolveResourcePath("photos2/baboon.png").c_str());
    getEnvir()->loadImage(resolveResourcePath("photos2/cgi/box.gif").c_str());

    // Loading an image with a custom name.
    getEnvir()->loadImage(resolveResourcePath("photos2/cgi/bunny.png").c_str(), "3d/hare");

    // This rectangle will always be placed around this module in its parent.
    bounds = new cRectangleFigure();
    bounds->setLineWidth(2);
    bounds->setLineColor("yellow");
    bounds->setZIndex(2);
    getParentModule()->getCanvas()->addFigure(bounds);

    // This line will always be placed on the connection between the two modules
    line = new cLineFigure();
    line->setLineWidth(3);
    line->setLineColor("purple");
    line->setZIndex(2);
    getParentModule()->getCanvas()->addFigure(line);

    zoom = new cTextFigure();
    zoom->setPosition({50, 350});
    zoom->setText("Zoom level: ?");
    getParentModule()->getCanvas()->addFigure(zoom);

    scheduleAt(simTime()+1, new cMessage());
}

void Tester::handleMessage(cMessage *msg)
{
    delete msg;
    scheduleAt(simTime()+1, new cMessage());
}

void Tester::refreshDisplay() const
{
    bounds->setBounds(getEnvir()->getSubmoduleBounds(this));
    auto cl = getEnvir()->getConnectionLine(gate("dummy$o"));

    if (cl.empty())
        line->setVisible(false);
    else {
        line->setVisible(true);
        line->setStart(cl.front());
        line->setEnd(cl.back());
    }

    zoom->setText(("Zoom level: " + std::to_string(getEnvir()->getZoomLevel(getParentModule()))).c_str());
}
