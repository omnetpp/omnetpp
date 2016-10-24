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
#include <QIcon>
#include <QTimer>
#include <QFont>
#include <QDebug>

namespace omnetpp {

class cPar;
class cComponent;

namespace qtenv {

class LogInspector;
class ModuleInspector;

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
int getObjectId(cObject *object);
const char *getObjectShortTypeName(cObject *object);
const char *getObjectFullTypeName(cObject *object);

QString makeObjectTooltip(cObject *obj);

LogInspector *isLogInspectorFor(cModule *mod, Inspector *insp);
ModuleInspector *isModuleInspectorFor(cModule *mod, Inspector *insp);

char *voidPtrToStr(void *ptr, char *buffer=nullptr);
void *strToVoidPtr(const char *s);

QDebug& operator<<(QDebug& d, const std::string& t);
QDebug& operator<<(QDebug& d, const SimTime& t);

inline char *ptrToStr(cObject *ptr, char *buffer=nullptr) {return voidPtrToStr((void *)ptr, buffer);}
inline cObject *strToPtr(const char *s) {return (cObject *)strToVoidPtr(s);}

cModule *findCommonAncestor(cModule *src, cModule *dest);

void resolveSendDirectHops(cModule *src, cModule *dest, std::vector<cModule*>& hops);

bool isAPL();

QString getObjectIcon(cObject *object);

const char *getObjectBaseClass(cObject *object);

QString getMessageShortInfoString(cMessage *msg);

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



