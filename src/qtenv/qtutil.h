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
QColor parseColor(const QString &name, const QColor &fallbackColor = QColor());

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

QTENV_API QString makeObjectTooltip(cObject *obj);

const QChar *skipEscapeSequences(const QChar *start);
QString stripFormatting(const QString& input);
QString stripSuffixes(const QString& from, const QString& suffix);

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

/**
 * Get linear ticks. This vector contains minor and major ticks.
 * Major ticks' second parameter is true.
 */
std::vector<std::pair<ChartTickDecimal, bool>> getLinearTicks(double start, double end, double approxDelta);


inline double clip(double min, double v, double max)
{
    return std::min(std::max(v, min), max);
}


template <typename K, typename V>
class QTENV_API OrderedMultiMap {

    using keys_t = std::multimap<K, size_t>;
    using values_t = std::vector<V>;

    // this multimap indexes into the values vector
    keys_t keys;
    // these are the values themselves
    values_t values;

    // Does not touch the values vector, only updating the keys map.
    // Erases the single element corresponding to the i'th element,
    // and decreases all indices that are greater than i,
    // so the keys map will be correct if the i'th value is erased.
    void removeIndex(size_t i) {
        auto it = keys.begin();
        while (it != keys.end()) {
            if (it->second == i) {
                it = keys.erase(it);
                continue;
            }

            if (it->second > i)
                --(it->second);

            ++it;
        }
    }

public:

    // operator[] would be misleading, since I don't want it to work like in std::map
    // (the implicit insertion of a default value upon addressing a previously non-existent key thing)

    V& getFirst(const K& k) {
        auto index = keys.find(k);
        ASSERT(index != keys.end());
        ASSERT(index->second >= 0 && index->second < values.size());
        return values[index->second];
    }

    V& getLast(const K& k) {
        auto range = keys.equal_range(k);
        ASSERT(range.first != range.second);
        return values[(--range.second)->second];
    }

    /*
    std::vector<V&> getAll(const K& k) {
        auto index = keys.find(k);
        ASSERT(index != keys.end());
        ASSERT(index->second >= 0 && index->second < values.size());
        return {values[index->second]};
    }*/

    void putSingle(const K& k, const V& v) {
        ASSERT(!containsKey(k));
        putMulti(k, v);
    }

    void putMulti(const K& k, const V& v) {
        keys.insert({k, values.size()});
        values.push_back(v);
    }

    bool containsKey(const K& k) const {
        return keys.find(k) != keys.end();
    }

    std::vector<K> getKeys() const {
        std::vector<K> result;
        for (auto it = keys.begin(); it != keys.end(); it = keys.upper_bound(it->first))
            result.push_back(it->first);
        return result;
    }

    void removeFirst(const K& k) {
        //ASSERT(containsKey(k));
        auto it = keys.find(k);
        if (it != keys.end()) {
            removeIndex(it->second);
            values.erase(values.begin() + it->second);
        }
    }

    /*
    void removeAll(const K &k) {
        //ASSERT(containsKey(k));

    }*/

    void removeValues(const V& v) {
        size_t i = 0;
        while (i < values.size()) {
            if (values[i] == v) {
                removeIndex(i); // this adjusts the keys map
                values.erase(values.begin() + i);
            } else
                ++i;
        }
    }

    bool empty() {
        return values.empty();
    }

    void clear() {
        keys.clear();
        values.clear();
    }

    typename values_t::iterator begin() { return values.begin(); }
    typename values_t::iterator end() { return values.end(); }
};


}  // namespace qtenv
}  // namespace omnetpp


#endif
