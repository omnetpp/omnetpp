//==========================================================================
//   EXPRESSION.CC  - part of
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

#include <math.h>
#include "expression.h"
#include "expressionyydefs.h"


#define eESTKOFLOW   "Stack overflow"
#define eESTKUFLOW   "Stack underflow"
#define eEBADARGS    "Wrong arguments for '%s'"
#define eBADEXP      "Malformed expression"
#define eECANTCAST   "Cannot cast to %s"


void Expression::Elem::operator=(const Elem& other)
{
    deleteOld();

    memcpy(this, &other, sizeof(Elem));

    if (type==STR)
        s = opp_clonestr(s);
    else if (type==FUNCTOR)
        fu = (Functor *) fu->dup();
}

Expression::Elem::~Elem()
{
    deleteOld();
}

void Expression::Elem::deleteOld()
{
    if (type==STR)
        delete [] s;
    else if (type==FUNCTOR)
        delete fu;
}

std::string Expression::StkValue::toString()
{
    char buf[32];
    switch (type)
    {
      case BOOL: return bl ? "true" : "false";
      case DBL:  sprintf(buf, "%g", dbl); return buf;
      case STR:  return opp_quotestr(str.c_str());
      default:   throw opp_runtime_error("internal error: bad StkValue type");
    }
}


Expression::Expression()
{
    elems = NULL;
    nelems = 0;
}

Expression::~Expression()
{
    delete [] elems;
}

Expression& Expression::operator=(const Expression& other)
{
    if (this==&other) return *this;

    delete [] elems;

    nelems = other.nelems;
    elems = new Elem[nelems];
    for (int i=0; i<nelems; i++)
        elems[i] = other.elems[i];
    return *this;
}

void Expression::setExpression(Elem e[], int n)
{
    delete [] elems;
    elems = e;
    nelems = n;
}

#define ulong(x) ((unsigned long)(x))

Expression::StkValue Expression::evaluate() const
{
    //XXX printf("    evaluating: %s\n", toString().c_str());
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
                 throw opp_runtime_error(eESTKOFLOW);
             stk[++tos] = e.b;
             break;

           case Elem::DBL:
             if (tos>=stksize-1)
                 throw opp_runtime_error(eESTKOFLOW);
             stk[++tos] = e.d;
             break;

           case Elem::STR:
             if (tos>=stksize-1)
                 throw opp_runtime_error(eESTKOFLOW);
             stk[++tos] = e.s;
             break;

           case Elem::FUNCTOR:
             {
             int numargs = e.fu->numArgs();
             int argpos = tos-numargs+1; // stk[] index of 1st arg to pass
             if (argpos<0)
                 throw opp_runtime_error(eESTKUFLOW);
             const char *argtypes = e.fu->argTypes();
             for (int i=0; i<numargs; i++)
                 if (stk[argpos+i].type != (argtypes[i]=='L' ? 'D' : argtypes[i]))
                     throw opp_runtime_error(eEBADARGS,e.fu->name());
             stk[argpos] = e.fu->evaluate(stk+argpos, numargs);
             tos = argpos;
             break;
             }

           case Elem::OP:
             if (e.op==NEG || e.op==NOT || e.op==BIN_NOT)
             {
                 // unary
                 if (tos<0)
                     throw opp_runtime_error(eESTKUFLOW);
                 switch (e.op)
                 {
                     case NEG:
                         if (stk[tos].type!=StkValue::DBL)
                             throw opp_runtime_error(eEBADARGS,"-");
                         stk[tos] = -stk[tos].dbl;
                         break;
                     case NOT:
                         if (stk[tos].type!=StkValue::BOOL)
                             throw opp_runtime_error(eEBADARGS,"!");
                         stk[tos] = !stk[tos].bl;
                         break;
                     case BIN_NOT:
                         if (stk[tos].type!=StkValue::DBL)
                             throw opp_runtime_error(eEBADARGS,"~");
                         stk[tos] = (double)~ulong(stk[tos].dbl);
                         break;
                     default: Assert(false);
                 }
             }
             else if (e.op==IIF)
             {
                 // tertiary
                 if (tos<2)
                     throw opp_runtime_error(eESTKUFLOW);
                 // 1st arg must be bool, others 2nd and 3rd can be anything
                 if (stk[tos-2].type!=StkValue::BOOL)
                     throw opp_runtime_error(eEBADARGS,""); //XXX fix error msg
                 stk[tos-2] = (stk[tos-2].bl ? stk[tos-1] : stk[tos]);
                 tos-=2;
             }
             else
             {
                 // binary
                 if (tos<1)
                     throw opp_runtime_error(eESTKUFLOW);
                 switch(e.op)
                 {
                   case ADD:
                       // double addition or string concatenation
                       if (stk[tos-1].type==StkValue::DBL && stk[tos].type==StkValue::DBL)
                           stk[tos-1] = stk[tos-1].dbl + stk[tos].dbl;
                       else if (stk[tos-1].type==StkValue::STR && stk[tos].type==StkValue::STR)
                           stk[tos-1] = stk[tos-1].str + stk[tos].str;
                       else
                           throw opp_runtime_error(eEBADARGS,"+");
                       tos--;
                       break;
                   case SUB:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw opp_runtime_error(eEBADARGS,"-");
                       stk[tos-1] = stk[tos-1].dbl - stk[tos].dbl;
                       tos--;
                       break;
                   case MUL:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw opp_runtime_error(eEBADARGS,"*");
                       stk[tos-1] = stk[tos-1].dbl * stk[tos].dbl;
                       tos--;
                       break;
                   case DIV:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw opp_runtime_error(eEBADARGS,"/");
                       stk[tos-1] = stk[tos-1].dbl / stk[tos].dbl;
                       tos--;
                       break;
                   case MOD:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw opp_runtime_error(eEBADARGS,"%");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) % ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case POW:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw opp_runtime_error(eEBADARGS,"^");
                       stk[tos-1] = pow(stk[tos-1].dbl, stk[tos].dbl);
                       tos--;
                       break;
                   case AND:
                       if (stk[tos].type!=StkValue::BOOL || stk[tos-1].type!=StkValue::BOOL)
                           throw opp_runtime_error(eEBADARGS,"&&");
                       stk[tos-1] = stk[tos-1].bl && stk[tos].bl;
                       tos--;
                       break;
                   case OR:
                       if (stk[tos].type!=StkValue::BOOL || stk[tos-1].type!=StkValue::BOOL)
                           throw opp_runtime_error(eEBADARGS,"||");
                       stk[tos-1] = stk[tos-1].bl || stk[tos].bl;
                       tos--;
                       break;
                   case XOR:
                       if (stk[tos].type!=StkValue::BOOL || stk[tos-1].type!=StkValue::BOOL)
                           throw opp_runtime_error(eEBADARGS,"##");
                       stk[tos-1] = stk[tos-1].bl != stk[tos].bl;
                       tos--;
                       break;
                   case BIN_AND:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw opp_runtime_error(eEBADARGS,"&");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) & ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case BIN_OR:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw opp_runtime_error(eEBADARGS,"|");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) | ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case BIN_XOR:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw opp_runtime_error(eEBADARGS,"#");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) ^ ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case LSHIFT:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw opp_runtime_error(eEBADARGS,"<<");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) << ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case RSHIFT:
                       if (stk[tos].type!=StkValue::DBL || stk[tos-1].type!=StkValue::DBL)
                           throw opp_runtime_error(eEBADARGS,">>");
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
                                     throw opp_runtime_error(eEBADARGS,#RELATION); \
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
                       throw opp_runtime_error(eBADEXP);
                 }
             }
             break;
           default:
             throw opp_runtime_error(eBADEXP);
       }
    }
    if (tos!=0)
        throw opp_runtime_error(eBADEXP);

    //XXX printf("        ==> returning %s\n", stk[tos].toString().c_str());

    return stk[tos];
}

bool Expression::boolValue()
{
    StkValue v = evaluate();
    if (v.type!=StkValue::BOOL)
        throw opp_runtime_error(eECANTCAST,"bool");
    return v.bl;
}

long Expression::longValue()
{
    StkValue v = evaluate();
    if (v.type!=StkValue::DBL)
        throw opp_runtime_error(eECANTCAST,"long");
    return (long) v.dbl;
}

double Expression::doubleValue()
{
    StkValue v = evaluate();
    if (v.type!=StkValue::DBL)
        throw opp_runtime_error(eECANTCAST,"double");
    return v.dbl;
}

std::string Expression::stringValue()
{
    StkValue v = evaluate();
    if (v.type!=StkValue::STR)
        throw opp_runtime_error(eECANTCAST,"string");
    return v.str;
}

std::string Expression::toString() const
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
                     throw opp_runtime_error(eESTKOFLOW);
                 strstk[++tos] = (e.b ? "true" : "false");
                 pristk[tos] = 0;
                 break;
               case Elem::DBL:
                 {
                 if (tos>=stksize-1)
                     throw opp_runtime_error(eESTKOFLOW);
                 char buf[32];
                 sprintf(buf, "%g", e.d);
                 strstk[++tos] = buf;
                 pristk[tos] = 0;
                 }
                 break;
               case Elem::STR:
                 if (tos>=stksize-1)
                     throw opp_runtime_error(eESTKOFLOW);
                 strstk[++tos] = opp_quotestr(e.s ? e.s : "");
                 pristk[tos] = 0;
                 break;
               case Elem::FUNCTOR:
                 {
                 int numargs = e.fu->numArgs();
                 int argpos = tos-numargs+1; // strstk[] index of 1st arg to pass
                 if (argpos<0)
                     throw opp_runtime_error(eESTKUFLOW);
                 strstk[argpos] = e.fu->toString(strstk+argpos, numargs);
                 tos = argpos;
                 break;
                 }
               case Elem::OP:
                 if (e.op==NEG || e.op==NOT || e.op==BIN_NOT)
                 {
                     if (tos<0)
                         throw opp_runtime_error(eESTKUFLOW);
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
                         throw opp_runtime_error(eESTKUFLOW);
                     strstk[tos-2] = strstk[tos-2] + " ? " + strstk[tos-1] + " : " + strstk[tos];
                     tos-=2;
                     pristk[tos] = 8;
                 }
                 else
                 {
                     // binary
                     if (tos<1)
                         throw opp_runtime_error(eESTKUFLOW);
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
                 throw opp_runtime_error(eBADEXP);
           }
        }
        if (tos!=0)
            throw opp_runtime_error(eBADEXP);
        return strstk[tos];
    }
    catch (std::exception& e)
    {
        std::string ret = std::string("[[ ") + e.what() + " ]]";
        return ret;
    }
}

void Expression::parse(const char *text)
{
    // throws exception if something goes wrong
    ::doParseExpression(text, elems, nelems);
}

bool Expression::isAConstant() const
{
    for (int i=0; i<nelems; i++)
    {
        switch(elems[i].type)
        {
            // literals and anything calculated from them are OK
            case Elem::BOOL:
            case Elem::DBL:
            case Elem::STR:
            case Elem::OP:
                continue; //OK
            default:
                return false;
        }
    }
    return true;
}


