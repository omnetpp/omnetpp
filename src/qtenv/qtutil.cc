//==========================================================================
//  QTUTIL.CC - part of
//
//                    OMNeT++/OMNEST
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

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
#include <cinttypes> // PRId64
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsEffect>
#include <QtCore/QDebug>

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
#include "omnetpp/cpacket.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cresultlistener.h"
#include "omnetpp/cresultfilter.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/cresultrecorder.h"
#include "omnetpp/coutvector.h"
#include "omnetpp/ccanvas.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/carray.h"
#include "omnetpp/cwatch.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cdynamicexpression.h"
#include "qtenv.h"
#include "qtutil.h"
#include "charttickdecimal.h"
#include "moduleinspector.h"
#include "loginspector.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {

#define INSPECTORLISTBOX_MAX_ITEMS    100000

QColor parseColor(const char *name, const QColor& fallbackColor)
{
    if (opp_isempty(name))
        return fallbackColor;

    if (opp_streq(name, "-"))
        return colors::TRANSPARENT;

    try {
        uint8_t r, g, b;
        common::parseColor(name, r, g, b);
        return QColor(r, g, b);
    }
    catch (std::runtime_error& e) {
        qDebug() << "Failed to parse color " << name << " because:" << e.what();
        return fallbackColor;
    }
}

namespace colors {
    const QColor BLACK("black");
    const QColor TRANSPARENT("transparent");
    const QColor GREY("grey");
    const QColor GREY82("grey82");
    const QColor LIGHTGREY("#eeeeee");
    const QColor RED("red");
    const QColor DARKGREEN("#005030");
    const QColor LIGHTGREEN("#a0e0a0");
    const QColor BLUE("blue");
    const QColor PURPLE("#8080ff");
}

//----------------------------------------------------------------------

bool cFindByPathVisitor::visit(cObject *obj)
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
        return true;
    }

    // found it?
    if (strcmp(fullPath, objPath.c_str()) == 0
        && (!className || (getObjectShortTypeName(obj, STRIPNAMESPACE_NONE) == className))
        && (objectId == -1 || objectId == getObjectId(obj)))
    {
        // found, collect it
        addPointer(obj);

        // makes no sense to go further down
        return true;
    }

    // search recursively
    obj->forEachChild(this);
    return true;
}

// =======================================================================

#define TRY2(CODE)    try { CODE; } catch (std::exception& e) { printf("<!> Warning: %s\n", e.what()); }


long getObjectId(cObject *object)
{
    long id = -1;
    if (cMessage *msg = dynamic_cast<cMessage *>(object))
        id = msg->getId();
    if (cComponent *component = dynamic_cast<cComponent *>(object))
        id = component->getId();
    return id;
}

const QString& stripNamespace(const char *className)
{
    return stripNamespace(className, getQtenv()->opt->stripNamespace);
}

const QString& stripNamespace(const char *className, StripNamespace stripMode)
{
    static std::unordered_map<const char *, QString> allStrippedCache;
    static std::unordered_map<const char *, QString> omnetppStrippedCache;
    static std::unordered_map<const char *, QString> noneStrippedCache;

    switch (stripMode) {
        case STRIPNAMESPACE_ALL: {
            auto it = allStrippedCache.find(className);
            if (it != allStrippedCache.end())
                return it->second;
            else {
                static QRegularExpression regExp("\\w*::", QRegularExpression::OptimizeOnFirstUsageOption);
                auto stripped = QString(className).replace(regExp, "");
                allStrippedCache[className] = stripped;
                // have to use the instance in the container, not 'stripped' directly, that will be destructed
                return allStrippedCache[className];
            }
        }

        case STRIPNAMESPACE_OMNETPP: {
            auto it = omnetppStrippedCache.find(className);
            if (it != omnetppStrippedCache.end())
                return it->second;
            else {
                // replacing either at the start of the string,
                // or if it's not nested, nor the suffix of the name of a different namespace
                static QRegularExpression regExp("((^omnetpp::)|((?<![\\w:])omnetpp::))", QRegularExpression::OptimizeOnFirstUsageOption);
                auto stripped = QString(className).replace(regExp, "");
                omnetppStrippedCache[className] = stripped;
                // have to use the instance in the container, not 'stripped' directly, that will be destructed
                return omnetppStrippedCache[className];
            }
        }

        default: // just to silence a compiler warning...
            ASSERT(false); // falling through gracefully to "none" stripping in release mode

        case STRIPNAMESPACE_NONE: {
            auto it = noneStrippedCache.find(className);
            if (it != noneStrippedCache.end())
                return it->second;
            else {
                noneStrippedCache[className] = className;
                // have to use the instance in the container, not 'stripped' directly, that will be destructed
                return noneStrippedCache[className];
            }
        }
    }
}

const QString &getObjectShortTypeName(cObject *object)
{
    return getObjectShortTypeName(object, getQtenv()->opt->stripNamespace);
}

const QString &getObjectShortTypeName(cObject *object, StripNamespace stripMode)
{
    if (cComponent *component = dynamic_cast<cComponent *>(object))
        TRY2(return stripNamespace(component->getComponentType()->getName(), stripMode));
    return stripNamespace(object->getClassName(), stripMode);
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
        snprintf(buffer, sizeof(staticbuf), "ptr%p", ptr);
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

//-----------------------------------------------------------------------

// Simply delegates to the ImageCache to make it easier to call.
QIcon getObjectIcon(cObject *object)
{
    return getQtenv()->icons.getObjectIcon(object);
}

const char *getObjectIconName(cObject *object)
{
    const char *iconName;
    if (object == nullptr)
        iconName = "none";
    else if (dynamic_cast<cModule *>(object) && ((cModule *)object)->isPlaceholder())
        iconName = "placeholder";
    else if (dynamic_cast<cSimpleModule *>(object))
        iconName = "simple";
    else if (dynamic_cast<cModule *>(object))
        iconName = "compound";
    else if (dynamic_cast<cWatchBase *>(object))
        iconName = "watch";
    else if (dynamic_cast<cPacket *>(object))
        iconName = "packet";
    else if (dynamic_cast<cMessage *>(object))
        iconName = "message";
    else if (dynamic_cast<cEvent *>(object))
        iconName = "event";
    else if (dynamic_cast<cArray *>(object))
        iconName = "container";
    else if (dynamic_cast<cQueue *>(object))
        iconName = "queue";
    else if (dynamic_cast<cGate *>(object))
        iconName = "gate";
    else if (dynamic_cast<cPar *>(object))
        iconName = "param";
    else if (dynamic_cast<cChannel *>(object))
        iconName = "chan";
    else if (dynamic_cast<cResultFilter *>(object))
        iconName = "filter";
    else if (dynamic_cast<cOutVector *>(object))
        iconName = "outvect";
    else if (dynamic_cast<cStatistic *>(object))
        iconName = "stat";
    else if (dynamic_cast<cResultRecorder *>(object))
        iconName = "stat";
    else if (dynamic_cast<cResultListener *>(object))
        iconName = "listener";
    else if (dynamic_cast<cFigure *>(object))
        iconName = "figure";
    else if (dynamic_cast<cCanvas *>(object))
        iconName = "canvas";
    else if (dynamic_cast<cSimulation *>(object))
        iconName = "container";
    else if (dynamic_cast<cFutureEventSet *>(object))
        iconName = "fes";
    else if (dynamic_cast<cRegistrationList *>(object))
        iconName = "list";
    else
        iconName = "cogwheel";

    return iconName;
}

const char *getObjectBaseClass(cObject *object)
{
    if (dynamic_cast<cModule *>(object) && ((cModule *)object)->isPlaceholder())
        return "cPlaceholderModule";
    else if (dynamic_cast<cSimpleModule *>(object))
        return "cSimpleModule";
    else if (dynamic_cast<cModule *>(object))
        return "cModule";
    else if (dynamic_cast<cMessage *>(object))
        return "cMessage";
    else if (dynamic_cast<cArray *>(object))
        return "cArray";
    else if (dynamic_cast<cQueue *>(object))
        return "cQueue";
    else if (dynamic_cast<cGate *>(object))
        return "cGate";
    else if (dynamic_cast<cPar *>(object))
        return "cPar";
    else if (dynamic_cast<cChannel *>(object))
        return "cChannel";
    else if (dynamic_cast<cOutVector *>(object))
        return "cOutVector";
    else if (dynamic_cast<cStatistic *>(object))
        return "cStatistic";
    else if (dynamic_cast<cFutureEventSet *>(object))
        return "cFutureEventSet";
    else if (dynamic_cast<cWatchBase *>(object))
        return "cWatchBase";
    else if (dynamic_cast<cCanvas *>(object))
        return "cCanvas";
    else if (dynamic_cast<cFigure *>(object))
        return "cFigure";
    else if (dynamic_cast<cSimulation *>(object))
        return "cSimulation";
    else if (dynamic_cast<cRegistrationList *>(object))
        return "cRegistrationList";
    else
        return object->getClassName();  // return itself as base class
}

QString getMessageShortInfoString(cMessage *msg)
{
    cModule *tomodp = msg->getArrivalModule();
    cModule *frommodp = msg->getSenderModule();

    QString modname = tomodp ? tomodp->getFullPath().c_str() : "<deleted module>";

    if (!tomodp)
        return "new msg";
    else if (msg->getKind() == MK_STARTER)
        return "starter for " + modname;
    else if (msg->getKind() == MK_TIMEOUT)
        return "timeoutmsg for " + modname;
    else if (frommodp == tomodp)
        return "selfmsg for " + modname;
    else
        return "msg for " + modname;
}

DisableDebugOnErrors::DisableDebugOnErrors()
{
    oldValue = getQtenv()->debugOnErrors;
    getQtenv()->debugOnErrors = false;
}

DisableDebugOnErrors::~DisableDebugOnErrors()
{
    getQtenv()->debugOnErrors = oldValue;
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

//----------------------------------------------------------------------


QString makeObjectTooltip(cObject *obj, bool verboseTooltip)
{
    // figures are handled specially
    if (auto fig = dynamic_cast<cFigure *>(obj)) {
        // if the tooltip has a non-null tooltip, use it
        if (const char *tt = fig->getTooltip())
            return tt;

        cObject *assocObj = fig->getAssociatedObject();
        // If two figures are associated to each other,
        // we have to avoid the infinite recursion.
        if (assocObj && !dynamic_cast<cFigure*>(assocObj))
            return makeObjectTooltip(assocObj, verboseTooltip);

        // figures inherit the tooltip if they themselves don't have one
        cFigure *parent = fig->getParentFigure();
        return parent ? makeObjectTooltip(parent, verboseTooltip) : "";
        // no other defaults for figures
    }

    // for components, use the "tt" DisplayString key
    // ("bgtt" is done by the ModuleCanvasViewer using ITEMDATA_TOOLTIP)
    if (auto comp = dynamic_cast<cComponent *>(obj)) {
        const char *userTooltip = comp->getDisplayString().getTagArg("tt", 0);
        if (!opp_isempty(userTooltip))
            return userTooltip;
    }

    // if none of the above applies, showing some generic info
    QString tooltip = QString("(") + getObjectShortTypeName(obj) + ") " + obj->getFullName();
    std::string objStr = obj->str();
    if (!objStr.empty())
        tooltip += QString(", ") + objStr.c_str();
    return tooltip;
}

LogInspector *isLogInspectorFor(cModule *mod, Inspector *insp)
{
    if (insp->getObject() != mod || insp->getType() != INSP_LOG)
        return nullptr;
    return dynamic_cast<LogInspector *>(insp);
}

ModuleInspector *isModuleInspectorFor(cModule *mod, Inspector *insp)
{
    if (insp->getObject() != mod || insp->getType() != INSP_GRAPHICAL)
        return nullptr;
    return dynamic_cast<ModuleInspector *>(insp);
}

QDebug& operator<<(QDebug& d, const std::string& s)
{
    return d << s.c_str();
}

QDebug& operator<<(QDebug& d, const SimTime& t)
{
    return d << t.format(SimTime::getScaleExp(), ".", "'", true);
}

cGate *getGateOtherHalf(cGate *gate)
{
    cGate::Type otherType = gate->getType() == cGate::INPUT ? cGate::OUTPUT : cGate::INPUT;
    int index = gate->isVector() ? gate->getIndex() : -1; // -1 will tell gateHalf that it's not a vector
    return gate->getOwnerModule()->gateHalf(gate->getBaseName(), otherType, index);
}

bool isTwoWayConnection(cGate *gate)
{
    if (!gate)
        return false;

    cGate *destGate = gate->getNextGate();

    if (!destGate)
        return false;

    // check if this gate is really part of an in/out gate pair
    // gate      o-------------------->o dest_gate
    // gate_pair o<--------------------o dest_gate_pair
    if (gate->getNameSuffix()[0]) {
        const cGate *gatePair = getGateOtherHalf(gate);

        if (destGate->getNameSuffix()[0]) {
            const cGate *destGatePair = getGateOtherHalf(destGate);
            return destGatePair == gatePair->getPreviousGate();
        }
    }

    return false;
}

std::vector<std::pair<ChartTickDecimal, bool>> getLinearTicks(double start, double end, double approxDelta)
{
    int scale = std::ceil(log10(approxDelta));

    ChartTickDecimal bdStart = ChartTickDecimal(start).floor(-scale);
    ChartTickDecimal bdEnd = ChartTickDecimal(end).ceil(-scale);
    ChartTickDecimal spacing(approxDelta);
    ChartTickDecimal delta(1, scale);

    ChartTickDecimal majorTickDelta;
    if (delta.over5() > spacing) {
        // use 2, 4, 6, 8, etc. if possible
        majorTickDelta = delta.over5();
        delta = delta.over10();
    }
    else if (delta.over2() > spacing) {
        // use 5, 10, 15, 20, etc. if possible
        majorTickDelta = delta.over2();
        delta = delta.over10();
    }
    else {
        majorTickDelta = delta;
        delta = delta.over5();
    }

    std::vector<std::pair<ChartTickDecimal, bool>> ticks;
    for (ChartTickDecimal current = bdStart; current <= bdEnd; current += delta) {
        bool isMajorTick = false;
        try {
             isMajorTick = current.multipleOf(majorTickDelta);
        } catch (opp_runtime_error &) {
            // This is a very rare case - it only used to occur in test/histograms,
            // in the UnexpectedExtremeValue1 config, at about event #1000. It is
            // now believed to be fixed in multipleOf(). But just to be safe,
            // we ignore this error, since we don't really care if some ticks
            // here and there that should be major, aren't. If we don't catch it
            // here, it would terminate the simulation - which would be unfortunate.
        }
        ticks.push_back(std::pair<ChartTickDecimal, bool>(current, isMajorTick));
    }

    return ticks;
}


const char *skipEscapeSequences(const char *start)
{
    while (*start != 0) {
        if (*start == '\x1b') {
            ++start;

            if (*start != '[')
                break; // invalid format, ending the control sequence here

            ++start;

            while (*start != 0 && *start != 'm') {
                if (*start != ';' && (*start < '0' || *start > '9'))
                    break; // invalid sequence, act as if it ended here
                ++start;
            }
            if (*start == 'm')
                ++start;
        } else
            break;
    }
    return start;
}

const std::string& stripFormatting(const std::string& input, std::string& buffer)
{
    if (strchr(input.c_str(), '\x1b') == nullptr)
        return input;

    buffer.resize(input.length());
    char *op = buffer.data();
    const char *p = input.c_str();
    while (*p) {
        if (*p == '\x1b' && *(p+1) == '[') {
            p += 2;
            while (*p && *p != 'm')  // skip until "m"
                p++;
            if (*p)
                p++; // skip past 'm'
        }
        else
            *op++ = *p++;
    }
    buffer.resize(op - buffer.data());
    return buffer;
}

std::string stripFormattingAndRemoveTrailingNewLine(const std::string& input)
{
    std::string tmp;
    std::string output = stripFormatting(input, tmp);
    return stripSuffixes(output, '\n');
}

std::string stripSuffixes(const std::string& from, const std::string& suffix)
{
    std::string result = from;
    size_t ss = suffix.size();
    while (result.size() >= ss && result.compare(result.size() - ss, ss, suffix) == 0)
        result.resize(result.size() - ss);
    return result;
}

std::string stripSuffixes(const std::string& from, char suffix)
{
    std::string result = from;
    while (!result.empty() && result.back() == suffix)
        result.pop_back();
    return result;
}

#ifdef _WIN32
// This is part of POSIX.1-2008, but not available on Windows.
static const char *strcasestr(const char *haystack, const char *needle)
{
    if (!*needle) {
        return haystack;
    }
    for (; *haystack; ++haystack) {
        if (toupper(*haystack) == toupper(*needle)) {
            const char *h, *n;
            for (h = haystack, n = needle; *h && *n; ++h, ++n) {
                if (toupper(*h) != toupper(*n)) {
                    break;
                }
            }
            if (!*n) { // Found needle
                return haystack;
            }
        }
    }
    return nullptr;
}
#endif

inline bool is_wordchar(char c) { return isalnum(c) || c == '_' || c == '@'; }

static const char *strstr_wholeword(const char *haystack, const char *needle, bool caseSensitive)
{
    size_t needle_len = strlen(needle);
    const char *p = haystack;
    while ((p = caseSensitive ? strstr(p, needle) : strcasestr(p, needle)) != nullptr) {
        if (p == haystack || !is_wordchar(p[-1])) {
            const char *end = p + needle_len;
            if (end[0] == '\0' || !is_wordchar(end[0]))
                return p;
        }
        p += needle_len;
    }
    return nullptr;
}

SearchResult findSubstring(const char *haystack, const char *needle, int startIndex, SearchFlags flags)
{
    if (flags & FIND_BACKWARDS) {
        // find last match (that finishes before startIndex)
        SearchResult prevMatch;
        while (true) {
            int innerStartIndex = prevMatch.matchStart ? (prevMatch.matchStart - haystack) + 1 : 0;
            SearchResult newMatch = findSubstring(haystack, needle, innerStartIndex, flags & ~FIND_BACKWARDS);
            if (!newMatch.matchStart || newMatch.matchStart + newMatch.matchLength > haystack + startIndex)
                return prevMatch;
            prevMatch = newMatch;
        }
    }

    bool caseSensitive = flags & FIND_CASE_SENSITIVE;
    bool wholeWords = flags & FIND_WHOLE_WORDS;

    if (flags & FIND_REGULAR_EXPRESSION) {
        // NOTE: QRegExp seems to be quite a bit faster than std::regex, that's why we use it here
        QString qhaystack = QString::fromUtf8(haystack + startIndex);
        QString qneedle = QString::fromUtf8(needle);

        if (wholeWords)
            qneedle = "\\b" + qneedle + "\\b";
        QRegExp re(qneedle, caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
        int index = re.indexIn(qhaystack);
        if (index == -1)
            return {nullptr, 0};
        return {haystack + startIndex + index, re.matchedLength()};
    }

    // not a regular expression search
    const char *startPos = haystack + startIndex;
    const char *matchStart = wholeWords
        ? strstr_wholeword(startPos, needle, caseSensitive)
        : (caseSensitive ? strstr(startPos, needle) : strcasestr(startPos, needle));

    return {matchStart, matchStart ? (int)strlen(needle) : 0};
}

}  // namespace qtenv
}  // namespace omnetpp

