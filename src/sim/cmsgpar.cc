//=========================================================================
//  CMSGPAR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <clocale>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include "common/opp_ctype.h"
#include "common/stringutil.h"
#include "omnetpp/cmsgpar.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/globals.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cexception.h"
#include "omnetpp/csimulation.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

using namespace omnetpp::common;

namespace omnetpp {

using std::ostream;
using std::string;

Register_Class(cMsgPar);

static const char *getTypeName(char typechar)
{
    switch (typechar) {
        case 'S': return "string (S)";
        case 'B': return "bool (B)";
        case 'L': return "long (L)";
        case 'D': return "double (D)";
        case 'F': return "function with constant args (F)";
        case 'T': return "random number from distribution (T)";
        case 'P': return "pointer (P)";
        case 'M': return "XML element (M)";
        default:  return "invalid type char";
    }
}

cMsgPar::cMsgPar(const char *name) : cOwnedObject(name)
{
    lng.val = 0L;
}

cMsgPar::cMsgPar(const cMsgPar& par) : cOwnedObject(par)
{
    lng.val = 0L;
    copy(par);
}

cMsgPar::cMsgPar(const char *name, cMsgPar& other) : cMsgPar(other)
{
    setName(name);
}

cMsgPar::~cMsgPar()
{
    beforeChange();
    deleteOld();
}

void cMsgPar::deleteOld()
{
    if (typeChar == 'S' && !ls.sht) {
        delete[] ls.str;
    }
    else if (typeChar == 'T') {
        if (dtr.res->getOwner() == this)
            dropAndDelete(dtr.res);
    }
    else if (typeChar == 'P') {
        if (ptr.dupfunc || ptr.itemsize > 0) {  // we're expected to do memory mgmt
            if (ptr.delfunc)
                ptr.delfunc(ptr.ptr);
            else
                delete[] (char *)ptr.ptr;  // cast because delete void* is not legal
        }
    }
    else if (typeChar == 'O') {
        if (obj.obj->getOwner() == this)
            dropAndDelete(obj.obj);
    }
    typeChar = 'L';
}

//----------------------------------------------------------------------
// redefine virtual cOwnedObject funcs


string cMsgPar::str() const
{
    char bb[128];
    bb[0] = 0;
    cNedMathFunction *ff;
    const char *fn;
    const char *s;

    switch (typeChar) {
        case 'S':
            s = ls.sht ? ss.str : ls.str;
            return string("\"") + s + "\"";

        case 'B':
            return string(lng.val ? "true" : "false");

        case 'L':
            snprintf(bb, sizeof(bb), "%ld", lng.val);
            return string(bb);

        case 'D':
            snprintf(bb, sizeof(bb), "%g", dbl.val);
            return string(bb);

        case 'F':
            ff = cNedMathFunction::findByPointer(func.f);
            fn = ff ? ff->getName() : "unknown";
            switch (func.argc) {
                case 0:
                    snprintf(bb, sizeof(bb), "()");
                    break;

                case 1:
                    snprintf(bb, sizeof(bb), "(%g)", func.p1);
                    break;

                case 2:
                    snprintf(bb, sizeof(bb), "(%g,%g)", func.p1, func.p2);
                    break;

                case 3:
                    snprintf(bb, sizeof(bb), "(%g,%g,%g)", func.p1, func.p2, func.p3);
                    break;

                case 4:
                    snprintf(bb, sizeof(bb), "(%g,%g,%g,%g)", func.p1, func.p2, func.p3, func.p4);
                    break;

                default:
                    snprintf(bb, sizeof(bb), "() with %d args", func.argc);
                    break;
            }
            ;
            return string(fn) + bb;

        case 'T':
            return string("distribution ") + (dtr.res ? dtr.res->getFullPath().c_str() : "nullptr");

        case 'P':
            snprintf(bb, sizeof(bb), "pointer %p", ptr.ptr);
            return string(bb);

        case 'O':
            return string("object ") + (obj.obj ? obj.obj->getFullPath().c_str() : "nullptr");

        case 'M':
            if (xmlp.node)
                return string("<") + xmlp.node->getTagName() + "> from " + xmlp.node->getSourceLocation();
            else
                return string("nullptr");
            break;

        default:
            return string("???");
    }
}

void cMsgPar::forEachChild(cVisitor *v)
{
    if (typeChar == 'T') {
        v->visit(dtr.res);
    }
    else if (typeChar == 'O') {
        if (obj.obj)
            v->visit(obj.obj);
    }
}

void cMsgPar::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cOwnedObject::parsimPack(buffer);

    buffer->pack(typeChar);
    buffer->pack(changedFlag);

    cNedMathFunction *ff;
    switch (typeChar) {
        case 'B':
            buffer->pack(lng.val);
            break;

        case 'S':
            buffer->pack(ls.sht ? ss.str : ls.str);
            break;

        case 'L':
            buffer->pack(lng.val);
            break;

        case 'D':
            buffer->pack(dbl.val);
            break;

        case 'F':
            ff = cNedMathFunction::findByPointer(func.f);
            if (ff == nullptr)
                throw cRuntimeError(this, "parsimPack(): Cannot transmit unregistered function");

            buffer->pack(ff->getName());
            buffer->pack(func.argc);
            buffer->pack(func.p1);
            buffer->pack(func.p2);
            buffer->pack(func.p3);
            buffer->pack(func.p4);
            break;

        case 'T':
            if (dtr.res && dtr.res->getOwner() != this)
                throw cRuntimeError(this, "parsimPack(): Cannot transmit pointer to \"external\" object");
            if (buffer->packFlag(dtr.res != nullptr))
                buffer->packObject(dtr.res);
            break;

        case 'P':
            throw cRuntimeError(this, "parsimPack(): Cannot transmit pointer to unknown data structure (type 'P')");

        case 'O':
            if (obj.obj && obj.obj->getOwner() != this)
                throw cRuntimeError(this, "parsimPack(): Cannot transmit pointer to \"external\" object");
            if (buffer->packFlag(obj.obj != nullptr))
                buffer->packObject(obj.obj);
            break;

        case 'M':
            throw cRuntimeError(this, "parsimPack(): Cannot transmit pointer to XML element (type 'M')");

        default:
            throw cRuntimeError(this, "parsimPack(): Unsupported type '%c'", typeChar);
    }
#endif
}

void cMsgPar::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    char *funcname;
    int argc;

    cOwnedObject::parsimUnpack(buffer);

    buffer->unpack(typeChar);
    buffer->unpack(changedFlag);

    cNedMathFunction *ff;
    switch (typeChar) {
        case 'B':
            buffer->unpack(lng.val);
            break;

        case 'S':
            ss.sht = false;
            buffer->unpack(ls.str);
            break;

        case 'L':
            buffer->unpack(lng.val);
            break;

        case 'D':
            buffer->unpack(dbl.val);
            break;

        case 'F':
            buffer->unpack(funcname);
            buffer->unpack(argc);
            ff = cNedMathFunction::find(funcname, argc);
            if (ff == nullptr) {
                delete[] funcname;
                throw cRuntimeError(this, "parsimUnpack(): Transmitted function '%s' with %d args not registered here",
                        funcname, argc);
            }
            func.f = ff->getMathFunc();
            func.argc = argc;
            buffer->unpack(func.p1);
            buffer->unpack(func.p2);
            buffer->unpack(func.p3);
            buffer->unpack(func.p4);
            delete[] funcname;
            break;

        case 'T':
            if (!buffer->checkFlag())
                dtr.res = nullptr;
            else
                take(dtr.res = (cStatistic *)buffer->unpackObject());
            break;

        case 'P':
        case 'M':
            throw cRuntimeError(this, "parsimUnpack(): Unpacking types I, P, M not implemented");

        case 'O':
            if (!buffer->checkFlag())
                obj.obj = nullptr;
            else
                take(obj.obj = dynamic_cast<cOwnedObject *>(buffer->unpackObject()));
            break;

        default:
            throw cRuntimeError(this, "parsimUnpack(): Unknown parameter data type");
    }
#endif
}

//----

char cMsgPar::getType() const
{
    return typeChar;
}

bool cMsgPar::hasChanged()
{
    bool ch = changedFlag;
    changedFlag = false;
    return ch;
}

//----

cMsgPar& cMsgPar::setStringValue(const char *s)
{
    beforeChange();
    deleteOld();
    typeChar = 'S';
    if (!s) {
        ls.sht = true;
        *ss.str = '\0';
    }
    else if ((ls.sht = (strlen(s) <= SHORTSTR_MAXLEN)) != 0)
        strcpy(ss.str, s);
    else
        ls.str = opp_strdup(s);
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setBoolValue(bool b)
{
    beforeChange();
    deleteOld();
    lng.val = b;
    typeChar = 'B';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setLongValue(long l)
{
    beforeChange();
    deleteOld();
    lng.val = l;
    typeChar = 'L';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(double d)
{
    beforeChange();
    deleteOld();
    dbl.val = d;
    typeChar = 'D';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(MathFuncNoArg f)
{
    beforeChange();
    deleteOld();
    func.f = (MathFunc)f;
    func.argc = 0;
    typeChar = 'F';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(MathFunc1Arg f, double p1)
{
    beforeChange();
    deleteOld();
    func.f = (MathFunc)f;
    func.argc = 1;
    func.p1 = p1;
    typeChar = 'F';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(MathFunc2Args f, double p1, double p2)
{
    beforeChange();
    deleteOld();
    func.f = (MathFunc)f;
    func.argc = 2;
    func.p1 = p1;
    func.p2 = p2;
    typeChar = 'F';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(MathFunc3Args f, double p1, double p2, double p3)
{
    beforeChange();
    deleteOld();
    func.f = (MathFunc)f;
    func.argc = 3;
    func.p1 = p1;
    func.p2 = p2;
    func.p3 = p3;
    typeChar = 'F';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(MathFunc4Args f, double p1, double p2, double p3, double p4)
{
    beforeChange();
    deleteOld();
    func.f = (MathFunc)f;
    func.argc = 4;
    func.p1 = p1;
    func.p2 = p2;
    func.p3 = p3;
    func.p4 = p4;
    typeChar = 'F';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(cStatistic *res)
{
    if (!res)
        throw cRuntimeError(this, E_BADINIT, getTypeName('T'));

    beforeChange();
    deleteOld();
    dtr.res = res;
    if (getTakeOwnership())
        take(res);
    typeChar = 'T';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setPointerValue(void *_ptr)
{
    beforeChange();
    // if it was a 'P' before, keep previous configuration
    if (typeChar != 'P') {
        deleteOld();
        ptr.delfunc = nullptr;
        ptr.dupfunc = nullptr;
        ptr.itemsize = 0;
        typeChar = 'P';
    }
    ptr.ptr = _ptr;
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setObjectValue(cOwnedObject *_obj)
{
    beforeChange();
    deleteOld();
    obj.obj = _obj;
    if (getTakeOwnership())
        take(_obj);
    typeChar = 'O';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setXMLValue(cXMLElement *node)
{
    beforeChange();
    deleteOld();
    xmlp.node = node;
    typeChar = 'M';
    afterChange();
    return *this;
}

void cMsgPar::configPointer(VoidDelFunc delfunc, VoidDupFunc dupfunc,
        size_t itemsize)
{
    if (typeChar != 'P')
        throw cRuntimeError(this, "configPointer(): Type is '%c'; should be 'P'", typeChar);
    ptr.delfunc = delfunc;
    ptr.dupfunc = dupfunc;
    ptr.itemsize = itemsize;
}

//----

const char *cMsgPar::stringValue()
{
    if (typeChar != 'S')
        throw cRuntimeError(this, E_BADCAST, getTypeName(typeChar), getTypeName('S'));
    return ss.sht ? ss.str : ls.str;
}

//
// Note:
// boolValue(), longValue() and doubleValue() are rather liberal: they all
// allow conversion from all of B,L and the double types D,T,F.
//

bool cMsgPar::boolValue()
{
    if (typeChar == 'B' || typeChar == 'L')
        return lng.val != 0;
    else if (isNumeric())
        return doubleValue() != 0;
    else
        throw cRuntimeError(this, E_BADCAST, getTypeName(typeChar), getTypeName('B'));
}

inline long _double_to_long(double d)
{
    // gcc 3.3 "feature": if double d=0xc0000000, (long)d yields 0x80000000!!!
    // This causes trouble if we in fact want to cast this long to unsigned long, see NED_expr_2.test.
    // Workaround follows. Note: even the ul variable is needed: when inlining it, gcc will do the wrong cast!
    long l = (long)d;
    unsigned long ul = (unsigned long)d;
    return d < 0 ? l : ul;
}

long cMsgPar::longValue()
{
    if (typeChar == 'L' || typeChar == 'B')
        return lng.val;
    else if (isNumeric())
        return _double_to_long(doubleValue());
    else
        throw cRuntimeError(this, E_BADCAST, getTypeName(typeChar), getTypeName('L'));
}

double cMsgPar::doubleValue()
{
    if (typeChar == 'B' || typeChar == 'L')
        return (double)lng.val;
    else if (typeChar == 'D')
        return dbl.val;
    else if (typeChar == 'T')
        return getFromstat();
    else if (typeChar == 'F')
        return func.argc == 0 ? ((MathFuncNoArg)func.f)() :
               func.argc == 1 ? ((MathFunc1Arg)func.f)(func.p1) :
               func.argc == 2 ? ((MathFunc2Args)func.f)(func.p1, func.p2) :
               func.argc == 3 ? ((MathFunc3Args)func.f)(func.p1, func.p2, func.p3) :
               ((MathFunc4Args)func.f)(func.p1, func.p2, func.p3, func.p4);
    else
        throw cRuntimeError(this, E_BADCAST, getTypeName(typeChar), getTypeName('D'));
}

void *cMsgPar::pointerValue()
{
    if (typeChar == 'P')
        return ptr.ptr;
    else
        throw cRuntimeError(this, E_BADCAST, getTypeName(typeChar), getTypeName('P'));
}

cOwnedObject *cMsgPar::getObjectValue()
{
    if (typeChar == 'O')
        return obj.obj;
    else
        throw cRuntimeError(this, E_BADCAST, getTypeName(typeChar), getTypeName('O'));
}

cXMLElement *cMsgPar::xmlValue()
{
    if (typeChar == 'M')
        return xmlp.node;
    else
        throw cRuntimeError(this, E_BADCAST, getTypeName(typeChar), getTypeName('M'));
}

bool cMsgPar::isNumeric() const
{
    return typeChar == 'B' ||
           typeChar == 'L' ||
           typeChar == 'D' ||
           typeChar == 'T' ||
           typeChar == 'F';
}

bool cMsgPar::isConstant() const
{
    return typeChar == 'S' ||
           typeChar == 'B' ||
           typeChar == 'L' ||
           typeChar == 'D';
}

bool cMsgPar::equalsTo(cMsgPar *par)
{
    if (typeChar != par->typeChar)
        return false;

    switch (typeChar) {
        case 'S': return strcmp(stringValue(),par->stringValue())==0;
        case 'B': return lng.val == par->lng.val;
        case 'L': return lng.val == par->lng.val;
        case 'D': return dbl.val == par->dbl.val;
        case 'F': if (func.f!=par->func.f) return 0;
                  switch (func.argc) {
                      case 4: if (func.p4 != par->func.p4) return 0; // no break
                      case 3: if (func.p3 != par->func.p3) return 0; // no break
                      case 2: if (func.p2 != par->func.p2) return 0; // no break
                      case 1: if (func.p1 != par->func.p1) return 0; // no break
                  }
                  return 1;
        case 'T': return dtr.res == par->dtr.res;
        case 'P': return ptr.ptr == par->ptr.ptr;
        case 'O': return obj.obj == par->obj.obj;
        case 'M': return xmlp.node == par->xmlp.node;
        default: return 0;
    }
}

//----

void cMsgPar::beforeChange()
{
}

void cMsgPar::afterChange()
{
    changedFlag = true;
}

static bool parseQuotedString(string& str, const char *& s)  //FIXME use opp_parsequotedstr() instead!
{
    while (*s == ' ' || *s == '\t')
        s++;
    if (*s != '"')
        return false;
    const char *beg = ++s;
    while (*s && (*s != '"' || *(s-1) == '\\'))
        s++;
    if (*s != '"')
        return false;
    str.assign(beg, s - beg);
    s++;
    return true;
}

bool cMsgPar::parse(const char *text, char tp)
{
    tp = (char)opp_toupper(tp);

    // create a working copy and cut whitespaces (from both sides)
    if (!text)
        return false;  // error: no string
    while (*text == ' ' || *text == '\t')
        text++;
    if (*text == '\0')
        return false;  // error: empty string (or only whitespace)
    char *tmp = opp_strdup(text);
    char *s = tmp + strlen(tmp)-1;
    while (s >= tmp && (*s == ' ' || *s == '\t'))
        *s-- = '\0';

    if (strcmp(tmp, "true") == 0 || strcmp(tmp, "TRUE") == 0 || strcmp(tmp, "True") == 0) {  // bool?
        if (!strchr("?B", tp))
            goto error;
        setBoolValue(true);
    }
    else if (strcmp(tmp, "false") == 0 || strcmp(tmp, "FALSE") == 0 || strcmp(tmp, "False") == 0) {  // bool?
        if (!strchr("?B", tp))
            goto error;
        setBoolValue(false);
    }
    else if (strcmp(tmp, "1") == 0 && tp == 'B') {  // bool?
        setBoolValue(true);
    }
    else if (strcmp(tmp, "0") == 0 && tp == 'B') {  // bool?
        setBoolValue(false);
    }
    else if (tmp[0] == '\'' && tmp[1] && tmp[2] == '\'' && !tmp[3]) {  // char? (->long)
        if (!strchr("?L", tp))
            goto error;
        setLongValue((long)tmp[1]);
    }
    else if (text[0] == '\"') {  // string?
        if (!strchr("?S", tp))
            goto error;

        // check closing quote
        if (!tmp[1] || tmp[strlen(tmp)-1] != '\"')
            goto error;  //FIXME use opp_parsequotedstr() and catch exception

        tmp[strlen(tmp)-1] = '\0';  // cut off closing quote
        setStringValue(tmp+1);
    }
    else if (strspn(tmp, "+-0123456789") == strlen(tmp)) {  // long?
        long num;
        int len;
        if (0 == sscanf(tmp, "%ld%n", &num, &len))
            goto error;
        if (len < (int)strlen(tmp) || !strchr("?LD", tp))
            goto error;
        if (tp == '?' || tp == 'L')
            setLongValue(num);
        else
            setDoubleValue(num);
    }
    else if (strspn(tmp, "+-.eE0123456789") == strlen(tmp)) {  // double?
        double num;
        int len;
        setlocale(LC_NUMERIC, "C");
        if (0 == sscanf(tmp, "%lf%n", &num, &len))
            goto error;
        if (len < (int)strlen(tmp) || !strchr("?D", tp))
            goto error;
        setDoubleValue(num);
    }
    else if (!strncmp(tmp, "xmldoc", 6)) {
        if (!strchr("?M", tp))
            goto error;

        // parse xmldoc("filename") or xmldoc("filename", "pathexpr")
        const char *s = tmp;
        s += 6;  // skip "xmldoc"
        while (*s == ' ' || *s == '\t')
            s++;
        if (*s != '(')
            goto error;  // no "("
        s++;  // skip "("
        std::string fname, pathexpr;
        while (*s == ' ' || *s == '\t')
            s++;
        if (!parseQuotedString(fname, s))
            goto error;
        while (*s == ' ' || *s == '\t')
            s++;
        if (*s != ',' && *s != ')')
            goto error;  // no ")" or ","
        if (*s == ',') {
            s++;  // skip ","
            if (!parseQuotedString(pathexpr, s))
                goto error;
            while (*s == ' ' || *s == '\t')
                s++;
            if (*s != ')')
                goto error;  // no ")"
        }
        s++;  // skip ")"
        while (*s == ' ' || *s == '\t')
            s++;
        if (*s)
            goto error;  // trailing rubbish

        cXMLElement *node = getEnvir()->getXMLDocument(fname.c_str(), pathexpr.empty() ? nullptr : pathexpr.c_str());
        if (!node)
            throw cRuntimeError(this, "%s: Element not found", tmp);
        setXMLValue(node->dupTree());
    }
    else {  // maybe function; try to parse it
        if (!strchr("?F", tp))
            goto error;
        if (!setfunction(tmp))
            goto error;
    }

    delete[] tmp;
    return true;

  error:
    delete[] tmp;
    return false;
}

static double parsedbl(const char *& s)
{
    while (*s == ' ' || *s == '\t')
        s++;
    int len = 0;
    double num = 0;
    setlocale(LC_NUMERIC, "C");
    sscanf(s, "%lf%n", &num, &len);
    s += len;
    while (*s == ' ' || *s == '\t')
        s++;
    return num;
}

bool cMsgPar::setfunction(char *text)
{
    // Note: this function *will* alter its input string

    // find '('
    char *d;
    for (d = text; *d != '(' && *d != '\0'; d++)
        ;
    if (*d != '(')
        return false;  // no opening paren
    char *args = d;

    // remove whitespaces in-place
    const char *s;
    for (s = d = args; *s; s++)
        if (!opp_isspace(*s))
            *d++ = *s;

    *d = '\0';

    // determine argccount: number of commas+1, or zero
    int commas = 0;
    for (d = args; *d; d++)
        if (*d == ',')
            commas++;

    int argc;
    if (commas == 0 && args[1] == ')')
        argc = 0;
    else
        argc = commas+1;

    // look up function name (temporarily overwriting '(' with a '\0')
    *args = '\0';
    cNedMathFunction *ff = cNedMathFunction::find(text, argc);
    *args = '(';
    if (ff == nullptr)
        return false;

    // now 'args' points to something like '(10,1.5E-3)', without spaces
    s = args;
    double p1, p2, p3, p4;
    switch (ff->getNumArgs()) {
        case 0:
            if (strcmp(s, "()") != 0)
                return false;
            setDoubleValue(ff->getMathFuncNoArg());
            return true;

        case 1:
            if (*s++ != '(')
                return false;
            p1 = parsedbl(s);
            if (*s++ != ')')
                return false;
            setDoubleValue(ff->getMathFunc1Arg(), p1);
            return true;

        case 2:
            if (*s++ != '(')
                return false;
            p1 = parsedbl(s);
            if (*s++ != ',')
                return false;
            p2 = parsedbl(s);
            if (*s++ != ')')
                return false;
            setDoubleValue(ff->getMathFunc2Args(), p1, p2);
            return true;

        case 3:
            if (*s++ != '(')
                return false;
            p1 = parsedbl(s);
            if (*s++ != ',')
                return false;
            p2 = parsedbl(s);
            if (*s++ != ',')
                return false;
            p3 = parsedbl(s);
            if (*s++ != ')')
                return false;
            setDoubleValue(ff->getMathFunc3Args(), p1, p2, p3);
            return true;

        case 4:
            if (*s++ != '(')
                return false;
            p1 = parsedbl(s);
            if (*s++ != ',')
                return false;
            p2 = parsedbl(s);
            if (*s++ != ',')
                return false;
            p3 = parsedbl(s);
            if (*s++ != ',')
                return false;
            p4 = parsedbl(s);
            if (*s++ != ')')
                return false;
            setDoubleValue(ff->getMathFunc4Args(), p1, p2, p3, p4);
            return true;

        default:
            return false;  // invalid argcount
    }
}

double cMsgPar::getFromstat()
{
    if (typeChar != 'T')
        throw cRuntimeError(this, E_BADCAST, getTypeName(typeChar), getTypeName('T'));
    return dtr.res->draw();
}

void cMsgPar::copy(const cMsgPar& val)
{
    typeChar = val.typeChar;
    changedFlag = val.changedFlag;

    // this line is supposed to copy the whole data area.
    memcpy(&ss, &val.ss, std::max(sizeof(ss), sizeof(func)));

    if (typeChar == 'S' && !ss.sht) {
        // make our copy of the string
        ls.str = opp_strdup(ls.str);
    }
    else if (typeChar == 'T') {
        cStatistic *& p = dtr.res;
        if (p->getOwner() == const_cast<cMsgPar *>(&val))
            take(p = (cStatistic *)p->dup());
    }
    else if (typeChar == 'P') {
        if (ptr.dupfunc)
            ptr.ptr = ptr.dupfunc(ptr.ptr);
        else if (ptr.itemsize > 0)
            memcpy(ptr.ptr = new char[ptr.itemsize], val.ptr.ptr, ptr.itemsize);
        // if no dupfunc or itemsize, only the pointer is copied
    }
    else if (typeChar == 'O') {
        cOwnedObject *& p = obj.obj;
        if (p->getOwner() == const_cast<cMsgPar *>(&val))
            take(p = (cOwnedObject *)p->dup());
    }
}

cMsgPar& cMsgPar::operator=(const cMsgPar& val)
{
    if (this == &val)
        return *this;
    beforeChange();
    deleteOld();
    cOwnedObject::operator=(val);
    copy(val);
    afterChange();
    return *this;
}

void cMsgPar::convertToConst()
{
    if (strchr("FT", typeChar)) {
        double d = doubleValue();
        setDoubleValue(d);
    }
}

}  // namespace omnetpp

