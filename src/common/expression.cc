//==========================================================================
//  EXPRESSION.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <sstream>
#include "expression.h"
#include "expressionyydefs.h"
#include "unitconversion.h"

NAMESPACE_BEGIN


#define eESTKOFLOW   "Stack overflow"
#define eESTKUFLOW   "Stack underflow"
#define eEBADARGS    "Wrong arguments for '%s'"
#define eBADEXP      "Malformed expression"
#define eECANTCAST   "Cannot cast to %s"
#define eDIMLESS     "Error in expression: `%s': argument(s) must be dimensionless"

CommonStringPool Expression::Elem::stringPool/*("Expression::Elem::stringPool")*/;

// should be member of Elem, but the VC++ 9.0 linker disagrees
void Expression::Elem_eq(Elem& e, const Elem& other)
{
    e.deleteOld();

    memcpy(&e, &other, sizeof(Elem));

    if (e.type==Elem::STR)
        e.s = opp_strdup(e.s);
    else if (e.type==Elem::FUNCTOR)
        e.fu = e.fu->dup();
}

// should be member of Elem, but the VC++ 9.0 linker disagrees
int Expression::Elem_getNumArgs(const Elem& e)
{
    switch(e.type)
    {
        case Elem::UNDEF: case Elem::BOOL: case Elem::DBL: case Elem::STR:
            return 0;
        case Elem::FUNCTOR:
            return e.getFunctor()->getNumArgs();
            break;
        case Elem::OP:
            switch(e.getOp()) {
                case NEG: case NOT: case BIN_NOT: return 1;
                case IIF: return 3;
                default: return 2;
            }
        default: Assert(false); return 0;
    }
}

//std::string Expression::Elem::str() const
std::string Expression::Elem_str(int type, bool b, double d, const char *s, Functor *fu, int op)
{
    std::stringstream os;
    switch(type)
    {
        case Elem::UNDEF: os << "<undefined>"; break;
        case Elem::BOOL: os << (b ? "true" : "false"); break;
        case Elem::DBL: os << d; break;
        case Elem::STR: os << "\"" << s << "\""; break;
        case Elem::FUNCTOR: os << fu->getName() << "(argc=" << fu->getNumArgs() << ")"; break;
        case Elem::OP: os << "op" << op; break;
        default: Assert(false);
    }
    return os.str();
}

std::string Expression::Value::str()
{
    char buf[32];
    switch (type)
    {
      case BOOL: return bl ? "true" : "false";
      case DBL:  sprintf(buf, "%g%s", dbl, opp_nulltoempty(dblunit)); return buf;
      case STR:  return opp_quotestr(s.c_str());
      default:   throw opp_runtime_error("internal error: bad Value type");
    }
}

// should be member of Function, but the VC++ 9.0 linker disagrees
std::string Expression::Function_str(const char *name, std::string args[], int numargs)
{
    std::stringstream os;
    os << name << "(";
    for (int i=0; i<numargs; i++)
        os << (i==0 ? "" : ", ") << args[i];
    os << ")";
    return os.str();
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

void Expression::copy(const Expression& other)
{
    delete [] elems;
    nelems = other.nelems;
    elems = new Elem[nelems];
    for (int i=0; i<nelems; i++)
        elems[i] = other.elems[i];
}

Expression& Expression::operator=(const Expression& other)
{
    if (this==&other) return *this;
    copy(other);
    return *this;
}

void Expression::setExpression(Elem e[], int n)
{
    delete [] elems;
    elems = e;
    nelems = n;
}

#define ulong(x) ((unsigned long)(x))

Expression::Value Expression::evaluate() const
{
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
                 throw opp_runtime_error(eESTKOFLOW);
             stk[++tos] = e.b;
             break;

           case Elem::DBL:
             if (tos>=stksize-1)
                 throw opp_runtime_error(eESTKOFLOW);
             stk[++tos].set(e.d.d, e.d.unit);
             break;

           case Elem::STR:
             if (tos>=stksize-1)
                 throw opp_runtime_error(eESTKOFLOW);
             stk[++tos] = e.s;
             break;

           case Elem::FUNCTOR:
             {
             int numargs = e.fu->getNumArgs();
             int argpos = tos-numargs+1; // stk[] index of 1st arg to pass
             if (argpos<0)
                 throw opp_runtime_error(eESTKUFLOW);
             const char *argtypes = e.fu->getArgTypes();
             for (int i=0; i<numargs; i++)
                 if (stk[argpos+i].type != (argtypes[i]=='L' ? 'D' : argtypes[i]))
                     throw opp_runtime_error(eEBADARGS,e.fu->getName());
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
                         if (stk[tos].type!=Value::DBL)
                             throw opp_runtime_error(eEBADARGS,"-");
                         stk[tos].dbl = -stk[tos].dbl;
                         break;
                     case NOT:
                         if (stk[tos].type!=Value::BOOL)
                             throw opp_runtime_error(eEBADARGS,"!");
                         stk[tos].bl = !stk[tos].bl;
                         break;
                     case BIN_NOT:
                         if (stk[tos].type!=Value::DBL)
                             throw opp_runtime_error(eEBADARGS,"~");
                         if (!opp_isempty(stk[tos].dblunit))
                             throw opp_runtime_error(eDIMLESS,"~");
                         stk[tos].dbl = ~ulong(stk[tos].dbl);
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
                 if (stk[tos-2].type!=Value::BOOL)
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
                       if (stk[tos-1].type==Value::DBL && stk[tos].type==Value::DBL) {
                           stk[tos].dbl = UnitConversion::convertUnit(stk[tos].dbl, stk[tos].dblunit, stk[tos-1].dblunit);
                           stk[tos-1].dbl = stk[tos-1].dbl + stk[tos].dbl;
                       }
                       else if (stk[tos-1].type==Value::STR && stk[tos].type==Value::STR)
                           stk[tos-1].s = stk[tos-1].s + stk[tos].s;
                       else
                           throw opp_runtime_error(eEBADARGS,"+");
                       tos--;
                       break;
                   case SUB:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw opp_runtime_error(eEBADARGS,"-");
                       stk[tos].dbl = UnitConversion::convertUnit(stk[tos].dbl, stk[tos].dblunit, stk[tos-1].dblunit);
                       stk[tos-1].dbl = stk[tos-1].dbl - stk[tos].dbl;
                       tos--;
                       break;
                   case MUL:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw opp_runtime_error(eEBADARGS,"*");
                       if (!opp_isempty(stk[tos].dblunit) && !opp_isempty(stk[tos-1].dblunit))
                           throw opp_runtime_error("Multiplying two quantities with units is not supported");
                       stk[tos-1].dbl = stk[tos-1].dbl * stk[tos].dbl;
                       if (opp_isempty(stk[tos-1].dblunit))
                           stk[tos-1].dblunit = stk[tos].dblunit;
                       tos--;
                       break;
                   case DIV:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw opp_runtime_error(eEBADARGS,"/");
                       // for now we only support num/num, unit/num, and unit/unit if the two units are convertible
                       if (!opp_isempty(stk[tos].dblunit))
                           stk[tos].dbl = UnitConversion::convertUnit(stk[tos].dbl, stk[tos].dblunit, stk[tos-1].dblunit);
                       stk[tos-1].dbl = stk[tos-1].dbl / stk[tos].dbl;
                       if (!opp_isempty(stk[tos].dblunit))
                           stk[tos-1].dblunit = NULL;
                       tos--;
                       break;
                   case MOD:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw opp_runtime_error(eEBADARGS,"%");
                       if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                           throw opp_runtime_error(eDIMLESS,"%");
                       stk[tos-1].dbl = (double)(ulong(stk[tos-1].dbl) % ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case POW:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw opp_runtime_error(eEBADARGS,"^");
                       if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                           throw opp_runtime_error(eDIMLESS,"^");
                       stk[tos-1].dbl = pow(stk[tos-1].dbl, stk[tos].dbl);
                       tos--;
                       break;
                   case AND:
                       if (stk[tos].type!=Value::BOOL || stk[tos-1].type!=Value::BOOL)
                           throw opp_runtime_error(eEBADARGS,"&&");
                       stk[tos-1] = stk[tos-1].bl && stk[tos].bl;
                       tos--;
                       break;
                   case OR:
                       if (stk[tos].type!=Value::BOOL || stk[tos-1].type!=Value::BOOL)
                           throw opp_runtime_error(eEBADARGS,"||");
                       stk[tos-1] = stk[tos-1].bl || stk[tos].bl;
                       tos--;
                       break;
                   case XOR:
                       if (stk[tos].type!=Value::BOOL || stk[tos-1].type!=Value::BOOL)
                           throw opp_runtime_error(eEBADARGS,"##");
                       stk[tos-1] = stk[tos-1].bl != stk[tos].bl;
                       tos--;
                       break;
                   case BIN_AND:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw opp_runtime_error(eEBADARGS,"&");
                       if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                           throw opp_runtime_error(eDIMLESS,"&");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) & ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case BIN_OR:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw opp_runtime_error(eEBADARGS,"|");
                       if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                           throw opp_runtime_error(eDIMLESS,"|");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) | ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case BIN_XOR:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw opp_runtime_error(eEBADARGS,"#");
                       if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                           throw opp_runtime_error(eDIMLESS,"#");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) ^ ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case LSHIFT:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw opp_runtime_error(eEBADARGS,"<<");
                       if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                           throw opp_runtime_error(eDIMLESS,"<<");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) << ulong(stk[tos].dbl));
                       tos--;
                       break;
                   case RSHIFT:
                       if (stk[tos].type!=Value::DBL || stk[tos-1].type!=Value::DBL)
                           throw opp_runtime_error(eEBADARGS,">>");
                       if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                           throw opp_runtime_error(eDIMLESS,">>");
                       stk[tos-1] = (double)(ulong(stk[tos-1].dbl) >> ulong(stk[tos].dbl));
                       tos--;
                       break;
#define COMPARISON(RELATION) \
                                 if (stk[tos-1].type==Value::DBL && stk[tos].type==Value::DBL) { \
                                     stk[tos].dbl = UnitConversion::convertUnit(stk[tos].dbl, stk[tos].dblunit, stk[tos-1].dblunit); \
                                     stk[tos-1] = (stk[tos-1].dbl RELATION stk[tos].dbl); \
                                 } else if (stk[tos-1].type==Value::STR && stk[tos].type==Value::STR) \
                                     stk[tos-1] = (stk[tos-1].s RELATION stk[tos].s); \
                                 else if (stk[tos-1].type==Value::BOOL && stk[tos].type==Value::BOOL) \
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

    return stk[tos];
}

bool Expression::boolValue()
{
    Value v = evaluate();
    if (v.type!=Value::BOOL)
        throw opp_runtime_error(eECANTCAST,"bool");
    return v.bl;
}

long Expression::longValue(const char *expectedUnit)
{
    Value v = evaluate();
    if (v.type!=Value::DBL)
        throw opp_runtime_error(eECANTCAST,"long");
    return (long)(UnitConversion::convertUnit(v.dbl, v.dblunit, expectedUnit));
}

double Expression::doubleValue(const char *expectedUnit)
{
    Value v = evaluate();
    if (v.type!=Value::DBL)
        throw opp_runtime_error(eECANTCAST,"double");
    return UnitConversion::convertUnit(v.dbl, v.dblunit, expectedUnit);
}

std::string Expression::stringValue()
{
    Value v = evaluate();
    if (v.type!=Value::STR)
        throw opp_runtime_error(eECANTCAST,"string");
    return v.s;
}

std::string Expression::str() const
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
                 sprintf(buf, "%g%s", e.d.d, opp_nulltoempty(e.d.unit));
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
                 int numargs = e.fu->getNumArgs();
                 int argpos = tos-numargs+1; // strstk[] index of 1st arg to pass
                 if (argpos<0)
                     throw opp_runtime_error(eESTKUFLOW);
                 strstk[argpos] = e.fu->str(strstk+argpos, numargs);
                 tos = argpos;
                 pristk[tos] = 0;
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

void Expression::parse(const char *text, Resolver *resolver)
{
    // fills elems[]; throws exception if something goes wrong
    ::doParseExpression(text, resolver, elems, nelems);
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

//----------------------

#define F0(name)   {#name, (double(*)(...)) (double(*)())name, 0}
#define F1(name)   {#name, (double(*)(...)) (double(*)(double))name, 1}
#define F2(name)   {#name, (double(*)(...)) (double(*)(double,double))name, 2}
#define F3(name)   {#name, (double(*)(...)) (double(*)(double,double,double))name, 3}

MathFunction::FuncDesc MathFunction::functable[] = {
    F1(acos),
    F1(asin),
    F1(atan),
    F2(atan2),
    F1(sin),
    F1(cos),
    F1(tan),
    F1(ceil),
    F1(floor),
    F1(exp),
    F2(pow),
    F1(sqrt),
    F1(fabs),
    F2(fmod),
    F2(hypot),
    F1(log),
    F1(log10),
    {NULL, NULL, 0}
};

MathFunction::MathFunction(const char *name)
{
    funcname = name;
    FuncDesc *fd = lookup(funcname.c_str());
    if (!fd)
        throw opp_runtime_error("unrecognized function %s", name);
    f = fd->f;
    argcount = fd->argcount;
}

MathFunction::~MathFunction()
{
}

Expression::Functor *MathFunction::dup() const
{
    return new MathFunction(getName());
}

const char *MathFunction::getName() const
{
     return funcname.c_str();
}

MathFunction::FuncDesc *MathFunction::lookup(const char *name)
{
    for (FuncDesc *f = functable; f->name!=NULL; f++)
        if (strcmp(f->name, name)==0)
            return f;
    return NULL;
}

bool MathFunction::supports(const char *name)
{
    return lookup(name)!=NULL;
}

const char *MathFunction::getArgTypes() const
{
    Assert(Expression::Value::DBL == 'D');
    FuncDesc *fd = lookup(funcname.c_str());
    int n = fd==NULL ? 0 : fd->argcount;
    const char *ddd = "DDDDDDDDDDDDDDDDDD";
    return ddd+strlen(ddd)-n;
}

char MathFunction::getReturnType() const
{
    return Expression::Value::DBL;
}

Expression::Value MathFunction::evaluate(Expression::Value args[], int numargs)
{
    Assert(numargs==argcount);
    switch (numargs)
    {
        case 0: return f();
        case 1: return f(args[0].dbl);
        case 2: return f(args[0].dbl, args[1].dbl);
        case 3: return f(args[0].dbl, args[1].dbl, args[2].dbl);
        default: throw opp_runtime_error("too many args");
    }
}

NAMESPACE_END

