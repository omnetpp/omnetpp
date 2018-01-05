//==========================================================================
//  TKUTIL.CC - part of
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

#include "common/stringutil.h"
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
#include "tkenv.h"
#include "tkutil.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace tkenv {

#define INSPECTORLISTBOX_MAX_ITEMS    100000

TclQuotedString::TclQuotedString()
{
    quotedstr = nullptr;
    buf[0] = '\0';
}

TclQuotedString::TclQuotedString(const char *s)
{
    int flags;
    int quotedlen = Tcl_ScanElement(TCLCONST(s), &flags);
    quotedstr = quotedlen < 80 ? buf : Tcl_Alloc(quotedlen+1);
    Tcl_ConvertElement(TCLCONST(s), quotedstr, flags);
}

TclQuotedString::TclQuotedString(const char *s, int n)
{
    int flags;
    int quotedlen = Tcl_ScanCountedElement(TCLCONST(s), n, &flags);
    quotedstr = quotedlen < 80 ? buf : Tcl_Alloc(quotedlen+1);
    Tcl_ConvertCountedElement(TCLCONST(s), n, quotedstr, flags);
}

void TclQuotedString::set(const char *s)
{
    int flags;
    int quotedlen = Tcl_ScanElement(TCLCONST(s), &flags);
    quotedstr = quotedlen < 80 ? buf : Tcl_Alloc(quotedlen+1);
    Tcl_ConvertElement(TCLCONST(s), quotedstr, flags);
}

void TclQuotedString::set(const char *s, int n)
{
    int flags;
    int quotedlen = Tcl_ScanCountedElement(TCLCONST(s), n, &flags);
    quotedstr = quotedlen < 80 ? buf : Tcl_Alloc(quotedlen+1);
    Tcl_ConvertCountedElement(TCLCONST(s), n, quotedstr, flags);
}

TclQuotedString::~TclQuotedString()
{
    if (quotedstr != buf)
        Tcl_Free(quotedstr);
}

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

//=======================================================================

#define TRY2(CODE)  try { CODE; } catch (std::exception& e) {printf("<!> Warning: %s\n", e.what());}

const char *stripNamespace(const char *className)
{
    switch (getTkenv()->opt->stripNamespace) {
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

void setObjectListResult(Tcl_Interp *interp, cCollectObjectsVisitor *visitor)
{
    int n = visitor->getArraySize();
    cObject **objs = visitor->getArray();
    const int ptrsize = 21;  // one ptr should be max 20 chars (good for even 64bit-ptrs)
    char *buf = Tcl_Alloc(ptrsize*n+1);
    char *s = buf;
    for (int i = 0; i < n; i++) {
        ptrToStr(objs[i], s);
        assert(strlen(s) <= 20);
        s += strlen(s);
        *s++ = ' ';
    }
    *s = '\0';
    Tcl_SetResult(interp, buf, TCL_DYNAMIC);
}

//-----------------------------------------------------------------------

std::string getObjectIcon(Tcl_Interp *interp, cObject *object)
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

    // look up the image in the icons() array
    const char *image = Tcl_GetVar2(interp, TCLCONST("icons"), TCLCONST(iconName), TCL_GLOBAL_ONLY);
    if (!image)
        throw cRuntimeError("getObjectIcon(): Undefined variable $icons(%s)", iconName);
    return image;
}

void insertIntoInspectorListbox(Tcl_Interp *interp, const char *listbox, cObject *obj, bool fullpath)
{
    const char *ptr = ptrToStr(obj);
    CHK(Tcl_VarEval(interp, listbox, " insert {} end "
                    "-image ", getObjectIcon(interp, obj).c_str(), " "
                    "-text {", "  "  /*padding*/, getObjectShortTypeName(obj), "} ",
                    "-values {",
                    TclQuotedString(fullpath ? obj->getFullPath().c_str() : obj->getFullName()).get(), " ",
                    TclQuotedString(obj->str().c_str()).get(), " ", ptr,
                    "}",
                    TCL_NULL));
}

void feedCollectionIntoInspectorListbox(cCollectObjectsVisitor *visitor, Tcl_Interp *interp, const char *listbox, bool fullpath)
{
    int n = visitor->getArraySize();
    cObject **objs = visitor->getArray();

    for (int i = 0; i < n; i++) {
        // insert into listbox
        insertIntoInspectorListbox(interp, listbox, objs[i], fullpath);
    }
}

int fillListboxWithChildObjects(cObject *object, Tcl_Interp *interp, const char *listbox, bool deep)
{
    int n;
    if (deep) {
        cCollectObjectsVisitor visitor;
        visitor.setSizeLimit(INSPECTORLISTBOX_MAX_ITEMS);
        visitor.process(object);
        n = visitor.getArraySize();
        feedCollectionIntoInspectorListbox(&visitor, interp, listbox, true);
    }
    else {
        cCollectChildrenVisitor visitor(object);
        visitor.setSizeLimit(INSPECTORLISTBOX_MAX_ITEMS);
        visitor.process(object);
        n = visitor.getArraySize();
        feedCollectionIntoInspectorListbox(&visitor, interp, listbox, false);
    }
    return n;
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
            throw cRuntimeError("%s '%s' has no parameter '%s'", what, component->getFullPath().c_str(), parname);
        else
            throw cRuntimeError("%s '%s' and its parent have no parameter '%s'", what, component->getFullPath().c_str(), parname);
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
              case cPar::INT: buffer += opp_stringf("%" PRId64, (int64_t) par->intValue()); break;
              case cPar::DOUBLE: buffer += opp_stringf("%g", par->doubleValue()); break;
              default: throw cRuntimeError("Cannot substitute parameter %s into display string: Wrong data type", par->getFullPath().c_str());
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
        return par->intValue();
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

void logTclError(const char *file, int line, Tcl_Interp *interp)
{
    getTkenv()->logTclError(file, line, interp);
}

void invokeTclCommand(Tcl_Interp *interp, Tcl_CmdInfo *cmd, int argc, const char *argv[])
{
    TclCmdProc cmdProc = (TclCmdProc)cmd->proc;
    if (cmdProc(cmd->clientData, interp, argc, (char **)argv) == TCL_ERROR) {
        std::stringstream os;
        os << Tcl_GetStringResult(interp) << "\n";
        os << "  while directly invoking Tcl command proc at " << cmdProc << "\n";
        os << "  with args: ";
        for (int i = 0; i < argc; i++)
            os << " '" << argv[i] << "'";
        os << "\n";
        getTkenv()->logTclError(__FILE__, __LINE__, os.str().c_str());
    }
}

}  // namespace tkenv
}  // namespace omnetpp

