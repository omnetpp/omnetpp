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

#include "OsgAnimator.h"

#include <osg/Group>
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>
#include <osg/Geode>

Define_Module(OsgAnimator);

void OsgAnimator::initialize()
{
    int population = par("population");

    playgroundWidth = strtod(getParentModule()->getDisplayString().getTagArg("bgb", 0), nullptr);
    playgroundHeight = strtod(getParentModule()->getDisplayString().getTagArg("bgb", 1), nullptr);

    if (playgroundWidth <=0 || playgroundHeight <=0)
        throw cRuntimeError("Specify playground size in the 'bgb' display string tag");

    cOsgCanvas *canvas = getParentModule()->getOsgCanvas();
    canvas->setGenericViewpoint(omnetpp::cOsgCanvas::Viewpoint({400, 300, 1200}, {400, 300, 0}, {0, 1, 0}));
    canvas->setClearColor("#444444");

    osg::Group *root = new osg::Group();
    canvas->setScene(root);

    // shared shape, center is offset for right-hand driving
    osg::Box *cuboid = new osg::Box(osg::Vec3(0,-1.5,0), 4.0f, 2.0f, 1.5f);

    for (int i = 0; i < population; i++) {
        osg::PositionAttitudeTransform *pat = new osg::PositionAttitudeTransform();
        root->addChild(pat);
        bees.push_back(pat);

        osg::Geode *geode = new osg::Geode();
        pat->addChild(geode);

        osg::ShapeDrawable *cuboidDrawable = new osg::ShapeDrawable(cuboid);
        geode->addDrawable(cuboidDrawable);

        cFigure::Color color = omnetpp::cFigure::GOOD_LIGHT_COLORS[i % cFigure::NUM_GOOD_LIGHT_COLORS];
        cuboidDrawable->setColor(osg::Vec4f(color.red/255.0f, color.green/255.0f, color.blue/255.0f, 1.0f));
    }

    getParentModule()->getCanvas()->setAnimationSpeed(1, nullptr);

    scheduleAt(0, new cMessage("dummy"));
}

void OsgAnimator::refreshDisplay() const
{
    const double a = 31, b = 43; // parameters of the Lissajous curve nodes follow
    int population = (int)bees.size();
    double now = simTime().dbl();

    for (int i = 0; i < population; i++) {
        double t = now/20 + i/(double)population * 6.2832;
        osg::PositionAttitudeTransform *bee = bees[i];

        double x = sin(a*t);
        double y = sin(b*t);
        x = (x+1) * playgroundWidth/2;
        y = (y+1) * playgroundHeight/2;

        bee->setPosition({x, y, 0.0});

        double dx = a * cos(a*t) * playgroundWidth;
        double dy = b * cos(b*t) * playgroundHeight;

        bee->setAttitude(osg::Quat(std::atan2(dy, dx), osg::Vec3d(0.0, 0.0, 1.0)));
    }
}

void OsgAnimator::handleMessage(cMessage *msg)
{
    scheduleAt(simTime()+1, msg);
}
