#include <omnetpp.h>

using namespace omnetpp;

class Animator : public cSimpleModule
{
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Animator);

void Animator::initialize()
{
    scheduleAt(simTime(), new cMessage());
}

void Animator::handleMessage(cMessage *msg)
{
    cFigure *figure = getParentModule()->getCanvas()->findFigureRecursively("root");
    figure->rotate(0.01, 200, 200);
    figure->scale(0.997, 0.997);

    cOvalFigure *oval = check_and_cast<cOvalFigure *>(getParentModule()->getCanvas()->findFigureRecursively("oval1"));
    int w = (int)(simTime().dbl())%20 + 1;
    oval->setLineWidth(w);

    oval->rotate(-0.005, 500, 150);

    cPathFigure *path = check_and_cast<cPathFigure *>(getParentModule()->getCanvas()->findFigureRecursively("path1"));
    path->move(10 * sin(simTime().dbl() / 10.0), 0);

    scheduleAt(simTime()+1, msg);
}

