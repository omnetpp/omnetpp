#include "Animator.h"

Define_Module(Animator);

void Animator::initialize()
{
    scheduleAt(simTime(), new cMessage());

    oval1 = new cOvalFigure();
    oval1->setFilled(true);
    getParentModule()->getCanvas()->addFigure(oval1);
    oval1->setFillColor(cFigure::RED);

    oval2 = new cOvalFigure();
    oval2->setFilled(true);
    getParentModule()->getCanvas()->addFigure(oval2);
    oval2->setFillColor(cFigure::GREEN);

    oval3 = new cOvalFigure();
    oval3->setFilled(true);
    getParentModule()->getCanvas()->addFigure(oval3);
    oval3->setFillColor(cFigure::BLUE);

    getParentModule()->getCanvas()->setAnimationSpeed(par("animationSpeed"), this);
}

void Animator::handleMessage(cMessage* msg)
{
    double eventInterArrivalTime = par("eventInterArrivalTime");
    double eventProcessingTime = par("eventProcessingTime");
    double handleMessageHoldTime = par("handleMessageHoldTime");

    usleep(eventProcessingTime * 1000000);

    scheduleAt(simTime() + eventInterArrivalTime, msg);

    getParentModule()->getCanvas()->holdSimulationFor(handleMessageHoldTime);
}

void Animator::refreshDisplay() const
{
    double st = simTime().dbl() * 2 * M_PI;

    double x = 300 + std::cos(st*10) * 100;
    double y = 300 + std::sin(st*10) * 100;

    oval1->setBounds(cFigure::Rectangle(x-10, y-10, 20, 20));

    x = 300 + std::cos(st) * 150;
    y = 300 + std::sin(st) * 150;

    oval2->setBounds(cFigure::Rectangle(x-10, y-10, 20, 20));

    x = 300 + std::cos(st/10) * 200;
    y = 300 + std::sin(st/10) * 200;

    oval3->setBounds(cFigure::Rectangle(x-10, y-10, 20, 20));

    double refreshDisplayHoldTime = par("refreshDisplayHoldTime");
    getParentModule()->getCanvas()->holdSimulationFor(refreshDisplayHoldTime);
}
