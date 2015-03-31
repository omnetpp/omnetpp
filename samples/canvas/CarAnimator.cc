//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2014 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

USING_NAMESPACE

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef cFigure::Point Point;

class CarAnimator : public cSimpleModule
{
    private:
        simtime_t timeStep;
        Point loc;
        double speed;
        double heading;
        double angularSpeed;
        int targetPointIndex;
        double distanceTravelled;
        cPolygonFigure *road;
        cImageFigure *car;
        cFigure *antenna;
        cPolylineFigure *trail;
        cTextFigure *distanceDisplay;
        cTextFigure *headingDisplay;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        void refresh();
};

Define_Module(CarAnimator);

void CarAnimator::initialize()
{
    timeStep = 1;
    speed = 2;
    heading = 0;
    angularSpeed = 0;
    targetPointIndex = 0;
    distanceTravelled = 0;

    cCanvas *canvas = getParentModule()->getCanvas();

    road = check_and_cast<cPolygonFigure*>(canvas->getFigure("road"));
    trail = check_and_cast<cPolylineFigure*>(canvas->getFigure("trail"));
    car = check_and_cast<cImageFigure*>(canvas->getFigure("car"));
    antenna = check_and_cast<cFigure*>(canvas->getFigureByPath("car.antenna"));
    distanceDisplay = check_and_cast<cTextFigure*>(canvas->getFigureByPath("status.distance"));
    headingDisplay = check_and_cast<cTextFigure*>(canvas->getFigureByPath("status.heading"));

    loc = road->getPoint(targetPointIndex);

    WATCH(timeStep);
    WATCH(loc.x);
    WATCH(loc.y);
    WATCH(speed);
    WATCH(heading);
    WATCH(angularSpeed);
    WATCH(targetPointIndex);
    WATCH(distanceTravelled);

    refresh();

    scheduleAt(simTime(), new cMessage());
}

void CarAnimator::refresh()
{
    cFigure::Transform t;
    t.rotate(heading);
    t.translate(loc.x, loc.y);
    car->setTransform(t);

    trail->addPoint(loc);
    if (trail->getNumPoints() > 500)
        trail->removePoint(0);

    char buf[20];
    sprintf(buf, "%gm", distanceTravelled);
    distanceDisplay->setText(buf);

    int degrees = -int(heading*180/M_PI);
    degrees = degrees - 360 * (int)floor(degrees / 360.0);
    sprintf(buf, "%d\xC2\xB0", degrees);
    headingDisplay->setText(buf);
}

void CarAnimator::handleMessage(cMessage *msg)
{
    // steer
    Point target = road->getPoint(targetPointIndex);
    Point vectorToTarget = target - loc;
    if (vectorToTarget.getLength() < 50)  // reached
        targetPointIndex = (targetPointIndex+1) % road->getNumPoints();

    double targetDirection = atan2(vectorToTarget.y, vectorToTarget.x);
    double diff = targetDirection - heading;
    while (diff < -M_PI)  diff += 2*M_PI;
    while (diff > M_PI)  diff -= 2*M_PI;

    angularSpeed = diff / 30;

    // move
    heading += angularSpeed * timeStep.dbl();
    double distance = speed * timeStep.dbl();
    loc.x += distance * cos(heading);
    loc.y += distance * sin(heading);
    antenna->rotate(-2*M_PI/180, 0, 0);
    distanceTravelled += distance;
    refresh();

    scheduleAt(simTime() + timeStep, msg);
}


