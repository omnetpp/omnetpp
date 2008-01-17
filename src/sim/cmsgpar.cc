//=========================================================================
//  CMSGPAR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "opp_ctype.h"
#include "cmsgpar.h"
#include "cstat.h"
#include "globals.h"
#include "cfunction.h"
#include "cxmlelement.h"
#include "cconfiguration.h"
#include "cenvir.h"
#include "cexception.h"
#include "csimulation.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

using std::ostream;
using std::string;


Register_Class(cMsgPar);

char *cMsgPar::possibletypes = "SBLDFTP";

static const char *typeName(char typechar)
{
    switch (typechar)
    {
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
    tkownership = false;
    changedflag = false;
    typechar = 'L';
    lng.val = 0L;
}

cMsgPar::cMsgPar(const cMsgPar& par) : cOwnedObject()
{
    tkownership = false;
    changedflag = false;
    typechar = 'L';
    lng.val = 0L;

    setName( par.name() );
    cMsgPar::operator=(par);
}

cMsgPar::cMsgPar(const char *name, cMsgPar& other) : cOwnedObject(name)
{
    tkownership = false;
    changedflag = false;
    typechar = 'L';
    lng.val = 0L;

    setName(name);
    operator=(other);
}

cMsgPar::~cMsgPar()
{
    beforeChange();
    deleteOld();
}

void cMsgPar::deleteOld()
{
    if (typechar=='S' && !ls.sht)
    {
        delete [] ls.str;
    }
    else if (typechar=='T')
    {
        if (dtr.res->owner()==this)
            dropAndDelete(dtr.res);
    }
    else if (typechar=='P')
    {
        if (ptr.dupfunc || ptr.itemsize>0) // we're expected to do memory mgmt
        {
            if (ptr.delfunc)
                ptr.delfunc(ptr.ptr);
            else
                delete [] (char *)ptr.ptr;  // cast because delete void* is not legal
        }
    }
    else if (typechar=='O')
    {
        if (obj.obj->owner()==this)
            dropAndDelete(obj.obj);
    }
    typechar = 'L';
}

//----------------------------------------------------------------------
// redefine virtual cOwnedObject funcs

std::string cMsgPar::info() const
{
    std::stringstream out;

    // append useful info
    cMathFunction *ff;
    const char *s;
    switch (typechar) {
        case 'S': s = ls.sht ? ss.str:ls.str;
                  if (!s) s = "";
                  out << "\"" << s << "\" (S)";
                  break;
        case 'L': out << lng.val << " (L)"; break;
        case 'D': out << dbl.val << " (D)"; break;
        case 'T': out << (dtr.res ? dtr.res->fullPath().c_str():"null") << " (T)"; break;
        case 'P': out << ptr.ptr << " (P)"; break;
        case 'O': out << (obj.obj ? obj.obj->fullPath().c_str():"null") << " (O)"; break;
        case 'F': ff = cMathFunction::findByPointer(func.f);
                  out << (ff ? ff->name() : "unknown") << "(";
                  switch(func.argc) {
                    case 0: out << ")"; break;
                    case 1: out << func.p1; break;
                    case 2: out << func.p1 << "," << func.p2; break;
                    case 3: out << func.p1 << "," << func.p2 << "," << func.p3; break;
                    case 4: out << func.p1 << "," << func.p2 << "," << func.p3 << "," << func.p4; break;
                    default:out << ") with " << func.argc << " args"; break;
                  };
                  out << " (F)";
                  break;
        case 'B': out << (lng.val?"true":"false") << " (B)"; break;
        case 'M': if (xmlp.node)
                      out << "<" << xmlp.node->getTagName() << "> from " << xmlp.node->getSourceLocation() << " (M)";
                  else
                      out << "null (M)";
                  break;
        default : out << "? (unknown type)"; break;
    }
    return out.str();
}

std::string cMsgPar::detailedInfo() const
{
    std::stringstream os;
    os << "  Type:  " << typechar << '\n';
    os << "  Value: " << toString().c_str() << '\n';
    return os.str();
}

void cMsgPar::forEachChild(cVisitor *v)
{
    if (typechar=='T')
    {
        v->visit(dtr.res);
    }
    else if (typechar=='O')
    {
        if (obj.obj)
            v->visit(obj.obj);
    }
}

void cMsgPar::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::netPack(buffer);

    // For error checking & handling
    if (typechar != 'S' && typechar != 'L' && typechar != 'D'
        && typechar != 'F' && typechar != 'T' && typechar != 'P'
        && typechar != 'O' && typechar != 'M')
    {
        throw cRuntimeError(this,"netPack: unsupported type '%c'",typechar);
    }

    buffer->pack(typechar);
    buffer->pack(changedflag);

    cMathFunction *ff;
    switch (typechar)
    {
    case 'S':
        buffer->pack(ls.sht);
        if (ls.sht)
            buffer->pack(ls.str, sizeof(ls.str));
        else
            buffer->pack(ss.str);
        break;

    case 'L':
        buffer->pack(lng.val);
        break;

    case 'D':
        buffer->pack(dbl.val);
        break;

    case 'F':
        ff = cMathFunction::findByPointer(func.f);
        if (ff == NULL)
            throw cRuntimeError(this,"netPack(): cannot transmit unregistered function");

        buffer->pack(ff->name());
        buffer->pack(func.argc);
        buffer->pack(func.p1);
        buffer->pack(func.p2);
        buffer->pack(func.p3);
        buffer->pack(func.p4);
        break;

    case 'T':
        if (dtr.res && dtr.res->owner() != this)
            throw cRuntimeError(this,"netPack(): cannot transmit pointer to \"external\" object");
        if (buffer->packFlag(dtr.res!=NULL))
            buffer->packObject(dtr.res);
        break;

    case 'P':
        throw cRuntimeError(this,"netPack(): cannot transmit pointer to unknown data structure (type 'P')");

    case 'O':
        if (obj.obj && obj.obj->owner() != this)
            throw cRuntimeError(this,"netPack(): cannot transmit pointer to \"external\" object");
        if (buffer->packFlag(obj.obj!=NULL))
            buffer->packObject(obj.obj);
        break;

    case 'M':
        throw cRuntimeError(this,"netPack(): cannot transmit pointer to XML element (type 'M')");
    }
#endif
}

void cMsgPar::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    char *funcname;
    int argc;

    cOwnedObject::netUnpack(buffer);

    buffer->unpack(typechar);
    buffer->unpack(changedflag);

    cMathFunction *ff;
    switch (typechar)
    {
    case 'S':
        buffer->unpack(ls.sht);
        ss.sht = ls.sht;
        if (ls.sht)
            buffer->unpack(ss.str, sizeof(ls.str));
        else
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
        ff = findFunction(funcname,argc);
        if (ff == NULL)
        {
            delete [] funcname;
            throw cRuntimeError(this,"netUnpack(): transmitted function `%s' with %d args not registered here",
                                    funcname, argc);
        }
        func.f = ff->mathFunc();
        func.argc = argc;
        buffer->unpack(func.p1);
        buffer->unpack(func.p2);
        buffer->unpack(func.p3);
        buffer->unpack(func.p4);
        delete [] funcname;
        break;

    case 'T':
        if (!buffer->checkFlag())
            dtr.res = NULL;
        else
            take(dtr.res = (cStatistic *) buffer->unpackObject());
        break;

    case 'P':
    case 'M':
        throw cRuntimeError(this,"netUnpack(): unpacking types I, P, M not implemented");

    case 'O':
        if (!buffer->checkFlag())
            obj.obj = NULL;
        else
            take(obj.obj = buffer->unpackObject());
        break;
    }
#endif
}

//----

char cMsgPar::type() const
{
     return typechar;
}

bool cMsgPar::changed()
{
     bool ch = changedflag;
     changedflag=false;
     return ch;
}

//----

cMsgPar& cMsgPar::setStringValue(const char *s)
{
     beforeChange();
     deleteOld();
     typechar = 'S';
     if (!s)
         {ls.sht=true; *ss.str='\0';}
     else if ((ls.sht=(strlen(s)<=SHORTSTR))!=0)
         opp_strcpy(ss.str, s);
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
    typechar = 'B';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setLongValue(long l)
{
    beforeChange();
    deleteOld();
    lng.val = l;
    typechar = 'L';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(double d)
{
    beforeChange();
    deleteOld();
    dbl.val = d;
    typechar = 'D';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(MathFuncNoArg f)
{
    beforeChange();
    deleteOld();
    func.f = (MathFunc)f;
    func.argc=0;
    typechar = 'F';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(MathFunc1Arg f, double p1)
{
    beforeChange();
    deleteOld();
    func.f = (MathFunc)f;
    func.argc=1;
    func.p1 = p1;
    typechar = 'F';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(MathFunc2Args f, double p1, double p2)
{
    beforeChange();
    deleteOld();
    func.f = (MathFunc)f;
    func.argc=2;
    func.p1 = p1;
    func.p2 = p2;
    typechar = 'F';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(MathFunc3Args f, double p1, double p2, double p3)
{
    beforeChange();
    deleteOld();
    func.f = (MathFunc)f;
    func.argc=3;
    func.p1 = p1;
    func.p2 = p2;
    func.p3 = p3;
    typechar = 'F';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(MathFunc4Args f, double p1, double p2, double p3, double p4)
{
    beforeChange();
    deleteOld();
    func.f = (MathFunc)f;
    func.argc=4;
    func.p1 = p1;
    func.p2 = p2;
    func.p3 = p3;
    func.p4 = p4;
    typechar = 'F';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setDoubleValue(cStatistic *res)
{
    if (!res)
        throw cRuntimeError(this,eBADINIT,typeName('T'));

    beforeChange();
    deleteOld();
    dtr.res = res;
    if (takeOwnership())
       take(res);
    typechar = 'T';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setPointerValue(void *_ptr)
{
    beforeChange();
    // if it was a 'P' before, keep previous configuration
    if (typechar!='P')
    {
        deleteOld();
        ptr.delfunc=NULL;
        ptr.dupfunc=NULL;
        ptr.itemsize=0;
        typechar = 'P';
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
    if (takeOwnership())
        take( _obj );
    typechar = 'O';
    afterChange();
    return *this;
}

cMsgPar& cMsgPar::setXMLValue(cXMLElement *node)
{
    beforeChange();
    deleteOld();
    xmlp.node = node;
    typechar = 'M';
    afterChange();
    return *this;
}

void cMsgPar::configPointer( VoidDelFunc delfunc, VoidDupFunc dupfunc,
                      size_t itemsize)
{
    if (typechar!='P')
        throw cRuntimeError(this,"configPointer(): type is '%c'; should be 'P'",typechar);
    ptr.delfunc = delfunc;
    ptr.dupfunc = dupfunc;
    ptr.itemsize = itemsize;
}

//----

const char *cMsgPar::stringValue()
{
    if (typechar!='S')
        throw cRuntimeError(this,eBADCAST,typeName(typechar),typeName('S'));
    return ss.sht ? ss.str : ls.str;
}

//
// Note:
// boolValue(), longValue() and doubleValue() are rather liberal: they all
// allow conversion from all of B,L and the double types D,T,F.
//

bool cMsgPar::boolValue()
{
    if (typechar=='B' || typechar=='L')
        return lng.val!=0;
    else if (isNumeric())
        return doubleValue()!=0;
    else
        throw cRuntimeError(this,eBADCAST,typeName(typechar),typeName('B'));
}

inline long _double_to_long(double d)
{
    // gcc 3.3 "feature": if double d=0xc0000000, (long)d yields 0x80000000!!!
    // This causes trouble if we in fact want to cast this long to unsigned long, see NED_expr_2.test.
    // Workaround follows. Note: even the ul variable is needed: when inlining it, gcc will do the wrong cast!
    long l = (long)d;
    unsigned long ul = (unsigned long)d;
    return d<0 ? l : ul;
}

long cMsgPar::longValue()
{
    if (typechar=='L' || typechar=='B')
        return lng.val;
    else if (isNumeric())
        return _double_to_long(doubleValue());
    else
        throw cRuntimeError(this,eBADCAST,typeName(typechar),typeName('L'));
}

double cMsgPar::doubleValue()
{
    if (typechar=='B' || typechar=='L')
        return (double)lng.val;
    else if (typechar=='D')
        return dbl.val;
    else if (typechar=='T')
        return fromstat();
    else if (typechar=='F')
        return func.argc==0 ? ((MathFuncNoArg)func.f)() :
               func.argc==1 ? ((MathFunc1Arg) func.f)(func.p1) :
               func.argc==2 ? ((MathFunc2Args)func.f)(func.p1,func.p2) :
               func.argc==3 ? ((MathFunc3Args)func.f)(func.p1,func.p2,func.p3) :
                              ((MathFunc4Args)func.f)(func.p1,func.p2,func.p3,func.p4);
    else
        throw cRuntimeError(this,eBADCAST,typeName(typechar),typeName('D'));
}

void *cMsgPar::pointerValue()
{
    if (typechar=='P')
        return ptr.ptr;
    else
        throw cRuntimeError(this,eBADCAST,typeName(typechar),typeName('P'));
}

cOwnedObject *cMsgPar::objectValue()
{
    if (typechar=='O')
        return obj.obj;
    else
        throw cRuntimeError(this,eBADCAST,typeName(typechar),typeName('O'));
}

cXMLElement *cMsgPar::xmlValue()
{
    if (typechar=='M')
        return xmlp.node;
    else
        throw cRuntimeError(this,eBADCAST,typeName(typechar),typeName('M'));
}

bool cMsgPar::isNumeric() const
{
    return typechar=='B' ||
           typechar=='L' ||
           typechar=='D' ||
           typechar=='T' ||
           typechar=='F';
}

bool cMsgPar::isConstant() const
{
    return typechar=='S' ||
           typechar=='B' ||
           typechar=='L' ||
           typechar=='D';
}

bool cMsgPar::equalsTo(cMsgPar *par)
{
    if (typechar != par->typechar)
        return false;

    switch (typechar) {
        case 'S': return strcmp(stringValue(),par->stringValue())==0;
        case 'B': return lng.val == par->lng.val;
        case 'L': return lng.val == par->lng.val;
        case 'D': return dbl.val == par->dbl.val;
        case 'F': if (func.f!=par->func.f) return 0;
                  switch(func.argc) {
                      case 4: if (func.p4!=par->func.p4) return 0; // no break
                      case 3: if (func.p3!=par->func.p3) return 0; // no break
                      case 2: if (func.p2!=par->func.p2) return 0; // no break
                      case 1: if (func.p1!=par->func.p1) return 0; // no break
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
    changedflag=true;
}

string cMsgPar::toString() const
{
    char bb[128];
    bb[0] = 0;
    cMathFunction *ff;
    const char *fn;
    const char *s;

    switch (typechar)
    {
       case 'S': s = ls.sht ? ss.str:ls.str;
                 return string("\"")+s+"\"";
       case 'B': return string(lng.val?"true":"false");
       case 'L': sprintf(bb,"%ld",lng.val);
                 return string(bb);
       case 'D': sprintf(bb,"%g",dbl.val);
                 return string(bb);
       case 'F': ff = cMathFunction::findByPointer(func.f);
                 fn = ff ? ff->name() : "unknown";
                 switch(func.argc) {
                 case 0: sprintf(bb,"()"); break;
                 case 1: sprintf(bb,"(%g)",func.p1); break;
                 case 2: sprintf(bb,"(%g,%g)",func.p1,func.p2); break;
                 case 3: sprintf(bb,"(%g,%g,%g)",func.p1,func.p2,func.p3); break;
                 case 4: sprintf(bb,"(%g,%g,%g,%g)",func.p1,func.p2,func.p3,func.p4); break;
                 default: sprintf(bb,"() with %d args",func.argc); break;
                 };
                 return string(fn)+bb;
       case 'T': return string("distribution ")+(dtr.res?dtr.res->fullPath().c_str():"NULL");
       case 'P': sprintf(bb,"pointer %p", ptr.ptr); return string(bb);
       case 'O': return string("object ")+(obj.obj?obj.obj->fullPath().c_str():"NULL");
       case 'M': if (xmlp.node)
                     return string("<")+xmlp.node->getTagName()+"> from "+xmlp.node->getSourceLocation();
                 else
                     return string("NULL");
                 break;
       default : return string("???");
    }
}

static bool parseQuotedString(string& str, const char *&s)  //FIXME use opp_parsequotedstr() instead!
{
    while (*s==' ' || *s=='\t') s++;
    if (*s!='"') return false;
    const char *beg = ++s;
    while (*s && (*s!='"' || *(s-1)=='\\'))
        s++;
    if (*s!='"') return false;
    str.assign(beg, s-beg);
    s++;
    return true;
}

bool cMsgPar::parse(const char *text, char tp)
{
    tp = (char) opp_toupper(tp);

    // create a working copy and cut whitespaces (from both sides)
    if (!text) return false;  // error: no string
    while (*text==' ' || *text=='\t') text++;
    if (*text=='\0') return false; // error: empty string (or only whitespace)
    char *tmp = opp_strdup(text);
    char *s = tmp+strlen(tmp)-1;
    while (s>=tmp && (*s==' ' || *s=='\t')) *s--='\0';

    if (strcmp(tmp,"true")==0 || strcmp(tmp,"TRUE")==0 || strcmp(tmp,"True")==0) // bool?
    {
        if (!strchr("?B",tp)) goto error;
        setBoolValue(true);
    }
    else if (strcmp(tmp,"false")==0 || strcmp(tmp,"FALSE")==0 || strcmp(tmp,"False")==0) // bool?
    {
        if (!strchr("?B",tp)) goto error;
        setBoolValue(false);
    }
    else if (strcmp(tmp,"1")==0 && tp=='B') // bool?
    {
        setBoolValue(true);
    }
    else if (strcmp(tmp,"0")==0 && tp=='B') // bool?
    {
        setBoolValue(false);
    }
    else if (tmp[0]=='\'' && tmp[1] && tmp[2]=='\''&& !tmp[3]) // char? (->long)
    {
        if (!strchr("?L",tp)) goto error;
        setLongValue((long)tmp[1]);
    }
    else if (text[0]=='\"') // string?
    {
        if (!strchr("?S",tp)) goto error;

        // check closing quote
        if (!tmp[1] || tmp[strlen(tmp)-1]!='\"') goto error;  //FIXME use opp_parsequotedstr() and catch exception

        tmp[strlen(tmp)-1] = '\0'; // cut off closing quote
        setStringValue(tmp+1);
    }
    else if (strspn(tmp,"+-0123456789")==strlen(tmp)) // long?
    {
        long num;
        int len;
        if (0==sscanf(tmp,"%ld%n",&num,&len)) goto error;
        if (len < (int)strlen(tmp) || !strchr("?LD",tp)) goto error;
        if (tp=='?' || tp=='L')
           setLongValue(num);
        else
           setDoubleValue(num);
    }
    else if (strspn(tmp,"+-.eE0123456789")==strlen(tmp)) // double?
    {
        double num;
        int len;
        if (0==sscanf(tmp,"%lf%n",&num,&len)) goto error;
        if (len < (int)strlen(tmp) || !strchr("?D",tp)) goto error;
        setDoubleValue(num);
    }
    else if (!strncmp(tmp,"xmldoc",6))
    {
        if (!strchr("?M",tp)) goto error;

        // parse xmldoc("filename") or xmldoc("filename", "pathexpr")
        const char *s=tmp;
        s+=6;  // skip "xmldoc"
        while (*s==' ' || *s=='\t') s++;
        if (*s!='(') goto error;  // no "("
        s++; // skip "("
        std::string fname, pathexpr;
        while (*s==' ' || *s=='\t') s++;
        if (!parseQuotedString(fname, s)) goto error;
        while (*s==' ' || *s=='\t') s++;
        if (*s!=',' && *s!=')') goto error;  // no ")" or ","
        if (*s==',')
        {
            s++;  // skip ","
            if (!parseQuotedString(pathexpr, s)) goto error;
            while (*s==' ' || *s=='\t') s++;
            if (*s!=')') goto error;  // no ")"
        }
        s++; // skip ")"
        while (*s==' ' || *s=='\t') s++;
        if (*s) goto error;  // trailing rubbish

        cXMLElement *node = ev.getXMLDocument(fname.c_str(), pathexpr.empty() ? NULL : pathexpr.c_str());
        if (!node)
            throw cRuntimeError(this,"%s: element not found", tmp);
        setXMLValue(node);
    }
    else // maybe function; try to parse it
    {
        if (!strchr("?F",tp)) goto error;
        if (!setfunction(tmp)) goto error;
    }

    delete [] tmp;
    return true;

    error:
    delete [] tmp;
    return false;
}

static double parsedbl(const char *&s)
{
    while (*s==' ' || *s=='\t') s++;
    int len = 0;
    double num = 0;
    sscanf(s, "%lf%n", &num, &len);
    s += len;
    while (*s==' ' || *s=='\t') s++;
    return num;
}


bool cMsgPar::setfunction(char *text)
{
    // Note: this function *will* alter its input string

    // find '('
    char *d;
    for (d=text; *d!='(' && *d!='\0'; d++);
    if (*d!='(') return false;  // no opening paren
    char *args = d;

    // remove whitespaces in-place
    const char *s;
    for (s=d=args; *s; s++)
       if (!opp_isspace(*s))
          *d++ = *s;
    *d = '\0';

    // determine argccount: number of commas+1, or zero
    int commas = 0;
    for (d=args; *d; d++)
        if (*d==',')
            commas++;
    int argc;
    if (commas==0 && args[1]==')')
        argc = 0;
    else
        argc = commas+1;

    // look up function name (temporarily overwriting '(' with a '\0')
    *args = '\0';
    cMathFunction *ff = cMathFunction::find(text, argc);
    *args = '(';
    if (ff==NULL) return false;


    // now `args' points to something like '(10,1.5E-3)', without spaces
    s = args;
    double p1,p2,p3,p4;
    switch(ff->numArgs())
    {
       case 0: if (strcmp(s,"()")!=0) return false;
               setDoubleValue(ff->mathFuncNoArg());
               return true;
       case 1: if (*s++!='(') return false;
               p1 = parsedbl(s);
               if (*s++!=')') return false;
               setDoubleValue(ff->mathFunc1Arg(), p1);
               return true;
       case 2: if (*s++!='(') return false;
               p1 = parsedbl(s);
               if (*s++!=',') return false;
               p2 = parsedbl(s);
               if (*s++!=')') return false;
               setDoubleValue(ff->mathFunc2Args(), p1,p2);
               return true;
       case 3: if (*s++!='(') return false;
               p1 = parsedbl(s);
               if (*s++!=',') return false;
               p2 = parsedbl(s);
               if (*s++!=',') return false;
               p3 = parsedbl(s);
               if (*s++!=')') return false;
               setDoubleValue(ff->mathFunc3Args(), p1,p2,p3);
               return true;
       case 4: if (*s++!='(') return false;
               p1 = parsedbl(s);
               if (*s++!=',') return false;
               p2 = parsedbl(s);
               if (*s++!=',') return false;
               p3 = parsedbl(s);
               if (*s++!=',') return false;
               p4 = parsedbl(s);
               if (*s++!=')') return false;
               setDoubleValue(ff->mathFunc4Args(), p1,p2,p3,p4);
               return true;
       default:
               return false; // invalid argcount
    }
}


double cMsgPar::fromstat()
{
    if (typechar!='T')
        throw cRuntimeError(this,eBADCAST,typeName(typechar),typeName('T'));
    return  dtr.res->random();
}


cMsgPar& cMsgPar::operator=(const cMsgPar& val)
{
    if (this==&val) return *this;

    beforeChange();
    deleteOld();

    cOwnedObject::operator=(val);
    typechar = val.typechar;
    changedflag = val.changedflag;

    // this line is supposed to copy the whole data area.
    memcpy( &ss, &val.ss, Max(sizeof(ss), sizeof(func)) );

    if (typechar=='S' && !ss.sht)
    {
         // make our copy of the string
         ls.str = opp_strdup(ls.str);
    }
    else if (typechar=='T')
    {
         cStatistic *&p = dtr.res;
         if (p->owner()==const_cast<cMsgPar*>(&val))
            take( p=(cStatistic *)p->dup() );
    }
    else if (typechar=='P')
    {
         if (ptr.dupfunc)
            ptr.ptr = ptr.dupfunc( ptr.ptr );
         else if (ptr.itemsize>0)
            memcpy(ptr.ptr=new char[ptr.itemsize],val.ptr.ptr,ptr.itemsize);
         // if no dupfunc or itemsize, only the pointer is copied
    }
    else if (typechar=='O')
    {
         cOwnedObject *&p = obj.obj;
         if (p->owner()==const_cast<cMsgPar*>(&val))
            take( p = (cOwnedObject *)p->dup() );
    }

    afterChange();
    return *this;
}


int cMsgPar::cmpbyvalue(cOwnedObject *one, cOwnedObject *other)
{
    double x = (double)(*(cMsgPar*)one)-(double)(*(cMsgPar*)other);
    return sgn(x);
}

void cMsgPar::convertToConst ()
{
    if (strchr("FT", typechar))
    {
       double d = doubleValue();
       setDoubleValue(d);
    }
}


