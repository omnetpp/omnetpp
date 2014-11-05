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

#include <map>
#include <algorithm>
#include "ccanvas.h"
#include "cproperty.h"
#include "cproperties.h"
#include "cstringtokenizer.h"
#include "stringutil.h"
#include "opp_ctype.h"

NAMESPACE_BEGIN

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


static std::ostream& operator<<(std::ostream& os, const cFigure::Point& p) {
    return os << "(" << p.x << ", " << p.y << ")";
}

static std::ostream& operator<<(std::ostream& os, const cFigure::Rectangle& r) {
    return os << "(" << r.x << ", " << r.y << ", w="<< r.width << ", h=" << r.height << ")";
}

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

void cFigure::parseBounds(cProperty *property, Point& p1, Point& p2)
{
    int numCoords = property->getNumValues("coords");
    if (numCoords == 4)
    {
        p1 = parsePoint(property, "coords", 0);
        p2 = parsePoint(property, "coords", 2);
    }
    else if (numCoords == 2)
    {
        Point p = parsePoint(property, "coords", 0);
        Point size = parsePoint(property, "size", 0);
        const char *anchorStr = property->getValue("anchor");
        Anchor anchor = opp_isblank(anchorStr) ? cFigure::ANCHOR_NW : parseAnchor(anchorStr);
        switch (anchor) {
            case cFigure::ANCHOR_CENTER:
                p1.x = p.x - size.x/2; p1.y = p.y - size.y/2;
                p2.x = p.x + size.x/2; p2.y = p.y + size.y/2;
                break;
            case cFigure::ANCHOR_N:
                p1.x = p.x - size.x/2; p1.y = p.y;
                p2.x = p.x + size.x/2; p2.y = p.y + size.y;
                break;
            case cFigure::ANCHOR_E:
                p1.x = p.x; p1.y = p.y - size.y/2;
                p2.x = p.x + size.x; p2.y = p.y + size.y/2;
                break;
            case cFigure::ANCHOR_S:
                p1.x = p.x - size.x/2; p1.y = p.y - size.y;
                p2.x = p.x + size.x/2; p2.y = p.y;
                break;
            case cFigure::ANCHOR_W:
                p1.x = p.x - size.x; p1.y = p.y - size.y/2;
                p2.x = p.x; p2.y = p.y + size.y/2;
                break;
            case cFigure::ANCHOR_NW:
                p1.x = p.x; p1.y = p.y;
                p2.x = p.x + size.x; p2.y = p.y + size.y;
                break;
            case cFigure::ANCHOR_NE:
                p1.x = p.x - size.x; p1.y = p.y;
                p2.x = p.x; p2.y = p.y + size.y;
                break;
            case cFigure::ANCHOR_SE:
                p1.x = p.x - size.x; p1.y = p.y - size.y;
                p2.x = p.x; p2.y = p.y;
                break;
            case cFigure::ANCHOR_SW:
                p1.x = p.x; p1.y = p.y - size.y;
                p2.x = p.x + size.x; p2.y = p.y;
                break;
            default: throw cRuntimeError("Unexpected anchor %d", anchor);
        }
    }
    else {
        throw cRuntimeError("Wrong number of coordinates: 2 or 4 expected");
    }
}

cFigure::Rectangle cFigure::parseBounds(cProperty *property)
{
    Point p1, p2;
    parseBounds(property, p1, p2);
    return Rectangle(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
}

cFigure::Font cFigure::parseFont(cProperty *property, const char *key)
{
    const char *typeface = property->getValue(key, 0);
    int size = opp_atol(opp_nulltoempty(property->getValue(key, 1)));
    if (size <= 0)
        size = 0;
    int flags = 0;
    cStringTokenizer tokenizer(property->getValue(key, 2));
    while (tokenizer.hasMoreTokens()) {
        const char *token = tokenizer.nextToken();
        if (!strcmp(token, "normal")) /*no-op*/;
        else if (!strcmp(token, "bold")) flags |= FONT_BOLD;
        else if (!strcmp(token, "italic")) flags |= FONT_ITALIC;
        else if (!strcmp(token, "underline")) flags |= FONT_UNDERLINE;
        else throw cRuntimeError("wrong font style '%s', token");
    }
    return Font(opp_nulltoempty(typeface), size, flags);
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
            throw cRuntimeError("wrong color syntax '%s', #rrggbb expected", s); //TODO better error checking
        return Color(r,g,b);
    }
    else {
        return Color::byName(s);
    }
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

cFigure::~cFigure()
{
    for (int i = 0; i < (int)children.size(); i++)
        dropAndDelete(children[i]);
}

void cFigure::parse(cProperty *property)
{
    const char *s;
    if ((s = property->getValue("visible")) != NULL)
        setVisible(parseBool(s));
    int numTags = property->getNumValues("tags");
    if (numTags > 0) {
        std::string tags;
        for (int i = 0; i < numTags; i++)
            tags += std::string(" ") + property->getValue("tags", i);
        setTags(tags.c_str());
    }
}

cFigure *cFigure::dupTree()
{
    cFigure *result = (cFigure *) dup();
    for (int i = 0; i < (int)children.size(); i++)
        result->addFigure(children[i]->dupTree());
    return result;
}

void cFigure::copy(const cFigure& other)
{
    setVisible(other.isVisible());
    setTags(other.getTags());
}

cFigure& cFigure::operator=(const cFigure& other)
{
    if (this == &other) return *this;
    cOwnedObject::operator=(other);
    copy(other);
    return *this;
}

void cFigure::forEachChild(cVisitor *v)
{
    for (int i = 0; i < (int)children.size(); i++)
        v->visit(children[i]);
}

std::string cFigure::info() const
{
    return "";
}

void cFigure::addFigure(cFigure *figure)
{
    if (!figure)
        throw cRuntimeError(this, "addFigure(): cannot insert NULL pointer");
    take(figure);
    children.push_back(figure);
    refreshTagBits();
    doStructuralChange();
}

void cFigure::addFigure(cFigure *figure, int pos)
{
    if (!figure)
        throw cRuntimeError(this, "addFigure(): cannot insert NULL pointer");
    if (pos < 0 || pos > (int)children.size())
        throw cRuntimeError(this,"addFigure(): insert position %d out of bounds", pos);
    take(figure);
    children.insert(children.begin()+pos, figure);
    refreshTagBits();
    doStructuralChange();
}

void cFigure::addFigureAbove(cFigure *figure, cFigure *referenceFigure)
{
    int refPos = findFigure(referenceFigure);
    if (refPos == -1)
        throw cRuntimeError(this, "addFigureAbove(): reference figure is not a child");
    addFigure(figure, refPos+1);
}

void cFigure::addFigureBelow(cFigure *figure, cFigure *referenceFigure)
{
    int refPos = findFigure(referenceFigure);
    if (refPos == -1)
        throw cRuntimeError(this, "addFigureBelow(): reference figure is not a child");
    addFigure(figure, refPos);
}

inline double getZ(cFigure *figure, const std::map<cFigure*,double>& orderMap)
{
    const double defaultZ = 0.0;
    std::map<cFigure*,double>::const_iterator it = orderMap.find(figure);
    return (it==orderMap.end()) ? defaultZ : it->second;
}

struct LessZ {
    std::map<cFigure*,double>& orderMap;
    LessZ(std::map<cFigure*,double>& orderMap) : orderMap(orderMap) {}
    bool operator()(cFigure *figure1, cFigure *figure2) { return getZ(figure1, orderMap) < getZ(figure2, orderMap); }
};

void cFigure::insertChild(cFigure *figure, std::map<cFigure*,double>& orderMap)
{
    // Assuming that existing children are z-ordered, insert a new child at the appropriate place.
    // Z-order comes from the orderMap; if a figure is not in the map, its Z is assumed to be zero.
    take(figure);
    std::vector<cFigure*>::iterator it = std::upper_bound(children.begin(), children.end(), figure, LessZ(orderMap));
    children.insert(it, figure);
    refreshTagBits();
    doStructuralChange();
}

cFigure *cFigure::removeFigure(int pos)
{
    if (pos < 0 || pos >= (int)children.size())
        throw cRuntimeError(this,"removeFigure(): index %d out of bounds", pos);
    cFigure *figure = children[pos];
    children.erase(children.begin()+pos);
    drop(figure);
    doStructuralChange();
    return figure;
}

cFigure *cFigure::removeFigure(cFigure *figure)
{
    int pos = findFigure(figure);
    if (pos == -1)
        throw cRuntimeError(this,"removeFigure(): figure is not a child");
    return removeFigure(pos);
}

int cFigure::findFigure(const char *name)
{
    for (int i = 0; i < (int)children.size(); i++)
        if (children[i]->isName(name))
            return i;
    return -1;
}

int cFigure::findFigure(cFigure *figure)
{
    for (int i = 0; i < (int)children.size(); i++)
        if (children[i] == figure)
            return i;
    return -1;
}

cFigure *cFigure::getFigure(int pos)
{
    if (pos < 0 || pos >= (int)children.size())
        throw cRuntimeError(this,"getFigure(): index %d out of bounds", pos);
    return children[pos];
}

cFigure *cFigure::getFigure(const char *name)
{
    for (int i = 0; i < (int)children.size(); i++) {
        cFigure *figure = children[i];
        if (figure->isName(name))
            return figure;
    }
    return NULL;
}

cFigure *cFigure::findFigureRecursively(const char *name)
{
    if (!strcmp(name, getFullName()))
        return this;
    for (int i = 0; i < (int)children.size(); i++) {
        cFigure *figure = children[i]->findFigureRecursively(name);
        if (figure)
            return figure;
    }
    return NULL;
}

cCanvas *cFigure::getCanvas()
{
    return dynamic_cast<cCanvas*>(getRootFigure()->getOwner());
}

static char *nextToken(char *&rest)
{
    if (!rest) return NULL;
    char *token = rest;
    rest = strchr(rest, '.');
    if (rest) *rest++ = '\0';
    return token;
}

cFigure *cFigure::getRootFigure()
{
    cFigure *figure = this;
    cFigure *parent;
    while ((parent = figure->getParentFigure()) != NULL)
        figure = parent;
    return figure;
}

cFigure *cFigure::getFigureByPath(const char *path)
{
    if (!path || !path[0])
        return NULL;

    // determine starting point
    bool isRelative = (path[0] == '.' || path[0] == '^');
    cFigure *rootFigure = isRelative ? NULL : getRootFigure();  // only needed when processing absolute paths
    cFigure *figure = isRelative ? this : rootFigure;
    if (path[0] == '.')
        path++; // skip initial dot

    // match components of the path
    opp_string pathbuf(path);
    char *rest = pathbuf.buffer();
    char *token = nextToken(rest);
    while (token && figure)
    {
        if (!token[0])
            ; /*skip empty path component*/
        else if (token[0] == '^' && token[1] == '\0')
            figure = figure->getParentFigure();
        else
            figure = figure->getFigure(token);
        token = nextToken(rest);
    }
    return figure;  // NULL if not found
}

void cFigure::refreshTagBits()
{
    cCanvas *canvas = getCanvas();
    if (canvas) {
        tagBits = canvas->parseTags(getTags());
        for (int i = 0; i < (int)children.size(); i++)
            children[i]->refreshTagBits();
    }
}

void cFigure::doChange(int flags)
{
    if ((localChange & flags) == 0) {  // not yet set
        localChange |= flags;
        for (cFigure *figure = getParentFigure(); figure != NULL; figure = figure->getParentFigure())
            figure->treeChange |= flags;
    }
}

void cFigure::clearChangeFlags()
{
    if (treeChange)
        for (int i = 0; i < (int)children.size(); i++)
            children[i]->clearChangeFlags();
    localChange = treeChange = 0;
}

void cFigure::raiseAbove(cFigure *figure)
{
    cFigure *parent = getParentFigure();
    if (!parent)
        throw cRuntimeError(this, "raiseAbove(): figure has no parent figure");
    int myPos = parent->findFigure(this);
    int refPos = parent->findFigure(figure);
    if (refPos == -1)
        throw cRuntimeError(this, "raiseAbove(): reference figure must have the same parent");
    if (myPos < refPos) {
        parent->children.erase(parent->children.begin() + myPos); // note: reference figure will be shifted down
        parent->children.insert(parent->children.begin() + refPos, this);
        doStructuralChange();
    }
}

void cFigure::lowerBelow(cFigure *figure)
{
    cFigure *parent = getParentFigure();
    if (!parent)
        throw cRuntimeError(this, "lowerBelow(): figure has no parent figure");
    int myPos = parent->findFigure(this);
    int refPos = parent->findFigure(figure);
    if (refPos == -1)
        throw cRuntimeError(this, "lowerBelow(): reference figure must have the same parent");
    if (myPos > refPos) {
        parent->children.erase(parent->children.begin() + myPos);
        parent->children.insert(parent->children.begin() + refPos, this);
        doStructuralChange();
    }
}

void cFigure::raiseToTop()
{
    cFigure *parent = getParentFigure();
    if (!parent)
        return; //done
    int myPos = parent->findFigure(this);
    if (myPos != (int)parent->children.size()-1) {
        parent->children.erase(parent->children.begin() + myPos);
        parent->children.push_back(this);
        doStructuralChange();
    }
}

void cFigure::lowerToBottom()
{
    cFigure *parent = getParentFigure();
    if (!parent)
        return; //done
    int myPos = parent->findFigure(this);
    if (myPos != 0) {
        parent->children.erase(parent->children.begin() + myPos);
        parent->children.insert(parent->children.begin(), this);
        doStructuralChange();
    }
}

//----

void cGroupFigure::copy(const cGroupFigure& other)
{
    setLocation(other.getLocation());
}

cGroupFigure& cGroupFigure::operator=(const cGroupFigure& other)
{
    if (this == &other) return *this;
    cFigure::operator=(other);
    copy(other);
    return *this;
}

std::string cGroupFigure::info() const
{
    return "";
}

void cGroupFigure::translate(double x, double y)
{
    location.x += x;
    location.y += y;
    doGeometryChange();
}

//----

void cAbstractLineFigure::copy(const cAbstractLineFigure& other)
{
    setLineColor(other.getLineColor());
    setLineStyle(other.getLineStyle());
    setLineWidth(other.getLineWidth());
    setCapStyle(other.getCapStyle());
    setStartArrowHead(other.getStartArrowHead());
    setEndArrowHead(other.getEndArrowHead());
}

cAbstractLineFigure& cAbstractLineFigure::operator=(const cAbstractLineFigure& other)
{
    if (this == &other) return *this;
    cFigure::operator=(other);
    copy(other);
    return *this;
}

std::string cAbstractLineFigure::info() const
{
    return ""; //TODO
}

void cAbstractLineFigure::parse(cProperty *property)
{
    cFigure::parse(property);
    const char *s;
    if ((s = property->getValue("lineColor")) != NULL)
        setLineColor(parseColor(s));
    if ((s = property->getValue("lineStyle")) != NULL)
        setLineStyle(parseLineStyle(s));
    if ((s = property->getValue("lineWidth")) != NULL)
        setLineWidth(opp_atof(s));
    if ((s = property->getValue("capStyle", 0)) != NULL)
        setCapStyle(parseCapStyle(s));
    if ((s = property->getValue("startArrowhead")) != NULL)
        setStartArrowHead(parseArrowHead(s));
    if ((s = property->getValue("endArrowhead")) != NULL)
        setEndArrowHead(parseArrowHead(s));
}

//----

void cLineFigure::copy(const cLineFigure& other)
{
    setStart(other.getStart());
    setEnd(other.getEnd());
}

cLineFigure& cLineFigure::operator=(const cLineFigure& other)
{
    if (this == &other) return *this;
    cAbstractLineFigure::operator=(other);
    copy(other);
    return *this;
}

std::string cLineFigure::info() const
{
    std::stringstream os;
    os << getStart() << " to " << getEnd();
    return os.str();
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
    doGeometryChange();
}

//----

void cArcFigure::copy(const cArcFigure& other)
{
    setBounds(other.getBounds());
    setStartAngle(other.getStartAngle());
    setEndAngle(other.getEndAngle());
}

cArcFigure& cArcFigure::operator=(const cArcFigure& other)
{
    if (this == &other) return *this;
    cAbstractLineFigure::operator=(other);
    copy(other);
    return *this;
}

std::string cArcFigure::info() const
{
    return ""; //TODO
}

void cArcFigure::parse(cProperty *property)
{
    cAbstractLineFigure::parse(property);

    setBounds(parseBounds(property));

    const char *s;
    if ((s = property->getValue("startAngle")) != NULL)
        setStartAngle(opp_atof(s));
    if ((s = property->getValue("endAngle")) != NULL)
        setEndAngle(opp_atof(s));
}

void cArcFigure::translate(double x, double y)
{
    bounds.x += x;
    bounds.y += y;
    doGeometryChange();
}

//----

void cPolylineFigure::copy(const cPolylineFigure& other)
{
    setPoints(other.getPoints());
    setSmooth(other.getSmooth());
    setJoinStyle(other.getJoinStyle());
}

cPolylineFigure& cPolylineFigure::operator=(const cPolylineFigure& other)
{
    if (this == &other) return *this;
    cAbstractLineFigure::operator=(other);
    copy(other);
    return *this;
}

std::string cPolylineFigure::info() const
{
    std::stringstream os;
    for (int i = 0; i < (int)points.size(); i++)
        os << points[i] << " ";
    return os.str();
}

void cPolylineFigure::parse(cProperty *property)
{
    cAbstractLineFigure::parse(property);

    const char *s;
    setPoints(parsePoints(property, "coords"));
    if ((s = property->getValue("smooth", 0)) != NULL)
        setSmooth(parseBool(s));
    if ((s = property->getValue("joinStyle", 0)) != NULL)
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

//----

void cAbstractShapeFigure::copy(const cAbstractShapeFigure& other)
{
    setOutlined(other.isOutlined());
    setFilled(other.isFilled());
    setLineColor(other.getLineColor());
    setFillColor(other.getFillColor());
    setLineStyle(other.getLineStyle());
    setLineWidth(other.getLineWidth());
}

cAbstractShapeFigure& cAbstractShapeFigure::operator=(const cAbstractShapeFigure& other)
{
    if (this == &other) return *this;
    cFigure::operator=(other);
    copy(other);
    return *this;
}

std::string cAbstractShapeFigure::info() const
{
    return ""; //TODO
}

void cAbstractShapeFigure::parse(cProperty *property)
{
    cFigure::parse(property);

    const char *s;
    if ((s = property->getValue("lineColor")) != NULL)
        setLineColor(parseColor(s));
    if ((s = property->getValue("fillColor")) != NULL) {
        setFillColor(parseColor(s));
        setFilled(true);
    }
    if ((s = property->getValue("lineStyle")) != NULL)
        setLineStyle(parseLineStyle(s));
    if ((s = property->getValue("lineWidth")) != NULL)
        setLineWidth(opp_atof(s));
}

//----

void cRectangleFigure::copy(const cRectangleFigure& other)
{
    setBounds(other.getBounds());
}

cRectangleFigure& cRectangleFigure::operator=(const cRectangleFigure& other)
{
    if (this == &other) return *this;
    cAbstractShapeFigure::operator=(other);
    copy(other);
    return *this;
}

std::string cRectangleFigure::info() const
{
    std::stringstream os;
    os << getBounds();
    return os.str();
}

void cRectangleFigure::parse(cProperty *property)
{
    cAbstractShapeFigure::parse(property);

    setBounds(parseBounds(property));
}

void cRectangleFigure::translate(double x, double y)
{
    bounds.x += x;
    bounds.y += y;
    doGeometryChange();
}

//----

void cOvalFigure::copy(const cOvalFigure& other)
{
    setBounds(other.getBounds());
}

cOvalFigure& cOvalFigure::operator=(const cOvalFigure& other)
{
    if (this == &other) return *this;
    cAbstractShapeFigure::operator=(other);
    copy(other);
    return *this;
}

std::string cOvalFigure::info() const
{
    std::stringstream os;
    os << getBounds();
    return os.str();
}

void cOvalFigure::parse(cProperty *property)
{
    cAbstractShapeFigure::parse(property);

    setBounds(parseBounds(property));
}

void cOvalFigure::translate(double x, double y)
{
    bounds.x += x;
    bounds.y += y;
    doGeometryChange();
}

//----

void cPieSliceFigure::copy(const cPieSliceFigure& other)
{
    setBounds(other.getBounds());
    setStartAngle(other.getStartAngle());
    setEndAngle(other.getEndAngle());
}

cPieSliceFigure& cPieSliceFigure::operator=(const cPieSliceFigure& other)
{
    if (this == &other) return *this;
    cAbstractShapeFigure::operator=(other);
    copy(other);
    return *this;
}

std::string cPieSliceFigure::info() const
{
    std::stringstream os;
    os << getBounds();
    return os.str();
}

void cPieSliceFigure::parse(cProperty *property)
{
    cAbstractShapeFigure::parse(property);

    setBounds(parseBounds(property));

    const char *s;
    if ((s = property->getValue("startAngle")) != NULL)
        setStartAngle(opp_atof(s));
    if ((s = property->getValue("endAngle")) != NULL)
        setEndAngle(opp_atof(s));
}

void cPieSliceFigure::translate(double x, double y)
{
    bounds.x += x;
    bounds.y += y;
    doGeometryChange();
}

//----

void cPolygonFigure::copy(const cPolygonFigure& other)
{
    setPoints(other.getPoints());
    setSmooth(other.getSmooth());
    setJoinStyle(other.getJoinStyle());
}

cPolygonFigure& cPolygonFigure::operator=(const cPolygonFigure& other)
{
    if (this == &other) return *this;
    cAbstractShapeFigure::operator=(other);
    copy(other);
    return *this;
}

std::string cPolygonFigure::info() const
{
    std::stringstream os;
    for (int i = 0; i < (int)points.size(); i++)
        os << points[i] << " ";
    return os.str();
}

void cPolygonFigure::parse(cProperty *property)
{
    cAbstractShapeFigure::parse(property);

    const char *s;
    setPoints(parsePoints(property, "coords"));
    if ((s = property->getValue("smooth", 0)) != NULL)
        setSmooth(parseBool(s));
    if ((s = property->getValue("joinStyle", 0)) != NULL)
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

//----

void cTextFigure::copy(const cTextFigure& other)
{
    setLocation(other.getLocation());
    setColor(other.getColor());
    setFont(other.getFont());
    setText(other.getText());
    setAnchor(other.getAnchor());
    setAlignment(other.getAlignment());
}

cTextFigure& cTextFigure::operator=(const cTextFigure& other)
{
    if (this == &other) return *this;
    cFigure::operator=(other);
    copy(other);
    return *this;
}

std::string cTextFigure::info() const
{
    std::stringstream os;
    os << "\"" << getText() << "\" at " << getLocation();
    return os.str();
}

void cTextFigure::parse(cProperty *property)
{
    cFigure::parse(property);

    const char *s;
    setLocation(parsePoint(property, "coords", 0));
    setText(opp_nulltoempty(property->getValue("text")));
    if ((s = property->getValue("color")) != NULL)
        setColor(parseColor(s));
    if (property->containsKey("font"))
        setFont(parseFont(property, "font"));
    if ((s = property->getValue("anchor")) != NULL)
        setAnchor(parseAnchor(s));
    if ((s = property->getValue("alignment")) != NULL)
        setAlignment(parseAlignment(s));
}

void cTextFigure::translate(double x, double y)
{
    location.x += x;
    location.y += y;
    doGeometryChange();
}

//----

void cImageFigure::copy(const cImageFigure& other)
{
    setLocation(other.getLocation());
    setImageName(other.getImageName());
    setTint(other.getTint());
    tinting = other.tinting;
    setAnchor(other.getAnchor());
}

cImageFigure& cImageFigure::operator=(const cImageFigure& other)
{
    if (this == &other) return *this;
    cFigure::operator=(other);
    copy(other);
    return *this;
}

std::string cImageFigure::info() const
{
    std::stringstream os;
    os << "\"" << getImageName() << "\" at " << getLocation();
    return os.str();
}

void cImageFigure::parse(cProperty *property)
{
    cFigure::parse(property);

    const char *s;
    setLocation(parsePoint(property, "coords", 0));
    setImageName(opp_nulltoempty(property->getValue("image")));
    if ((s = property->getValue("tint", 0)) != NULL)
        setTint(parseColor(s));
}

void cImageFigure::translate(double x, double y)
{
    location.x += x;
    location.y += y;
    doGeometryChange();
}

//------

cCanvas::cCanvas(const char *name) : cOwnedObject(name)
{
    rootFigure = new cGroupFigure("rootFigure");
    take(rootFigure);
}

cCanvas::~cCanvas()
{
    dropAndDelete(rootFigure);
}

void cCanvas::copy(const cCanvas& other)
{
    dropAndDelete(rootFigure);
    tagBitIndex.clear();
    rootFigure = other.getRootFigure()->dupTree();
    take(rootFigure);
}

cCanvas& cCanvas::operator=(const cCanvas& other)
{
    if (this == &other) return *this;
    cOwnedObject::operator=(other);
    copy(other);
    return *this;
}

void cCanvas::forEachChild(cVisitor *v)
{
    rootFigure->forEachChild(v); // skip the root figure from the tree
}

std::string cCanvas::info() const
{
    return ""; //TODO
}

bool cCanvas::containsCanvasItems(cProperties *properties)
{
    for (int i = 0; i < properties->getNumProperties(); i++)
    {
        cProperty *property = properties->get(i);
        if (property->isName("figure"))
            return true;
    }
    return false;
}

void cCanvas::addFiguresFrom(cProperties *properties)
{
    std::map<cFigure*,double> orderMap;

    //TODO currently fails with "parent not found" if child figure precedes parent in the property order
    for (int i = 0; i < properties->getNumProperties(); i++)
    {
        cProperty *property = properties->get(i);
        if (property->isName("figure"))
            parseFigure(property, orderMap);
    }
}

void cCanvas::parseFigure(cProperty *property, std::map<cFigure*,double>& orderMap)
{
    try {
        const char *path = property->getIndex();
        if (!path)
            throw cRuntimeError("@figure property is expected to have an index which will become the figure name, e.g. @figure[foo]");
        const char *lastDot = strrchr(path, '.');
        cFigure *parent;
        const char *name;
        if (lastDot) {
            std::string parentPath = std::string(path, lastDot - path);
            parent = getFigureByPath(parentPath.c_str());
            if (!parent)
                throw cRuntimeError("parent figure \"%s\" not found", parentPath.c_str());
            name = lastDot+1;
        }
        else {
            parent = rootFigure;
            name = path;
        }
        if (!name[0])
            throw cRuntimeError("figure name cannot be empty");

        cFigure *figure = parent->getFigure(name);
        if (!figure)
        {
            const char *type = property->getValue("type");
            figure = createFigure(type);
            figure->setName(name);
            const char *order = property->getValue("childZ");
            if (order)
                orderMap[figure] = opp_atof(order);
            parent->insertChild(figure, orderMap);
        }
        else {
            parent->removeFigure(figure);
            parent->addFigure(figure);  //TODO use raiseToTop() instead!
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
    else if (!strcmp(type, "arc"))
        figure = new cArcFigure();
    else if (!strcmp(type, "polyline"))
        figure = new cPolylineFigure();
    else if (!strcmp(type, "rectangle"))
        figure = new cRectangleFigure();
    else if (!strcmp(type, "oval"))
        figure = new cOvalFigure();
    else if (!strcmp(type, "pieslice"))
        figure = new cPieSliceFigure();
    else if (!strcmp(type, "polygon"))
        figure = new cPolygonFigure();
    else if (!strcmp(type, "text"))
        figure = new cTextFigure();
    else if (!strcmp(type, "image"))
        figure = new cImageFigure();
    else {
        // find registered class named "<type>Figure" or "c<type>Figure"
        std::string className = std::string("c") + type + "Figure";
        className[1] = opp_toupper(className[1]);
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

cFigure *cCanvas::getSubmodulesLayer() const
{
    return rootFigure->getFigure("submodules");
}

uint64 cCanvas::parseTags(const char *s)
{
    uint64 result = 0;
    cStringTokenizer tokenizer(s);
    while (tokenizer.hasMoreTokens())
    {
        const char *tag = tokenizer.nextToken();
        int bitIndex;
        std::map<std::string,int>::const_iterator it = tagBitIndex.find(tag);
        if (it != tagBitIndex.end())
            bitIndex = it->second;
        else {
            bitIndex = tagBitIndex.size();
            if (bitIndex >= 64)
                throw cRuntimeError(this, "Cannot add figure tag \"%s\": maximum 64 tags supported", tag);
            tagBitIndex[tag] = bitIndex;
        }
        result |= ((uint64)1) << bitIndex;
    }
    return result;
}

std::string cCanvas::getAllTags() const
{
    std::stringstream os;
    for (std::map<std::string,int>::const_iterator it = tagBitIndex.begin(); it != tagBitIndex.end(); ++it) {
        if (it != tagBitIndex.begin())
            os << " ";
        os << it->first;
    }
    return os.str();
}

std::vector<std::string> cCanvas::getAllTagsAsVector() const
{
    std::vector<std::string> result;
    for (std::map<std::string,int>::const_iterator it = tagBitIndex.begin(); it != tagBitIndex.end(); ++it)
        result.push_back(it->first);
    return result;
}

//---

static std::map<std::string, cFigure::Color> colors;

static std::string lc(const char *s)
{
    std::string tmp = s;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
    return tmp;
}

cFigure::Color cFigure::Color::byName(const char *name)
{
    if (colors.empty()) {
        colors[lc("antiqueWhite")] = cFigure::Color(250,235,215);
        colors[lc("antiqueWhite1")] = cFigure::Color(255,239,219);
        colors[lc("antiqueWhite2")] = cFigure::Color(238,223,204);
        colors[lc("antiqueWhite3")] = cFigure::Color(205,192,176);
        colors[lc("antiqueWhite4")] = cFigure::Color(139,131,120);
        colors[lc("blanchedAlmond")] = cFigure::Color(255,235,205);
        colors[lc("blueViolet")] = cFigure::Color(138,43,226);
        colors[lc("cadetBlue")] = cFigure::Color(95,158,160);
        colors[lc("cadetBlue1")] = cFigure::Color(152,245,255);
        colors[lc("cadetBlue2")] = cFigure::Color(142,229,238);
        colors[lc("cadetBlue3")] = cFigure::Color(122,197,205);
        colors[lc("cadetBlue4")] = cFigure::Color(83,134,139);
        colors[lc("cornflowerBlue")] = cFigure::Color(100,149,237);
        colors[lc("darkBlue")] = cFigure::Color(0,0,139);
        colors[lc("darkCyan")] = cFigure::Color(0,139,139);
        colors[lc("darkGoldenrod")] = cFigure::Color(184,134,11);
        colors[lc("darkGoldenrod1")] = cFigure::Color(255,185,15);
        colors[lc("darkGoldenrod2")] = cFigure::Color(238,173,14);
        colors[lc("darkGoldenrod3")] = cFigure::Color(205,149,12);
        colors[lc("darkGoldenrod4")] = cFigure::Color(139,101,8);
        colors[lc("darkGreen")] = cFigure::Color(0,100,0);
        colors[lc("darkGrey")] = cFigure::Color(169,169,169);
        colors[lc("darkKhaki")] = cFigure::Color(189,183,107);
        colors[lc("darkMagenta")] = cFigure::Color(139,0,139);
        colors[lc("darkOliveGreen")] = cFigure::Color(85,107,47);
        colors[lc("darkOliveGreen1")] = cFigure::Color(202,255,112);
        colors[lc("darkOliveGreen2")] = cFigure::Color(188,238,104);
        colors[lc("darkOliveGreen3")] = cFigure::Color(162,205,90);
        colors[lc("darkOliveGreen4")] = cFigure::Color(110,139,61);
        colors[lc("darkOrange")] = cFigure::Color(255,140,0);
        colors[lc("darkOrange1")] = cFigure::Color(255,127,0);
        colors[lc("darkOrange2")] = cFigure::Color(238,118,0);
        colors[lc("darkOrange3")] = cFigure::Color(205,102,0);
        colors[lc("darkOrange4")] = cFigure::Color(139,69,0);
        colors[lc("darkOrchid")] = cFigure::Color(153,50,204);
        colors[lc("darkOrchid1")] = cFigure::Color(191,62,255);
        colors[lc("darkOrchid2")] = cFigure::Color(178,58,238);
        colors[lc("darkOrchid3")] = cFigure::Color(154,50,205);
        colors[lc("darkOrchid4")] = cFigure::Color(104,34,139);
        colors[lc("darkRed")] = cFigure::Color(139,0,0);
        colors[lc("darkSalmon")] = cFigure::Color(233,150,122);
        colors[lc("darkSeaGreen")] = cFigure::Color(143,188,143);
        colors[lc("darkSeaGreen1")] = cFigure::Color(193,255,193);
        colors[lc("darkSeaGreen2")] = cFigure::Color(180,238,180);
        colors[lc("darkSeaGreen3")] = cFigure::Color(155,205,155);
        colors[lc("darkSeaGreen4")] = cFigure::Color(105,139,105);
        colors[lc("darkSlateBlue")] = cFigure::Color(72,61,139);
        colors[lc("darkSlateGrey")] = cFigure::Color(47,79,79);
        colors[lc("darkTurquoise")] = cFigure::Color(0,206,209);
        colors[lc("darkViolet")] = cFigure::Color(148,0,211);
        colors[lc("deepPink")] = cFigure::Color(255,20,147);
        colors[lc("deepPink1")] = cFigure::Color(255,20,147);
        colors[lc("deepPink2")] = cFigure::Color(238,18,137);
        colors[lc("deepPink3")] = cFigure::Color(205,16,118);
        colors[lc("deepPink4")] = cFigure::Color(139,10,80);
        colors[lc("deepSkyBlue")] = cFigure::Color(0,191,255);
        colors[lc("deepSkyBlue1")] = cFigure::Color(0,191,255);
        colors[lc("deepSkyBlue2")] = cFigure::Color(0,178,238);
        colors[lc("deepSkyBlue3")] = cFigure::Color(0,154,205);
        colors[lc("deepSkyBlue4")] = cFigure::Color(0,104,139);
        colors[lc("dimGrey")] = cFigure::Color(105,105,105);
        colors[lc("dodgerBlue")] = cFigure::Color(30,144,255);
        colors[lc("dodgerBlue1")] = cFigure::Color(30,144,255);
        colors[lc("dodgerBlue2")] = cFigure::Color(28,134,238);
        colors[lc("dodgerBlue3")] = cFigure::Color(24,116,205);
        colors[lc("dodgerBlue4")] = cFigure::Color(16,78,139);
        colors[lc("floralWhite")] = cFigure::Color(255,250,240);
        colors[lc("forestGreen")] = cFigure::Color(34,139,34);
        colors[lc("ghostWhite")] = cFigure::Color(248,248,255);
        colors[lc("greenYellow")] = cFigure::Color(173,255,47);
        colors[lc("hotPink")] = cFigure::Color(255,105,180);
        colors[lc("hotPink1")] = cFigure::Color(255,110,180);
        colors[lc("hotPink2")] = cFigure::Color(238,106,167);
        colors[lc("hotPink3")] = cFigure::Color(205,96,144);
        colors[lc("hotPink4")] = cFigure::Color(139,58,98);
        colors[lc("indianRed")] = cFigure::Color(205,92,92);
        colors[lc("indianRed1")] = cFigure::Color(255,106,106);
        colors[lc("indianRed2")] = cFigure::Color(238,99,99);
        colors[lc("indianRed3")] = cFigure::Color(205,85,85);
        colors[lc("indianRed4")] = cFigure::Color(139,58,58);
        colors[lc("lavenderBlush")] = cFigure::Color(255,240,245);
        colors[lc("lavenderBlush1")] = cFigure::Color(255,240,245);
        colors[lc("lavenderBlush2")] = cFigure::Color(238,224,229);
        colors[lc("lavenderBlush3")] = cFigure::Color(205,193,197);
        colors[lc("lavenderBlush4")] = cFigure::Color(139,131,134);
        colors[lc("lawnGreen")] = cFigure::Color(124,252,0);
        colors[lc("lemonChiffon")] = cFigure::Color(255,250,205);
        colors[lc("lemonChiffon1")] = cFigure::Color(255,250,205);
        colors[lc("lemonChiffon2")] = cFigure::Color(238,233,191);
        colors[lc("lemonChiffon3")] = cFigure::Color(205,201,165);
        colors[lc("lemonChiffon4")] = cFigure::Color(139,137,112);
        colors[lc("lightBlue")] = cFigure::Color(173,216,230);
        colors[lc("lightBlue1")] = cFigure::Color(191,239,255);
        colors[lc("lightBlue2")] = cFigure::Color(178,223,238);
        colors[lc("lightBlue3")] = cFigure::Color(154,192,205);
        colors[lc("lightBlue4")] = cFigure::Color(104,131,139);
        colors[lc("lightCoral")] = cFigure::Color(240,128,128);
        colors[lc("lightCyan")] = cFigure::Color(224,255,255);
        colors[lc("lightCyan1")] = cFigure::Color(224,255,255);
        colors[lc("lightCyan2")] = cFigure::Color(209,238,238);
        colors[lc("lightCyan3")] = cFigure::Color(180,205,205);
        colors[lc("lightCyan4")] = cFigure::Color(122,139,139);
        colors[lc("lightGoldenrod")] = cFigure::Color(238,221,130);
        colors[lc("lightGoldenrod1")] = cFigure::Color(255,236,139);
        colors[lc("lightGoldenrod2")] = cFigure::Color(238,220,130);
        colors[lc("lightGoldenrod3")] = cFigure::Color(205,190,112);
        colors[lc("lightGoldenrod4")] = cFigure::Color(139,129,76);
        colors[lc("lightGoldenrodYellow")] = cFigure::Color(250,250,210);
        colors[lc("lightGreen")] = cFigure::Color(144,238,144);
        colors[lc("lightGrey")] = cFigure::Color(211,211,211);
        colors[lc("lightPink")] = cFigure::Color(255,182,193);
        colors[lc("lightPink1")] = cFigure::Color(255,174,185);
        colors[lc("lightPink2")] = cFigure::Color(238,162,173);
        colors[lc("lightPink3")] = cFigure::Color(205,140,149);
        colors[lc("lightPink4")] = cFigure::Color(139,95,101);
        colors[lc("lightSalmon")] = cFigure::Color(255,160,122);
        colors[lc("lightSalmon1")] = cFigure::Color(255,160,122);
        colors[lc("lightSalmon2")] = cFigure::Color(238,149,114);
        colors[lc("lightSalmon3")] = cFigure::Color(205,129,98);
        colors[lc("lightSalmon4")] = cFigure::Color(139,87,66);
        colors[lc("lightSeaGreen")] = cFigure::Color(32,178,170);
        colors[lc("lightSkyBlue")] = cFigure::Color(135,206,250);
        colors[lc("lightSkyBlue1")] = cFigure::Color(176,226,255);
        colors[lc("lightSkyBlue2")] = cFigure::Color(164,211,238);
        colors[lc("lightSkyBlue3")] = cFigure::Color(141,182,205);
        colors[lc("lightSkyBlue4")] = cFigure::Color(96,123,139);
        colors[lc("lightSlateBlue")] = cFigure::Color(132,112,255);
        colors[lc("lightSlateGrey")] = cFigure::Color(119,136,153);
        colors[lc("lightSteelBlue")] = cFigure::Color(176,196,222);
        colors[lc("lightSteelBlue1")] = cFigure::Color(202,225,255);
        colors[lc("lightSteelBlue2")] = cFigure::Color(188,210,238);
        colors[lc("lightSteelBlue3")] = cFigure::Color(162,181,205);
        colors[lc("lightSteelBlue4")] = cFigure::Color(110,123,139);
        colors[lc("lightYellow")] = cFigure::Color(255,255,224);
        colors[lc("lightYellow1")] = cFigure::Color(255,255,224);
        colors[lc("lightYellow2")] = cFigure::Color(238,238,209);
        colors[lc("lightYellow3")] = cFigure::Color(205,205,180);
        colors[lc("lightYellow4")] = cFigure::Color(139,139,122);
        colors[lc("limeGreen")] = cFigure::Color(50,205,50);
        colors[lc("mediumAquamarine")] = cFigure::Color(102,205,170);
        colors[lc("mediumBlue")] = cFigure::Color(0,0,205);
        colors[lc("mediumOrchid")] = cFigure::Color(186,85,211);
        colors[lc("mediumOrchid1")] = cFigure::Color(224,102,255);
        colors[lc("mediumOrchid2")] = cFigure::Color(209,95,238);
        colors[lc("mediumOrchid3")] = cFigure::Color(180,82,205);
        colors[lc("mediumOrchid4")] = cFigure::Color(122,55,139);
        colors[lc("mediumPurple")] = cFigure::Color(147,112,219);
        colors[lc("mediumPurple1")] = cFigure::Color(171,130,255);
        colors[lc("mediumPurple2")] = cFigure::Color(159,121,238);
        colors[lc("mediumPurple3")] = cFigure::Color(137,104,205);
        colors[lc("mediumPurple4")] = cFigure::Color(93,71,139);
        colors[lc("mediumSeaGreen")] = cFigure::Color(60,179,113);
        colors[lc("mediumSlateBlue")] = cFigure::Color(123,104,238);
        colors[lc("mediumSpringGreen")] = cFigure::Color(0,250,154);
        colors[lc("mediumTurquoise")] = cFigure::Color(72,209,204);
        colors[lc("mediumVioletRed")] = cFigure::Color(199,21,133);
        colors[lc("midnightBlue")] = cFigure::Color(25,25,112);
        colors[lc("mintCream")] = cFigure::Color(245,255,250);
        colors[lc("mistyRose")] = cFigure::Color(255,228,225);
        colors[lc("mistyRose1")] = cFigure::Color(255,228,225);
        colors[lc("mistyRose2")] = cFigure::Color(238,213,210);
        colors[lc("mistyRose3")] = cFigure::Color(205,183,181);
        colors[lc("mistyRose4")] = cFigure::Color(139,125,123);
        colors[lc("navajoWhite")] = cFigure::Color(255,222,173);
        colors[lc("navajoWhite1")] = cFigure::Color(255,222,173);
        colors[lc("navajoWhite2")] = cFigure::Color(238,207,161);
        colors[lc("navajoWhite3")] = cFigure::Color(205,179,139);
        colors[lc("navajoWhite4")] = cFigure::Color(139,121,94);
        colors[lc("navyBlue")] = cFigure::Color(0,0,128);
        colors[lc("oldLace")] = cFigure::Color(253,245,230);
        colors[lc("oliveDrab")] = cFigure::Color(107,142,35);
        colors[lc("oliveDrab1")] = cFigure::Color(192,255,62);
        colors[lc("oliveDrab2")] = cFigure::Color(179,238,58);
        colors[lc("oliveDrab3")] = cFigure::Color(154,205,50);
        colors[lc("oliveDrab4")] = cFigure::Color(105,139,34);
        colors[lc("orangeRed")] = cFigure::Color(255,69,0);
        colors[lc("orangeRed1")] = cFigure::Color(255,69,0);
        colors[lc("orangeRed2")] = cFigure::Color(238,64,0);
        colors[lc("orangeRed3")] = cFigure::Color(205,55,0);
        colors[lc("orangeRed4")] = cFigure::Color(139,37,0);
        colors[lc("paleGoldenrod")] = cFigure::Color(238,232,170);
        colors[lc("paleGreen")] = cFigure::Color(152,251,152);
        colors[lc("paleGreen1")] = cFigure::Color(154,255,154);
        colors[lc("paleGreen2")] = cFigure::Color(144,238,144);
        colors[lc("paleGreen3")] = cFigure::Color(124,205,124);
        colors[lc("paleGreen4")] = cFigure::Color(84,139,84);
        colors[lc("paleTurquoise")] = cFigure::Color(175,238,238);
        colors[lc("paleTurquoise1")] = cFigure::Color(187,255,255);
        colors[lc("paleTurquoise2")] = cFigure::Color(174,238,238);
        colors[lc("paleTurquoise3")] = cFigure::Color(150,205,205);
        colors[lc("paleTurquoise4")] = cFigure::Color(102,139,139);
        colors[lc("paleVioletRed")] = cFigure::Color(219,112,147);
        colors[lc("paleVioletRed1")] = cFigure::Color(255,130,171);
        colors[lc("paleVioletRed2")] = cFigure::Color(238,121,159);
        colors[lc("paleVioletRed3")] = cFigure::Color(205,104,137);
        colors[lc("paleVioletRed4")] = cFigure::Color(139,71,93);
        colors[lc("papayaWhip")] = cFigure::Color(255,239,213);
        colors[lc("peachPuff")] = cFigure::Color(255,218,185);
        colors[lc("peachPuff1")] = cFigure::Color(255,218,185);
        colors[lc("peachPuff2")] = cFigure::Color(238,203,173);
        colors[lc("peachPuff3")] = cFigure::Color(205,175,149);
        colors[lc("peachPuff4")] = cFigure::Color(139,119,101);
        colors[lc("powderBlue")] = cFigure::Color(176,224,230);
        colors[lc("rosyBrown")] = cFigure::Color(188,143,143);
        colors[lc("rosyBrown1")] = cFigure::Color(255,193,193);
        colors[lc("rosyBrown2")] = cFigure::Color(238,180,180);
        colors[lc("rosyBrown3")] = cFigure::Color(205,155,155);
        colors[lc("rosyBrown4")] = cFigure::Color(139,105,105);
        colors[lc("royalBlue")] = cFigure::Color(65,105,225);
        colors[lc("royalBlue1")] = cFigure::Color(72,118,255);
        colors[lc("royalBlue2")] = cFigure::Color(67,110,238);
        colors[lc("royalBlue3")] = cFigure::Color(58,95,205);
        colors[lc("royalBlue4")] = cFigure::Color(39,64,139);
        colors[lc("saddleBrown")] = cFigure::Color(139,69,19);
        colors[lc("sandyBrown")] = cFigure::Color(244,164,96);
        colors[lc("seaGreen")] = cFigure::Color(46,139,87);
        colors[lc("seaGreen1")] = cFigure::Color(84,255,159);
        colors[lc("seaGreen2")] = cFigure::Color(78,238,148);
        colors[lc("seaGreen3")] = cFigure::Color(67,205,128);
        colors[lc("seaGreen4")] = cFigure::Color(46,139,87);
        colors[lc("skyBlue")] = cFigure::Color(135,206,235);
        colors[lc("skyBlue1")] = cFigure::Color(135,206,255);
        colors[lc("skyBlue2")] = cFigure::Color(126,192,238);
        colors[lc("skyBlue3")] = cFigure::Color(108,166,205);
        colors[lc("skyBlue4")] = cFigure::Color(74,112,139);
        colors[lc("slateBlue")] = cFigure::Color(106,90,205);
        colors[lc("slateBlue1")] = cFigure::Color(131,111,255);
        colors[lc("slateBlue2")] = cFigure::Color(122,103,238);
        colors[lc("slateBlue3")] = cFigure::Color(105,89,205);
        colors[lc("slateBlue4")] = cFigure::Color(71,60,139);
        colors[lc("slateGrey")] = cFigure::Color(112,128,144);
        colors[lc("springGreen")] = cFigure::Color(0,255,127);
        colors[lc("springGreen1")] = cFigure::Color(0,255,127);
        colors[lc("springGreen2")] = cFigure::Color(0,238,118);
        colors[lc("springGreen3")] = cFigure::Color(0,205,102);
        colors[lc("springGreen4")] = cFigure::Color(0,139,69);
        colors[lc("steelBlue")] = cFigure::Color(70,130,180);
        colors[lc("steelBlue1")] = cFigure::Color(99,184,255);
        colors[lc("steelBlue2")] = cFigure::Color(92,172,238);
        colors[lc("steelBlue3")] = cFigure::Color(79,148,205);
        colors[lc("steelBlue4")] = cFigure::Color(54,100,139);
        colors[lc("violetRed")] = cFigure::Color(208,32,144);
        colors[lc("violetRed1")] = cFigure::Color(255,62,150);
        colors[lc("violetRed2")] = cFigure::Color(238,58,140);
        colors[lc("violetRed3")] = cFigure::Color(205,50,120);
        colors[lc("violetRed4")] = cFigure::Color(139,34,82);
        colors[lc("whiteSmoke")] = cFigure::Color(245,245,245);
        colors[lc("yellowGreen")] = cFigure::Color(154,205,50);
        colors[lc("aquamarine")] = cFigure::Color(127,255,212);
        colors[lc("aquamarine1")] = cFigure::Color(127,255,212);
        colors[lc("aquamarine2")] = cFigure::Color(118,238,198);
        colors[lc("aquamarine3")] = cFigure::Color(102,205,170);
        colors[lc("aquamarine4")] = cFigure::Color(69,139,116);
        colors[lc("azure")] = cFigure::Color(240,255,255);
        colors[lc("azure1")] = cFigure::Color(240,255,255);
        colors[lc("azure2")] = cFigure::Color(224,238,238);
        colors[lc("azure3")] = cFigure::Color(193,205,205);
        colors[lc("azure4")] = cFigure::Color(131,139,139);
        colors[lc("beige")] = cFigure::Color(245,245,220);
        colors[lc("bisque")] = cFigure::Color(255,228,196);
        colors[lc("bisque1")] = cFigure::Color(255,228,196);
        colors[lc("bisque2")] = cFigure::Color(238,213,183);
        colors[lc("bisque3")] = cFigure::Color(205,183,158);
        colors[lc("bisque4")] = cFigure::Color(139,125,107);
        colors[lc("black")] = cFigure::Color(0,0,0);
        colors[lc("blue")] = cFigure::Color(0,0,255);
        colors[lc("blue1")] = cFigure::Color(0,0,255);
        colors[lc("blue2")] = cFigure::Color(0,0,238);
        colors[lc("blue3")] = cFigure::Color(0,0,205);
        colors[lc("blue4")] = cFigure::Color(0,0,139);
        colors[lc("brown")] = cFigure::Color(165,42,42);
        colors[lc("brown1")] = cFigure::Color(255,64,64);
        colors[lc("brown2")] = cFigure::Color(238,59,59);
        colors[lc("brown3")] = cFigure::Color(205,51,51);
        colors[lc("brown4")] = cFigure::Color(139,35,35);
        colors[lc("burlywood")] = cFigure::Color(222,184,135);
        colors[lc("burlywood1")] = cFigure::Color(255,211,155);
        colors[lc("burlywood2")] = cFigure::Color(238,197,145);
        colors[lc("burlywood3")] = cFigure::Color(205,170,125);
        colors[lc("burlywood4")] = cFigure::Color(139,115,85);
        colors[lc("chartreuse")] = cFigure::Color(127,255,0);
        colors[lc("chartreuse1")] = cFigure::Color(127,255,0);
        colors[lc("chartreuse2")] = cFigure::Color(118,238,0);
        colors[lc("chartreuse3")] = cFigure::Color(102,205,0);
        colors[lc("chartreuse4")] = cFigure::Color(69,139,0);
        colors[lc("chocolate")] = cFigure::Color(210,105,30);
        colors[lc("chocolate1")] = cFigure::Color(255,127,36);
        colors[lc("chocolate2")] = cFigure::Color(238,118,33);
        colors[lc("chocolate3")] = cFigure::Color(205,102,29);
        colors[lc("chocolate4")] = cFigure::Color(139,69,19);
        colors[lc("coral")] = cFigure::Color(255,127,80);
        colors[lc("coral1")] = cFigure::Color(255,114,86);
        colors[lc("coral2")] = cFigure::Color(238,106,80);
        colors[lc("coral3")] = cFigure::Color(205,91,69);
        colors[lc("coral4")] = cFigure::Color(139,62,47);
        colors[lc("cornsilk")] = cFigure::Color(255,248,220);
        colors[lc("cornsilk1")] = cFigure::Color(255,248,220);
        colors[lc("cornsilk2")] = cFigure::Color(238,232,205);
        colors[lc("cornsilk3")] = cFigure::Color(205,200,177);
        colors[lc("cornsilk4")] = cFigure::Color(139,136,120);
        colors[lc("cyan")] = cFigure::Color(0,255,255);
        colors[lc("cyan1")] = cFigure::Color(0,255,255);
        colors[lc("cyan2")] = cFigure::Color(0,238,238);
        colors[lc("cyan3")] = cFigure::Color(0,205,205);
        colors[lc("cyan4")] = cFigure::Color(0,139,139);
        colors[lc("firebrick")] = cFigure::Color(178,34,34);
        colors[lc("firebrick1")] = cFigure::Color(255,48,48);
        colors[lc("firebrick2")] = cFigure::Color(238,44,44);
        colors[lc("firebrick3")] = cFigure::Color(205,38,38);
        colors[lc("firebrick4")] = cFigure::Color(139,26,26);
        colors[lc("gainsboro")] = cFigure::Color(220,220,220);
        colors[lc("gold")] = cFigure::Color(255,215,0);
        colors[lc("gold1")] = cFigure::Color(255,215,0);
        colors[lc("gold2")] = cFigure::Color(238,201,0);
        colors[lc("gold3")] = cFigure::Color(205,173,0);
        colors[lc("gold4")] = cFigure::Color(139,117,0);
        colors[lc("goldenrod")] = cFigure::Color(218,165,32);
        colors[lc("goldenrod1")] = cFigure::Color(255,193,37);
        colors[lc("goldenrod2")] = cFigure::Color(238,180,34);
        colors[lc("goldenrod3")] = cFigure::Color(205,155,29);
        colors[lc("goldenrod4")] = cFigure::Color(139,105,20);
        colors[lc("green")] = cFigure::Color(0,255,0);
        colors[lc("green1")] = cFigure::Color(0,255,0);
        colors[lc("green2")] = cFigure::Color(0,238,0);
        colors[lc("green3")] = cFigure::Color(0,205,0);
        colors[lc("green4")] = cFigure::Color(0,139,0);
        colors[lc("grey")] = cFigure::Color(192,192,192);
        colors[lc("grey0")] = cFigure::Color(0,0,0);
        colors[lc("grey1")] = cFigure::Color(3,3,3);
        colors[lc("grey10")] = cFigure::Color(26,26,26);
        colors[lc("grey100")] = cFigure::Color(255,255,255);
        colors[lc("grey11")] = cFigure::Color(28,28,28);
        colors[lc("grey12")] = cFigure::Color(31,31,31);
        colors[lc("grey13")] = cFigure::Color(33,33,33);
        colors[lc("grey14")] = cFigure::Color(36,36,36);
        colors[lc("grey15")] = cFigure::Color(38,38,38);
        colors[lc("grey16")] = cFigure::Color(41,41,41);
        colors[lc("grey17")] = cFigure::Color(43,43,43);
        colors[lc("grey18")] = cFigure::Color(46,46,46);
        colors[lc("grey19")] = cFigure::Color(48,48,48);
        colors[lc("grey2")] = cFigure::Color(5,5,5);
        colors[lc("grey20")] = cFigure::Color(51,51,51);
        colors[lc("grey21")] = cFigure::Color(54,54,54);
        colors[lc("grey22")] = cFigure::Color(56,56,56);
        colors[lc("grey23")] = cFigure::Color(59,59,59);
        colors[lc("grey24")] = cFigure::Color(61,61,61);
        colors[lc("grey25")] = cFigure::Color(64,64,64);
        colors[lc("grey26")] = cFigure::Color(66,66,66);
        colors[lc("grey27")] = cFigure::Color(69,69,69);
        colors[lc("grey28")] = cFigure::Color(71,71,71);
        colors[lc("grey29")] = cFigure::Color(74,74,74);
        colors[lc("grey3")] = cFigure::Color(8,8,8);
        colors[lc("grey30")] = cFigure::Color(77,77,77);
        colors[lc("grey31")] = cFigure::Color(79,79,79);
        colors[lc("grey32")] = cFigure::Color(82,82,82);
        colors[lc("grey33")] = cFigure::Color(84,84,84);
        colors[lc("grey34")] = cFigure::Color(87,87,87);
        colors[lc("grey35")] = cFigure::Color(89,89,89);
        colors[lc("grey36")] = cFigure::Color(92,92,92);
        colors[lc("grey37")] = cFigure::Color(94,94,94);
        colors[lc("grey38")] = cFigure::Color(97,97,97);
        colors[lc("grey39")] = cFigure::Color(99,99,99);
        colors[lc("grey4")] = cFigure::Color(10,10,10);
        colors[lc("grey40")] = cFigure::Color(102,102,102);
        colors[lc("grey41")] = cFigure::Color(105,105,105);
        colors[lc("grey42")] = cFigure::Color(107,107,107);
        colors[lc("grey43")] = cFigure::Color(110,110,110);
        colors[lc("grey44")] = cFigure::Color(112,112,112);
        colors[lc("grey45")] = cFigure::Color(115,115,115);
        colors[lc("grey46")] = cFigure::Color(117,117,117);
        colors[lc("grey47")] = cFigure::Color(120,120,120);
        colors[lc("grey48")] = cFigure::Color(122,122,122);
        colors[lc("grey49")] = cFigure::Color(125,125,125);
        colors[lc("grey5")] = cFigure::Color(13,13,13);
        colors[lc("grey50")] = cFigure::Color(127,127,127);
        colors[lc("grey51")] = cFigure::Color(130,130,130);
        colors[lc("grey52")] = cFigure::Color(133,133,133);
        colors[lc("grey53")] = cFigure::Color(135,135,135);
        colors[lc("grey54")] = cFigure::Color(138,138,138);
        colors[lc("grey55")] = cFigure::Color(140,140,140);
        colors[lc("grey56")] = cFigure::Color(143,143,143);
        colors[lc("grey57")] = cFigure::Color(145,145,145);
        colors[lc("grey58")] = cFigure::Color(148,148,148);
        colors[lc("grey59")] = cFigure::Color(150,150,150);
        colors[lc("grey6")] = cFigure::Color(15,15,15);
        colors[lc("grey60")] = cFigure::Color(153,153,153);
        colors[lc("grey61")] = cFigure::Color(156,156,156);
        colors[lc("grey62")] = cFigure::Color(158,158,158);
        colors[lc("grey63")] = cFigure::Color(161,161,161);
        colors[lc("grey64")] = cFigure::Color(163,163,163);
        colors[lc("grey65")] = cFigure::Color(166,166,166);
        colors[lc("grey66")] = cFigure::Color(168,168,168);
        colors[lc("grey67")] = cFigure::Color(171,171,171);
        colors[lc("grey68")] = cFigure::Color(173,173,173);
        colors[lc("grey69")] = cFigure::Color(176,176,176);
        colors[lc("grey7")] = cFigure::Color(18,18,18);
        colors[lc("grey70")] = cFigure::Color(179,179,179);
        colors[lc("grey71")] = cFigure::Color(181,181,181);
        colors[lc("grey72")] = cFigure::Color(184,184,184);
        colors[lc("grey73")] = cFigure::Color(186,186,186);
        colors[lc("grey74")] = cFigure::Color(189,189,189);
        colors[lc("grey75")] = cFigure::Color(191,191,191);
        colors[lc("grey76")] = cFigure::Color(194,194,194);
        colors[lc("grey77")] = cFigure::Color(196,196,196);
        colors[lc("grey78")] = cFigure::Color(199,199,199);
        colors[lc("grey79")] = cFigure::Color(201,201,201);
        colors[lc("grey8")] = cFigure::Color(20,20,20);
        colors[lc("grey80")] = cFigure::Color(204,204,204);
        colors[lc("grey81")] = cFigure::Color(207,207,207);
        colors[lc("grey82")] = cFigure::Color(209,209,209);
        colors[lc("grey83")] = cFigure::Color(212,212,212);
        colors[lc("grey84")] = cFigure::Color(214,214,214);
        colors[lc("grey85")] = cFigure::Color(217,217,217);
        colors[lc("grey86")] = cFigure::Color(219,219,219);
        colors[lc("grey87")] = cFigure::Color(222,222,222);
        colors[lc("grey88")] = cFigure::Color(224,224,224);
        colors[lc("grey89")] = cFigure::Color(227,227,227);
        colors[lc("grey9")] = cFigure::Color(23,23,23);
        colors[lc("grey90")] = cFigure::Color(229,229,229);
        colors[lc("grey91")] = cFigure::Color(232,232,232);
        colors[lc("grey92")] = cFigure::Color(235,235,235);
        colors[lc("grey93")] = cFigure::Color(237,237,237);
        colors[lc("grey94")] = cFigure::Color(240,240,240);
        colors[lc("grey95")] = cFigure::Color(242,242,242);
        colors[lc("grey96")] = cFigure::Color(245,245,245);
        colors[lc("grey97")] = cFigure::Color(247,247,247);
        colors[lc("grey98")] = cFigure::Color(250,250,250);
        colors[lc("grey99")] = cFigure::Color(252,252,252);
        colors[lc("honeydew")] = cFigure::Color(240,255,240);
        colors[lc("honeydew1")] = cFigure::Color(240,255,240);
        colors[lc("honeydew2")] = cFigure::Color(224,238,224);
        colors[lc("honeydew3")] = cFigure::Color(193,205,193);
        colors[lc("honeydew4")] = cFigure::Color(131,139,131);
        colors[lc("ivory")] = cFigure::Color(255,255,240);
        colors[lc("ivory1")] = cFigure::Color(255,255,240);
        colors[lc("ivory2")] = cFigure::Color(238,238,224);
        colors[lc("ivory3")] = cFigure::Color(205,205,193);
        colors[lc("ivory4")] = cFigure::Color(139,139,131);
        colors[lc("khaki")] = cFigure::Color(240,230,140);
        colors[lc("khaki1")] = cFigure::Color(255,246,143);
        colors[lc("khaki2")] = cFigure::Color(238,230,133);
        colors[lc("khaki3")] = cFigure::Color(205,198,115);
        colors[lc("khaki4")] = cFigure::Color(139,134,78);
        colors[lc("lavender")] = cFigure::Color(230,230,250);
        colors[lc("linen")] = cFigure::Color(250,240,230);
        colors[lc("magenta")] = cFigure::Color(255,0,255);
        colors[lc("magenta1")] = cFigure::Color(255,0,255);
        colors[lc("magenta2")] = cFigure::Color(238,0,238);
        colors[lc("magenta3")] = cFigure::Color(205,0,205);
        colors[lc("magenta4")] = cFigure::Color(139,0,139);
        colors[lc("maroon")] = cFigure::Color(176,48,96);
        colors[lc("maroon1")] = cFigure::Color(255,52,179);
        colors[lc("maroon2")] = cFigure::Color(238,48,167);
        colors[lc("maroon3")] = cFigure::Color(205,41,144);
        colors[lc("maroon4")] = cFigure::Color(139,28,98);
        colors[lc("moccasin")] = cFigure::Color(255,228,181);
        colors[lc("navy")] = cFigure::Color(0,0,128);
        colors[lc("orange")] = cFigure::Color(255,165,0);
        colors[lc("orange1")] = cFigure::Color(255,165,0);
        colors[lc("orange2")] = cFigure::Color(238,154,0);
        colors[lc("orange3")] = cFigure::Color(205,133,0);
        colors[lc("orange4")] = cFigure::Color(139,90,0);
        colors[lc("orchid")] = cFigure::Color(218,112,214);
        colors[lc("orchid1")] = cFigure::Color(255,131,250);
        colors[lc("orchid2")] = cFigure::Color(238,122,233);
        colors[lc("orchid3")] = cFigure::Color(205,105,201);
        colors[lc("orchid4")] = cFigure::Color(139,71,137);
        colors[lc("peru")] = cFigure::Color(205,133,63);
        colors[lc("pink")] = cFigure::Color(255,192,203);
        colors[lc("pink1")] = cFigure::Color(255,181,197);
        colors[lc("pink2")] = cFigure::Color(238,169,184);
        colors[lc("pink3")] = cFigure::Color(205,145,158);
        colors[lc("pink4")] = cFigure::Color(139,99,108);
        colors[lc("plum")] = cFigure::Color(221,160,221);
        colors[lc("plum1")] = cFigure::Color(255,187,255);
        colors[lc("plum2")] = cFigure::Color(238,174,238);
        colors[lc("plum3")] = cFigure::Color(205,150,205);
        colors[lc("plum4")] = cFigure::Color(139,102,139);
        colors[lc("purple")] = cFigure::Color(160,32,240);
        colors[lc("purple1")] = cFigure::Color(155,48,255);
        colors[lc("purple2")] = cFigure::Color(145,44,238);
        colors[lc("purple3")] = cFigure::Color(125,38,205);
        colors[lc("purple4")] = cFigure::Color(85,26,139);
        colors[lc("red")] = cFigure::Color(255,0,0);
        colors[lc("red1")] = cFigure::Color(255,0,0);
        colors[lc("red2")] = cFigure::Color(238,0,0);
        colors[lc("red3")] = cFigure::Color(205,0,0);
        colors[lc("red4")] = cFigure::Color(139,0,0);
        colors[lc("salmon")] = cFigure::Color(250,128,114);
        colors[lc("salmon1")] = cFigure::Color(255,140,105);
        colors[lc("salmon2")] = cFigure::Color(238,130,98);
        colors[lc("salmon3")] = cFigure::Color(205,112,84);
        colors[lc("salmon4")] = cFigure::Color(139,76,57);
        colors[lc("seashell")] = cFigure::Color(255,245,238);
        colors[lc("seashell1")] = cFigure::Color(255,245,238);
        colors[lc("seashell2")] = cFigure::Color(238,229,222);
        colors[lc("seashell3")] = cFigure::Color(205,197,191);
        colors[lc("seashell4")] = cFigure::Color(139,134,130);
        colors[lc("sienna")] = cFigure::Color(160,82,45);
        colors[lc("sienna1")] = cFigure::Color(255,130,71);
        colors[lc("sienna2")] = cFigure::Color(238,121,66);
        colors[lc("sienna3")] = cFigure::Color(205,104,57);
        colors[lc("sienna4")] = cFigure::Color(139,71,38);
        colors[lc("snow")] = cFigure::Color(255,250,250);
        colors[lc("snow1")] = cFigure::Color(255,250,250);
        colors[lc("snow2")] = cFigure::Color(238,233,233);
        colors[lc("snow3")] = cFigure::Color(205,201,201);
        colors[lc("snow4")] = cFigure::Color(139,137,137);
        colors[lc("tan")] = cFigure::Color(210,180,140);
        colors[lc("tan1")] = cFigure::Color(255,165,79);
        colors[lc("tan2")] = cFigure::Color(238,154,73);
        colors[lc("tan3")] = cFigure::Color(205,133,63);
        colors[lc("tan4")] = cFigure::Color(139,90,43);
        colors[lc("thistle")] = cFigure::Color(216,191,216);
        colors[lc("thistle1")] = cFigure::Color(255,225,255);
        colors[lc("thistle2")] = cFigure::Color(238,210,238);
        colors[lc("thistle3")] = cFigure::Color(205,181,205);
        colors[lc("thistle4")] = cFigure::Color(139,123,139);
        colors[lc("tomato")] = cFigure::Color(255,99,71);
        colors[lc("tomato1")] = cFigure::Color(255,99,71);
        colors[lc("tomato2")] = cFigure::Color(238,92,66);
        colors[lc("tomato3")] = cFigure::Color(205,79,57);
        colors[lc("tomato4")] = cFigure::Color(139,54,38);
        colors[lc("turquoise")] = cFigure::Color(64,224,208);
        colors[lc("turquoise1")] = cFigure::Color(0,245,255);
        colors[lc("turquoise2")] = cFigure::Color(0,229,238);
        colors[lc("turquoise3")] = cFigure::Color(0,197,205);
        colors[lc("turquoise4")] = cFigure::Color(0,134,139);
        colors[lc("violet")] = cFigure::Color(238,130,238);
        colors[lc("wheat")] = cFigure::Color(245,222,179);
        colors[lc("wheat1")] = cFigure::Color(255,231,186);
        colors[lc("wheat2")] = cFigure::Color(238,216,174);
        colors[lc("wheat3")] = cFigure::Color(205,186,150);
        colors[lc("wheat4")] = cFigure::Color(139,126,102);
        colors[lc("white")] = cFigure::Color(255,255,255);
        colors[lc("yellow")] = cFigure::Color(255,255,0);
        colors[lc("yellow1")] = cFigure::Color(255,255,0);
        colors[lc("yellow2")] = cFigure::Color(238,238,0);
        colors[lc("yellow3")] = cFigure::Color(205,205,0);
        colors[lc("yellow4")] = cFigure::Color(139,139,0);
    }
    std::map<std::string, cFigure::Color>::const_iterator it = colors.find(lc(name));
    if (it == colors.end())
        throw cRuntimeError("No such color: %s", name);
    return it->second;
}

NAMESPACE_END
