#include <omnetpp.h>

using namespace omnetpp;

class ArrangeFigures : public cModule
{
    virtual void initialize() override;
    virtual void addBBox(cFigure *figure);
    virtual void addAnchor(cFigure *figure);
    virtual void addReferenceLine(cFigure *figure);
    virtual void addLabel(const char *s, double x, double y, cFigure::Anchor anchor);
};

Define_Module(ArrangeFigures);

inline const char *nullToEmpty(const char *s) {return !s ? "" : s;}

void ArrangeFigures::initialize()
{
    // initially, all test figures are at the same location; distribute
    // them vertically (using translate()), and label them with their
    // source @figure property

    // process @arrange property
    double startPosX = 0;
    double startPosY = 0;
    int lineSpacing = 20;
    int columnWidth = 220;
    bool drawBBox = true;
    bool drawAnchor = true;
    bool drawReferenceLine = true;

    // process @arrange
    cProperties *properties = getProperties();
    omnetpp::cProperty* arrangeProperty = properties->get("arrange");
    if (arrangeProperty) {
        if (const char *s = arrangeProperty->getValue("startPos", 0))
            startPosX = atoi(s);
        if (const char *s = arrangeProperty->getValue("startPos", 1))
            startPosY = atoi(s);
        if (const char *s = arrangeProperty->getValue("lineSpacing"))
            lineSpacing = atoi(s);
        if (const char *s = arrangeProperty->getValue("columnWidth"))
            columnWidth = atoi(s);
        if (const char *s = arrangeProperty->getValue("drawBBox"))
            drawBBox = strcmp(s, "true")==0;
        if (const char *s = arrangeProperty->getValue("drawAnchor"))
            drawAnchor = strcmp(s, "true")==0;
        if (const char *s = arrangeProperty->getValue("addReferenceLine"))
            drawReferenceLine = strcmp(s, "true")==0;
    }

    // add column labels
    std::vector<const char*> addColumnPropertyIndices = properties->getIndicesFor("addColumn");
    for (int i = 0; i < (int)addColumnPropertyIndices.size(); i++) {
        const char *index = addColumnPropertyIndices[i];
        cProperty *addColumnProperty = properties->get("addColumn", index);
        addLabel(addColumnProperty->str().c_str(), startPosX + (i+1)*columnWidth, startPosY-50+(i%3)*15, cFigure::ANCHOR_SW);
    }

    // distribute the figures and add labels for them
    cCanvas *canvas = getCanvas();
    double yoffset = startPosY;
    for (int i = 0; i < properties->getNumProperties(); i++) {
        cProperty *figureProperty = properties->get(i);
        if (figureProperty->isName("figure")) {

            // move and decorate corresponding figure
            double xoffset = startPosX;
            const char *name = figureProperty->getIndex();
            cFigure *figure = canvas->getFigure(name);
            ASSERT(figure != nullptr);
            figure->move(xoffset, yoffset);

            if (drawBBox)
                addBBox(figure);
            if (drawAnchor)
                addAnchor(figure);
            if (drawReferenceLine)
                addReferenceLine(figure);
            xoffset += columnWidth;

            // iterate
            for (int j = 0; j < (int)addColumnPropertyIndices.size(); j++) {
                // merge @addColumns's contents into the @figure
                const char *index = addColumnPropertyIndices[j];
                cProperty *addColumnProperty = properties->get("addColumn", index);
                cProperty *figureColumnProperty = figureProperty->dup();
                figureColumnProperty->updateWith(addColumnProperty);

                // give it a new name or parseFigure() will update the existing one
                std::string newIndex = std::string(figureProperty->getIndex()) + "-" + nullToEmpty(addColumnProperty->getIndex());
                figureColumnProperty->setIndex(newIndex.c_str());

                // instantiate figure from the updated property
                cFigure *copy = getCanvas()->parseFigure(figureColumnProperty);
                delete figureColumnProperty;
                copy->move(xoffset, yoffset);
                xoffset += columnWidth;

                if (drawBBox)
                    addBBox(copy);
                if (drawAnchor)
                    addAnchor(copy);
                if (drawReferenceLine)
                    addReferenceLine(copy);
            }

            // generate label
            addLabel(figureProperty->str().c_str(), xoffset, yoffset, cFigure::ANCHOR_NW);
            yoffset += lineSpacing;
        }
    }
}

void ArrangeFigures::addBBox(cFigure *figure)
{
    cFigure::Rectangle bbox;
    if (auto f = dynamic_cast<cAbstractTextFigure*>(figure))
        bbox = f->getBounds();
    else if (auto f = dynamic_cast<cAbstractImageFigure*>(figure))
        bbox = f->getBounds();
    else if (auto f = dynamic_cast<cArcFigure*>(figure))
        bbox = f->getBounds();
    else if (auto f = dynamic_cast<cOvalFigure*>(figure))
        bbox = f->getBounds();
    else if (auto f = dynamic_cast<cRingFigure*>(figure))
        bbox = f->getBounds();
    else if (auto f = dynamic_cast<cPieSliceFigure*>(figure))
        bbox = f->getBounds();
    if (bbox.width != 0 || bbox.height != 0) {
        cRectangleFigure *bboxFigure = new cRectangleFigure("bbox");
        bboxFigure->setBounds(bbox);
        bboxFigure->setLineOpacity(0.4);
        figure->getParentFigure()->addFigure(bboxFigure);
    }
}

void ArrangeFigures::addAnchor(cFigure *figure)
{
    cFigure::Point position(-1, -1);
    if (auto f = dynamic_cast<cAbstractTextFigure*>(figure))
        position = f->getPosition();
    else if (auto f = dynamic_cast<cAbstractImageFigure*>(figure))
        position = f->getPosition();
    if (position.x >= 0 || position.y >= 0) {
        cIconFigure *anchorFigure = new cIconFigure("anchor");
        anchorFigure->setImageName("old/bwsink");
        anchorFigure->setSize(16, 16);
        anchorFigure->setPosition(position);
        anchorFigure->setAnchor(cFigure::ANCHOR_CENTER);
        figure->getParentFigure()->addFigure(anchorFigure);
    }
}

void ArrangeFigures::addReferenceLine(cFigure *figure)
{
    if (auto polyline = dynamic_cast<cPolylineFigure*>(figure)) {
        if (polyline->getSmooth()) {
            cPolylineFigure *reference = new cPolylineFigure("ref");
            reference->setLineOpacity(0.4);
            reference->insertBefore(polyline);
            reference->setPoints(polyline->getPoints());
        }
    }
    if (auto polygon = dynamic_cast<cPolygonFigure*>(figure)) {
        if (polygon->getSmooth()) {
            cPolygonFigure *reference = new cPolygonFigure("ref");
            reference->setLineOpacity(0.4);
            reference->insertBefore(polygon);
            reference->setPoints(polygon->getPoints());
        }
    }
}

void ArrangeFigures::addLabel(const char *s, double x, double y, cFigure::Anchor anchor)
{
    cLabelFigure *label = new cLabelFigure();
    label->setText(s);
    label->setPosition(cFigure::Point(x, y));
    label->setAnchor(anchor);
    label->setColor(cFigure::GREY);
    getCanvas()->addFigure(label);
}
