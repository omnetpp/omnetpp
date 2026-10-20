#include <omnetpp.h>

using namespace omnetpp;

class ClippingAnimator : public cModule
{
  protected:
    virtual void refreshDisplay() const override;
};

Define_Module(ClippingAnimator);

class ClippingTicker : public cSimpleModule
{
  protected:
    virtual void initialize() override {scheduleAt(simTime(), new cMessage("tick"));}
    virtual void handleMessage(cMessage *msg) override {scheduleAt(simTime() + 0.05, msg);}
};

Define_Module(ClippingTicker);

void ClippingAnimator::refreshDisplay() const
{
    double t = simTime().dbl();
    cCanvas *canvas = getCanvas();

    // 13: rotating clipper, moving child
    cFigure *c13 = canvas->getFigure("c13");
    c13->setTransform(cFigure::Transform().rotate(t * 0.5, 160, 800));
    auto oval13 = check_and_cast<cOvalFigure *>(c13->getFigure("oval"));
    oval13->setBounds(cFigure::Rectangle(60 + 100 * std::sin(t), 760, 80, 80));

    // 14: clipper geometry changes
    auto c14 = check_and_cast<cRectangleFigure *>(canvas->getFigure("c14"));
    double w = 120 + 80 * std::sin(t);
    c14->setBounds(cFigure::Rectangle(430 - w / 2, 740, w, 120));

    // 15: clipping toggled
    auto c15 = check_and_cast<cAbstractShapeFigure *>(canvas->getFigure("c15"));
    c15->setClipChildren(((int)(t / 2)) % 2 == 0);

    // 16: zIndex of the clipper toggled; the effective Z-indices of its subtree follow it,
    // so the whole subtree must move above/below the green bar together
    cAbstractShapeFigure *c16 = check_and_cast<cAbstractShapeFigure *>(canvas->getFigure("c16"));
    c16->setClipChildren(((int)(t / 2)) % 2 == 0);
}
