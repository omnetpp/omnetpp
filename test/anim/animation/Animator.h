#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <omnetpp.h>

using namespace omnetpp;

class Animator : public cSimpleModule {
    cOvalFigure *oval1, *oval2, *oval3;

public:
    virtual void initialize() override;
    virtual void handleMessage(cMessage* msg) override;
    virtual void refreshDisplay() const override;
};

#endif // ANIMATOR_H
