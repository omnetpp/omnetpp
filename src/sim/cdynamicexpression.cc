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
#include "stringutil.h"
#include "unitconversion.h"

USING_NAMESPACE

cStringPool cDynamicExpression::Elem::stringPool;

void cDynamicExpression::Elem::operator=(const Elem& other)
{
    deleteOld();

    memcpy(this, &other, sizeof(Elem));

    if (type==STR)
        s = stringPool.get(s);
    else if (type==DBL)
        d.unit = stringPool.get(d.unit);
    else if (type==FUNCTOR)
        fu = (Functor *) fu->dup();
    else if (type==CONSTSUBEXPR)
        constexpr = (cExpression *) constexpr->dup();
}

cDynamicExpression::Elem::~Elem()
{
    deleteOld();
}

void cDynamicExpression::Elem::deleteOld()
{
    if (type==STR)
        stringPool.release(s);
    else if (type==DBL)
        stringPool.release(d.unit);
    else if (type==FUNCTOR)
        delete fu;
    else if (type==CONSTSUBEXPR)
        delete constexpr;
}

void cDynamicExpression::Value::operator=(const cPar& par)
{
    switch (par.type())
    {
      case cPar::BOOL: *this = par.boolValue(); break;
      case cPar::DOUBLE: *this = par.doubleValue(); dblunit = par.unit(); break;
      case cPar::LONG: *this = par.doubleValue(); break;
      case cPar::STRING: *this = par.stringValue(); break;
      case cPar::XML: *this = par.xmlValue(); break;
      default: throw cRuntimeError("internal error: bad cPar type: %s", par.fullPath().c_str());
    }
}

std::string cDynamicExpression::Value::toString()
{
    char buf[32];
    switch (type)
    {
      case BOOL: return bl ? "true" : "false";
      case DBL:  sprintf(buf, "%g%s", dbl, opp_nulltoempty(dblunit)); return buf;
      case STR:  return opp_quotestr(str.c_str());
      case XML:  return xml->detailedInfo();
                 //or: return std::string("<")+xml->getTagName()+" ... >, " + opp_nulltoempty(xml->getSourceLocation());
      default:   throw cRuntimeError("internal error: bad Value type");
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

bool cDynamicExpression::boolValue(cComponent *context)
{
    Value v = evaluate(context);
    if (v.type!=Value::BOOL)
        throw cRuntimeError(this, eECANTCAST, "bool");
    return v.bl;
}

//FIXME rename these methods!!! evaluateToLong() etc
long cDynamicExpression::longValue(cComponent *context, const char *expectedUnit)
{
    Value v = evaluate(context);
    if (v.type!=Value::DBL)
        throw cRuntimeError(this, eECANTCAST, "long");
    return double_to_long(UnitConversion::convertUnit(v.dbl, v.dblunit, expectedUnit));
}

double cDynamicExpression::doubleValue(cComponent *context, const char *expectedUnit)
{
    Value v = evaluate(context);
    if (v.type!=Value::DBL)
        throw cRuntimeError(this, eECANTCAST, "double");
    return UnitConversion::convertUnit(v.dbl, v.dblunit, expectedUnit);
}

std::string cDynamicExpression::stringValue(cComponent *context)
{
    Value v = evaluate(context);
    if (v.type!=Value::STR)
        throw cRuntimeError(this, eECANTCAST, "string");
    return v.str;
}

cXMLElement *cDynamicExpression::xmlValue(cComponent *context)
{
    Value v = evaluate(context);
    if (v.type!=Value::XML)
        throw cRuntimeError(this, eECANTCAST, "XML element");
    return v.xml;
}

#define ulong(x) ((unsigned long)(x))

cDynamicExpression::Value cDynamicExpression::evaluate(cComponent *context) const
{
    printf("    evaluating: %s\n", toString().c_str()); //XXX

    const int stksize = 20;
    Value stk[stksize];

    int tos = -1;
    for (int i = 0; i < nelems; i++)
    {
       Elem& e = elems[i];
       switch (e.type)
       {
           case Elem::BOOL:
             if (tos>=stksize-1)
                 throw cRuntimeError(this,eESTKOFLOW);
             stk[++tos] = e.b;
             break;

           case Elem::DBL:
             if (tos>=stksize-1)
                 throw cRuntimeError(this,eESTKOFLOW);
             stk[++tos] = e.d.d;
             stk[tos].dblunit = e.d.unit;
             break;

           case Elem::STR:
             if (tos>=stksize-1)
                 throw cRuntimeError(this,eESTKOFLOW);
             stk[++tos] = e.s;
             break;

           case Elem::XML:
             if (tos>=stksize-1)
                 throw cRuntimeError(this,eESTKOFLOW);
             stk[++tos] = e.x;
             break;

           case Elem::CPAR:
             if (tos>=stksize-1)
                 throw cRuntimeError(this,eESTKOFLOW);
             stk[++tos] = *(e.p); break;
             stk[tos].dblunit = e.p->unit();
             break;

           case Elem::MATHFUNC:
             switch (e.f->numArgs())
             {
               case 0:
                   stk[++tos] = e.f->mathFuncNoArg()();
                   break;
               case 1:
                   if (tos<0)
                       throw cRuntimeError(this,eESTKUFLOW);
                   if (stk[tos].type!=Value::DBL)
                       throw cRuntimeError(this,eEBADARGS,e.f->name());
                   stk[tos] = e.f->mathFunc1Arg()(stk[tos].dbl);
                   break;
               case 2:
                   if (tos<1)
                       throw cRuntimeError(this,eESTKUFLOW);
                   if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                       throw cRuntimeError(this,eEBADARGS,e.f->name());
                   stk[tos-1] = e.f->mathFunc2Args()(stk[tos-1].dbl, stk[tos].dbl);
                   tos--;
                   break;
               case 3:
                   if (tos<2)
                       throw cRuntimeError(this,eESTKUFLOW);
                   if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL || stk[tos-2].type!=Value::DBL)
                       throw cRuntimeError(this,eEBADARGS,e.f->name());
                   stk[tos-2] = e.f->mathFunc3Args()(stk[tos-2].dbl, stk[tos-1].dbl, stk[tos].dbl);
                   tos-=2;
                   break;
               case 4:
                   if (tos<3)
                       throw cRuntimeError(this,eESTKUFLOW);
                   if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL || stk[tos-2].type!=Value::DBL || stk[tos-3].type!=Value::DBL)
                       throw cRuntimeError(this,eEBADARGS,e.f->name());
                   stk[tos-3] = e.f->mathFunc4Args()(stk[tos-3].dbl, stk[tos-2].dbl, stk[tos-1].dbl, stk[tos].dbl);
                   tos-=3;
                   break;
               default:
                   throw cRuntimeError(this,eBADEXP);
             }
             break;

           case Elem::NEDFUNC:
             {
             int numargs = e.af->numArgs();
             int argpos = tos-numargs+1; // stk[] index of 1st arg to pass
             if (argpos<0)
                 throw cRuntimeError(this,eESTKUFLOW);
             const char *argtypes = e.af->argTypes();
             for (int i=0; i<numargs; i++)
                 if (argtypes[i] != '*' && stk[argpos+i].type != (argtypes[i]=='L' ? 'D' : argtypes[i]))
                     throw cRuntimeError(this,eEBADARGS,e.af->name());
             stk[argpos] = e.af->functionPointer()(context, stk+argpos, numargs);
             tos = argpos;
             break;
             }

           case Elem::FUNCTOR:
             {
             int numargs = e.fu->numArgs();
             int argpos = tos-numargs+1; // stk[] index of 1st arg to pass
             if (argpos<0)
                 throw cRuntimeError(this,eESTKUFLOW);
             const char *argtypes = e.fu->argTypes();
             for (int i=0; i<numargs; i++)
                 if (argtypes[i] != '*' && stk[argpos+i].type != (argtypes[i]=='L' ? 'D' : argtypes[i]))
                     throw cRuntimeError(this,eEBADARGS,e.fu->fullName());
             stk[argpos] = e.fu->evaluate(context, stk+argpos, numargs);
             tos = argpos;
             break;
             }

           case Elem::CONSTSUBEXPR:
             // this should not occur
             throw cRuntimeError("evaluate: constant subexpressions should have already been evaluated");

           case Elem::OP:
             if (e.op==NEG || e.op==NOT || e.op==BIN_NOT)
             {
                 // unary
                 if (tos<0)
                     throw cRuntimeError(this,eESTKUFLOW);
                 switch (e.op)
                 {
                     case NEG:
                         if (stk[tos].type!=Value::DBL)
                             throw cRuntimeError(this,eEBADARGS,"-");
                         stk[tos] = -stk[tos].dbl;
                         break;
                     case NOT:
                         if (stk[tos].type!=Value::BOOL)
                             throw cRuntimeError(this,eEBADARGS,"!");
                         stk[tos] = !stk[tos].bl;
                         break;
                     case BIN_NOT:
                         if (stk[tos].type!=Value::DBL)
                             throw cRuntimeError(this,eEBADARGS,"~");
                         stk[tos] = (double)~ulong(stk[tos].dbl);
                         break;
                     default: ASSERT(false);
                 }
             }
             else if (e.op==IIF)
             {
                 // tertiary
                 if (tos<2)
                     throw cRuntimeError(this,eESTKUFLOW);
                 // 1st arg must be bool, others 2nd and 3rd can be anything
                 if (stk[tos-2].type!=Value::BOOL)
                     throw cRuntimeError(this,eEBADARGS,""); //XXX fix error msg
                 stk[tos-2] = (stk[tos-2].bl ? stk[tos-1] : stk[tos]);
                 tos-=2;
             }
             else
             {
                 // binary
                 if (tos<1)
                     throw cRuntimeError(this,eESTKUFLOW);
                 switch(e.op)
                 {
                   case ADD:
                       // double addition or string concatenation
                       if (stk[tos-1].type==Value::DBL && stk[tos].type==Value::DBL)
                           stk[tos-1] = stk[tos-1].dbl + stk[tos].dbl;
                       else if (stk[tos-1].type==Value::STR && stk[tos].type==Value::STR)
                           stk[tos-1] = stk[tos-1].str + stk[tos].str;
                       else
                           throw cRuntimeError(this,eEBADARGS,"+");
                       tos--;
                       break;
                   case SUB:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw cRuntimeError(this,eEBADARGS,"-");
                       stk[tos-1] = stk[tos-1].dbl - stk[tos].dbl;
                       tos--;
                       break;
                   case MUL:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw cRuntimeError(this,eEBADARGS,"*");
                       stk[tos-1] = stk[tos-1].dbl * stk[tos].dbl;
                       tos--;
                       break;
                   case DIV:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw cRuntimeError(this,eEBADARGS,"/");
                       stk[tos-1] = stk[tos-1].dbl / stk[tos].dbl;
                       tos--;
                       break;
                   case MOD:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw cRuntimeError(this,eEBADARGS,"%");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) % ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case POW:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw cRuntimeError(this,eEBADARGS,"^");
                       stk[tos-1] = pow(stk[tos-1].dbl, stk[tos].dbl);
                       tos--;
                       break;
                   case AND:
                       if (stk[tos].type!=Value::BOOL || stk[tos-1].type!=Value::BOOL)
                           throw cRuntimeError(this,eEBADARGS,"&&");
                       stk[tos-1] = stk[tos-1].bl && stk[tos].bl;
                       tos--;
                       break;
                   case OR:
                       if (stk[tos].type!=Value::BOOL || stk[tos-1].type!=Value::BOOL)
                           throw cRuntimeError(this,eEBADARGS,"||");
                       stk[tos-1] = stk[tos-1].bl || stk[tos].bl;
                       tos--;
                       break;
                   case XOR:
                       if (stk[tos].type!=Value::BOOL || stk[tos-1].type!=Value::BOOL)
                           throw cRuntimeError(this,eEBADARGS,"##");
                       stk[tos-1] = stk[tos-1].bl != stk[tos].bl;
                       tos--;
                       break;
                   case BIN_AND:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw cRuntimeError(this,eEBADARGS,"&");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) & ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case BIN_OR:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw cRuntimeError(this,eEBADARGS,"|");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) | ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case BIN_XOR:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw cRuntimeError(this,eEBADARGS,"#");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) ^ ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case LSHIFT:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw cRuntimeError(this,eEBADARGS,"<<");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) << ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case RSHIFT:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw cRuntimeError(this,eEBADARGS,">>");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) >> ulong(stk[tos].dbl));
                       tos--;
                       break;
#define COMPARISON(RELATION) \
                                 if (stk[tos-1].type==Value::DBL && stk[tos].type==Value::DBL) \
                                     stk[tos-1] = (stk[tos-1].dbl RELATION stk[tos].dbl); \
                                 else if (stk[tos-1].type==Value::STR && stk[tos].type==Value::STR) \
                                     stk[tos-1] = (stk[tos-1].str RELATION stk[tos].str); \
                                 else if (stk[tos-1].type==Value::BOOL && stk[tos].type==Value::BOOL) \
                                     stk[tos-1] = (stk[tos-1].bl RELATION stk[tos].bl); \
                                 else \
                                     throw cRuntimeError(this,eEBADARGS,#RELATION); \
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
                       throw cRuntimeError(this,eBADEXP);
                 }
             }
             break;
           default:
             throw cRuntimeError(this,eBADEXP);
       }
    }
    if (tos!=0)
        throw cRuntimeError(this,eBADEXP);

    printf("        ==> returning %s\n", stk[tos].toString().c_str()); //XXX

    return stk[tos];
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
                     throw cRuntimeError(this,eESTKOFLOW);
                 strstk[++tos] = (e.b ? "true" : "false");
                 pristk[tos] = 0;
                 break;
               case Elem::DBL:
                 {
                 if (tos>=stksize-1)
                     throw cRuntimeError(this,eESTKOFLOW);
                 char buf[32];
                 sprintf(buf, "%g%s", e.d.d, opp_nulltoempty(e.d.unit));
                 strstk[++tos] = buf;
                 pristk[tos] = 0;
                 }
                 break;
               case Elem::STR:
                 if (tos>=stksize-1)
                     throw cRuntimeError(this,eESTKOFLOW);
                 strstk[++tos] = opp_quotestr(e.s ? e.s : "");
                 pristk[tos] = 0;
                 break;
               case Elem::XML:
                 if (tos>=stksize-1)
                     throw cRuntimeError(this,eESTKOFLOW);
                 strstk[++tos] = std::string("<") + (e.x ? e.x->getTagName() : "null") +">"; //FIXME plus location info?
                 pristk[tos] = 0;
                 break;
               case Elem::CPAR:
                 if (!e.p || tos>=stksize-1)
                     throw cRuntimeError(this,eESTKOFLOW);
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
                     throw cRuntimeError(this,eESTKUFLOW);
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
                     throw cRuntimeError(this,eESTKUFLOW);
                 strstk[argpos] = e.fu->toString(strstk+argpos, numargs);
                 tos = argpos;
                 break;
                 }
               case Elem::CONSTSUBEXPR:
                 strstk[++tos] = std::string("const(")+e.constexpr->toString()+")";
                 break;
               case Elem::OP:
                 if (e.op==NEG || e.op==NOT || e.op==BIN_NOT)
                 {
                     if (tos<0)
                         throw cRuntimeError(this,eESTKUFLOW);
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
                 else if (e.op==IIF)  // conditional (tertiary)
                 {
                     if (tos<2)
                         throw cRuntimeError(this,eESTKUFLOW);
                     strstk[tos-2] = strstk[tos-2] + " ? " + strstk[tos-1] + " : " + strstk[tos];
                     tos-=2;
                     pristk[tos] = 8;
                 }
                 else
                 {
                     // binary
                     if (tos<1)
                         throw cRuntimeError(this,eESTKUFLOW);
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
                         case EQ:  op=" == "; pri=6; break;
                         case NE:  op=" != "; pri=6; break;
                         case LT:  op=" < "; pri=6; break;
                         case GT:  op=" > "; pri=6; break;
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
                 throw cRuntimeError(this,eBADEXP);
           }
        }
        if (tos!=0)
            throw cRuntimeError(this,eBADEXP);
        return strstk[tos];
    }
    catch (std::exception& e)
    {
        std::string ret = std::string("[[ ") + e.what() + " ]]";
        return ret;
    }
}

void cDynamicExpression::parse(const char *text)
{
    // throws exception if something goes wrong
    ::doParseExpression(text, elems, nelems);
}

bool cDynamicExpression::isAConstant() const
{
    for (int i=0; i<nelems; i++)
    {
        switch(elems[i].type)
        {
            // literals and anything calculated from them are OK
            case Elem::BOOL:
            case Elem::DBL:
            case Elem::STR:
            case Elem::XML:
            case Elem::OP:
                continue; //OK
            default:
                return false;
        }
    }
    return true;
}

const char *cDynamicExpression::unit() const
{
    // this can be made more sophisticated, like unit of (3*5s) is still "s"
    if (nelems==1 && elems[0].type==Elem::DBL)
        return elems[0].d.unit;
    return NULL;
}

