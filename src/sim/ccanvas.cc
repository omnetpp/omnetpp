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

int cFigure::lastId = 0;

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
}

void cFigure::addChild(cFigure *figure)
{
    if (!figure)
        throw cRuntimeError(this, "addChild(): cannot insert NULL pointer");
    take(figure);
    children.push_back(figure);
}

void cFigure::addChild(cFigure *figure, int pos)
{
    if (!figure)
        throw cRuntimeError(this, "addChild(): cannot insert NULL pointer");
    if (pos < 0 || pos > (int)children.size())
        throw cRuntimeError(this,"addChild(): insert position %d out of bounds", pos);
    take(figure);
    children.insert(children.begin()+pos, figure);
    doStructuralChange();
}

cFigure *cFigure::removeChild(int pos)
{
    if (pos < 0 || pos >= (int)children.size())
        throw cRuntimeError(this,"removeChild(): index %d out of bounds", pos);
    cFigure *figure = children[pos];
    children.erase(children.begin()+pos);
    drop(figure);
    doStructuralChange();
    return figure;
}

cFigure *cFigure::removeChild(cFigure *figure)
{
    int pos = findChild(figure);
    if (pos == -1)
        throw cRuntimeError(this,"removeChild(): figure is not a child");
    return removeChild(pos);
}

int cFigure::findChild(const char *name)
{
    for (int i = 0; i < (int)children.size(); i++)
        if (children[i]->isName(name))
            return i;
    return -1;
}

int cFigure::findChild(cFigure *figure)
{
    for (int i = 0; i < (int)children.size(); i++)
        if (children[i] == figure)
            return i;
    return -1;
}

cFigure *cFigure::getChild(int pos)
{
    if (pos < 0 || pos >= (int)children.size())
        throw cRuntimeError(this,"getChild(): index %d out of bounds", pos);
    return children[pos];
}

cFigure *cFigure::getChild(const char *name)
{
    for (int i = 0; i < (int)children.size(); i++) {
        cFigure *figure = children[i];
        if (figure->isName(name))
            return figure;
    }
    return NULL;
}

cFigure *cFigure::getFigureByName(const char *name)
{
    if (!strcmp(name, getFullName()))
        return this;
    for (int i = 0; i < (int)children.size(); i++) {
        cFigure *figure = children[i]->getFigureByName(name);
        if (figure)
            return figure;
    }
    return NULL;
}

cCanvas *cFigure::getCanvas() const
{
    //TODO search up the owner chain
    return NULL;
}

void cFigure::doChange(int flags)
{
    localChange |= flags;
    for (cFigure *figure = getParent(); figure != NULL; figure = figure->getParent())
        figure->treeChange |= flags;
}

void cFigure::clearChangeFlags()
{
    if (treeChange)
        for (int i = 0; i < (int)children.size(); i++)
            children[i]->clearChangeFlags();
    localChange = treeChange = 0;
}

void cLayer::translate(double x, double y)
{
    loc.x += x;
    loc.y += y;
    doGeometryChange();
}

void cGroupFigure::translate(double x, double y)
{
    loc.x += x;
    loc.y += y;
    doGeometryChange();
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

    const char *s;
    setStart(parsePoint(property, "coords", 0));
    setEnd(parsePoint(property, "coords", 2));
    if ((s = property->getValue("capstyle", 0)) != NULL)
        setCapStyle(parseCapStyle(s));
}

void cLineFigure::translate(double x, double y)
{
    start.x += x;
    start.y += y;
    end.x += x;
    end.y += y;
    doGeometryChange();
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
    doGeometryChange();
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
    //TODO understand this too: (coords=x,y;size=w,h;anchor=nw)
}

void cRectangleFigure::translate(double x, double y)
{
    p1.x += x;
    p1.y += y;
    p2.x += x;
    p2.y += y;
    doGeometryChange();
}

void cOvalFigure::parse(cProperty *property)
{
    cAbstractShapeFigure::parse(property);

    setP1(parsePoint(property, "coords", 0));
    setP2(parsePoint(property, "coords", 2));
    //TODO understand this too: (coords=x,y;size=w,h;anchor=nw)
}

void cOvalFigure::translate(double x, double y)
{
    p1.x += x;
    p1.y += y;
    p2.x += x;
    p2.y += y;
    doGeometryChange();
}

void cArcFigure::parse(cProperty *property)
{
    cAbstractShapeFigure::parse(property);

    const char *s;
    setP1(parsePoint(property, "coords", 0));
    setP2(parsePoint(property, "coords", 2));
    if ((s = property->getValue("startangle")) != NULL)
        setStartAngle(opp_atof(s));
    if ((s = property->getValue("endangle")) != NULL)
        setEndAngle(opp_atof(s));
}

void cArcFigure::translate(double x, double y)
{
    p1.x += x;
    p1.y += y;
    p2.x += x;
    p2.y += y;
    doGeometryChange();
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
    doGeometryChange();
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
    doGeometryChange();
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
    doGeometryChange();
}

//------

void cCanvas::cLayerContainerFigure::translate(double x, double y)
{
    throw cRuntimeError(this, "translate(): illegal operation");
}

cFigure::Point cCanvas::cLayerContainerFigure::getLocation() const
{
    return Point(0,0);
}

void cCanvas::cLayerContainerFigure::setLocation(const Point& loc)
{
    throw cRuntimeError(this, "setLocation(): illegal operation");
}

void cCanvas::cLayerContainerFigure::addChild(cFigure *figure)
{
    if (!dynamic_cast<cLayer*>(figure))
        throw cRuntimeError(this, "addChild(): %s cannot be added, only cLayers", figure->getClassName());
    cFigure::addChild(figure);
}

void cCanvas::cLayerContainerFigure::addChild(cFigure *figure, int pos)
{
    if (!dynamic_cast<cLayer*>(figure))
        throw cRuntimeError(this, "addChild(): %s cannot be added, only cLayers", figure->getClassName());
    cFigure::addChild(figure, pos);
}

//------

cCanvas::cCanvas()
{
    rootFigure = new cLayerContainerFigure();
    rootFigure->addChild(new cLayer("default"));
    take(rootFigure);
}

cCanvas::~cCanvas()
{
    dropAndDelete(rootFigure);
}

bool cCanvas::containsCanvasItems(cProperties *properties)
{
    for (int i = 0; i < properties->getNumProperties(); i++)
    {
        cProperty *property = properties->get(i);
        if (property->isName("layer") || property->isName("figure"))
            return true;
    }
    return false;
}

void cCanvas::addLayersAndFiguresFrom(cProperties *properties)
{
    // parse @layers first, then @figures
    for (int i = 0; i < properties->getNumProperties(); i++)
    {
        cProperty *property = properties->get(i);
        if (property->isName("layer"))
            parseLayer(property);
    }
    for (int i = 0; i < properties->getNumProperties(); i++)
    {
        cProperty *property = properties->get(i);
        if (property->isName("figure"))
            parseFigure(property);
    }
}

void cCanvas::parseLayer(cProperty *property)
{
    const char *name = property->getIndex();
    if (getToplevelLayerByName(name) == NULL) {
        cLayer *layer = new cLayer();
        layer->setName(name); //TODO set description too
        addToplevelLayer(layer);
    }
}

void cCanvas::parseFigure(cProperty *property)
{
    try {
        const char *name = property->getIndex();
        if (!name)
            throw cRuntimeError("@figure property is expected to have an index which will become the figure name, e.g. @figure[foo]");
        cFigure *figure = getFigureByName(name);
        if (!figure)
        {
            const char *type = property->getValue("type");
            figure = createFigure(type);
            figure->setName(name);

            cFigure *parent = parseParentFigure(property);
            parent->addChild(figure);
        }

        figure->parse(property);
    }
    catch (std::exception& e) {
        throw cRuntimeError(this, "Error creating figure from NED property @%s: %s", property->getFullName(), e.what());
    }
}

cFigure *cCanvas::createFigure(const char *type)
{
    cFigure *figure;
    if (!strcmp(type, "group"))
        figure = new cGroupFigure();
    else if (!strcmp(type, "line"))
        figure = new cLineFigure();
    else if (!strcmp(type, "polyline"))
        figure = new cPolylineFigure();
    else if (!strcmp(type, "rectangle"))
        figure = new cRectangleFigure();
    else if (!strcmp(type, "oval"))
        figure = new cOvalFigure();
    else if (!strcmp(type, "arc"))
        figure = new cArcFigure();
    else if (!strcmp(type, "polygon"))
        figure = new cPolygonFigure();
    else if (!strcmp(type, "text"))
        figure = new cTextFigure();
    else if (!strcmp(type, "image"))
        figure = new cImageFigure();
    else {
        // find registered class named "<type>Figure" or "c<type>Figure"
        std::string className = std::string("c") + type + "Figure";
        cObjectFactory *factory = cObjectFactory::find(className.c_str() + 1); // type without leading "c"
        if (!factory)
            factory = cObjectFactory::find(className.c_str()); // try with leading "c"
        if (!factory)
            throw cRuntimeError("No figure class registered with name '%s' or '%s'", className.c_str()+1, className.c_str());
        cObject *obj = factory->createOne();
        figure = dynamic_cast<cFigure*>(obj);
        if (!figure)
            throw cRuntimeError("Wrong figure class: cannot cast %s to cFigure", obj->getClassName());
    }
    return figure;
}

cFigure *cCanvas::parseParentFigure(cProperty *property)
{
    cFigure *parent;
    const char *s;
    if ((s = property->getValue("parent")) != NULL) {
        parent = getFigureByName(s);
        if (!parent)
            throw cRuntimeError("Wrong parent: no figure named '%s'", s);
    }
    else if ((s = property->getValue("layer")) != NULL) {
        parent = getToplevelLayerByName(s);
        if (!parent)
            throw cRuntimeError("No such layer: '%s'", s);
    }
    else {
        parent = getDefaultLayer();
    }
    return parent;
}

cFigure *cCanvas::getFigureByName(const char *name)
{
    return rootFigure->getFigureByName(name);
}

void cCanvas::addToplevelLayer(cLayer *layer)
{
    rootFigure->addChild(layer);
}

cLayer *cCanvas::removeToplevelLayer(cLayer *layer)
{
    if (layer->getParent() != rootFigure)
        throw cRuntimeError("cCanvas::removeToplevelLayer(): layer is not a toplevel layer in this canvas");
    return (cLayer *)rootFigure->removeChild(layer);
}

cLayer *cCanvas::getToplevelLayerByName(const char *name)
{
    return (cLayer *)rootFigure->getChild(name);
}

cLayer *cCanvas::getDefaultLayer() const
{
    cLayer *layer = (cLayer *)rootFigure->getChild("default");
    if (!layer)
        throw cRuntimeError("cCanvas::getDefaultLayer(): no toplevel layer named 'default' in this canvas");
    return layer;
}
