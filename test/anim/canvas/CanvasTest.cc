#include <omnetpp.h>

class CanvasTest : public cModule
{
    virtual void initialize();
};

Define_Module(CanvasTest);

void CanvasTest::initialize()
{
    // initially, all test figures are at the same location; distribute 
    // them vertically (using translate()), and label them with their 
    // source @figure property

    double dy = 0;
    cCanvas *canvas = getCanvas();
    cProperties *properties = getProperties();
    for (int i=0; i<properties->getNumProperties(); i++)
    {
        cProperty *property = properties->get(i);
        if (property->isName("figure")) {
            const char *name = property->getIndex();
            cFigure *figure = canvas->getFigureByName(name);
            figure->translate(0.0, dy);
            cTextFigure *label = new cTextFigure();
            label->setText(property->info().c_str());
            label->setPos(cFigure::Point(210, dy));
            canvas->addFigure(label);
            dy += 20;
        }
    }
}
