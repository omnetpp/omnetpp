//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

using namespace omnetpp;

typedef cFigure::Point Point;

class CarAnimator : public cSimpleModule
{
    private:
        simtime_t timeStep;
        simtime_t lastStep;
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
        cFigure *navView;
        cFigure *navCar;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
        void refresh() const;
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

    canvas->setAnimationSpeed(50.0, this);

    road = check_and_cast<cPolygonFigure *>(canvas->getFigure("road"));
    trail = check_and_cast<cPolylineFigure *>(canvas->getFigure("trail"));
    car = check_and_cast<cImageFigure *>(canvas->getFigure("car"));
    antenna = check_and_cast<cFigure *>(canvas->getFigureByPath("car.antenna"));
    distanceDisplay = check_and_cast<cTextFigure *>(canvas->getFigureByPath("status.distance"));
    headingDisplay = check_and_cast<cTextFigure *>(canvas->getFigureByPath("status.heading"));

    // navigator: a magnified copy of the road with a marker for the car;
    // the navigator window clips it (see clipChildren in the NED file)
    navView = new cGroupFigure("view");
    navView->addFigure(road->dup());
    navView->addFigure(canvas->getFigure("paint")->dup());
    cOvalFigure *marker = new cOvalFigure("car");
    marker->setBounds(cFigure::Rectangle(-5, -5, 10, 10));
    marker->setFilled(true);
    marker->setFillColor(cFigure::RED);
    navCar = marker;
    navView->addFigure(navCar);
    canvas->getFigureByPath("status.nav")->addFigure(navView);

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

void CarAnimator::refresh() const
{
    double t = (simTime() - lastStep) / timeStep;

    ASSERT(t >= 0);
    ASSERT(t <= 1);

    cFigure::Transform carTr;
    carTr.rotate(heading + angularSpeed * t);

    double distance = speed * t;
    Point carPos(loc.x + distance * cos(heading), loc.y + distance * sin(heading));
    carTr.translate(carPos.x, carPos.y);
    car->setTransform(carTr);

    // keep the car in the middle of the navigator window (whose center is at (0,160) in "status"), at 2x magnification
    navCar->setTransform(cFigure::Transform().translate(carPos.x, carPos.y));
    navView->setTransform(cFigure::Transform().translate(-carPos.x, -carPos.y).scale(2).translate(0, 160));

    cFigure::Transform antTr;
    antTr.rotate(-2 * simTime().dbl()*M_PI/180);
    antenna->setTransform(antTr);

    char buf[20];
    snprintf(buf, sizeof(buf), "%.0fm", distanceTravelled);
    distanceDisplay->setText(buf);

    int degrees = -int(heading*180/M_PI);
    degrees = degrees - 360 * (int)floor(degrees / 360.0);
    snprintf(buf, sizeof(buf), "%d\xC2\xB0", degrees);
    headingDisplay->setText(buf);
}

void CarAnimator::refreshDisplay() const
{
    refresh();
}

void CarAnimator::handleMessage(cMessage *msg)
{
    // move
    double distance = speed * timeStep.dbl();
    loc.x += distance * cos(heading);
    loc.y += distance * sin(heading);

    Point target = road->getPoint(targetPointIndex);
    Point vectorToTarget = target - loc;
    if (vectorToTarget.getLength() < 50)  // reached
        targetPointIndex = (targetPointIndex+1) % road->getNumPoints();

    double targetDirection = atan2(vectorToTarget.y, vectorToTarget.x);
    double diff = targetDirection - heading;
    while (diff < -M_PI)
        diff += 2*M_PI;
    while (diff > M_PI)
        diff -= 2*M_PI;

    // steer
    heading += angularSpeed * timeStep.dbl();

    angularSpeed = diff / 30;

    distanceTravelled += distance;
    refresh();

    trail->addPoint(loc);
    if (trail->getNumPoints() > 500)
        trail->removePoint(0);

    lastStep = simTime();
    scheduleAt(simTime() + timeStep, msg);
}

