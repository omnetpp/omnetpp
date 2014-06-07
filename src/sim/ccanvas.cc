//==========================================================================
//   CCANVAS.CC  -  part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "ccanvas.h"
#include "cproperty.h"
#include "cproperties.h"
#include "stringutil.h"

USING_NAMESPACE

const cFigure::Color cFigure::BLACK(0,0,0);
const cFigure::Color cFigure::WHITE(255,255,255);
const cFigure::Color cFigure::GREY(128,128,128);
const cFigure::Color cFigure::RED(255,0,0);
const cFigure::Color cFigure::GREEN(0,255,0);
const cFigure::Color cFigure::BLUE(0,0,255);
const cFigure::Color cFigure::YELLOW(255,255,0);
const cFigure::Color cFigure::CYAN(0,255,255);
const cFigure::Color cFigure::MAGENTA(255,0,255);


cFigure::Point cFigure::parsePoint(cProperty *property, const char *key, int index)
{
    double x = opp_atof(property->getValue(key, index));
    double y = opp_atof(property->getValue(key, index+1));
    return Point(x,y);
}

std::vector<cFigure::Point> cFigure::parsePoints(cProperty *property, const char *key)
{
    std::vector<Point> points;
    int n = property->getNumValues(key);
    if (n%2 != 0)
        throw cRuntimeError("number of coordinates must be even");
    for (int i = 0; i < n; i += 2)
        points.push_back(parsePoint(property, key, i));
    return points;
}

bool cFigure::parseBool(const char *s)
{
    if (!strcmp(s,"true")) return true;
    if (!strcmp(s,"false")) return false;
    throw cRuntimeError("invalid boolean value '%s'", s);
}

cFigure::Color cFigure::parseColor(const char *s)
{
    if (s[0] == '#') {
        int r, g, b;
        if (sscanf(s+1, "%2x%2x%2x", &r, &g, &b) != 3)
            throw cRuntimeError("wrong color syntax '%s', #rrggbb expected", s); //TODO
        return Color(r,g,b);
    }
    throw cRuntimeError("unrecognized color '%s'", s);
}

cFigure::LineStyle cFigure::parseLineStyle(const char *s)
{
    if (!strcmp(s,"solid")) return LINE_SOLID;
    if (!strcmp(s,"dotted")) return LINE_DOTTED;
    if (!strcmp(s,"dashed")) return LINE_DASHED;
    throw cRuntimeError("invalid line style '%s'", s);
}

cFigure::CapStyle cFigure::parseCapStyle(const char *s)
{
    if (!strcmp(s,"butt")) return CAP_BUTT;
    if (!strcmp(s,"square")) return CAP_SQUARE;
    if (!strcmp(s,"round")) return CAP_ROUND;
    throw cRuntimeError("invalid cap style '%s'", s);
}

cFigure::JoinStyle cFigure::parseJoinStyle(const char *s)
{
    if (!strcmp(s,"bevel")) return JOIN_BEVEL;
    if (!strcmp(s,"miter")) return JOIN_MITER;
    if (!strcmp(s,"round")) return JOIN_ROUND;
    throw cRuntimeError("invalid join style '%s'", s);
}

cFigure::ArrowHead cFigure::parseArrowHead(const char *s)
{
    if (!strcmp(s,"none")) return ARROW_NONE;
    if (!strcmp(s,"simple")) return ARROW_SIMPLE;
    if (!strcmp(s,"triangle")) return ARROW_TRIANGLE;
    if (!strcmp(s,"barbed")) return ARROW_BARBED;
    throw cRuntimeError("invalid arrowhead style '%s'", s);
}

cFigure::Anchor cFigure::parseAnchor(const char *s)
{
    if (!strcmp(s,"c")) return ANCHOR_CENTER;
    if (!strcmp(s,"center")) return ANCHOR_CENTER;
    if (!strcmp(s,"n")) return ANCHOR_N;
    if (!strcmp(s,"e")) return ANCHOR_E;
    if (!strcmp(s,"s")) return ANCHOR_S;
    if (!strcmp(s,"w")) return ANCHOR_W;
    if (!strcmp(s,"nw")) return ANCHOR_NW;
    if (!strcmp(s,"ne")) return ANCHOR_NE;
    if (!strcmp(s,"se")) return ANCHOR_SE;
    if (!strcmp(s,"sw")) return ANCHOR_SW;
    throw cRuntimeError("invalid anchor '%s'", s);
}

cFigure::Alignment cFigure::parseAlignment(const char *s)
{
    if (!strcmp(s,"left")) return ALIGN_LEFT;
    if (!strcmp(s,"right")) return ALIGN_RIGHT;
    if (!strcmp(s,"center")) return ALIGN_CENTER;
    throw cRuntimeError("invalid text alignment '%s'", s);
}

void cFigure::parse(cProperty *property)
{
    //TODO layer, etc.
}

void cAbstractLineFigure::parse(cProperty *property)
{
    cFigure::parse(property);
    const char *s;
    if ((s = property->getValue("linecolor")) != NULL)
        setLineColor(parseColor(s));
    if ((s = property->getValue("linestyle")) != NULL)
        setLineStyle(parseLineStyle(s));
    if ((s = property->getValue("linewidth")) != NULL)
        setLineWidth(opp_atol(s));
    if ((s = property->getValue("startarrowhead")) != NULL)
        setStartArrowHead(parseArrowHead(s));
    if ((s = property->getValue("endarrowhead")) != NULL)
        setEndArrowHead(parseArrowHead(s));
}

void cLineFigure::parse(cProperty *property)
{
    cAbstractLineFigure::parse(property);

    setStart(parsePoint(property, "coords", 0));
    setEnd(parsePoint(property, "coords", 2));
}

void cLineFigure::translate(double x, double y)
{
    start.x += x;
    start.y += y;
    end.x += x;
    end.y += y;
    changed();
}

void cPolylineFigure::parse(cProperty *property)
{
    cAbstractLineFigure::parse(property);

    const char *s;
    setPoints(parsePoints(property, "coords"));
    if ((s = property->getValue("smooth", 0)) != NULL)
        setSmooth(parseBool(s));
    if ((s = property->getValue("capstyle", 0)) != NULL)
        setCapStyle(parseCapStyle(s));
    if ((s = property->getValue("joinstyle", 0)) != NULL)
        setJoinStyle(parseJoinStyle(s));
}

void cPolylineFigure::translate(double x, double y)
{
    for (int i = 0; i < (int)points.size(); i++) {
        points[i].x += x;
        points[i].y += y;
    }
    changed();
}

void cAbstractShapeFigure::parse(cProperty *property)
{
    cFigure::parse(property);

    const char *s;
    if ((s = property->getValue("linecolor")) != NULL)
        setLineColor(parseColor(s));
    if ((s = property->getValue("fillcolor")) != NULL) {
        setFillColor(parseColor(s));
        setFilled(true);
    }
    if ((s = property->getValue("linestyle")) != NULL)
        setLineStyle(parseLineStyle(s));
    if ((s = property->getValue("linewidth")) != NULL)
        setLineWidth(opp_atol(s));
}

void cRectangleFigure::parse(cProperty *property)
{
    cAbstractShapeFigure::parse(property);

    setP1(parsePoint(property, "coords", 0));
    setP2(parsePoint(property, "coords", 2));
}

void cRectangleFigure::translate(double x, double y)
{
    p1.x += x;
    p1.y += y;
    p2.x += x;
    p2.y += y;
    changed();
}

void cOvalFigure::parse(cProperty *property)
{
    cAbstractShapeFigure::parse(property);

    setP1(parsePoint(property, "coords", 0));
    setP2(parsePoint(property, "coords", 2));
}

void cOvalFigure::translate(double x, double y)
{
    p1.x += x;
    p1.y += y;
    p2.x += x;
    p2.y += y;
    changed();
}

void cPolygonFigure::parse(cProperty *property)
{
    cAbstractShapeFigure::parse(property);

    const char *s;
    setPoints(parsePoints(property, "coords"));
    if ((s = property->getValue("smooth", 0)) != NULL)
        setSmooth(parseBool(s));
    if ((s = property->getValue("joinstyle", 0)) != NULL)
        setJoinStyle(parseJoinStyle(s));
}

void cPolygonFigure::translate(double x, double y)
{
    for (int i = 0; i < (int)points.size(); i++) {
        points[i].x += x;
        points[i].y += y;
    }
    changed();
}

void cTextFigure::parse(cProperty *property)
{
    cFigure::parse(property);

    const char *s;
    setPos(parsePoint(property, "coords", 0));
    setText(opp_nulltoempty(property->getValue("text")));
    if ((s = property->getValue("color")) != NULL)
        setColor(parseColor(s));
    //TODO font;
    if ((s = property->getValue("anchor")) != NULL)
        setAnchor(parseAnchor(s));
    if ((s = property->getValue("alignment")) != NULL)
        setAlignment(parseAlignment(s));
}

void cTextFigure::translate(double x, double y)
{
    pos.x += x;
    pos.y += y;
    changed();
}

void cImageFigure::parse(cProperty *property)
{
    cFigure::parse(property);

    const char *s;
    setPos(parsePoint(property, "coords", 0));
    setImageName(opp_nulltoempty(property->getValue("image")));
    if ((s = property->getValue("color", 0)) != NULL)
        setColor(parseColor(s));
    if ((s = property->getValue("color", 1)) != NULL) {
        int d = opp_atol(s);
        if (d<0 || d>100)
            throw cRuntimeError("image colorization percentage %d is out of range 0..100", d);
        setColorization(d);
    }
}

void cImageFigure::translate(double x, double y)
{
    pos.x += x;
    pos.y += y;
    changed();
}


//------

cCanvas::cCanvas()
{
}

cCanvas::~cCanvas()
{
    for (int i=0; i<figures.size(); i++)
        delete figures[i];
    for (int i=0; i<layers.size(); i++)
        delete layers[i];
}

void cCanvas::addFigure(cFigure *figure)
{
    if (figure->getCanvas())
        throw cRuntimeError("cCanvas::addFigure(): figure is already added to a canvas");
    figures.push_back(figure);
    figure->setCanvas(this);
}

void cCanvas::deleteFigure(cFigure *figure)
{
    if (figure->getCanvas() != this)
        throw cRuntimeError("cCanvas::deleteFigure(): figure hasn't been added to this canvas");
    std::vector<cFigure*>::iterator it = std::find(figures.begin(), figures.end(), figure);
    ASSERT(it!=figures.end());
    figures.erase(it);
    delete figure;
}

cFigure *cCanvas::getFigureByName(const char *name)
{
    for (int i=0; i<figures.size(); i++)
        if (!strcmp(name, figures[i]->getFullName()))
            return figures[i];
    return NULL;
}

void cCanvas::addLayer(cLayer *layer)
{
    if (layer->getCanvas())
        throw cRuntimeError("cCanvas::addLayer(): layer is already added to a canvas");
    layers.push_back(layer);
    layer->setCanvas(this);
}

void cCanvas::deleteLayer(cLayer *layer)
{
    if (layer->getCanvas() != this)
        throw cRuntimeError("cCanvas::deleteLayer(): layer hasn't been added to this canvas");
    for (int i=0; i<figures.size(); i++)
        if (figures[i]->getLayer() == layer)
            throw cRuntimeError("cCanvas::deleteLayer(): refusing to delete layer: still has figures");
    std::vector<cLayer*>::iterator it = std::find(layers.begin(), layers.end(), layer);
    ASSERT(it!=layers.end());
    layers.erase(it);
    delete layer;
}

cLayer *cCanvas::getLayerByName(const char *name)
{
    for (int i=0; i<layers.size(); i++)
        if (!strcmp(name, layers[i]->getFullName()))
            return layers[i];
    return NULL;
}

bool cCanvas::containsCanvasItems(cProperties *properties)
{
    for (int i=0; i<properties->getNumProperties(); i++)
    {
        cProperty *property = properties->get(i);
        if (property->isName("layer") || property->isName("figure"))
            return true;
    }
    return false;
}

void cCanvas::addItemsFrom(cProperties *properties)
{
    // parse @layers first, then @figures
    for (int i=0; i<properties->getNumProperties(); i++)
    {
        cProperty *property = properties->get(i);
        if (property->isName("layer"))
            parseLayer(property);
    }
    for (int i=0; i<properties->getNumProperties(); i++)
    {
        cProperty *property = properties->get(i);
        if (property->isName("figure"))
            parseFigure(property);
    }
}

void cCanvas::parseLayer(cProperty *property)
{
    const char *name = property->getIndex();
    if (getLayerByName(name) == NULL) {
        cLayer *layer = new cLayer();
        layer->setName(name);
        addLayer(layer);
    }
}

void cCanvas::parseFigure(cProperty *property)
{
    const char *name = property->getIndex();
    if (!name)
        throw cRuntimeError(property, "@figure is expected to have an index, e.g. @figure[foo62]");
    cFigure *figure = getFigureByName(name);
    if (!figure)
    {
        const char *type = property->getValue("type");
        if (!strcmp(type, "line"))
            figure = new cLineFigure();
        else if (!strcmp(type, "polyline"))
            figure = new cPolylineFigure();
        else if (!strcmp(type, "rectangle"))
            figure = new cRectangleFigure();
        else if (!strcmp(type, "oval"))
            figure = new cOvalFigure();
        else if (!strcmp(type, "polygon"))
            figure = new cPolygonFigure();
        else if (!strcmp(type, "text"))
            figure = new cTextFigure();
        else if (!strcmp(type, "image"))
            figure = new cImageFigure();
        else
            throw cRuntimeError(property, "Unknown figure type %s", type);
        figure->setName(name);
        addFigure(figure);
    }

    try {
        figure->parse(property);
    }
    catch (std::exception& e) {
        throw cRuntimeError(property, e.what());  //TODO add location info! cProperties doesn't know its owner!!!
    }
}
