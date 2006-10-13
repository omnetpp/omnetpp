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
#include "cenvir.h"
#include "cmodule.h"

//FIXME add handling of functors! they must be deleted in cDynamicExpression dtor!
//FIXME functors must support dup()!


void cDynamicExpression::Elem::operator=(const Elem& other)
{
    if (type==STR)
        delete [] s;
    memcpy(this, &other, sizeof(Elem));
    if (type==STR)
        s = opp_strdup(s);
    else if (type==FUNCTOR)
        fu = (Functor *) fu->dup();
}

cDynamicExpression::Elem::~Elem()
{
    if (type==STR)
        delete [] s;
    else if (type==FUNCTOR)
        delete fu;
}

void cDynamicExpression::StkValue::operator=(const cPar& par)
{
    switch (par.type())
    {
      case 'B': *this = par.boolValue(); break;
      case 'D': *this = par.doubleValue(); break;
      case 'L': *this = par.doubleValue(); break;
      case 'S': *this = par.stringValue(); break;
      case 'X': *this = par.xmlValue(); break;
      default: throw new cRuntimeError("internal error: bad cPar type: %s", par.fullPath().c_str());
    }
}

std::string cDynamicExpression::StkValue::toString()
{
    char buf[32];
    switch (type)
    {
      case BOOL: return bl ? "true" : "false";
      case DBL:  sprintf(buf, "%g", dbl); return buf;
      case STR:  return std::string("\"")+str+"\"";
      case XML:  return std::string("<")+xml->getTagName()+">"; //XXX
      default:   throw new cRuntimeError("internal error: bad StkValue type");
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
printf("    evaluating: %s\n", toString().c_str()); //XXX
    const int stksize = 20;
    StkValue stk[stksize];

    int tos = -1;
    for (int i = 0; i < nelems; i++)
    {
       Elem& e = elems[i];
       switch (e.type)
       {
           case Elem::BOOL:
             if (tos>=stksize-1)
                 throw new cRuntimeError(this,eESTKOFLOW);
             stk[++tos] = e.b;
             break;

           case Elem::DBL:
             if (tos>=stksize-1)
                 throw new cRuntimeError(this,eESTKOFLOW);
             stk[++tos] = e.d;
             break;

           case Elem::STR:
             if (tos>=stksize-1)
                 throw new cRuntimeError(this,eESTKOFLOW);
             stk[++tos] = e.s;
             break;

           case Elem::XML:
             if (tos>=stksize-1)
                 throw new cRuntimeError(this,eESTKOFLOW);
             stk[++tos] = e.x;
             break;

           case Elem::CPAR:
             if (tos>=stksize-1)
                 throw new cRuntimeError(this,eESTKOFLOW);
             stk[++tos] = *(e.p); break;
             break;

           case Elem::MATHFUNC:
             switch (e.f->numArgs())
             {
               case 0:
                   stk[++tos] = e.f->mathFuncNoArg()();
                   break;
               case 1:
                   if (tos<0)
                       throw new cRuntimeError(this,eESTKUFLOW);
                   if (stk[tos].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eEBADARGS,e.f->name());
                   stk[tos] = e.f->mathFunc1Arg()(stk[tos].dbl);
                   break;
               case 2:
                   if (tos<1)
                       throw new cRuntimeError(this,eESTKUFLOW);
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eEBADARGS,e.f->name());
                   stk[tos-1] = e.f->mathFunc2Args()(stk[tos-1].dbl, stk[tos].dbl);
                   tos--;
                   break;
               case 3:
                   if (tos<2)
                       throw new cRuntimeError(this,eESTKUFLOW);
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL || stk[tos-2].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eEBADARGS,e.f->name());
                   stk[tos-2] = e.f->mathFunc3Args()(stk[tos-2].dbl, stk[tos-1].dbl, stk[tos].dbl);
                   tos-=2;
                   break;
               case 4:
                   if (tos<3)
                       throw new cRuntimeError(this,eESTKUFLOW);
                   if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL || stk[tos-2].type!=StkValue::DBL || stk[tos-3].type!=StkValue::DBL)
                       throw new cRuntimeError(this,eEBADARGS,e.f->name());
                   stk[tos-3] = e.f->mathFunc4Args()(stk[tos-3].dbl, stk[tos-2].dbl, stk[tos-1].dbl, stk[tos].dbl);
                   tos-=3;
                   break;
               default:
                   throw new cRuntimeError(this,eBADEXP);
             }
             break;

           case Elem::NEDFUNC:
             {
             int numargs = e.af->numArgs();
             int argpos = tos-numargs+1; // stk[] index of 1st arg to pass
             if (argpos<0)
                 throw new cRuntimeError(this,eESTKUFLOW);
             const char *argtypes = e.af->argTypes();
             for (int i=0; i<numargs; i++)
                 if (stk[argpos+i].type != (argtypes[i]=='L' ? 'D' : argtypes[i]))
                     throw new cRuntimeError(this,eEBADARGS,e.af->name());
             stk[argpos] = e.af->functionPointer()(context, stk+argpos, numargs);
             tos = argpos;
             break;
             }

           case Elem::FUNCTOR:
             {
             int numargs = e.fu->numArgs();
             int argpos = tos-numargs+1; // stk[] index of 1st arg to pass
             if (argpos<0)
                 throw new cRuntimeError(this,eESTKUFLOW);
             const char *argtypes = e.fu->argTypes();
             for (int i=0; i<numargs; i++)
                 if (stk[argpos+i].type != (argtypes[i]=='L' ? 'D' : argtypes[i]))
                     throw new cRuntimeError(this,eEBADARGS,e.fu->fullName());
             stk[argpos] = e.fu->evaluate(context, stk+argpos, numargs);
             tos = argpos;
             break;
             }

           case Elem::OP:
             if (e.op==NEG || e.op==NOT || e.op==BIN_NOT)
             {
                 // unary
                 if (tos<0)
                     throw new cRuntimeError(this,eESTKUFLOW);
                 //FIXME better error messages! "cannot convert from x to y" etc!!
                 switch (e.op)
                 {
                     case NEG:
                         if (stk[tos].type!=StkValue::DBL)
                             throw new cRuntimeError(this,eEBADARGS,"-");
                         stk[tos] = -stk[tos].dbl;
                         break;
                     case NOT:
                         if (stk[tos].type!=StkValue::BOOL)
                             throw new cRuntimeError(this,eEBADARGS,"!");
                         stk[tos] = !stk[tos].bl;
                         break;
                     case BIN_NOT:
                         if (stk[tos].type!=StkValue::DBL)
                             throw new cRuntimeError(this,eEBADARGS,"~");
                         stk[tos] = (double)~ulong(stk[tos].dbl);
                         break;
                 }
             }
             else if (e.op==IIF)
             {
                 // tertiary
                 if (tos<2)
                     throw new cRuntimeError(this,eESTKUFLOW);
                 // 1st arg must be bool, others 2nd and 3rd can be anything
                 if (stk[tos-2].type!=StkValue::BOOL)
                     throw new cRuntimeError(this,eEBADARGS,"");
                 stk[tos-2] = (stk[tos-2].bl ? stk[tos-1] : stk[tos]);
                 tos-=2;
             }
             else
             {
                 // binary
                 if (tos<1)
                     throw new cRuntimeError(this,eESTKUFLOW);
                 switch(e.op)
                 {
                   case ADD:
                       // double addition or string concatenation
                       if (stk[tos-1].type==StkValue::DBL && stk[tos].type==StkValue::DBL)
                           stk[tos-1] = stk[tos-1].dbl + stk[tos].dbl;
                       else if (stk[tos-1].type==StkValue::STR && stk[tos].type==StkValue::STR)
                           stk[tos-1] = stk[tos-1].str + stk[tos].str;
                       else
                           throw new cRuntimeError(this,eEBADARGS,"+");
                       tos--;
                       break;
                   case SUB:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw new cRuntimeError(this,eEBADARGS,"-");
                       stk[tos-1] = stk[tos-1].dbl - stk[tos].dbl;
                       tos--;
                       break;
                   case MUL:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw new cRuntimeError(this,eEBADARGS,"*");
                       stk[tos-1] = stk[tos-1].dbl * stk[tos].dbl;
                       tos--;
                       break;
                   case DIV:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw new cRuntimeError(this,eEBADARGS,"/");
                       stk[tos-1] = stk[tos-1].dbl / stk[tos].dbl;
                       tos--;
                       break;
                   case MOD:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw new cRuntimeError(this,eEBADARGS,"%");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) % ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case POW:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw new cRuntimeError(this,eEBADARGS,"^");
                       stk[tos-1] = pow(stk[tos-1].dbl, stk[tos].dbl);
                       tos--;
                       break;
                   case AND:
                       if (stk[tos].type!=StkValue::BOOL || stk[tos-1].type!=StkValue::BOOL)
                           throw new cRuntimeError(this,eEBADARGS,"&&");
                       stk[tos-1] = stk[tos-1].bl && stk[tos].bl;
                       tos--;
                       break;
                   case OR:
                       if (stk[tos].type!=StkValue::BOOL || stk[tos-1].type!=StkValue::BOOL)
                           throw new cRuntimeError(this,eEBADARGS,"||");
                       stk[tos-1] = stk[tos-1].bl || stk[tos].bl;
                       tos--;
                       break;
                   case XOR:
                       if (stk[tos].type!=StkValue::BOOL || stk[tos-1].type!=StkValue::BOOL)
                           throw new cRuntimeError(this,eEBADARGS,"##");
                       stk[tos-1] = stk[tos-1].bl != stk[tos].bl;
                       tos--;
                       break;
                   case BIN_AND:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw new cRuntimeError(this,eEBADARGS,"&");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) & ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case BIN_OR:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw new cRuntimeError(this,eEBADARGS,"|");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) | ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case BIN_XOR:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw new cRuntimeError(this,eEBADARGS,"#");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) ^ ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case LSHIFT:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw new cRuntimeError(this,eEBADARGS,"<<");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) << ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case RSHIFT:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw new cRuntimeError(this,eEBADARGS,">>");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) >> ulong(stk[tos].dbl));
                       tos--;
                       break;
#define COMPARISON(RELATION) \
                                 if (stk[tos-1].type==StkValue::DBL && stk[tos].type==StkValue::DBL) \
                                     stk[tos-1] = (stk[tos-1].dbl RELATION stk[tos].dbl); \
                                 else if (stk[tos-1].type==StkValue::STR && stk[tos].type==StkValue::STR) \
                                     stk[tos-1] = (stk[tos-1].str RELATION stk[tos].str); \
                                 else if (stk[tos-1].type==StkValue::BOOL && stk[tos].type==StkValue::BOOL) \
                                     stk[tos-1] = (stk[tos-1].bl RELATION stk[tos].bl); \
                                 else \
                                     throw new cRuntimeError(this,eEBADARGS,#RELATION); \
                                 tos--;
                   case EQ:
                       COMPARISON(==);
                       break;
                   case NE:
                       COMPARISON(!=);
                       break;
                   case LT:
                       COMPARISON(<);
                       break;
                   case LE:
                       COMPARISON(<=);
                       break;
                   case GT:
                       COMPARISON(>);
                       break;
                   case GE:
                       COMPARISON(>=);
                       break;
#undef COMPARISON
                   default:
                       throw new cRuntimeError(this,eBADEXP);
                 }
             }
             break;
           default:
             throw new cRuntimeError(this,eBADEXP);
       }
    }
    if (tos!=0)
        throw new cRuntimeError(this,eBADEXP);

printf("        ==> returning %s\n", stk[tos].toString().c_str()); //XXX

    return stk[tos];
}

bool cDynamicExpression::boolValue(cComponent *context)
{
    StkValue v = evaluate(context);
    if (v.type!=StkValue::BOOL)
        throw new cRuntimeError(this, eECANTCAST,"bool");
    return v.bl;
}

long cDynamicExpression::longValue(cComponent *context)
{
    StkValue v = evaluate(context);
    if (v.type!=StkValue::DBL)
        throw new cRuntimeError(this, eECANTCAST,"long");
    return double_to_long(v.dbl);
}

double cDynamicExpression::doubleValue(cComponent *context)
{
    StkValue v = evaluate(context);
    if (v.type!=StkValue::DBL)
        throw new cRuntimeError(this, eECANTCAST,"double");
    return v.dbl;
}

std::string cDynamicExpression::stringValue(cComponent *context)
{
    StkValue v = evaluate(context);
    if (v.type!=StkValue::STR)
        throw new cRuntimeError(this, eECANTCAST,"string");
    return v.str;
}

cXMLElement *cDynamicExpression::xmlValue(cComponent *context)
{
    StkValue v = evaluate(context);
    if (v.type!=StkValue::XML)
        throw new cRuntimeError(this, eECANTCAST,"XML element");
    return v.xml;
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
               case Elem::BOOL:
                 if (tos>=stksize-1)
                     throw new cRuntimeError(this,eESTKOFLOW);
                 strstk[++tos] = (e.b ? "true" : "false");
                 pristk[tos] = 0;
                 break;
               case Elem::DBL:
                 {
                 if (tos>=stksize-1)
                     throw new cRuntimeError(this,eESTKOFLOW);
                 char buf[32];
                 sprintf(buf, "%g", e.d);
                 strstk[++tos] = buf;
                 pristk[tos] = 0;
                 }
                 break;
               case Elem::STR:
                 if (tos>=stksize-1)
                     throw new cRuntimeError(this,eESTKOFLOW);
                 strstk[++tos] = std::string("\"") + e.s+"\"";
                 pristk[tos] = 0;
                 break;
               case Elem::XML:
                 if (tos>=stksize-1)
                     throw new cRuntimeError(this,eESTKOFLOW);
                 strstk[++tos] = std::string("<") + (e.x ? e.x->getTagName() : "null") +">"; //FIXME plus location info?
                 pristk[tos] = 0;
                 break;
               case Elem::CPAR:
                 if (!e.p || tos>=stksize-1)
                     throw new cRuntimeError(this,eESTKOFLOW);
                 strstk[++tos] = e.p->fullPath();
                 pristk[tos] = 0;
                 break;
               case Elem::MATHFUNC:
               case Elem::NEDFUNC:
                 {
                 int numargs = (e.type==Elem::MATHFUNC) ? e.f->numArgs() : e.af->numArgs();
                 std::string name = (e.type==Elem::MATHFUNC) ? e.f->name() : e.af->name();
                 int argpos = tos-numargs+1; // strstk[] index of 1st arg to pass
                 if (argpos<0)
                     throw new cRuntimeError(this,eESTKUFLOW);
                 std::string tmp = name+"(";
                 for (int i=0; i<numargs; i++)
                     tmp += (i==0 ? "" : ", ") + strstk[argpos+i];
                 tmp += ")";
                 strstk[argpos] = tmp;
                 tos = argpos;
                 pristk[tos] = 0;
                 break;
                 }
               case Elem::FUNCTOR:
                 {
                 int numargs = e.fu->numArgs();
                 int argpos = tos-numargs+1; // strstk[] index of 1st arg to pass
                 if (argpos<0)
                     throw new cRuntimeError(this,eESTKUFLOW);
                 strstk[argpos] = e.fu->toString(strstk+argpos, numargs);
                 tos = argpos;
                 break;
                 }
               case Elem::OP:
                 if (e.op==NEG || e.op==NOT || e.op==BIN_NOT)
                 {
                     if (tos<0)
                         throw new cRuntimeError(this,eESTKUFLOW);
                     const char *op;
                     switch (e.op)
                     {
                         case NEG: op=" -"; break;
                         case NOT: op=" !"; break;
                         case BIN_NOT: op=" ~"; break;
                         default:  op=" ???";
                     }
                     strstk[tos] = std::string(op) + strstk[tos]; // pri=0: never needs parens
                     pristk[tos] = 0;
                 }
                 if (e.op==IIF)  // conditional (tertiary)
                 {
                     if (tos<2)
                         throw new cRuntimeError(this,eESTKUFLOW);
                     strstk[tos-2] = strstk[tos-2] + " ? " + strstk[tos-1] + " : " + strstk[tos];
                     tos-=2;
                     pristk[tos] = 8;
                 }
                 else
                 {
                     // binary
                     if (tos<1)
                         throw new cRuntimeError(this,eESTKUFLOW);
                     int pri;
                     const char *op;
                     switch(e.op)
                     {
                         //
                         // Precedences, based on expr.y:
                         //   prec=7: && || ##
                         //   prec=6: == != > >= < <=
                         //   prec=5: & | #
                         //   prec=4: << >>
                         //   prec=3: + -
                         //   prec=2: * / %
                         //   prec=1: ^
                         //   prec=0: UMIN ! ~
                         //
                         case ADD: op=" + "; pri=3; break;
                         case SUB: op=" - "; pri=3; break;
                         case MUL: op=" * "; pri=2; break;
                         case DIV: op=" / "; pri=2; break;
                         case MOD: op=" % "; pri=2; break;
                         case POW: op=" ^ "; pri=1; break;
                         case EQ:  op=" = "; pri=6; break;
                         case LT:  op=" < "; pri=6; break;
                         case GT:  op=" > "; pri=6; break;
                         case NE:  op=" != "; pri=6; break;
                         case LE:  op=" <= "; pri=6; break;
                         case GE:  op=" >= "; pri=6; break;
                         case AND: op=" && "; pri=7; break;
                         case OR:  op=" || "; pri=7; break;
                         case XOR: op=" ## "; pri=7; break;
                         case BIN_AND: op=" & "; pri=5; break;
                         case BIN_OR:  op=" | "; pri=5; break;
                         case BIN_XOR: op=" # "; pri=5; break;
                         case LSHIFT:  op=" << "; pri=4; break;
                         case RSHIFT:  op=" >> "; pri=4; break;
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

//--------

namespace NEDSupport
{

ModuleIndex::ModuleIndex()
{
}

StkValue ModuleIndex::evaluate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==0 && context!=NULL);
    cModule *module = dynamic_cast<cModule *>(context);
    if (!module)
        throw new cRuntimeError(context,"cannot evaluate `index' operator in expression: context is not a module");
    return (double) module->index();
}

std::string ModuleIndex::toString(std::string args[], int numargs)
{
    return "index";
}

//----

ParameterRef::ParameterRef(const char *paramName, bool ofParent, bool printThis)
{
    this->paramName = paramName;
    this->ofParent = ofParent;
    this->printThis = printThis;
}

StkValue ParameterRef::evaluate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==0 && context!=NULL);
    cModule *module = dynamic_cast<cModule *>(ofParent ? context->owner() : context);
    if (!module)
        throw new cRuntimeError(context,eENOPARENT);
    return module->par(paramName.c_str());
}

std::string ParameterRef::toString(std::string args[], int numargs)
{
    if (printThis)
        return std::string("this.")+paramName;
    else
        return paramName;
}

//----

SiblingModuleParameterRef::SiblingModuleParameterRef(const char *moduleName, const char *paramName, bool ofParent, bool withModuleIndex)
{
    this->moduleName = moduleName;
    this->paramName = paramName;
    this->ofParent = ofParent;
    this->withModuleIndex = withModuleIndex;
}

StkValue SiblingModuleParameterRef::evaluate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(context!=NULL);
    ASSERT(!withModuleIndex || (withModuleIndex && numargs==1 && args[0].type==StkValue::DBL));
    cModule *compoundModule = dynamic_cast<cModule *>(ofParent ? context->owner() : context); // this works for channels too
    if (!compoundModule)
        throw new cRuntimeError(context,eENOPARENT);
    int moduleIndex = withModuleIndex ? (int)args[0].dbl : -1;
    cModule *siblingModule = compoundModule->submodule(moduleName.c_str(), moduleIndex);
    if (!siblingModule)
        throw new cRuntimeError(context,"cannot find submodule `%[%d]' for parameter `%s[%d].%s'",
                                moduleName, moduleIndex, moduleName, moduleIndex, paramName);
    return siblingModule->par(paramName.c_str());
}

std::string SiblingModuleParameterRef::toString(std::string args[], int numargs)
{
    if (withModuleIndex)
        return moduleName+"["+args[0]+"]."+paramName;
    else
        return moduleName+"."+paramName;
}

//----

const char *LoopVar::varNames[32];
long LoopVar::vars[32];
int LoopVar::varCount = 0;

long& LoopVar::pushVar(const char *varName)
{
    ASSERT(varCount<32);
    varNames[varCount] = varName;
    return vars[varCount++];
}

void LoopVar::popVar()
{
    ASSERT(varCount>0);
    varCount--;
}

void LoopVar::reset()
{
    varCount = 0;
}

StkValue LoopVar::evaluate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==0);
    const char *var = varName.c_str();
    for (int i=0; i<varCount; i++)
        if (strcmp(var, varNames[i])==0)
            return vars[i];
    throw new cRuntimeError(context, "loop variable %s not found", varName.c_str());
}

std::string LoopVar::toString(std::string args[], int numargs)
{
    // return varName;
    return std::string("(loopvar)")+varName;  //XXX debugging only
}

//---

Sizeof::Sizeof(const char *ident, bool ofParent, bool printThis)
{
    this->ident = ident;
    this->ofParent = ofParent;
    this->printThis = printThis;
}

StkValue Sizeof::evaluate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==0 && context!=NULL);
    cModule *module = dynamic_cast<cModule *>(ofParent ? context->owner() : context);
    if (!module)
        throw new cRuntimeError(context,eENOPARENT);

    // ident might be a gate vector of the *parent* module, or a sibling submodule vector
    // Note: it might NOT mean gate vector of this module
    if (module->hasGate(ident.c_str()))
    {
        return (long) module->gateSize(ident.c_str()); // returns 1 if it's not a vector
    }
    else
    {
        // Find ident among submodules. If there's no such submodule, it may
        // be that such submodule vector never existed, or can be that it's zero
        // size -- we cannot tell, so we have to return 0 (and cannot throw error).
        cModule *siblingModule = module->submodule(ident.c_str(), 0); // returns NULL if submodule is not a vector
        if (!siblingModule && module->submodule(ident.c_str()))
            return 1L; // return 1 if submodule exists but not a vector
        return (long) siblingModule ? siblingModule->size() : 0L;
    }

}

std::string Sizeof::toString(std::string args[], int numargs)
{
    return std::string(printThis ? "sizeof(this." : "sizeof(") + ident + ")";
}



};

/*
//FIXME make error messages consistent

typedef cDynamicExpression::StkValue StkValue; // abbreviation for local use

//
// internal function to support NED: resolves a sizeof(moduleOrGateVectorName) reference
//
StkValue cDynamicExpression::sizeofIdent(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==1 && args[0].type==StkValue::STR);
    const char *ident = args[0].str.c_str();

    // ident might be a gate vector of the *parent* module, or a sibling submodule vector
    // Note: it might NOT mean gate vector of this module
    cModule *parentModule = dynamic_cast<cModule *>(context->owner()); // this works for channels too
    if (!parentModule)
        throw new cRuntimeError(context, "sizeof(%s) occurs in wrong context", ident);
    if (parentModule->hasGate(ident))
    {
        return (long) parentModule->gateSize(ident); // returns 1 if it's not a vector
    }
    else
    {
        // Find ident among submodules. If there's no such submodule, it may
        // be that such submodule vector never existed, or can be that it's zero
        // size -- we cannot tell, so we have to return 0.
        cModule *siblingModule = parentModule->submodule(ident, 0); // returns NULL if submodule is not a vector
        if (!siblingModule && parentModule->submodule(ident))
            return 1L; // return 1 if submodule exists but not a vector
        return (long) siblingModule ? siblingModule->size() : 0L;
    }
}

//
// internal function to support NED: resolves a sizeof(this.gateVectorName) reference
//
StkValue cDynamicExpression::sizeofGate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==1 && args[0].type==StkValue::STR);
    const char *gateName = args[0].str.c_str();
    cModule *module = dynamic_cast<cModule *>(context);
    if (!module || !module->hasGate(gateName))
        throw new cRuntimeError(context, "error evaluating sizeof(): no such gate: `%s'", gateName);
    return (long) module->gateSize(gateName); // returns 1 if it's not a vector
}

//
// internal function to support NED: resolves a sizeof(parent.gateVectorName) reference
//
StkValue cDynamicExpression::sizeofParentModuleGate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==1 && args[0].type==StkValue::STR);
    const char *gateName = args[0].str.c_str();
    cModule *parentModule = dynamic_cast<cModule *>(context->owner()); // this works for channels too
    if (!parentModule)
        throw new cRuntimeError(context, "sizeof() occurs in wrong context", gateName);
    if (!parentModule->hasGate(gateName))
        throw new cRuntimeError(context, "error evaluating sizeof(): no such gate: `%s'", gateName);
    return (long) parentModule->gateSize(gateName); // returns 1 if it's not a vector
}

//
// internal function to support NED: resolves a sizeof(module.gateName) reference
//
StkValue cDynamicExpression::sizeofSiblingModuleGate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==2 && args[0].type==StkValue::STR && args[1].type==StkValue::STR);
    const char *siblingModuleName = args[0].str.c_str();
    const char *gateName = args[1].str.c_str();

    cModule *parentModule = dynamic_cast<cModule *>(context->owner()); // this works for channels too
    if (!parentModule)
        throw new cRuntimeError(context, "sizeof() occurs in wrong context", gateName);
    cModule *siblingModule = parentModule->submodule(siblingModuleName); // returns NULL if submodule is not a vector
    if (!siblingModule->hasGate(gateName))
        throw new cRuntimeError(context, "error evaluating sizeof(): no such gate: `%s'", gateName);
    return (long) siblingModule->gateSize(gateName); // returns 1 if it's not a vector
}

//
// internal function to support NED: resolves a sizeof(module.gateName) reference
//
StkValue cDynamicExpression::sizeofIndexedSiblingModuleGate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==3 && args[0].type==StkValue::STR && args[1].type==StkValue::STR && args[2].type==StkValue::DBL);
    const char *gateName = args[1].str.c_str();
    const char *siblingModuleName = args[1].str.c_str();
    int siblingModuleIndex = (int)args[2].dbl;
    cModule *parentModule = dynamic_cast<cModule *>(context->owner()); // this works for channels too
    cModule *siblingModule = parentModule ? parentModule->submodule(siblingModuleName, siblingModuleIndex) : NULL;
    if (!siblingModule)
        throw new cRuntimeError(context,"sizeof(): cannot find submodule %[%d]",
                                siblingModuleName, siblingModuleIndex,
                                siblingModuleName, siblingModuleIndex, gateName);
    return (long) siblingModule->gateSize(gateName); // returns 1 if it's not a vector
}
*/
