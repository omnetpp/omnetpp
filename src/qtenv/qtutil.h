//==========================================================================
//  QTUTIL.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_QTUTIL_H
#define __OMNETPP_QTENV_QTUTIL_H

#include "envir/visitor.h"
#include "omnetpp/cobject.h"
#include "qtenvdefs.h"
#include <QGraphicsObject>
#include <QGraphicsEffect>
#include <QIcon>
#include <QTimer>

namespace omnetpp {

class cPar;
class cComponent;

namespace qtenv {

class LogInspector;
class ModuleInspector;

// does something similar to QGraphicsColorizeEffect,
// but in a way that matches the Tkenv colorization,
// and also preserves smooth scaling of pixmaps
class ColorizeEffect : public QGraphicsEffect {
    QColor color = QColor(255, 0, 0); // alpha is ignored
    double weight = 0; // [0..1]

    // if true, chachedImage has to be recomputed before the next draw
    bool changed = true;
    // we store the colorized pixels here, so we won't have to colorize every frame
    QImage cachedImage;
    QPoint offset;
    bool smooth = false;

public:
    QColor getColor();
    void setColor(const QColor &color);

    double getWeight();
    void setWeight(double weight);

    bool getSmooth();
    void setSmooth(double smooth);

protected:
    void draw(QPainter *painter) override;
    void sourceChanged(ChangeFlags flags) override;
};


// used in the ModuleInspector and some related classes
class GraphicsLayer : public QGraphicsObject {
    Q_OBJECT

public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void addItem(QGraphicsItem *item);
    void removeItem(QGraphicsItem *item);
    void clear();
};


// Used in the ModuleInspector to make text more readable on a cluttered background.
// If the functionality is not enough, just implement more of QGraphicsSimpleTextItems
// functions, and forward them to one or both members accordingly.
// The occasional small "offset" of the outline relative to the text
// itself (mostly with small fonts) is likely caused by font hinting.
class OutlinedTextItem : public QGraphicsItem {
protected:
    // these are NOT PART of the scene, not even children of this object
    // we just misuse them in the paint method
    QGraphicsSimpleTextItem *outlineItem; // never has a Brush
    QGraphicsSimpleTextItem *fillItem; // never has a Pen
    QBrush backgroundBrush;

public:
    OutlinedTextItem(QGraphicsItem *parent = nullptr, QGraphicsScene *scene = nullptr);
    virtual ~OutlinedTextItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setText(const QString &text);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setBackgroundBrush(const QBrush &brush);
};

// Label in the bottom right corner that display zoom factor
class ZoomLabel : public QGraphicsSimpleTextItem
{
private:
    double zoomFactor;

protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override;

public:
    ZoomLabel();

    void setZoomFactor(double zoomFactor);
};

class BubbleItem : public QGraphicsObject {
    Q_OBJECT

protected:

    // the distance between the tip of the handle, and the bottom of the text bounding rectangle
    // includes the bottom margin (the yellow thingy will begin a margin size lower than this)
    static constexpr double vertOffset = 15;
    static constexpr double margin = 3; // also acts as corner rounding radius

    QString text;
    QTimer timer;

    QPainterPath path;
    bool pathBuilt = false;

protected slots:
    void onTimerElapsed();

public:
    BubbleItem(QPointF position, const QString &text, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

// wraps the one in common to be more convenient, also adds support for a fallback color
// and accepts 2 special values: "-" (the fallback), and "transparent" (obvious)
QColor parseColor(const QString &name, const QColor &fallbackColor = QColor());

/**
 * Find objects by full path, and optionally also matching class name and/or Id.
 */
class cFindByPathVisitor : public omnetpp::envir::cCollectObjectsVisitor
{
  private:
    const char *fullPath;
    const char *className; // optional
    long objectId; // optional message or component Id; use -1 for none
  protected:
    virtual void visit(cObject *obj) override;
    bool idMatches(cObject *obj);
  public:
    cFindByPathVisitor(const char *fullPath, const char *className=nullptr, long objectId=-1) :
        fullPath(fullPath), className(className), objectId(objectId) {}
    ~cFindByPathVisitor() {}
};

/*
 * Visitor to collect the descendants of an object which are of type T.
 * T must be a subclass of cObject, and the resulting array should be safely casted to T**.
 */
template<typename T>
class cCollectObjectsOfTypeVisitor : public omnetpp::envir::cCollectObjectsVisitor {
  protected:
    void visit(cObject *obj) override {
        if (dynamic_cast<T*>(obj))
            addPointer(obj);

        // go to children
        obj->forEachChild(this);
    }
};

//
// Utility functions:
//
const char *stripNamespace(const char *className);
const char *getObjectShortTypeName(cObject *object);
const char *getObjectFullTypeName(cObject *object);

LogInspector *isLogInspectorFor(cModule *mod, Inspector *insp);
ModuleInspector *isModuleInspectorFor(cModule *mod, Inspector *insp);

char *voidPtrToStr(void *ptr, char *buffer=nullptr);
void *strToVoidPtr(const char *s);

inline char *ptrToStr(cObject *ptr, char *buffer=nullptr) {return voidPtrToStr((void *)ptr, buffer);}
inline cObject *strToPtr(const char *s) {return (cObject *)strToVoidPtr(s);}

cModule *findCommonAncestor(cModule *src, cModule *dest);

void resolveSendDirectHops(cModule *src, cModule *dest, std::vector<cModule*>& hops);

bool isAPL();

QString getObjectIcon(cObject *object);

const char *getObjectBaseClass(cObject *object);

const char *getMessageShortInfoString(cMessage *msg);

/**
 * Looks up the given parameter
 */
cPar *displayStringPar(const char *parname, cComponent *component, bool searchparent);

/**
 * Returns true if the given (partial or full) display string contains
 * module or channel parameter references ("$param" or "${param}")
 */
bool displayStringContainsParamRefs(const char *dispstr);

/**
 * If the given string is a single parameter reference in the syntax "$name" or
 * "${name}", returns the parameter, otherwise returns nullptr.
 */
cPar *resolveDisplayStringParamRef(const char *dispstr, cComponent *component, bool searchparent);

/**
 * Substitutes module/channel param references into the given (partial)
 * display string. The returned string may be the same as the one passed in,
 * or may point into the buffer std::string.
 */
const char *substituteDisplayStringParamRefs(const char *dispstr, std::string& buffer, cComponent *component, bool searchparent);

bool resolveBoolDispStrArg(const char *arg, cComponent *component, bool defaultValue);
long resolveLongDispStrArg(const char *arg, cComponent *component, int defaultValue);
double resolveDoubleDispStrArg(const char *arg, cComponent *component, double defaultValue);

} // namespace qtenv
} // namespace omnetpp


#endif



