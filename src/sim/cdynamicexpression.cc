//==========================================================================
//   CDYNAMICEXPRESSION.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cdynamicexpression.h"
#include "cxmlelement.h"
#include "cfunction.h"
#include "cnedfunction.h"
#include "cexception.h"
#include "expryydefs.h"
#include "cpar.h"


void cDynamicExpression::Elem::operator=(const Elem& other)
{
    if (type=='S') delete [] s;
    memcpy(this, &other, sizeof(Elem));
    if (type=='S') s = opp_strdup(s);
}

void cDynamicExpression::StkValue::operator=(const cPar& par)
{
    switch (par.type()) {
        case 'B': *this = par.boolValue(); break;
        case 'D': *this = par.doubleValue(); break;
        case 'L': *this = par.doubleValue(); break;
        case 'S': *this = par.stringValue(); break;
        case 'X': *this = par.xmlValue(); break;
        default: throw new cRuntimeError("internal error: bad cPar type: %s", par.fullPath().c_str());
    }
}


cDynamicExpression::cDynamicExpression()
{
    elems = NULL;
    nelems = 0;
}

cDynamicExpression::~cDynamicExpression()
{
    delete [] elems;
}

cDynamicExpression& cDynamicExpression::operator=(const cDynamicExpression& other)
{
    if (this==&other) return *this;

    delete [] elems;

    nelems = other.nelems;
    elems = new Elem[nelems];
    for (int i=0; i<nelems; i++)
        elems[i] = other.elems[i];
    return *this;
}

std::string cDynamicExpression::info() const
{
    return toString();
}

void cDynamicExpression::setExpression(Elem e[], int n)
{
    delete [] elems;
    elems = e;
    nelems = n;
}

#define ulong(x) ((unsigned long)(x))

cDynamicExpression::StkValue cDynamicExpression::evaluate(cComponent *context) const
{
    const int stksize = 20;
    StkValue stk[stksize];

    int tos = -1;
    for (int i = 0; i < nelems; i++)
    {
       Elem& e = elems[i];
       switch (toupper(e.type))
       {
           case 'B':
             if (tos>=stksize-1)
                 throw new cRuntimeError(this,eBADEXP);
             stk[++tos] = e.b;
             break;
           case 'D':
             if (tos>=stksize-1)
                 throw new cRuntimeError(this,eBADEXP);
             stk[++tos] = e.d;
             break;
           case 'P':
             if (tos>=stksize-1)
                 throw new cRuntimeError(this,eBADEXP);
             switch (e.p->type())
             {
                 case 'B': stk[++tos] = e.p->boolValue(); break;
                 case 'L': stk[++tos] = (double)e.p->longValue(); break;
                 case 'D': stk[++tos] = e.p->doubleValue(); break;
                 case 'S': stk[++tos] = e.p->stringValue(); break;
                 case 'X': stk[++tos] = e.p->xmlValue(); break;
                 default: throw new cRuntimeError(this,eBADEXP);
             }
             break;
           case 'A':
             {
             int numargs = e.af->numArgs();
             int argpos = tos-numargs+1; // stk[] index of 1st arg to pass
             if (argpos<0)
                 throw new cRuntimeError(this,eBADEXP);
             const char *argtypes = e.af->argTypes();
             for (int i=0; i<numargs; i++)
                 if (stk[argpos+i].type != (argtypes[i]=='L' ? 'D' : argtypes[i]))
                     throw new cRuntimeError(this,eBADEXP);
             stk[argpos] = e.af->functionPointer()(context, stk+argpos, numargs);
             tos = argpos;
             break;
             }
           case 'F':
             switch (e.f->numArgs())
             {
               case 0:
                   stk[++tos] = e.f->mathFuncNoArg()();
                   break;
               case 1:
                   if (tos<0)
                       throw new cRuntimeError(this,eBADEXP);
                   if (stk[tos].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos] = e.f->mathFunc1Arg()(stk[tos].dbl);
                   break;
               case 2:
                   if (tos<1)
                       throw new cRuntimeError(this,eBADEXP);
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = e.f->mathFunc2Args()(stk[tos-1].dbl, stk[tos].dbl);
                   tos--;
                   break;
               case 3:
                   if (tos<2)
                       throw new cRuntimeError(this,eBADEXP);
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL || stk[tos-2].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-2] = e.f->mathFunc3Args()(stk[tos-2].dbl, stk[tos-1].dbl, stk[tos].dbl);
                   tos-=2;
                   break;
               case 4:
                   if (tos<3)
                       throw new cRuntimeError(this,eBADEXP);
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL || stk[tos-2].type!=StkValue::DBL || stk[tos-3].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-3] = e.f->mathFunc4Args()(stk[tos-3].dbl, stk[tos-2].dbl, stk[tos-1].dbl, stk[tos].dbl);
                   tos-=3;
                   break;
               default:
                   throw new cRuntimeError(this,eBADEXP);
             }
             break;
           case '@':
             if (tos<1)
                 throw new cRuntimeError(this,eBADEXP);
             switch(e.op)
             {
               case '+':
                   // double addition or string concatenation
                   if (stk[tos-1].type==StkValue::DBL && stk[tos].type==StkValue::DBL)
                       stk[tos-1] = stk[tos-1].dbl + stk[tos].dbl;
                   else if (stk[tos-1].type==StkValue::STR && stk[tos].type==StkValue::STR)
                       stk[tos-1] = stk[tos-1].str + stk[tos].str;
                   else
                       throw new cRuntimeError(this,eBADEXP);
                   tos--;
                   break;
               case '-':
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = stk[tos-1].dbl - stk[tos].dbl;
                   tos--;
                   break;
               case '*':
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = stk[tos-1].dbl * stk[tos].dbl;
                   tos--;
                   break;
               case '/':
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = stk[tos-1].dbl / stk[tos].dbl;
                   tos--;
                   break;
               case '%':
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = (double)(ulong(stk[tos-1].dbl) % ulong(stk[tos].dbl));
                   tos--;
                   break;
               case '^':
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = pow(stk[tos-1].dbl, stk[tos].dbl);
                   tos--;
                   break;
               case 'A':
                   if (stk[tos].type!=StkValue::BOOL || stk[tos-1].type!=StkValue::BOOL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = stk[tos-1].bl && stk[tos].bl;
                   tos--;
                   break;
               case 'O':
                   if (stk[tos].type!=StkValue::BOOL || stk[tos-1].type!=StkValue::BOOL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = stk[tos-1].bl || stk[tos].bl;
                   tos--;
                   break;
               case 'X':
                   if (stk[tos].type!=StkValue::BOOL || stk[tos-1].type!=StkValue::BOOL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = stk[tos-1].bl != stk[tos].bl;
                   tos--;
                   break;
               case 'N':
                   if (stk[tos].type!=StkValue::BOOL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos] = !stk[tos].bl;
                   break;
               case '&':
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = (double)(ulong(stk[tos-1].dbl) & ulong(stk[tos].dbl));
                   tos--;
                   break;
               case '|':
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = (double)(ulong(stk[tos-1].dbl) | ulong(stk[tos].dbl));
                   tos--;
                   break;
               case '#':
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = (double)(ulong(stk[tos-1].dbl) ^ ulong(stk[tos].dbl));
                   tos--;
                   break;
               case '~':
                   if (stk[tos].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = (double)~ulong(stk[tos-1].dbl);
                   tos--;
                   break;
               case 'M':
                   if (stk[tos].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos] = -stk[tos].dbl;
                   break;
               case 'L':
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = (double)(ulong(stk[tos-1].dbl) << ulong(stk[tos].dbl));
                   tos--;
                   break;
               case 'R':
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eBADEXP);
                   stk[tos-1] = (double)(ulong(stk[tos-1].dbl) >> ulong(stk[tos].dbl));
                   tos--;
                   break;
                   break;

#define COMPARISON(RELATION) \
                             if (stk[tos-1].type==StkValue::DBL && stk[tos].type==StkValue::DBL) \
                                 stk[tos-1] = (stk[tos-1].dbl RELATION stk[tos].dbl); \
                             else if (stk[tos-1].type==StkValue::STR && stk[tos].type==StkValue::STR) \
                                 stk[tos-1] = (stk[tos-1].str RELATION stk[tos].str); \
                             else if (stk[tos-1].type==StkValue::BOOL && stk[tos].type==StkValue::BOOL) \
                                 stk[tos-1] = (stk[tos-1].bl RELATION stk[tos].bl); \
                             else \
                                 throw new cRuntimeError(this,eBADEXP); \
                             tos--;
               case '=':
                   COMPARISON(==);
                   break;
               case '!':
                   COMPARISON(!=);
                   break;
               case '<':
                   COMPARISON(<);
                   break;
               case '{':
                   COMPARISON(<=);
                   break;
               case '>':
                   COMPARISON(>);
                   break;
               case '}':
                   COMPARISON(>=);
                   break;
#undef COMPARISON
               case '?':
                   // 1st arg must be bool, others 2nd and 3rd can be anything
                   if (stk[tos-2].type!=StkValue::BOOL) throw new cRuntimeError(this,eBADEXP);
                   stk[tos-2] = (stk[tos-2].bl ? stk[tos-1] : stk[tos]);
                   tos-=2;
                   break;
               default:
                   throw new cRuntimeError(this,eBADEXP);
             }
             break;
           default:
             throw new cRuntimeError(this,eBADEXP);
       }
    }
    if (tos!=0)
        throw new cRuntimeError(this,eBADEXP);
    return stk[tos];
}

bool cDynamicExpression::boolValue(cComponent *context)
{
    StkValue v = evaluate(context);
    if (v.type!=StkValue::BOOL)
        throw new cRuntimeError(this, eBADEXP);
    return v.bl;
}

long cDynamicExpression::longValue(cComponent *context)
{
    StkValue v = evaluate(context);
    if (v.type!=StkValue::DBL)
        throw new cRuntimeError(this, eBADEXP);
    return double_to_long(v.dbl);
}

double cDynamicExpression::doubleValue(cComponent *context)
{
    StkValue v = evaluate(context);
    if (v.type!=StkValue::DBL)
        throw new cRuntimeError(this, eBADEXP);
    return v.dbl;
}

std::string cDynamicExpression::stringValue(cComponent *context)
{
    StkValue v = evaluate(context);
    if (v.type!=StkValue::STR)
        throw new cRuntimeError(this, eBADEXP);
    return v.str;
}

cXMLElement *cDynamicExpression::xmlValue(cComponent *context)
{
    StkValue v = evaluate(context);
    if (v.type!=StkValue::XML)
        throw new cRuntimeError(this, eBADEXP);
    return v.xml;
}

cDynamicExpression& cDynamicExpression::read()
{
    // TODO
    return *this;
}

std::string cDynamicExpression::toString() const
{
    // We perform the same algorithm as during evaluation (i.e. stack machine),
    // only instead of actual calculations we store the result as string.
    // We need to keep track of operator precendences to be able to add parens where needed.

    try
    {
        const int stksize = 20;
        std::string strstk[stksize];
        int pristk[stksize];

        int tos = -1;
        for (int i = 0; i < nelems; i++)
        {
           Elem& e = elems[i];
           switch (e.type)
           {
               case 'B':
                 if (tos>=stksize-1)
                     throw new cRuntimeError(this,eBADEXP);
                 strstk[++tos] = (e.b ? "true" : "false");
                 pristk[tos] = 0;
                 break;
               case 'D':
                 {
                 if (tos>=stksize-1)
                     throw new cRuntimeError(this,eBADEXP);
                 char buf[32];
                 sprintf(buf, "%g", e.d);
                 strstk[++tos] = buf;
                 pristk[tos] = 0;
                 }
                 break;
               case 'P':
                 if (!e.p || tos>=stksize-1)
                     throw new cRuntimeError(this,eBADEXP);
                 strstk[++tos] = e.p->fullPath();
                 pristk[tos] = 0;
                 break;
               case 'F':
               case 'A':
                 {
                 int numargs = (e.type=='F') ? e.f->numArgs() : e.af->numArgs();
                 std::string name = (e.type=='F') ? e.f->name() : e.af->name();
                 int argpos = tos-numargs+1; // strstk[] index of 1st arg to pass
                 if (argpos<0)
                     throw new cRuntimeError(this,eBADEXP);
                 std::string tmp = name+"(";
                 for (int i=0; i<numargs; i++)
                     tmp += (i==0 ? "" : ", ") + strstk[argpos+i];
                 tmp += ")";
                 strstk[argpos] = tmp;
                 tos = argpos;
                 pristk[tos] = 0;
                 break;
                 }
               case '@':
                 if (tos<1)
                     throw new cRuntimeError(this,eBADEXP);
                 if (e.op=='?')  // conditional (tertiary)
                 {
                     strstk[tos-2] = strstk[tos-2] + " ? " + strstk[tos-1] + " : " + strstk[tos];
                     tos-=2;
                     pristk[tos] = 8;
                 }
                 else if (e.op=='N' || e.op=='~' || e.op=='M')
                 {
                     const char *op;
                     switch (e.op)
                     {
                         case 'N': op=" !"; break;
                         case '~': op=" ~"; break;
                         case 'M': op=" -"; break;
                         default:  op=" ???";
                     }
                     strstk[tos] = std::string(op) + strstk[tos]; // pri=0: never needs parens
                     pristk[tos] = 0;
                 }
                 else
                 {
                     int pri;
                     const char *op;
                     switch(e.op)
                     {
                         //
                         // Precedences, based on expr.y:
                         //   prec=7: AND OR XOR
                         //   prec=6: EQ NE '>' GE '<' LE
                         //   prec=5: BIN_AND BIN_OR BIN_XOR
                         //   prec=4: SHIFT_LEFT SHIFT_RIGHT
                         //   prec=3: '+' '-'
                         //   prec=2: '*' '/' '%'
                         //   prec=1: '^'
                         //   prec=0: UMIN NOT BIN_COMPL
                         //
                         case '+': op=" + "; pri=3; break;
                         case '-': op=" - "; pri=3; break;
                         case '*': op=" * "; pri=2; break;
                         case '/': op=" / "; pri=2; break;
                         case '%': op=" % "; pri=2; break;
                         case '^': op=" ^ "; pri=1; break;
                         case '=': op=" = "; pri=6; break;
                         case '<': op=" < "; pri=6; break;
                         case '>': op=" > "; pri=6; break;
                         case '!': op=" != "; pri=6; break;
                         case '{': op=" <= "; pri=6; break;
                         case '}': op=" >= "; pri=6; break;
                         case 'A': op=" && "; pri=7; break;
                         case 'O': op=" || "; pri=7; break;
                         case 'X': op=" ## "; pri=7; break;
                         case '&': op=" & "; pri=5; break;
                         case '|': op=" | "; pri=5; break;
                         case '#': op=" # "; pri=5; break;
                         case 'L': op=" << "; pri=4; break;
                         case 'R': op=" >> "; pri=4; break;
                         default:  op=" ??? "; pri=10; break;
                     }

                     // add parens, depending on the operator precedences
                     std::string tmp;
                     if (pri < pristk[tos-1])
                         tmp = std::string("(") + strstk[tos-1] + ")";
                     else
                         tmp = strstk[tos-1];
                     tmp += op;
                     if (pri < pristk[tos])
                         tmp += std::string("(") + strstk[tos] + ")";
                     else
                         tmp += strstk[tos];
                     strstk[tos-1] = tmp;
                     tos--;
                     pristk[tos] = pri;
                     break;
                 }
                 break;
               default:
                 throw new cRuntimeError(this,eBADEXP);
           }
        }
        if (tos!=0)
            throw new cRuntimeError(this,eBADEXP);
        return strstk[tos];
    }
    catch (cException *e)
    {
        std::string ret = std::string("[[ ") + e->message() + " ]]";
        delete e;
        return ret;
    }
}

bool cDynamicExpression::parse(const char *text)
{
    // try parse it
    Elem *tmpelems;
    int tmpnelems;
    ::doParseExpression(text, tmpelems, tmpnelems);
    if (!tmpelems)
        return false;

    // OK, store it
    delete [] elems;
    elems = tmpelems;
    nelems = tmpnelems;
    return true;
}


