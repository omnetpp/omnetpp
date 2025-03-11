//==========================================================================
//  QTUTIL.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_QTUTIL_H
#define __OMNETPP_QTENV_QTUTIL_H

#include "envir/visitor.h"
#include "omnetpp/cobject.h"
#include "omnetpp/cdisplaystring.h"
#include "qtenvdefs.h"
#include "charttickdecimal.h"
#include <QtGui/QIcon>
#include <QtCore/QTimer>
#include <QtGui/QFont>
#include <QtCore/QDebug>

namespace omnetpp {

class cPar;
class cComponent;

namespace qtenv {

enum StripNamespace
{
    STRIPNAMESPACE_NONE,
    STRIPNAMESPACE_OMNETPP,
    STRIPNAMESPACE_ALL
};

class LogInspector;
class ModuleInspector;

// wraps the one in common to be more convenient, also adds support for a fallback color
// and accepts 2 special values: "-" (the fallback), and "transparent" (obvious)
QColor parseColor(const char *name, const QColor &fallbackColor = QColor());

// A collection of global QColor constant instances used throughout various parts of Qtenv.
// Their names don't necessarily coincide with any existing X11 or SVG color names.
// They are collected here to avoid looking up the colors by name at every usage.
namespace colors {
    extern const QColor BLACK;
    extern const QColor TRANSPARENT;
    extern const QColor GREY;
    extern const QColor GREY82;
    extern const QColor LIGHTGREY;
    extern const QColor RED;
    extern const QColor DARKGREEN;
    extern const QColor LIGHTGREEN;
    extern const QColor BLUE;
    extern const QColor PURPLE;
}

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
    virtual bool visit(cObject *obj) override;
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
class QTENV_API cCollectObjectsOfTypeVisitor : public omnetpp::envir::cCollectObjectsVisitor {
  protected:
    bool visit(cObject *obj) override {
        if (dynamic_cast<T*>(obj))
            addPointer(obj);

        // go to children
        obj->forEachChild(this);
        return true;
    }
};

//
// Utility functions:
//
long getObjectId(cObject *object);
const QString& stripNamespace(const char *className); // uses the current setting in getQtenv()->opt
const QString& stripNamespace(const char *className, StripNamespace stripMode);
const QString &getObjectShortTypeName(cObject *object); // uses the current setting in getQtenv()->opt
const QString &getObjectShortTypeName(cObject *object, StripNamespace stripMode);
const char *getObjectFullTypeName(cObject *object);

QTENV_API QString makeObjectTooltip(cObject *obj, bool verboseTooltip);
QTENV_API std::string getComponentDocumentationForTooltip(cComponent *comp);

const std::string& stripFormatting(const std::string& input, std::string& buffer);

const char *skipEscapeSequences(const char *start);
std::string stripFormattingAndRemoveTrailingNewLine(const std::string& input);
std::string stripSuffixes(const std::string& from, const std::string& suffix);
std::string stripSuffixes(const std::string& from, char suffix);


enum SearchFlag {
    FIND_REGULAR_EXPRESSION = 1<<0, // 0b0001
    FIND_WHOLE_WORDS        = 1<<1, // 0b0010
    FIND_CASE_SENSITIVE     = 1<<2, // 0b0100
    FIND_BACKWARDS          = 1<<3, // 0b1000
};

Q_DECLARE_FLAGS(SearchFlags, SearchFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(SearchFlags)

struct SearchResult {
    const char *matchStart = nullptr;
    int matchLength = 0;
};

SearchResult findSubstring(const char *haystack, const char *needle, int startIndex, SearchFlags flags);

LogInspector *isLogInspectorFor(cModule *mod, Inspector *insp);
ModuleInspector *isModuleInspectorFor(cModule *mod, Inspector *insp);

char *voidPtrToStr(void *ptr, char *buffer=nullptr);
void *strToVoidPtr(const char *s);

QDebug& operator<<(QDebug& d, const std::string& t);
QDebug& operator<<(QDebug& d, const SimTime& t);

inline char *ptrToStr(cObject *ptr, char *buffer=nullptr) {return voidPtrToStr((void *)ptr, buffer);}
inline cObject *strToPtr(const char *s) {return (cObject *)strToVoidPtr(s);}

// Prevent the Debug dialog from kicking in if there is an exception in GUI code
struct DisableDebugOnErrors {
    bool oldValue;
    DisableDebugOnErrors();
    ~DisableDebugOnErrors();
};

cModule *findCommonAncestor(cModule *src, cModule *dest);

void resolveSendDirectHops(cModule *src, cModule *dest, std::vector<cModule*>& hops);

bool isAPL();


QIcon getObjectIcon(cObject *object);
const char *getObjectIconName(cObject *object);

const char *getObjectBaseClass(cObject *object);

QString getMessageShortInfoString(cMessage *msg);

cGate *getGateOtherHalf(cGate *gate);
// check if this is a two way connection (an other connection is pointing back
// to the this gate's pair from the next gate's pair)
bool isTwoWayConnection(cGate *gate);



/**
 * Get linear ticks. This vector contains minor and major ticks.
 * Major ticks' second parameter is true.
 */
std::vector<std::pair<ChartTickDecimal, bool>> getLinearTicks(double start, double end, double approxDelta);


inline double clip(double min, double v, double max)
{
    return std::min(std::max(v, min), max);
}


}  // namespace qtenv
}  // namespace omnetpp


#endif
