#include <omnetpp.h>

using namespace omnetpp;

class AssociateObjects : public cModule
{
    virtual void initialize() override;
};

Define_Module(AssociateObjects);

void AssociateObjects::initialize()
{
    cCanvas *canvas = getCanvas();


    cModule *a = getModuleByPath(".a");
    cModule *b = getModuleByPath(".b");
    cModule *c = getModuleByPath(".c");

    canvas->getFigureByPath("a1")->setAssociatedObject(a);
    canvas->getFigureByPath("b1.b2")->setAssociatedObject(b);
    canvas->getFigureByPath("c1.c2.c3")->setAssociatedObject(c);

    canvas->getFigureByPath("w1")->setAssociatedObject(a);
    canvas->getFigureByPath("w1.w2")->setAssociatedObject(b);
    canvas->getFigureByPath("w1.w2.w3")->setAssociatedObject(c);


    cFigure *x1 = canvas->getFigureByPath("x1");
    cFigure *x2 = canvas->getFigureByPath("x2");
    
    x1->setAssociatedObject(x2);
    x2->setAssociatedObject(x1);
}
