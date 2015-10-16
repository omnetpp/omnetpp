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
#include <QIcon>
#include <QTimer>

namespace omnetpp {

class cPar;
class cComponent;

namespace qtenv {


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
class OutlinedTextItem : public QGraphicsItem {
protected:
    // these are NOT PART of the scene, not even children of this object
    // we just misuse them in the paint method
    QGraphicsSimpleTextItem *outlineItem; // never has a Brush
    QGraphicsSimpleTextItem *fillItem; // never has a Pen

public:
    OutlinedTextItem(QGraphicsItem *parent = nullptr, QGraphicsScene *scene = nullptr);
    virtual ~OutlinedTextItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setText(const QString &text);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
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
/*TCLKILL
//
// In some installations Tcl headers files have 'char*' without 'const char*'
// in arg lists -- we have to cast away 'const char*' from args in our Tcl calls.
//
#define TCLCONST(x)   const_cast<char*>(x)

//
// Print error message on console if Tcl code returns error
//
#ifdef _NDEBUG
#define CHK(tcl_eval_statement)   tcl_eval_statement
#else
#define CHK(tcl_eval_statement)    \
  do { if (tcl_eval_statement==TCL_ERROR) \
        logTclError(__FILE__,__LINE__, interp); \
  } while(0)
#endif

//
// Turns exceptions into Tcl errors
//
#define TRY(code) \
  try {code;} catch (std::exception& e) { \
      Tcl_SetResult(interp, TCLCONST(e.what()), TCL_VOLATILE); \
      return TCL_ERROR; \
  }

#define E_TRY   try {

#define E_CATCH \
  } catch (std::exception& e) { \
      Tcl_SetResult(interp, TCLCONST(e.what()), TCL_VOLATILE); \
      return TCL_ERROR; \
  }



//TCLKILL  Quoting strings for Tcl_Eval
class QTENV_API TclQuotedString
{
  private:
    char buf[80];  // used only if string is short, to spare alloc/dealloc
    char *quotedstr;
  public:
    TclQuotedString();
    TclQuotedString(const char *s);
    TclQuotedString(const char *s, int n);

    void set(const char *s);
    void set(const char *s, int n);

    ~TclQuotedString();
    const char *get() {return quotedstr;}
    operator const char *() {return quotedstr;}
};
*/
/**
 * Find objects by full path, and optionally also matching class name and/or Id.
 */
class QTENV_API cFindByPathVisitor : public omnetpp::envir::cCollectObjectsVisitor
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

//
// Utility functions:
//
const char *stripNamespace(const char *className);
const char *getObjectShortTypeName(cObject *object);
const char *getObjectFullTypeName(cObject *object);

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
/*TCLKILL
void setObjectListResult(Tcl_Interp *interp, omnetpp::envir::cCollectObjectsVisitor *visitor);

void insertIntoInspectorListbox(Tcl_Interp *interp, const char *listbox, cObject *obj, bool fullpath);

void feedCollectionIntoInspectorListbox(omnetpp::envir::cCollectObjectsVisitor *visitor, Tcl_Interp *interp, const char *listbox, bool fullpath);

int fillListboxWithChildObjects(cObject *object, Tcl_Interp *interp, const char *listbox, bool deep);
*/
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
/*TCLKILL
void logTclError(const char *file, int line, Tcl_Interp *interp);


typedef int (*TclCmdProc)(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);

void invokeTclCommand(Tcl_Interp *interp, Tcl_CmdInfo *cmd, int argc, const char *argv[]);
*/
} // namespace qtenv
} // namespace omnetpp


#endif



