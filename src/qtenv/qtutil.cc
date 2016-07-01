//==========================================================================
//  QTUTIL.CC - part of
//
//                    OMNeT++/OMNEST
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

// Work around an MSVC .NET bug: WinSCard.h causes compilation
// error due to faulty LPCVOID declaration, so prevent it from
// being pulled in (we don't need the SmartCard API here anyway ;-)
#define _WINSCARD_H_

#include <cstdio>
#include <cstring>
#include <ctime>
#include <cassert>
#include <sstream>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsEffect>
#include <QDebug>

#include "common/stringutil.h"
#include "common/colorutil.h"
#include "common/opp_ctype.h"
#include "common/patternmatcher.h"
#include "common/ver.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/cregistrationlist.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/coutvector.h"
#include "omnetpp/ccanvas.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/carray.h"
#include "omnetpp/cwatch.h"
#include "qtenv.h"
#include "qtutil.h"
#include "moduleinspector.h"
#include "loginspector.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {


// ---- GraphicsLayer implementation ----

QRectF GraphicsLayer::boundingRect() const {
    return QRectF(); // doesn't matter
}

void GraphicsLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // nothing here...
}

void GraphicsLayer::addItem(QGraphicsItem *item) {
    item->setParentItem(this);
}

void GraphicsLayer::removeItem(QGraphicsItem *item) {
    item->setParentItem(nullptr);
}

void GraphicsLayer::clear() {
    while (!childItems().isEmpty())
        delete childItems()[0];
}

// ---- end of GraphicsLayer ----

// ---- ZoomLabel implementation ----

ZoomLabel::ZoomLabel()
{
    setText("ZoomLabel");
}

void ZoomLabel::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    QFont f = font();
    f.setBold(true);
    painter->setFont(f);
    painter->setBrush(Qt::lightGray);
    painter->setPen(Qt::lightGray);
    painter->drawRect(boundingRect());

    QGraphicsSimpleTextItem::paint(painter, option, widget);
}

void ZoomLabel::setZoomFactor(double zoomFactor)
{
    if(this->zoomFactor == zoomFactor)
        return;

    this->zoomFactor = zoomFactor;
    setText(" Zoom:" + QString::number(zoomFactor, 'f', 2) + "x");
    update();
}

// ---- end of ZoomLabel ----

// ---- ColorizeEffect implementation ----

QColor ColorizeEffect::getColor() {
    return color;
}

void ColorizeEffect::setColor(const QColor &color) {
    if (this->color != color) {
        this->color = color;
        changed = true;
        update();
    }
}

double ColorizeEffect::getWeight() {
    return weight;
}

void ColorizeEffect::setWeight(double weight) {
    if (this->weight != weight) {
        this->weight = weight;
        changed = true;
        update();
    }
}

bool ColorizeEffect::getSmooth() {
    return smooth;
}

void ColorizeEffect::setSmooth(double smooth) {
    if (this->smooth != smooth) {
        this->smooth = smooth;
        smooth = true;
        update();
    }
}

void ColorizeEffect::draw(QPainter *painter) {
    if (changed) {
        cachedImage = sourcePixmap(Qt::LogicalCoordinates, &offset, NoPad).toImage();

        if (weight > 0) {
            double rdest = color.redF();
            double gdest = color.greenF();
            double bdest = color.blueF();

            for (int y = 0; y < cachedImage.height(); y++) {
                QRgb *scanLine = reinterpret_cast<QRgb*>(cachedImage.scanLine(y));
                for (int x = 0; x < cachedImage.width(); x++) {
                    QRgb &pixel = scanLine[x];

                    int r = qRed(pixel);
                    int g = qGreen(pixel);
                    int b = qBlue(pixel);

                    // transform - code taken from tkenv/tkcmd.cc (colorizeImage_cmd)
                    int lum = (int)(0.2126*r + 0.7152*g + 0.0722*b);
                    r = (int)((1-weight)*r + weight*lum*rdest);
                    g = (int)((1-weight)*g + weight*lum*gdest);
                    b = (int)((1-weight)*b + weight*lum*bdest);

                    pixel = qRgba(r, g, b, qAlpha(pixel));
                }
            }
        }

        changed = false;
    }

    painter->save();
    painter->setRenderHint(QPainter::SmoothPixmapTransform, smooth);
    painter->drawImage(cachedImage.rect().translated(offset), cachedImage);
    painter->restore();
}

void ColorizeEffect::sourceChanged(QGraphicsEffect::ChangeFlags flags) {
    if (flags.testFlag(SourceAttached)
            || flags.testFlag(SourceDetached)
            || flags.testFlag(SourceBoundingRectChanged))
        changed = true;
    update();
}

// ---- end of ColorizeEffect ----


// ---- OutlinedTextItem implementation ----

OutlinedTextItem::OutlinedTextItem(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItem(parent)
{
    outlineItem = new QGraphicsSimpleTextItem();
    outlineItem->setFont(getQtenv()->getCanvasFont());
    fillItem = new QGraphicsSimpleTextItem();
    fillItem->setFont(getQtenv()->getCanvasFont());

    outlineItem->setBrush(Qt::NoBrush);
    QColor col = parseColor("grey82");
    col.setAlphaF(0.6); // 4 pixels wide, so 2 pixels will go outwards
    outlineItem->setPen(QPen(col, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    fillItem->setPen(Qt::NoPen);
    // the default brush is good for fillItem

    // Makes scrolling the module (with many submodules) a lot faster while
    // not degrading performance, but using (hopefully just) a bit more memory.
    // Needed because text outline drawing is very slow in Qt (4.8).
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

OutlinedTextItem::~OutlinedTextItem() {
    delete fillItem;
    delete outlineItem;
}

void OutlinedTextItem::setText(const QString &text) {
    if (text != fillItem->text()) {
        fillItem->setText(text);
        outlineItem->setText(text);
        update();
    }
}

QRectF OutlinedTextItem::boundingRect() const {
    return fillItem->boundingRect().united(outlineItem->boundingRect());
}

void OutlinedTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->save();
    painter->setBrush(backgroundBrush);
    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());
    painter->restore();
    outlineItem->paint(painter, option, widget);
    fillItem->paint(painter, option, widget);
}

void OutlinedTextItem::setPen(const QPen &pen) {
    if (pen != outlineItem->pen()) {
        outlineItem->setPen(pen);
        update();
    }
}

void OutlinedTextItem::setBrush(const QBrush &brush) {
    if (brush != fillItem->brush()) {
        fillItem->setBrush(brush);
        update();
    }
}

void OutlinedTextItem::setBackgroundBrush(const QBrush &brush)
{
    if (backgroundBrush != brush) {
        backgroundBrush = brush;
        update();
    }
}

// ---- end of OutlinedTextItem ----


// ---- BubbleItem implementation ----

BubbleItem::BubbleItem(QPointF position, const QString &text, QGraphicsItem *parent)
    : QGraphicsObject(parent), text(text)
{
    setPos(position);
    timer.singleShot(1000 / (0.1 + getQtenv()->opt->animationSpeed), this, SLOT(onTimerElapsed()));
}

void BubbleItem::onTimerElapsed() {
    delete this; // BOOM!
}

QRectF BubbleItem::boundingRect() const {
    QFontMetrics metrics(getQtenv()->getCanvasFont());
    double textWidth = metrics.width(text);
    double textHeight = metrics.height();

    // the -1 and +2 is the line width, and also "just to be safe".
    return QRectF(-textWidth/2 - margin - 1,
                  -vertOffset - textHeight - margin - 1,
                  textWidth + margin*2 + 2,
                  margin + textHeight + vertOffset + 2);
}

void BubbleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QFontMetrics metrics(getQtenv()->getCanvasFont());

    double textWidth = metrics.width(text);
    double textHeight = metrics.height();

    if (!pathBuilt) {
        // will translate into the 4 corners
        QRectF arcRect(-margin, -margin, margin*2, margin*2);

        path.moveTo(0, 0);
        path.lineTo(0, -vertOffset + margin); // moving up (left edge of the triangle)
        path.arcTo(arcRect.translated(-textWidth/2, -vertOffset),               -90, -90); // lower left
        path.arcTo(arcRect.translated(-textWidth/2, -vertOffset - textHeight), -180, -90); // upper left
        path.arcTo(arcRect.translated( textWidth/2, -vertOffset - textHeight),   90, -90); // upper right
        path.arcTo(arcRect.translated( textWidth/2, -vertOffset),                 0, -90); // lower right
        path.lineTo(std::min(textWidth/2, vertOffset/2), -vertOffset + margin); // moving a bit to the left (the right half of the bottom edge)
        path.closeSubpath(); // the right edge of the triangle

        pathBuilt = true;
    }

    painter->save();
    painter->setPen(QPen(QColor("black"), 0));
    painter->setBrush(QColor(248, 248, 216)); // yellowish
    painter->drawPath(path);
    // uses the pen as color, also have to move up by the descent, to align the baseline
    painter->setFont(getQtenv()->getCanvasFont());
    painter->drawText(-textWidth/2, -vertOffset - metrics.descent(), text);
    painter->restore();
}

// ---- end of BubbleItem ----


QColor parseColor(const QString &name, const QColor &fallbackColor) {
    if (name.isEmpty())
        return fallbackColor;

    if (name == "-")
        return QColor("transparent");

    try {
        uint8_t r, g, b;
        omnetpp::common::parseColor(name.toStdString().c_str(), r, g, b);
        return QColor(r, g, b);
    } catch (std::runtime_error &e) {
        qDebug() << "Failed to parse color" << name << "because:" << e.what();
        return fallbackColor;
    }
}


#define INSPECTORLISTBOX_MAX_ITEMS    100000

//----------------------------------------------------------------------

void cFindByPathVisitor::visit(cObject *obj)
{
    // we have to do exhaustive search here, because full path may not exactly
    // follow the object hierarchy (some objects may be omitted)
    std::string objPath = obj->getFullPath();

    // however, a module's name and the future event set's name is not hidden,
    // so if this object is a module or the FES and its name does not match
    // the beginning of fullpath, we can prune the search here.
    if ((dynamic_cast<cModule *>(obj) || dynamic_cast<cFutureEventSet *>(obj))
        && !opp_stringbeginswith(fullPath, objPath.c_str()))
    {
        // skip (do not search) this subtree
        return;
    }

    // found it?
    if (strcmp(fullPath, objPath.c_str()) == 0
        && (!className || strcmp(className, getObjectShortTypeName(obj)) == 0)
        && (objectId == -1 || idMatches(obj)))
    {
        // found, collect it
        addPointer(obj);

        // makes no sense to go further down
        return;
    }

    // search recursively
    obj->forEachChild(this);
}

bool cFindByPathVisitor::idMatches(cObject *obj)
{
    // only messages and components have IDs
    if (cMessage *msg = dynamic_cast<cMessage *>(obj))
        return msg->getId() == objectId;
    if (cComponent *component = dynamic_cast<cComponent*>(obj))
        return component->getId() == objectId;
    return true;
}

// =======================================================================

#define TRY2(CODE)    try { CODE; } catch (std::exception& e) { printf("<!> Warning: %s\n", e.what()); }

const char *stripNamespace(const char *className)
{
    switch (getQtenv()->opt->stripNamespace) {
        case STRIPNAMESPACE_ALL: {
            const char *lastColon = strrchr(className, ':');
            if (lastColon)
                className = lastColon+1;
            break;
        }

        case STRIPNAMESPACE_OMNETPP: {
            if (className[0] == 'o' && opp_stringbeginswith(className, "omnetpp::"))
                className += sizeof("omnetpp::")-1;
            break;
        }

        case STRIPNAMESPACE_NONE:
            break;
    }
    return className;
}

const char *getObjectShortTypeName(cObject *object)
{
    if (cComponent *component = dynamic_cast<cComponent *>(object))
        TRY2(return component->getComponentType()->getName());
    return stripNamespace(object->getClassName());
}

const char *getObjectFullTypeName(cObject *object)
{
    if (cComponent *component = dynamic_cast<cComponent *>(object))
        TRY2(return component->getComponentType()->getFullName());
    return object->getClassName();
}

char *voidPtrToStr(void *ptr, char *buffer)
{
    static char staticbuf[20];
    if (buffer == nullptr)
        buffer = staticbuf;

    if (ptr == nullptr)
        strcpy(buffer, "ptr0");  // GNU C++'s sprintf() says "nil"
    else
        sprintf(buffer, "ptr%p", ptr);
    return buffer;
}

void *strToVoidPtr(const char *s)
{
    // accept "" and malformed strings too, and return them as nullptr
    if (s[0] == 'p' && s[1] == 't' && s[2] == 'r')
        s += 3;
    else if (s[0] == '0' && s[1] == 'x')
        s += 2;
    else
        return nullptr;

    void *ptr;
    sscanf(s, "%p", &ptr);
    return ptr;
}

// -----------------------------------------------------------------------

QString getObjectIcon(cObject *object)
{
    const char *iconName;
    if (object == nullptr)
        iconName = "none_vs";
    else if (dynamic_cast<cModule *>(object) && ((cModule *)object)->isPlaceholder())
        iconName = "placeholder_vs";
    else if (dynamic_cast<cSimpleModule *>(object))
        iconName = "simple_vs";
    else if (dynamic_cast<cModule *>(object))
        iconName = "compound_vs";
    else if (dynamic_cast<cWatchBase *>(object))
        iconName = "cogwheel_vs";
    else if (dynamic_cast<cMessage *>(object))
        iconName = "message_vs";
    else if (dynamic_cast<cArray *>(object))
        iconName = "container_vs";
    else if (dynamic_cast<cQueue *>(object))
        iconName = "queue_vs";
    else if (dynamic_cast<cGate *>(object))
        iconName = "gate_vs";
    else if (dynamic_cast<cPar *>(object))
        iconName = "param_vs";
    else if (dynamic_cast<cChannel *>(object))
        iconName = "chan_vs";
    else if (dynamic_cast<cOutVector *>(object))
        iconName = "outvect_vs";
    else if (dynamic_cast<cStatistic *>(object))
        iconName = "stat_vs";
    else if (dynamic_cast<cFigure *>(object))
        iconName = "figure_vs";
    else if (dynamic_cast<cCanvas *>(object))
        iconName = "canvas_vs";
    else if (dynamic_cast<cSimulation *>(object))
        iconName = "container_vs";
    else if (dynamic_cast<cFutureEventSet *>(object))
        iconName = "container_vs";
    else if (dynamic_cast<cRegistrationList *>(object))
        iconName = "container_vs";
    else
        iconName = "cogwheel_vs";

    return iconName;
}

const char *getObjectBaseClass(cObject *object)
{
    if (dynamic_cast<cModule *>(object) && ((cModule *)object)->isPlaceholder())
        return "cPlaceholderModule";
    else if (dynamic_cast<cSimpleModule *>(object))
        return"cSimpleModule";
    else if (dynamic_cast<cModule *>(object))
        return"cModule";
    else if (dynamic_cast<cMessage *>(object))
        return"cMessage";
    else if (dynamic_cast<cArray *>(object))
        return"cArray";
    else if (dynamic_cast<cQueue *>(object))
        return"cQueue";
    else if (dynamic_cast<cGate *>(object))
        return"cGate";
    else if (dynamic_cast<cPar *>(object))
        return"cPar";
    else if (dynamic_cast<cChannel *>(object))
        return"cChannel";
    else if (dynamic_cast<cOutVector *>(object))
        return"cOutVector";
    else if (dynamic_cast<cStatistic *>(object))
        return"cStatistic";
    else if (dynamic_cast<cFutureEventSet *>(object))
        return"cFutureEventSet";
    else if (dynamic_cast<cWatchBase *>(object))
        return"cWatchBase";
    else if (dynamic_cast<cCanvas *>(object))
        return"cCanvas";
    else if (dynamic_cast<cFigure *>(object))
        return"cFigure";
    else if (dynamic_cast<cSimulation *>(object))
        return"cSimulation";
    else if (dynamic_cast<cRegistrationList *>(object))
        return"cRegistrationList";
    else
        return object->getClassName();  // return itself as base class
}

const char *getMessageShortInfoString(cMessage *msg)
{
    cModule *tomodp = msg->getArrivalModule();
    cModule *frommodp = msg->getSenderModule();
    std::stringstream out;
    const char *deletedstr = "<deleted module>";

#define MODNAME(modp)    ((modp) ? (modp)->getFullPath().c_str() : deletedstr)
    if (!tomodp)
        out << "new msg";
    else if (msg->getKind() == MK_STARTER)
        out << "starter for " << MODNAME(tomodp);
    else if (msg->getKind() == MK_TIMEOUT)
        out << "timeoutmsg for " << MODNAME(tomodp);
    else if (frommodp == tomodp)
        out << "selfmsg for " << MODNAME(tomodp);
    else
        out << "msg for " << MODNAME(tomodp);
#undef MODNAME

    return out.str().c_str();
}

cModule *findCommonAncestor(cModule *src, cModule *dest)
{
    cModule *candidate = src;
    while (candidate) {
        // try to find common ancestor among ancestors of dest
        cModule *m = dest;
        while (m && candidate != m)
            m = m->getParentModule();
        if (candidate == m)
            break;
        candidate = candidate->getParentModule();
    }
    return candidate;
}

void resolveSendDirectHops(cModule *src, cModule *dest, std::vector<cModule *>& hops)
{
    // find common ancestor, and record modules from src up to it;
    // the ancestor module itself is NOT recorded
    cModule *ancestor = src;
    while (ancestor) {
        // is 'ancestor' also an ancestor of dest? if so, break!
        cModule *m = dest;
        while (m && ancestor != m)
            m = m->getParentModule();
        if (ancestor == m)
            break;
        hops.push_back(ancestor);
        ancestor = ancestor->getParentModule();
    }
    ASSERT(ancestor != nullptr);

    if (src == ancestor)
        hops.push_back(src);

    if (dest == ancestor)
        hops.push_back(dest);
    else {
        // ascend from dest up to the common ancestor, and record modules in reverse order
        cModule *m = dest;
        int pos = hops.size();
        while (m && ancestor != m) {
            hops.insert(hops.begin()+pos, m);
            m = m->getParentModule();
        }
        ASSERT(m == ancestor);
    }
}

bool isAPL()
{
    return OMNETPP_EDITION[0] == 'A';
}

// ----------------------------------------------------------------------

cPar *displayStringPar(const char *parname, cComponent *component, bool searchparent)
{
    // look up locally
    cPar *par = nullptr;
    int k = component->findPar(parname);
    if (k >= 0)
        par = &(component->par(k));

    // look up in parent
    if (!par && searchparent && component->getParentModule()) {
        k = component->getParentModule()->findPar(parname);
        if (k >= 0)
            par = &(component->getParentModule()->par(k));
    }
    if (!par) {
        // not found -- generate suitable error message
        const char *what = component->isModule() ? "module" : "channel";
        if (!searchparent)
            throw cRuntimeError("%s `%s' has no parameter `%s'", what, component->getFullPath().c_str(), parname);
        else
            throw cRuntimeError("%s `%s' and its parent have no parameter `%s'", what, component->getFullPath().c_str(), parname);
    }
    return par;
}

bool displayStringContainsParamRefs(const char *dispstr)
{
    for (const char *s = dispstr; *s; s++)
        if (*s == '$' && (*(s+1) == '{' || opp_isalphaext(*(s+1))))
            return true;

    return false;
}

cPar *resolveDisplayStringParamRef(const char *dispstr, cComponent *component, bool searchparent)
{
    if (dispstr[0] != '$')
        return nullptr;
    if (dispstr[1] != '{')
        return displayStringPar(dispstr+1, component, searchparent);  // rest of string is assumed to be the param name
    else if (dispstr[strlen(dispstr)-1] != '}')
        return nullptr;  // unterminated brace (or close brace not the last char)
    else
        // starts with "${" and ends with "}" -- everything in between is taken to be the parameter name
        return displayStringPar(std::string(dispstr+2, strlen(dispstr)-3).c_str(), component, searchparent);
}

const char *substituteDisplayStringParamRefs(const char *src, std::string& buffer, cComponent *component, bool searchparent)
{
    if (!strchr(src, '$') || !component)  // cannot resolve args if component==nullptr
        return src;

    // recognize "$param" and "${param}" syntax inside the string
    buffer = "";
    for (const char *s = src; *s; ) {
        if (*s != '$')
            buffer += *s++;
        else {
            // extract parameter name
            s++;  // skip '$'
            std::string name;
            if (*s == '{') {
                s++;  // skip '{'
                while (*s && *s != '}')
                    name += *s++;
                if (*s)
                    s++;  // skip '}'
            }
            else {
                while (opp_isalnumext(*s) || *s == '_')
                    name += *s++;
            }

            // append its value
            cPar *par = displayStringPar(name.c_str(), component, searchparent);
            switch (par->getType()) {
              case cPar::BOOL: buffer += (par->boolValue() ? "1" : "0"); break;
              case cPar::STRING: buffer += par->stdstringValue(); break;
              case cPar::LONG: buffer += opp_stringf("%ld", par->longValue()); break;
              case cPar::DOUBLE: buffer += opp_stringf("%g", par->doubleValue()); break;
              default: throw cRuntimeError("Cannot substitute parameter %s into display string: wrong data type", par->getFullPath().c_str());
            }
        }
    }

    return buffer.c_str();
}

bool resolveBoolDispStrArg(const char *arg, cComponent *component, bool defaultValue)
{
    if (!arg || !*arg)
        return defaultValue;
    if (!displayStringContainsParamRefs(arg))
        return strcmp("0", arg) != 0 && strcmp("false", arg) != 0;  // not 0 and not false
    cPar *par = resolveDisplayStringParamRef(arg, component, true);
    if (par && par->getType() == cPar::BOOL)
        return par->boolValue();
    std::string buffer;
    const char *arg2 = substituteDisplayStringParamRefs(arg, buffer, component, true);
    return strcmp("0", arg2) != 0 && strcmp("false", arg2) != 0;  // not 0 and not false
}

long resolveLongDispStrArg(const char *arg, cComponent *component, int defaultValue)
{
    if (!arg || !*arg)
        return defaultValue;
    if (!displayStringContainsParamRefs(arg))
        return (long)atol(arg);
    cPar *par = resolveDisplayStringParamRef(arg, component, true);
    if (par && par->isNumeric())
        return par->longValue();
    std::string buffer;
    const char *arg2 = substituteDisplayStringParamRefs(arg, buffer, component, true);
    return (long)atol(arg2);
}

double resolveDoubleDispStrArg(const char *arg, cComponent *component, double defaultValue)
{
    if (!arg || !*arg)
        return defaultValue;
    if (!displayStringContainsParamRefs(arg))
        return atof(arg);
    cPar *par = resolveDisplayStringParamRef(arg, component, true);
    if (par && par->isNumeric())
        return par->doubleValue();
    std::string buffer;
    const char *arg2 = substituteDisplayStringParamRefs(arg, buffer, component, true);
    return atof(arg2);
}

LogInspector *isLogInspectorFor(cModule *mod, Inspector *insp)
{
    if (insp->getObject() != mod || insp->getType() != INSP_MODULEOUTPUT)
        return nullptr;
    return dynamic_cast<LogInspector *>(insp);
}

ModuleInspector *isModuleInspectorFor(cModule *mod, Inspector *insp)
{
    if (insp->getObject() != mod || insp->getType() != INSP_GRAPHICAL)
        return nullptr;
    return dynamic_cast<ModuleInspector *>(insp);
}

} // namespace qtenv
} // namespace omnetpp

