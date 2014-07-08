#include <omnetpp.h>

class Animator : public cSimpleModule
{
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Animator);

void Animator::initialize()
{
    scheduleAt(simTime(), new cMessage());
}

void Animator::handleMessage(cMessage *msg)
{
    cFigure *figure = getParentModule()->getCanvas()->findFigureRecursively("root");
    figure->translate(1,1);

    cOvalFigure *oval = check_and_cast<cOvalFigure*>(getParentModule()->getCanvas()->findFigureRecursively("oval1"));
    int w = (int)(simTime().dbl())%20;
    oval->setLineWidth(w);

    scheduleAt(simTime()+1, msg);
}

