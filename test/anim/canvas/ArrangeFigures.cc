#include <omnetpp.h>

class ArrangeFigures : public cModule
{
    virtual void initialize();
};

Define_Module(ArrangeFigures);

void ArrangeFigures::initialize()
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
            cFigure *figure = canvas->getFigure(name);
            ASSERT(figure!=NULL);
            figure->translate(0.0, dy);
            cTextFigure *label = new cTextFigure();
            label->setText(property->info().c_str());
            label->setLocation(cFigure::Point(210, dy));
            canvas->addFigure(label);
            dy += 20;
        }
    }
}
