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
  Copyright (C) 1992,99 Andras Varga
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
#include <iostream.h>
#include "cpar.h"
#include "cstat.h"
#include "macros.h"
#include "errmsg.h"
#include "cmodule.h"  // for cModulePar
#include "csimul.h"   // for cModulePar
#include "cenvir.h"

//==============================================
//=== Registration
Register_Class( cPar )

//==========================================================================
//=== cPar - member functions

char *cPar::possibletypes = "SBLDFIXTP";

// constructors
cPar::cPar(char *namestr) : cObject( namestr )
{
        takeOwnership( FALSE );         // doesn't take the objects!
        changedflag = inputflag = FALSE;
        typechar = 'L'; lng.val = 0L;
}

cPar::cPar(cPar& par) : cObject()
{
        takeOwnership( FALSE );         // doesn't take the objects!
        changedflag = inputflag = FALSE;
        typechar = 'L'; lng.val = 0L;

        setName( par.name() );
        cPar::operator=(par);
}

cPar::cPar(char *namestr, cPar& other) : cObject(namestr)
{
        takeOwnership( FALSE );         // doesn't take the objects!
        changedflag = inputflag = FALSE;
        typechar = 'L'; lng.val = 0L;

        setName(namestr);
        operator=(other);
}

#define CONSTRUCT_CPAR(args)  \
                 takeOwnership( FALSE ); \
                 changedflag = inputflag = FALSE; \
                 typechar = 'L'; lng.val = 0L; \
                 cPar::setValue args ; \
                 changedflag = FALSE;

cPar::cPar(char *namestr, char tp, char *s) : cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, s ))
}

cPar::cPar(char *namestr, char tp, int l) :  cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, (long)l ))
}

cPar::cPar(char *namestr, char tp, long l) :  cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, l ))
}

cPar::cPar(char *namestr, char tp, double d) : cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, d ))
}

cPar::cPar(char *namestr, char tp, MathFuncNoArg f): cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, f ))
}

cPar::cPar(char *namestr, char tp, MathFunc1Arg f, double p1) : cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, f, p1 ))
}

cPar::cPar(char *namestr, char tp, MathFunc2Args f, double p1, double p2) : cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, f, p1,p2 ))
}

cPar::cPar(char *namestr, char tp, MathFunc3Args f, double p1, double p2, double p3) : cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, f, p1,p2,p3 ))
}

cPar::cPar(char *namestr, char tp, void *ptr) : cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, ptr ))
}

cPar::cPar(char *namestr, char tp, cObject *obj) : cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, obj ))
}

cPar::cPar(char *namestr, char tp, cPar *par) : cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, par ))
}

cPar::cPar(char *namestr, char tp, cStatistic *res) : cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, res ))
}

cPar::cPar(char *namestr, char tp, sXElem *x, int n) : cObject(namestr)
{
        CONSTRUCT_CPAR(( tp, x, n ))
}

#undef CONSTRUCT_CPAR(args)

cPar::~cPar()
{
        valueChanges();
        if (typechar=='I') cancelIndirection();
        deleteold();
}

void cPar::deleteold()
{
        if (typechar=='S' && !ls.sht)
        {
             delete ls.str;
        }
        else if (typechar=='T')
        {
             if (dtr.res->owner()==this) free( dtr.res );
        }
        else if (typechar=='P')
        {
             if (ptr.dupfunc || ptr.itemsize>0) // we're expected to do memory mgmt
             {
                 if (ptr.delfunc) ptr.delfunc(ptr.ptr); else delete ptr.ptr;
             }
        }
        else if (typechar=='O')
        {
             if (obj.obj->owner()==this) free( obj.obj );
        }
        else if (typechar=='X')
        {
             for(int i=0; i<expr.n; i++)
                  if (expr.xelem[i].type=='R')  // should be ours
                      free( expr.xelem[i].p );
             delete expr.xelem;
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

        // indirection?
        if (typechar=='I')
        {
            strcpy(b,"--> ");
            strcpy(b+4, ind.par->fullPath() );
            return;
        }

        // append useful info
        cFunctionType *ff;
        char *fn,*s;
        switch (typechar) {
           case 'S': s = ls.sht ? ss.str:ls.str;
                     if (!s) s = "";
                     if (strlen(s)<=40)
                       sprintf(b,"\"%s\" (S)", s);
                     else
                       sprintf(b,"\"%.40s...\" [truncated] (S)", s);
                     break;
           case 'L': sprintf(b,"%ld (L)",lng.val); break;
           case 'D': sprintf(b,"%lg (D)",dbl.val); break;
           case 'X': sprintf(b,"expression"); break;
           case 'T': sprintf(b,"(%s) (T)", dtr.res ? dtr.res->fullPath():"nil"); break;
           case 'P': sprintf(b,"(%p) (P)", ptr.ptr); break;
           case 'O': sprintf(b,"(%s) (O)", obj.obj ? obj.obj->fullPath():"nil"); break;
           case 'F': ff = findfunctionbyptr(func.f);
                     if (ff) fn=ff->name(); else fn="unknown";
                     switch(func.argc) {
                     case 0: sprintf(b,"%s() (F)",fn); break;
                     case 1: sprintf(b,"%s(%lg) (F)",fn,func.p1); break;
                     case 2: sprintf(b,"%s(%lg,%lg) (F)",fn,func.p1,func.p2); break;
                     case 3: sprintf(b,"%s(%lg,%lg,%lg) (F)",fn,func.p1,func.p2,func.p3); break;
                     };
                     break;
           default : strcat(b, "? (unknown type)"); break;
        }
}

void cPar::writeContents(ostream& os)
{
        char buf[256];
        if (typechar=='I')
        {
           os << "  Type:   indirection ('I')\n";
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

char cPar::type()
{
         if (typechar=='I') {return ind.par->type();}
         return typechar;
}

char *cPar::prompt()
{
         if (typechar=='I') {return ind.par->prompt();}
         return promptstr;
}

bool cPar::isInput()
{
         if (typechar=='I') {return ind.par->isInput();}
         return inputflag;
}

bool cPar::changed()
{
         if (typechar=='I') {return ind.par->changed();}
         bool ch = changedflag;
         changedflag=FALSE;
         return ch;
}

void cPar::setPrompt(char *s)
{
         if (typechar=='I') {ind.par->setPrompt(s);return;}
         valueChanges();
         promptstr = s;     // string's operator=() does delete+opp_strdup()
}

void cPar::setInput(bool ip)
{
         if (typechar=='I') {ind.par->setInput(ip);return;}
         if (inputflag!=ip)
         {
            valueChanges();
            inputflag = ip;
         }
}

//-----------------------------------------------------------------------
// setValue() funcs

cPar& cPar::setValue(char tp, char *s)
{
        if (typechar=='I') {return ind.par->setValue(tp,s);}

        valueChanges();
        tp = (char) toupper(tp);
        if (tp=='S')
        {
            deleteold();
            typechar = tp; inputflag=FALSE;
            if ((ls.sht=(strlen(s)<=SHORTSTR))!=0)
                opp_strcpy(ss.str, s);
            else
                ls.str = opp_strdup(s);
        } else
            opp_error(eBADINIT,isA(),name(),tp);
        return *this;
}

cPar& cPar::setValue(char tp, int i )
{
        return setValue(tp, (long)i );
}

cPar& cPar::setValue(char tp, long l)
{
        if (typechar=='I') {return ind.par->setValue(tp,l);}

        valueChanges();
        tp = (char) toupper(tp);

        switch(tp) {
          case 'B':
          case 'L':  deleteold();
                     lng.val = l;
                     typechar = tp;  inputflag=FALSE;
                     break;
          default:   opp_error(eBADINIT,isA(),name(),tp);
        }
        return *this;
}

cPar& cPar::setValue(char tp, double d)
{
        if (typechar=='I') {return ind.par->setValue(tp,d);}

        valueChanges();
        tp = (char) toupper(tp);
        switch(tp) {
            case 'L': deleteold();
                      lng.val = (long)d;
                      typechar = tp;  inputflag=FALSE;
                      break;
            case 'D': deleteold();
                      dbl.val = d;
                      typechar = tp; inputflag=FALSE;
                      break;
            default:  opp_error(eBADINIT,isA(),name(),tp);
        }
        return *this;
}

cPar& cPar::setValue(char tp, MathFuncNoArg f)
{
        if (typechar=='I') {return ind.par->setValue(tp,f);}

        valueChanges();
        tp = (char) toupper(tp);
        switch(tp) {
            case 'F': deleteold();
                      func.f = (MathFunc)f; func.argc=0;
                      typechar = 'F'; inputflag=FALSE;
            default:  opp_error(eBADINIT,isA(),name(),tp);
        }
        return *this;
}

cPar& cPar::setValue(char tp, MathFunc1Arg f, double p1)
{
        if (typechar=='I') {return ind.par->setValue(tp,f,p1);}

        valueChanges();
        tp = (char) toupper(tp);
        switch(tp) {
            case 'F': deleteold();
                      func.f = (MathFunc)f; func.argc=1;
                      func.p1 = p1;
                      typechar = 'F'; inputflag=FALSE; break;
            default:  opp_error(eBADINIT,isA(),name(),tp);
        }
        return *this;
}

cPar& cPar::setValue(char tp, MathFunc2Args f, double p1, double p2)
{
        if (typechar=='I') {return ind.par->setValue(tp,f,p1,p2);}

        valueChanges();
        tp = (char) toupper(tp);
        switch(tp) {
            case 'F': deleteold();
                      func.f = (MathFunc)f; func.argc=2;
                      func.p1 = p1; func.p2 = p2;
                      typechar = 'F'; inputflag=FALSE; break;
            default:  opp_error(eBADINIT,isA(),name(),tp);
        }
        return *this;
}

cPar& cPar::setValue(char tp, MathFunc3Args f, double p1, double p2, double p3)
{
        if (typechar=='I') {return ind.par->setValue(tp,f,p1,p2,p3);}

        valueChanges();
        tp = (char) toupper(tp);
        switch(tp) {
            case 'F': deleteold();
                      func.f = (MathFunc)f; func.argc=3;
                      func.p1 = p1; func.p2 = p2; func.p3 = p3;
                      typechar = 'F'; inputflag=FALSE; break;
            default:  opp_error(eBADINIT,isA(),name(),tp);
        }
        return *this;
}

cPar& cPar::setValue(char tp, sXElem *x, int n)
{
        if (typechar=='I') {return ind.par->setValue(tp,x,n);}

        valueChanges();
        tp = (char) toupper(tp);
        if (tp!='X')
            opp_error(eBADINIT,isA(),name(),tp);
        else
        {
            if (x==NULL) {opp_error(eBADINIT,isA(),name(),tp);return *this;}
            deleteold();
            expr.n = n; expr.xelem = x;
            typechar = tp; inputflag=FALSE;
            for(int i=0; i<expr.n; i++)
            {
               if (expr.xelem[i].type=='R')
               {
                   //  sXElem::op= has already dupped the pointed cPar for us
                   cPar *p = expr.xelem[i].p;

                   // if the pointed cPar is not indirect and it is a constant,
                   // there's really no need to keep the object
                   if (p->indirection()==NULL && (p->type()=='D' || p->type()=='L'))
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
        }
        return *this;
}

cPar& cPar::setValue(char tp, cPar *par)
{
        if (typechar=='I') {return ind.par->setValue(tp,par);}

        valueChanges();
        tp = (char) toupper(tp);
        if (tp=='O')
        {
              return setValue(tp,(cObject *)par);
        }
        else if (tp=='I' && par)
        {
              // check for circular references
              cPar *p = par;
              while (p)
              {
                 if (p==this) {opp_error(eCIRCREF,isA(),name());return *this;}
                 p = (p->typechar=='I') ? p->ind.par : NO(cPar);
              }

              // set indirection
              deleteold();
              ind.par = par;
              // don't take ownership of passed cPar object
              typechar = tp; inputflag=FALSE;
        } else
              opp_error(eBADINIT,isA(),name(),tp);
        return *this;
}

cPar& cPar::setValue(char tp, cStatistic *res)
{
        if (typechar=='I') {return ind.par->setValue(tp,res);}

        valueChanges();
        tp = (char) toupper(tp);
        if (tp=='T' && res) {
              deleteold();
              dtr.res = res;
              if (takeOwnership()) take( res );
              typechar = tp; inputflag=FALSE;
        } else
              opp_error(eBADINIT,isA(),name(),tp);
        return *this;
}

cPar& cPar::setValue(char tp, void *_ptr)
{
        if (typechar=='I') {return ind.par->setValue(tp,_ptr);}

        valueChanges();
        tp = (char) toupper(tp);
        if (tp!='P')
              opp_error(eBADINIT,isA(),name(),tp);
        else
        {
              // if it was a 'P' before, keep previous configuration
              if (typechar!='P')
              {
                   deleteold();
                   ptr.delfunc=NULL;
                   ptr.dupfunc=NULL;
                   ptr.itemsize=0;
                   typechar = tp;
              }
              ptr.ptr = _ptr;
              inputflag=FALSE;
        }
        return *this;
}

cPar& cPar::setValue(char tp, cObject *_obj)
{
        if (typechar=='I') {return ind.par->setValue(tp,_obj);}

        valueChanges();
        tp = (char) toupper(tp);
        if (tp!='O')
              opp_error(eBADINIT,isA(),name(),tp);
        else
        {
              deleteold();
              obj.obj = _obj;
              if (takeOwnership()) take( _obj );
              typechar = tp; inputflag=FALSE;
        }
        return *this;
}

void cPar::configPointer( VoidDelFunc delfunc, VoidDupFunc dupfunc,
                          size_t itemsize)
{
        if (typechar!='P')
              opp_error("(%s)%s: configPointer(): type is '%c';"
                               " should be 'P'",isA(),name(),typechar);
        else
        {
             ptr.delfunc = delfunc;
             ptr.dupfunc = dupfunc;
             ptr.itemsize = itemsize;
        }
}

//------------------------------------------------------------------------
// functions returning the stored value

char *cPar::stringValue()
{
        if (typechar=='I') {return ind.par->stringValue();}

        if (isInput()) read();
        if (typechar=='S')
             {if (ss.sht) return ss.str; else return ls.str;}
        else
             {opp_error(eBADCAST,isA(),name(),typechar,'S');return NULL;}
}

long cPar::longValue()
{
        if (typechar=='I') {return ind.par->longValue();}

        if (isInput()) read();
        if (typechar=='L' || typechar=='B')
             return lng.val;
        else
             return (long)doubleValue();
}

bool cPar::boolValue()
{
        if (typechar=='I') {return ind.par->boolValue();}

        if (isInput()) read();
        if (typechar=='B')
             return lng.val;
        else
             {opp_error(eBADCAST,isA(),name(),typechar,'B');return 0.0;}
}

double cPar::doubleValue()
{
        if (typechar=='I') {return ind.par->doubleValue();}

        if (isInput()) read();
        if (typechar=='L')
            return (double)lng.val;
        else if (typechar=='D')
            return dbl.val;
        else if (typechar=='T')
             return fromstat();
        else if (typechar=='X')
             return evaluate();
        else if (typechar=='F')
             return func.argc==0 ? ((MathFuncNoArg)func.f)() :
                    func.argc==1 ? ((MathFunc1Arg) func.f)(func.p1) :
                    func.argc==2 ? ((MathFunc2Args)func.f)(func.p1,func.p2) :
                                   ((MathFunc3Args)func.f)(func.p1,func.p2,func.p3);
        else
             {opp_error(eBADCAST,isA(),name(),typechar,'D');return 0.0;}
}

void *cPar::pointerValue()
{
        if (typechar=='I') {return ind.par->pointerValue();}

        if (isInput()) read();
        if (typechar=='P')
            return ptr.ptr;
        else
            {opp_error(eBADCAST,isA(),name(),typechar,'P'); return NULL;}
}

cObject *cPar::objectValue()
{
        if (typechar=='I') {return ind.par->objectValue();}

        if (isInput()) read();
        if (typechar=='O')
            return obj.obj;
        else
            {opp_error(eBADCAST,isA(),name(),typechar,'O'); return NULL;}
}

bool cPar::equalsTo(cPar *par)
{
    if (typechar != par->typechar) return FALSE;

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
       case 'X': opp_error("(%s): equalsTo() with X type not implemented",isA());
       default: return 0;
    }
}

//------------------------------------------------------------------------
// misc funcs

void cPar::valueChanges()
{
        changedflag=TRUE;
}

cPar *cPar::indirection()
{
        if (typechar=='I')
              return ind.par;
        else
              return NULL;
}

void cPar::cancelIndirection()
{
        if (typechar=='I')
        {
              // operator=( *ind.par ); -- the user may do this by hand
              typechar = 'L'; lng.val = 0L;
        }
}

void cPar::getAsText(char *buf, int maxlen)
{
        if (typechar=='I') {
            ind.par->getAsText(buf, maxlen );
            return;
        }

        buf[0] = 0;
        if( maxlen<=1 ) return;

        char bb[128];
        bb[0] = 0;
        cFunctionType *ff;
        char *fn,*s;
        switch (typechar) {
           case 'S': s = ls.sht ? ss.str:ls.str;
                     if (!s) s = "";
                     if (strlen(s)<=125)
                       sprintf(bb,"\"%s\"", s);
                     else
                       sprintf(bb,"\"%.110s...\" [truncated]", s);
                     break;
           case 'B': strcpy(bb,lng.val?"TRUE":"FALSE"); break;
           case 'L': sprintf( bb,"%ld",lng.val); break;
           case 'D': sprintf( bb,"%lg",dbl.val); break;
           case 'F': ff = findfunctionbyptr(func.f);
                     if (ff) fn=ff->name(); else fn="unknown";
                     switch(func.argc) {
                     case 0: sprintf(bb,"%s()",fn); break;
                     case 1: sprintf(bb,"%s(%lg)",fn,func.p1); break;
                     case 2: sprintf(bb,"%s(%lg,%lg)",fn,func.p1,func.p2); break;
                     case 3: sprintf(bb,"%s(%lg,%lg,%lg)",fn,func.p1,func.p2,func.p3); break;
                     };
                     break;
           case 'T': sprintf(bb,"distribution %.99s", dtr.res ? dtr.res->fullPath():"nil"); break;
           case 'P': sprintf(bb,"pointer %p", ptr.ptr); break;
           case 'O': sprintf(bb,"object %.99s", obj.obj ? obj.obj->fullPath():"nil"); break;
           case 'X': sprintf(bb,"expression"); break;
           default : bb[0]='?'; bb[1]=0; break;
        }
        strncpy( buf, bb, maxlen<128 ? maxlen : 128);
}

   /*-------------------------------
     setFromText() - (for internal use only) Tries to interpret text as
     a 'tp' typed value. tp=='?' means that even type is to be determined.
     On success, cPar is updated and TRUE is returned, otherwise
     it returns FALSE. No error message is ever generated.
   ----------------------------*/

bool cPar::setFromText( char *text, char tp)
{
       tp = (char) toupper(tp);

       // create a working copy and cut whitespaces (from both sides)
       if (!text) return FALSE;  // error: no string
       while (*text==' ' || *text=='\t') text++;
       if (*text=='\0') return FALSE; // error: empty string (or only whitespace)
       char *tmp = opp_strdup(text);
       char *s = tmp+strlen(tmp)-1;
       while (s>=tmp && (*s==' ' || *s=='\t')) *s--='\0';

       double d;

       if (strcmp(tmp,"TRUE")==0 || strcmp(tmp,"true")==0 || strcmp(tmp,"True")==0) // bool?
       {
           if (!strchr("?B",tp)) goto error;
           setValue('B',(int)TRUE);
       }
       else if (strcmp(tmp,"FALSE")==0 || strcmp(tmp,"false")==0 || strcmp(tmp,"False")==0) // bool?
       {
           if (!strchr("?B",tp)) goto error;
           setValue('B',(int)FALSE);
       }
       else if (strcmp(tmp,"1")==0 && tp=='B') // bool?
       {
           setValue('B',(int)TRUE);
       }
       else if (strcmp(tmp,"0")==0 && tp=='B') // bool?
       {
           setValue('B',(int)FALSE);
       }
       else if (tmp[0]=='\'' && tmp[1] && tmp[2]=='\''&& !tmp[3]) // char? (->long)
       {
           if (!strchr("?L",tp)) goto error;
           setValue('L', (long)tmp[1]);
       }
       else if (text[0]=='\"') // string?
       {
           if (!strchr("?S",tp)) goto error;

           // check closing quote
           if (!tmp[1] || tmp[strlen(tmp)-1]!='\"') goto error;

           tmp[strlen(tmp)-1] = '\0'; // cut off closing quote
           setValue('S', tmp+1);
       }
       else if (strspn(tmp,"+-0123456789")==strlen(tmp)) // long?
       {
           long num;
           int len;
           if (0==sscanf(tmp,"%ld%n",&num,&len)) goto error;
           if (len<strlen(tmp) || !strchr("?LD",tp)) goto error;
           setValue((tp=='?'?'L':tp), num);
       }
       else if (strspn(tmp,"+-.eE0123456789")==strlen(tmp)) // double?
       {
           double num;
           int len;
           if (0==sscanf(tmp,"%lf%n",&num,&len)) goto error;
           if (len<strlen(tmp) || !strchr("?D",tp)) goto error;
           setValue('D', num);
       }
       else if ((d=strToSimtime(tmp))>=0.0) // double given as time?
       {
           if (!strchr("?D",tp)) goto error;
           setValue('D',d);
       }
       else // maybe function; try to parse it
       {
           if (!strchr("?F",tp)) goto error;
           if (!setfunction(tmp)) goto error;
       }

       delete tmp;
       return TRUE;

       error:
       delete tmp;
       return FALSE;
}

bool cPar::setfunction(char *text)
{
       char buf[32];
       char *s,*d;

       // remove spaces, tabs etc.
       for(s=d=text; *s; s++)
          if (!isspace(*s))
             *d++ = *s;
       *d = 0;

       // extract first word (expected to be the function name) into buf
       for( s=text,d=buf; s-text<31 && (isalnum(*s) || *s=='_'); *d++ = *s++);
       *d=0;

       // find function
       cFunctionType *ff = findFunction( buf );
       if (ff==NULL) return FALSE;

       // now `s' points to something like '(10,1.5E-3)'
       double p1,p2,p3;
       switch(ff->argcount)
       {
          case 0: if (strcmp(s,"()")!=0) return FALSE;
                  setValue('F',(MathFuncNoArg)ff->f);
                  return TRUE;
          case 1: if (*s++!='(') return FALSE;
                  p1 = strToSimtime0(s);
                  if (*s++!=')') return FALSE;
                  setValue('F',(MathFunc1Arg)ff->f, p1);
                  return TRUE;
          case 2: if (*s++!='(') return FALSE;
                  p1 = strToSimtime0(s);
                  if (*s++!=',') return FALSE;
                  p2 = strToSimtime0(s);
                  if (*s++!=')') return FALSE;
                  setValue('F',(MathFunc2Args)ff->f, p1,p2);
                  return TRUE;
          case 3: if (*s++!='(') return FALSE;
                  p1 = strToSimtime0(s);
                  if (*s++!=',') return FALSE;
                  p2 = strToSimtime0(s);
                  if (*s++!=',') return FALSE;
                  p3 = strToSimtime0(s);
                  if (*s++!=')') return FALSE;
                  setValue('F',(MathFunc3Args)ff->f, p1,p2,p3);
                  return TRUE;
       }
       return FALSE; // to make compiler happy
}


cPar& cPar::read()
{
        char buf[256];

        // get it from ini file
        char *s = ev.getParameter(simulation.runNumber(), fullPath());
        if (s!=NULL)
        {
           strcpy(buf,s);
           bool success = setFromText(buf,'?');
           if (!success)
                 opp_error( "Wrong value `%s' for parameter `%s'",
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
                // opp_error(eCANCEL); -- already issued in cEnvir::gets()
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
                 if(tos>=stksize - 1) {opp_error(eBADEXP,isA(),name());return 0.0;}
                 stk[++tos] = e.d;
                 break;
               case 'P':
               case 'R':
                 if(!e.p || tos>=stksize - 1) {opp_error(eBADEXP,isA(),name());return 0.0;}
                 stk[++tos] = (double)(*e.p);
                 break;
               case '0':
                 if(!e.f0) {opp_error(eBADEXP,isA(),name());return 0.0;}
                 stk[++tos] = e.f0();
                 break;
               case '1':
                 if(!e.f1 || tos<0) {opp_error(eBADEXP,isA(),name());return 0.0;}
                 stk[tos] = e.f1( stk[tos] );
                 break;
               case '2':
                 if(!e.f2 || tos<1) {opp_error(eBADEXP,isA(),name());return 0.0;}
                 stk[tos-1] = e.f2( stk[tos-1], stk[tos] );
                 tos--;
                 break;
               case '3':
                 if(!e.f3 || tos<2) {opp_error(eBADEXP,isA(),name());return 0.0;}
                 stk[tos-2] = e.f3( stk[tos-2], stk[tos-1], stk[tos] );
                 tos-=2;
                 break;
               case '@':
                 if(!e.f2 || tos<1) {opp_error(eBADEXP,isA(),name());return 0.0;}
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
                     opp_error(eBADEXP,isA(),name());
                       return 0.0;
                 }
                 break;
               default:
                 opp_error(eBADEXP,isA(),name());
                 return 0.0;
           }
        }
        if(tos==0)
            return stk[tos];
        else
            {opp_error(eBADEXP,isA(),name());return 0.0;}
}

double cPar::fromstat()
{
        if (typechar=='T')
            return  dtr.res->random();
        else
            {opp_error(eBADCAST,isA(),name(),typechar,'T');return 0.0;}
}


cPar& cPar::operator=(cPar& val)
{
        // This function is sort of tricky:
        //   It copies the 'val' object (whether it is of type 'I' OR NOT)
        //   into this object or the object this object refers to.
        // If this cPar is of type 'I', and you want to overwrite
        // this very object and not what it points to,
        // use cancelIndirection() first!

        if (typechar=='I') {return ind.par->operator=(val);}

        valueChanges();
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
             if (p->owner()==&val)
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
             if (p->owner()==&val)
                take( p=p->dup() );
        }
        // type 'I' does not use ownership so we can skip it.

        return *this;
}


int cPar::cmpbyvalue(cObject *one, cObject *other)
{
        double x = (double)(*(cPar*)one)-(double)(*(cPar*)other);
        return sgn(x);
}

void cPar::convertToConst ()
{
        if (typechar=='I') cancelIndirection();

        if (strchr("FXTI", typechar)) // if type is any or F,X,T,I,...
        {
                double d = doubleValue ();
                setValue ('D', d);
        }
}

//=========================================================================
//=== cModulePar - constructors
// strange style of defining constructors, but simply there is too many of them

void cModulePar::_construct()
{
     omodp=NULL;
     log_initialised=FALSE;
     log_ID=0;
     lastchange=simulation.simTime();
}

cModulePar::cModulePar(cPar& other) : cPar(other) {_construct();}
cModulePar::cModulePar(char *namestr) : cPar(namestr) {_construct();}
cModulePar::cModulePar(char *namestr, cPar& other) : cPar(namestr, other) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, char *s) : cPar(namestr, tp, s) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, int i) : cPar(namestr, tp, i) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, long l) : cPar(namestr, tp, l) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, double d) : cPar(namestr, tp, d) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, MathFuncNoArg f) : cPar(namestr, tp, f) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, MathFunc1Arg  f, double p1) : cPar(namestr, tp, f, p1) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, MathFunc2Args f, double p1, double p2) : cPar(namestr, tp, f, p1, p2) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, MathFunc3Args f, double p1, double p2, double p3) : cPar(namestr, tp, f, p1, p2, p3) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, void *ptr) :  cPar(namestr, tp, ptr) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, cObject *obj) :  cPar(namestr, tp, obj) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, cPar *par) :  cPar(namestr, tp, par) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, cStatistic *res) : cPar(namestr, tp, res) {_construct();}
cModulePar::cModulePar(char *namestr, char tp, sXElem *x, int n) : cPar(namestr, tp, x, n) {_construct();}

cModulePar::~cModulePar()
{
   // valueChanges() is also called in cPar's destructor, but it cannot call
   // OUR virtual function any more. So we have to do it here.
   valueChanges();
}

// other member functions

char *cModulePar::fullPath()
{
   // use cObject::fullPath()'s static buffer
   // hide param vector: skip directly to owner module
   char *buf = omodp->fullPath();
   strcat(buf,".");
   strcat(buf,fullName());
   return buf;
}

cModulePar& cModulePar::operator=(cModulePar& otherpar)
{
   cPar::operator=(otherpar);
   return *this;
}

#define CHECK(fprintf)    if (fprintf<0) opp_error(ePARCHF)
void cModulePar::valueChanges()
{
   // this function is called BEFORE each value change

   cPar::valueChanges();

   if (simulation.logparchanges)
   {
      FILE *f = simulation.parchangefilemgr.filePointer();
      if (f!=NULL)
      {
         if ( !log_initialised )
         {
            log_ID = simulation.parchangefilemgr.getNewID();
            CHECK(fprintf(f,"parameter %ld  \"%s\"\n", log_ID, fullPath()));
            log_initialised=TRUE;
         }

         // module parameter logging:
         //   since this function is called BEFORE each change,
         //   we will write out what the value was SINCE the last change.
         //   The value to which the object is now changing will be written
         //   right before the NEXT value change. Last value change is written
         //   by the destructor.
         char value[128];
         getAsText(value,127);
         CHECK(fprintf(f,"%ld\t%lg\t%s\n",log_ID, lastchange, value));
      }
      lastchange = simulation.simTime();
   }
}
#undef CHECK

