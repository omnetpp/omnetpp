//=========================================================================
//
//  CPAR.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Member functions of
//     cPar       : object holding a value; a number of types supported
//     cModulePar : module parameter
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
Copyright (C) 1992-2001 Andras Varga
Technical University of Budapest, Dept. of Telecommunications,
Stoczek u.2, H-1111 Budapest, Hungary.

This file is distributed WITHOUT ANY WARRANTY. See the file
`license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "cpar.h"
#include "cstat.h"
#include "macros.h"
#include "errmsg.h"
#include "cmodule.h"  // for cModulePar
#include "csimul.h"   // for cModulePar
#include "cenvir.h"
#include "cexception.h"

//==============================================
//=== Registration
Register_Class(cPar);

//==========================================================================
//=== cDoubleExpression - member functions

void cDoubleExpression::getAsText(char *buf, int maxlen)
{
    sprintf(buf, "%d", evaluate());
}

bool cDoubleExpression::parseText(const char *text)
{
    throw new cException("cDoubleExpression: parseText() does not work with compiled expressions");
    return false;
}

//==========================================================================
//=== cPar - member functions

char *cPar::possibletypes = "SBLDFIXCTP";

// constructors
cPar::cPar(const char *name) : cObject( name )
{
    takeOwnership( false );         // doesn't take the objects!
    changedflag = inputflag = false;
    typechar = 'L'; lng.val = 0L;
}

cPar::cPar(const cPar& par) : cObject()
{
    takeOwnership( false );         // doesn't take the objects!
    changedflag = inputflag = false;
    typechar = 'L'; lng.val = 0L;

    setName( par.name() );
    cPar::operator=(par);
}

cPar::cPar(const char *name, cPar& other) : cObject(name)
{
    takeOwnership( false );         // doesn't take the objects!
    changedflag = inputflag = false;
    typechar = 'L'; lng.val = 0L;

    setName(name);
    operator=(other);
}

cPar::~cPar()
{
    beforeChange();
    if (isRedirected())
        cancelRedirection();
    deleteold();
}

void cPar::deleteold()
{
    if (typechar=='S' && !ls.sht)
    {
        delete [] ls.str;
    }
    else if (typechar=='T')
    {
        if (dtr.res->owner()==this)
            discard( dtr.res );
    }
    else if (typechar=='C')
    {
        delete cexpr.expr;
    }
    else if (typechar=='P')
    {
        if (ptr.dupfunc || ptr.itemsize>0) // we're expected to do memory mgmt
        {
            if (ptr.delfunc)
                ptr.delfunc(ptr.ptr);
            else
                delete [] (char *)ptr.ptr;  // delete void* is no longer legal :-(
        }
    }
    else if (typechar=='O')
    {
        if (obj.obj->owner()==this)
            discard( obj.obj );
    }
    else if (typechar=='X')
    {
        for(int i=0; i<expr.n; i++)
            if (expr.xelem[i].type=='R')  // should be ours
                discard( expr.xelem[i].p );
        delete [] expr.xelem;
    }
    typechar = 'L';
}

//----------------------------------------------------------------------
// redefine virtual cObject funcs

void cPar::info( char *buf )
{
    cObject::info( buf );

    // find end of string and append a space
    char *b = buf;
    while(*b) b++;
    *b++ = ' '; *b='\0';

    // redirection?
    if (isRedirected())
    {
        strcpy(b,"--> ");
        strcpy(b+4, ind.par->fullPath() );
        return;
    }

    // append useful info
    cFunctionType *ff;
    const char *fn;
    char *s;
    switch (typechar) {
        case 'S': s = ls.sht ? ss.str:ls.str;
                  if (!s) s = "";
                  if (strlen(s)<=40)
                    sprintf(b,"\"%s\" (S)", s);
                  else
                    sprintf(b,"\"%.40s...\" [truncated] (S)", s);
                  break;
        case 'L': sprintf(b,"%ld (L)",lng.val); break;
        case 'D': sprintf(b,"%g (D)",dbl.val); break;
        case 'C': sprintf(b,"compiled expression"); break;
        case 'X': sprintf(b,"reverse Polish expression"); break;
        case 'T': sprintf(b,"(%s) (T)", dtr.res ? dtr.res->fullPath():"nil"); break;
        case 'P': sprintf(b,"(%p) (P)", ptr.ptr); break;
        case 'O': sprintf(b,"(%s) (O)", obj.obj ? obj.obj->fullPath():"nil"); break;
        case 'F': ff = findfunctionbyptr(func.f);
                  if (ff) fn=ff->name(); else fn="unknown";
                  switch(func.argc) {
                  case 0: sprintf(b,"%s() (F)",fn); break;
                  case 1: sprintf(b,"%s(%g) (F)",fn,func.p1); break;
                  case 2: sprintf(b,"%s(%g,%g) (F)",fn,func.p1,func.p2); break;
                  case 3: sprintf(b,"%s(%g,%g,%g) (F)",fn,func.p1,func.p2,func.p3); break;
                  };
                  break;
        case 'B': sprintf(b,"%s (B)", lng.val?"true":"false"); break;
        default : strcat(b, "? (unknown type)"); break;
    }
}

void cPar::writeContents(ostream& os)
{
    char buf[256];
    if (isRedirected())
    {
        os << "  Type:   redirection ('I')\n";
        os << "  Target: " << ind.par->fullPath() << '\n';
    }
    else
    {
        os << "  Type:  " << typechar << '\n';
        os << "  Value: " << (getAsText(buf,255), buf) << '\n';
    }
}

//-------------------------------------------------------------------------
// set/get flags

char cPar::type() const
{
     if (isRedirected())
         return ind.par->type();
     return typechar;
}

const char *cPar::prompt()
{
     if (isRedirected())
         return ind.par->prompt();
     return promptstr;
}

bool cPar::isInput() const
{
     if (isRedirected())
         return ind.par->isInput();
     return inputflag;
}

bool cPar::changed()
{
     if (isRedirected())
         return ind.par->changed();
     bool ch = changedflag;
     changedflag=false;
     return ch;
}

void cPar::setPrompt(const char *s)
{
     if (isRedirected())
         {ind.par->setPrompt(s);return;}
     beforeChange();
     promptstr = s;     // string's operator=() does delete+opp_strdup()
     afterChange();
}

void cPar::setInput(bool ip)
{
     if (isRedirected())
         {ind.par->setInput(ip);return;}
     if (inputflag!=ip)
     {
         beforeChange();
         inputflag = ip;
         afterChange();
     }
}

//-----------------------------------------------------------------------
// setXxxValue() funcs

cPar& cPar::setStringValue(const char *s)
{
     if (isRedirected())
         return ind.par->setStringValue(s);

     beforeChange();
     deleteold();
     typechar = 'S';
     inputflag=false;
     if (!s)
         {ls.sht=true; *ss.str='\0';}
     else if ((ls.sht=(strlen(s)<=SHORTSTR))!=0)
         opp_strcpy(ss.str, s);
     else
         ls.str = opp_strdup(s);
     afterChange();
     return *this;
}

cPar& cPar::setBoolValue(bool b)
{
    if (isRedirected())
        return ind.par->setBoolValue(b);

    beforeChange();
    deleteold();
    lng.val = b;
    typechar = 'B';
    inputflag=false;
    afterChange();
    return *this;
}

cPar& cPar::setLongValue(long l)
{
    if (isRedirected())
        return ind.par->setLongValue(l);

    beforeChange();
    deleteold();
    lng.val = l;
    typechar = 'L';
    inputflag=false;
    afterChange();
    return *this;
}

cPar& cPar::setDoubleValue(double d)
{
    if (isRedirected())
        return ind.par->setDoubleValue(d);

    beforeChange();
    deleteold();
    dbl.val = d;
    typechar = 'D';
    inputflag=false;
    afterChange();
    return *this;
}

cPar& cPar::setDoubleValue(MathFuncNoArg f)
{
    if (isRedirected())
        return ind.par->setDoubleValue(f);

    beforeChange();
    deleteold();
    func.f = (MathFunc)f;
    func.argc=0;
    typechar = 'F';
    inputflag=false;
    afterChange();
    return *this;
}

cPar& cPar::setDoubleValue(MathFunc1Arg f, double p1)
{
    if (isRedirected())
        return ind.par->setDoubleValue(f,p1);

    beforeChange();
    deleteold();
    func.f = (MathFunc)f;
    func.argc=1;
    func.p1 = p1;
    typechar = 'F';
    inputflag=false;
    afterChange();
    return *this;
}

cPar& cPar::setDoubleValue(MathFunc2Args f, double p1, double p2)
{
    if (isRedirected())
        return ind.par->setDoubleValue(f,p1,p2);

    beforeChange();
    deleteold();
    func.f = (MathFunc)f;
    func.argc=2;
    func.p1 = p1;
    func.p2 = p2;
    typechar = 'F';
    inputflag=false;
    afterChange();
    return *this;
}

cPar& cPar::setDoubleValue(MathFunc3Args f, double p1, double p2, double p3)
{
    if (isRedirected())
        return ind.par->setDoubleValue(f,p1,p2,p3);

    beforeChange();
    deleteold();
    func.f = (MathFunc)f;
    func.argc=3;
    func.p1 = p1;
    func.p2 = p2;
    func.p3 = p3;
    typechar = 'F';
    inputflag=false;
    afterChange();
    return *this;
}

cPar& cPar::setDoubleValue(sXElem *x, int n)
{
    if (isRedirected())
        return ind.par->setDoubleValue(x,n);

    if (!x)
        throw new cException(eBADINIT,className(),name(), 'X');

    beforeChange();
    deleteold();
    expr.n = n;
    expr.xelem = x;
    typechar = 'X';
    inputflag=false;

    // ownership game: take objects given to us in the x[] array
    for(int i=0; i<expr.n; i++)
    {
       if (expr.xelem[i].type=='R')
       {
           //  sXElem::op= has already dupped the pointed cPar for us
           cPar *p = expr.xelem[i].p;

           // if the pointed cPar is not indirect and it is a constant,
           // there's really no need to keep the object
           if (!p->isRedirected() && (p->type()=='D' || p->type()=='L'))
           {
               expr.xelem[i] = (double)(*p);
               delete p;
           }
           else // otherwise, we'll become the owner
           {
               take( p );
           }
       }
    }
    afterChange();
    return *this;
}

cPar& cPar::setDoubleValue(cDoubleExpression *p)
{
    if (isRedirected())
        return ind.par->setDoubleValue(p);

    if (!p)
        throw new cException(eBADINIT,className(),name(), 'P');

    beforeChange();
    deleteold();
    cexpr.expr = p;
    typechar = 'C';
    inputflag=false;
    afterChange();
    return *this;
}

cPar& cPar::setDoubleValue(cStatistic *res)
{
    if (isRedirected())
        return ind.par->setDoubleValue(res);

    if (!res)
        throw new cException(eBADINIT,className(),name(), 'T');

    beforeChange();
    deleteold();
    dtr.res = res;
    if (takeOwnership())
       take(res);
    typechar = 'T';
    inputflag=false;
    afterChange();
    return *this;
}

cPar& cPar::setPointerValue(void *_ptr)
{
    if (isRedirected())
        return ind.par->setPointerValue(_ptr);

    beforeChange();
    // if it was a 'P' before, keep previous configuration
    if (typechar!='P')
    {
        deleteold();
        ptr.delfunc=NULL;
        ptr.dupfunc=NULL;
        ptr.itemsize=0;
        typechar = 'P';
    }
    ptr.ptr = _ptr;
    inputflag=false;
    afterChange();
    return *this;
}

cPar& cPar::setObjectValue(cObject *_obj)
{
    if (isRedirected())
        return ind.par->setObjectValue(_obj);

    beforeChange();
    deleteold();
    obj.obj = _obj;
    if (takeOwnership())
        take( _obj );
    typechar = 'O';
    inputflag=false;
    afterChange();
    return *this;
}

cPar& cPar::setRedirection(cPar *par)
{
    if (isRedirected())
        return ind.par->setRedirection(par);

    if (!par)
        throw new cException(eBADINIT,className(),name(), 'I');

    // check for circular references
    cPar *p = par;
    while (p)
    {
        if (p==this)
            throw new cException(eCIRCREF,className(),name());
        p = p->isRedirected() ? p->ind.par : NO(cPar);
    }

    // set redirection
    beforeChange();
    deleteold();
    ind.par = par; // do NOT take ownership of passed cPar object
    typechar = 'I';
    inputflag=false;
    afterChange();
    return *this;
}

void cPar::configPointer( VoidDelFunc delfunc, VoidDupFunc dupfunc,
                      size_t itemsize)
{
    if (typechar!='P')
        throw new cException("(%s)%s: configPointer(): type is '%c';"
                  " should be 'P'",className(),name(),typechar);
    else
    {
         ptr.delfunc = delfunc;
         ptr.dupfunc = dupfunc;
         ptr.itemsize = itemsize;
    }
}

//------------------------------------------------------------------------
// functions returning the stored value

const char *cPar::stringValue()
{
    if (isRedirected())
        return ind.par->stringValue();

    if (isInput()) read();
    if (typechar=='S')
         return ss.sht ? ss.str : ls.str;
    else
         throw new cException(eBADCAST,className(),name(),typechar,'S');
}

//
// Note:
// boolValue(), longValue() and doubleValue() are rather liberal: they all
// allow conversion from all of B,L and the double types D,T,X,F.
// Aesthetically, this is nasty a feature (because it is against the strongly
// typed nature of C++; you can do narrowing conversions [double->bool]
// without a warning, etc.), but it is necessary because of the way parameter
// assignments in NED are handled. For example, bool and long expressions
// (e.g. "i==0 || i==n") are compiled into cPar X expressions which will _need_
// to be converted to bool and long after evaluation.
//

bool cPar::boolValue()
{
    if (isRedirected())
        return ind.par->boolValue();

    if (isInput()) read();

    if (typechar=='B' || typechar=='L')
        return lng.val!=0;
    else if (isNumeric())
        return doubleValue()!=0;
    else
        throw new cException(eBADCAST,className(),name(),typechar,'B');
}

long cPar::longValue()
{
    if (isRedirected())
        return ind.par->longValue();

    if (isInput()) read();
    if (typechar=='L' || typechar=='B')
        return lng.val;
    else if (isNumeric())
        return (long)doubleValue();
    else
        throw new cException(eBADCAST,className(),name(),typechar,'L');
}

double cPar::doubleValue()
{
    if (isRedirected())
        return ind.par->doubleValue();

    if (isInput()) read();
    if (typechar=='B' || typechar=='L')
        return (double)lng.val;
    else if (typechar=='D')
        return dbl.val;
    else if (typechar=='T')
        return fromstat();
    else if (typechar=='X')
        return evaluate();
    else if (typechar=='C')
        return cexpr.expr->evaluate();
    else if (typechar=='F')
        return func.argc==0 ? ((MathFuncNoArg)func.f)() :
               func.argc==1 ? ((MathFunc1Arg) func.f)(func.p1) :
               func.argc==2 ? ((MathFunc2Args)func.f)(func.p1,func.p2) :
                              ((MathFunc3Args)func.f)(func.p1,func.p2,func.p3);
    else
        throw new cException(eBADCAST,className(),name(),typechar,'D');
}

void *cPar::pointerValue()
{
    if (isRedirected())
        return ind.par->pointerValue();

    if (isInput()) read();
    if (typechar=='P')
        return ptr.ptr;
    else
        throw new cException(eBADCAST,className(),name(),typechar,'P');
}

cObject *cPar::objectValue()
{
    if (isRedirected())
        return ind.par->objectValue();

    if (isInput()) read();
    if (typechar=='O')
        return obj.obj;
    else
        throw new cException(eBADCAST,className(),name(),typechar,'O');
}

bool cPar::isNumeric() const
{
    // returns true if it is safe to call doubleValue()/longValue()/boolValue()
    if (isRedirected())
        return ind.par->isNumeric();

    return typechar=='B' ||
           typechar=='L' ||
           typechar=='D' ||
           typechar=='T' ||
           typechar=='X' ||
           typechar=='C' ||
           typechar=='F';
}


bool cPar::equalsTo(cPar *par)
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
                      case 3: if (func.p3!=par->func.p3) return 0; // no break
                      case 2: if (func.p2!=par->func.p2) return 0; // no break
                      case 1: if (func.p1!=par->func.p1) return 0; // no break
                  }
                  return 1;
        case 'T': return dtr.res == par->dtr.res;
        case 'P': return ptr.ptr == par->ptr.ptr;
        case 'O': return obj.obj == par->obj.obj;
        case 'X': throw new cException("(%s): equalsTo() with X type not implemented",className());
        case 'C': throw new cException("(%s): equalsTo() with C type not implemented",className());
        default: return 0;
    }
}

//------------------------------------------------------------------------
// misc funcs

void cPar::beforeChange()
{
}

void cPar::afterChange()
{
    changedflag=true;
}

cPar *cPar::redirection()
{
    if (isRedirected())
        return ind.par;
    else
        return NULL;
}

void cPar::cancelRedirection()
{
    if (isRedirected())
    {
        // operator=( *ind.par ); -- the user may do this by hand
        typechar = 'L'; lng.val = 0L;
    }
}

void cPar::getAsText(char *buf, int maxlen)
{
    if (isRedirected()) {
        ind.par->getAsText(buf, maxlen );
        return;
    }

    buf[0] = 0;
    if( maxlen<=1 ) return;

    char bb[128];
    bb[0] = 0;
    cFunctionType *ff;
    const char *fn;
    char *s;
    switch (typechar) {
       case 'S': s = ls.sht ? ss.str:ls.str;
                 if (!s) s = "";
                 if (strlen(s)<=125)
                   sprintf(bb,"\"%s\"", s);
                 else
                   sprintf(bb,"\"%.110s...\" [truncated]", s);
                 break;
       case 'B': strcpy(bb,lng.val?"true":"false"); break;
       case 'L': sprintf( bb,"%ld",lng.val); break;
       case 'D': sprintf( bb,"%g",dbl.val); break;
       case 'F': ff = findfunctionbyptr(func.f);
                 if (ff) fn=ff->name(); else fn="unknown";
                 switch(func.argc) {
                 case 0: sprintf(bb,"%s()",fn); break;
                 case 1: sprintf(bb,"%s(%g)",fn,func.p1); break;
                 case 2: sprintf(bb,"%s(%g,%g)",fn,func.p1,func.p2); break;
                 case 3: sprintf(bb,"%s(%g,%g,%g)",fn,func.p1,func.p2,func.p3); break;
                 };
                 break;
       case 'T': sprintf(bb,"distribution %.99s", dtr.res ? dtr.res->fullPath():"nil"); break;
       case 'P': sprintf(bb,"pointer %p", ptr.ptr); break;
       case 'O': sprintf(bb,"object %.99s", obj.obj ? obj.obj->fullPath():"nil"); break;
       case 'C': sprintf(bb,"compiled expression"); break;
       case 'X': sprintf(bb,"reverse Polish expression"); break;
       default : bb[0]='?'; bb[1]=0; break;
    }
    strncpy( buf, bb, maxlen<128 ? maxlen : 128);
}

/*-------------------------------
 setFromText() - (for internal use only) Tries to interpret text as
 a 'tp' typed value. tp=='?' means that even type is to be determined.
 On success, cPar is updated and true is returned, otherwise
 it returns false. No error message is ever generated.
----------------------------*/

bool cPar::setFromText(const char *text, char tp)
{
    tp = (char) toupper(tp);

    // create a working copy and cut whitespaces (from both sides)
    if (!text) return false;  // error: no string
    while (*text==' ' || *text=='\t') text++;
    if (*text=='\0') return false; // error: empty string (or only whitespace)
    char *tmp = opp_strdup(text);
    char *s = tmp+strlen(tmp)-1;
    while (s>=tmp && (*s==' ' || *s=='\t')) *s--='\0';

    double d;

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
        if (!tmp[1] || tmp[strlen(tmp)-1]!='\"') goto error;

        tmp[strlen(tmp)-1] = '\0'; // cut off closing quote
        setStringValue(tmp+1);
    }
    else if (strspn(tmp,"+-0123456789")==strlen(tmp)) // long?
    {
        long num;
        unsigned len;
        if (0==sscanf(tmp,"%ld%n",&num,&len)) goto error;
        if (len<strlen(tmp) || !strchr("?LD",tp)) goto error;
        if (tp=='?' || tp=='L')
           setLongValue(num);
        else
           setDoubleValue(num);
    }
    else if (strspn(tmp,"+-.eE0123456789")==strlen(tmp)) // double?
    {
        double num;
        unsigned len;
        if (0==sscanf(tmp,"%lf%n",&num,&len)) goto error;
        if (len<strlen(tmp) || !strchr("?D",tp)) goto error;
        setDoubleValue(num);
    }
    else if ((d=strToSimtime(tmp))>=0.0) // double given as time?
    {
        if (!strchr("?D",tp)) goto error;
        setDoubleValue(d);
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

bool cPar::setfunction(char *text)
{
    // Note: this function *will* alter its input string

    // find '('
    char *d;
    for (d=text; *d!='(' && *d!='\0'; d++);
    if (*d!='(') return false;  // no opening paren
    char *args = d;

    // look up function name (temporarily overwriting '(' with a '\0')
    *args = '\0';
    cFunctionType *ff = findFunction( text );
    *args = '(';
    if (ff==NULL) return false;

    // remove whitespaces in-place
    const char *s;
    for (s=d=args; *s; s++)
       if (!isspace(*s))
          *d++ = *s;
    *d = '\0';

    // now `args' points to something like '(10,1.5E-3)', without spaces
    s = args;
    double p1,p2,p3;
    switch(ff->argcount)
    {
       case 0: if (strcmp(s,"()")!=0) return false;
               setDoubleValue((MathFuncNoArg)ff->f);
               return true;
       case 1: if (*s++!='(') return false;
               p1 = strToSimtime0(s);
               if (*s++!=')') return false;
               setDoubleValue((MathFunc1Arg)ff->f, p1);
               return true;
       case 2: if (*s++!='(') return false;
               p1 = strToSimtime0(s);
               if (*s++!=',') return false;
               p2 = strToSimtime0(s);
               if (*s++!=')') return false;
               setDoubleValue((MathFunc2Args)ff->f, p1,p2);
               return true;
       case 3: if (*s++!='(') return false;
               p1 = strToSimtime0(s);
               if (*s++!=',') return false;
               p2 = strToSimtime0(s);
               if (*s++!=',') return false;
               p3 = strToSimtime0(s);
               if (*s++!=')') return false;
               setDoubleValue((MathFunc3Args)ff->f, p1,p2,p3);
               return true;
       default:
               return false; // invalid argcount
    }
}


cPar& cPar::read()
{
    char buf[256];

    // get it from ini file
    const char *s = ev.getParameter(simulation.runNumber(), fullPath());
    if (s!=NULL)
    {
       strcpy(buf,s);
       bool success = setFromText(buf,'?');
       if (!success)
             throw new cException( "Wrong value `%s' for parameter `%s'",
                           buf, fullPath() );
       return *this;
    }

    // ask the user
    bool success;
    do {
        bool esc;
        getAsText( buf, 255 );
        if (promptstr)
            esc = ev.gets(promptstr,buf);
        else if (name())
            esc = ev.askf(buf,255, "Enter parameter `%s':",fullPath());
        else
            esc = ev.gets("Enter unnamed parameter:",buf);

        if (esc) {
            // throw new cException(eCANCEL); -- already issued in cEnvir::gets()
            return *this;
        }

        success = setFromText(buf,'?');
        if (!success)
              ev.printfmsg( "Syntax error, try again." );
    } while (!success);
    return *this;
}


double cPar::evaluate()
{
    const int stksize = 20;

      /*---------------------------------
         Type X (expression) - interprets the expression given
            in the sXElem vector as a reversed Polish one and
            evaluates it.
         --------------------------*/
    if (typechar!='X')
         return 0.0;

    double stk[stksize];

    int tos = -1;

    for(int i=0; i<expr.n; i++) {
       sXElem& e = expr.xelem[i];
       switch( toupper(e.type) ) {
           case 'D':
             if(tos>=stksize - 1) throw new cException(eBADEXP,className(),name());
             stk[++tos] = e.d;
             break;
           case 'P':
           case 'R':
             if(!e.p || tos>=stksize - 1) throw new cException(eBADEXP,className(),name());
             stk[++tos] = (double)(*e.p);
             break;
           case '0':
             if(!e.f0) throw new cException(eBADEXP,className(),name());
             stk[++tos] = e.f0();
             break;
           case '1':
             if(!e.f1 || tos<0) throw new cException(eBADEXP,className(),name());
             stk[tos] = e.f1( stk[tos] );
             break;
           case '2':
             if(!e.f2 || tos<1) throw new cException(eBADEXP,className(),name());
             stk[tos-1] = e.f2( stk[tos-1], stk[tos] );
             tos--;
             break;
           case '3':
             if(!e.f3 || tos<2) throw new cException(eBADEXP,className(),name());
             stk[tos-2] = e.f3( stk[tos-2], stk[tos-1], stk[tos] );
             tos-=2;
             break;
           case '@':
             if(!e.f2 || tos<1) throw new cException(eBADEXP,className(),name());
             switch(e.op) {
                case '+':
                   stk[tos-1] = stk[tos-1] + stk[tos];
                   tos--;
                   break;
                case '-':
                   stk[tos-1] = stk[tos-1] - stk[tos];
                   tos--;
                   break;
                case '*':
                   stk[tos-1] = stk[tos-1] * stk[tos];
                   tos--;
                   break;
                case '/':
                   stk[tos-1] = stk[tos-1] / stk[tos];
                   tos--;
                   break;
                case '%':
                   stk[tos-1] = (long)stk[tos-1] % (long)stk[tos];
                   tos--;
                   break;
                case '^':
                   stk[tos-1] = pow( stk[tos-1], stk[tos] );
                   tos--;
                   break;
                case '=':
                   stk[tos-1] = stk[tos-1] == stk[tos];
                   tos--;
                   break;
                case '!':
                   stk[tos-1] = stk[tos-1] != stk[tos];
                   tos--;
                   break;
                case '<':
                   stk[tos-1] = stk[tos-1] < stk[tos];
                   tos--;
                   break;
                case '{':
                   stk[tos-1] = stk[tos-1] <= stk[tos];
                   tos--;
                   break;
                case '>':
                   stk[tos-1] = stk[tos-1] > stk[tos];
                   tos--;
                   break;
                case '}':
                   stk[tos-1] = stk[tos-1] >= stk[tos];
                   tos--;
                   break;
                case '?':
                   stk[tos-2] = stk[tos-2]!=0.0 ? stk[tos-1] : stk[tos];
                   tos-=2;
                   break;
                default:
                   throw new cException(eBADEXP,className(),name());
                   return 0.0;
             }
             break;
           default:
             throw new cException(eBADEXP,className(),name());
             return 0.0;
       }
    }
    if(tos==0)
        return stk[tos];
    else
        throw new cException(eBADEXP,className(),name());
}

double cPar::fromstat()
{
    if (typechar=='T')
        return  dtr.res->random();
    else
        throw new cException(eBADCAST,className(),name(),typechar,'T');
}


cPar& cPar::operator=(const cPar& val)
{
    //
    // NOTE (duplicate of Doxygen comment in cpar.h):
    //
    // This function copies the 'val' object (whether it is of type 'I' or not)
    // into this object, *or*, if this is redirected, into the object this object
    // refers to.
    //

    if (this==&val) return *this;

    if (isRedirected())
        return ind.par->operator=(val);

    beforeChange();
    deleteold();

    cObject::operator=( val );
    typechar = val.typechar;
    changedflag = val.changedflag;
    inputflag = val.inputflag;
    promptstr = val.promptstr;

    // this line is supposed to copy the whole data area.
    memcpy( &ss, &val.ss, Max(sizeof(ss), sizeof(func)) );

    if (typechar=='S' && !ss.sht)
    {
         // make our copy of the string
         ls.str = opp_strdup(ls.str);
    }
    else if (typechar=='C')
    {
         // make our copy of expression
         cexpr.expr = (cDoubleExpression *) cexpr.expr->dup();
    }
    else if (typechar=='X')
    {
         // play with ownership stuff
         memcpy( expr.xelem = new sXElem[expr.n], val.expr.xelem,
                 expr.n*sizeof(sXElem) );
         for(int i=0; i<expr.n; i++)
         {
             if (expr.xelem[i].type=='R')
             {
                // create a copy for ourselves
                cPar *&p = expr.xelem[i].p;
                take( p=(cPar *)p->dup() );
             }
         }
    }
    else if (typechar=='T')
    {
         cStatistic *&p = dtr.res;
         if (p->owner()==const_cast<cPar*>(&val))
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
         cObject *&p = obj.obj;
         if (p->owner()==const_cast<cPar*>(&val))
            take( p=p->dup() );
    }
    // type 'I' does not use ownership so we can skip it.

    afterChange();
    return *this;
}


int cPar::cmpbyvalue(cObject *one, cObject *other)
{
    double x = (double)(*(cPar*)one)-(double)(*(cPar*)other);
    return sgn(x);
}

void cPar::convertToConst ()
{
    if (isRedirected())
       cancelRedirection();

    if (strchr("FXTI", typechar)) // if type is any or F,X,T,I,...
    {
       double d = doubleValue();
       setDoubleValue(d);
    }
}

//=========================================================================
//=== cModulePar

void cModulePar::_construct()
{
    omodp=NULL;
}

cModulePar::cModulePar(const cPar& other) : cPar(other)
{
    _construct();
}

cModulePar::cModulePar(const char *name) : cPar(name)
{
    _construct();
}

cModulePar::cModulePar(const char *name, cPar& other) : cPar(name, other)
{
    _construct();
}

cModulePar::~cModulePar()
{
    // beforeChange() is also called in cPar's destructor, but it cannot call
    // OUR virtual function any more. So we have to do it here.
    beforeChange();
}

// other member functions

const char *cModulePar::fullPath() const
{
    return fullPath(fullpathbuf,FULLPATHBUF_SIZE);
}

const char *cModulePar::fullPath(char *buffer, int bufsize) const
{
    // check we got a decent buffer
    if (!buffer || bufsize<4)
    {
        if (buffer) buffer[0]='\0';
        return "(fullPath(): no or too small buffer)";
    }

    // follows module hierarchy instead of ownership hierarchy
    // append parent path + "."
    char *buf = buffer;
    if (omodp!=NULL)
    {
       omodp->fullPath(buf,bufsize);
       int len = strlen(buf);
       buf+=len;
       bufsize-=len;
       *buf++ = '.';
       bufsize--;
    }

    // append our own name
    opp_strprettytrunc(buf, fullName(), bufsize-1);
    return buffer;
}

cModulePar& cModulePar::operator=(const cModulePar& otherpar)
{
    if (this==&otherpar) return *this;

    cPar::operator=(otherpar);
    return *this;
}

