//==========================================================================
//   CDYNAMICEXPRESSION.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cmath>
#include "common/stringutil.h"
#include "common/unitconversion.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cmodule.h"
#include "expryydefs.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {

cStringPool cDynamicExpression::Elem::stringPool("cDynamicExpression::Elem::stringPool");

void cDynamicExpression::Elem::copy(const Elem& other)
{
    memcpy(this, &other, sizeof(Elem));
    if (type == STR)
        s = stringPool.get(s);
    else if (type == DBL)
        d.unit = stringPool.get(d.unit);
    else if (type == FUNCTOR)
        fu = (Functor *)fu->dup();
    else if (type == CONSTSUBEXPR)
        constExpr = (cExpression *)constExpr->dup();
}

void cDynamicExpression::Elem::operator=(const Elem& other)
{
    if (this == &other)
        return;
    deleteOld();
    copy(other);
}

cDynamicExpression::Elem::~Elem()
{
    deleteOld();
}

void cDynamicExpression::Elem::deleteOld()
{
    if (type == STR)
        stringPool.release(s);
    else if (type == DBL)
        stringPool.release(d.unit);
    else if (type == FUNCTOR)
        delete fu;
    else if (type == CONSTSUBEXPR)
        delete constExpr;
}

void cDynamicExpression::Elem::set(cNEDFunction* f, int argc)
{
    deleteOld();
    type = NEDFUNC;
    ASSERT(f);
    nf.f = f;
    nf.argc = argc;
}

void cDynamicExpression::Elem::operator=(double _d)
{
    deleteOld();
    type = DBL;
    d.d = _d;
    d.unit = nullptr;
}

void cDynamicExpression::Elem::operator=(long _l)
{
    deleteOld();
    type = DBL;
    d.d = _l;
    d.unit = nullptr;
}

void cDynamicExpression::Elem::operator=(short _i)
{
    deleteOld();
    type = DBL;
    d.d = _i;
    d.unit = nullptr;
}

void cDynamicExpression::Elem::operator=(int _i)
{
    deleteOld();
    type = DBL;
    d.d = _i;
    d.unit = nullptr;
}

void cDynamicExpression::Elem::operator=(bool _b)
{
    deleteOld();
    type = BOOL;
    b = _b;
}

void cDynamicExpression::Elem::operator=(cNEDMathFunction* _f)
{
    deleteOld();
    type = MATHFUNC;
    ASSERT(_f);
    f = _f;
}

void cDynamicExpression::Elem::operator=(cExpression* _expr)
{
    deleteOld();
    type = CONSTSUBEXPR;
    constExpr = _expr;
}

void cDynamicExpression::Elem::operator=(OpType _op)
{
    deleteOld();
    type = OP;
    op = _op;
}

void cDynamicExpression::Elem::operator=(Functor* f)
{
    deleteOld();
    type = FUNCTOR;
    ASSERT(f);
    fu = f;
}

void cDynamicExpression::Elem::operator=(cXMLElement* _x)
{
    deleteOld();
    type = XML;
    x = _x;
}

void cDynamicExpression::Elem::operator=(const char* _s)
{
    deleteOld();
    type = STR;
    s = stringPool.get(_s);
}

int cDynamicExpression::Elem::compare(const Elem& other) const
{
    if (type != other.type)
        return type - other.type;

    switch (type) {
#define CMP(x)    (x == other.x ? 0 : x < other.x ? -1 : 1)
        case BOOL: return (int)other.b - (int)b;
        case DBL: return d.d == other.d.d ? omnetpp::opp_strcmp(d.unit, other.d.unit) : d.d < other.d.d ? -1 : 1;
        case STR: return CMP(s);
        case XML: return CMP(x);
        case MATHFUNC: return CMP(f);
        case NEDFUNC: return nf.argc == other.nf.argc ? CMP(nf.f) : (other.nf.argc-nf.argc);
        case FUNCTOR: return CMP(fu);
        case OP: return other.op - op;
        case CONSTSUBEXPR: return constExpr->compare(other.constExpr);
        default: throw cRuntimeError("Internal error: Bad Elem type");
#undef CMP
    }
}

//---

cDynamicExpression::cDynamicExpression()
{
    elems = nullptr;
    size = 0;
}

cDynamicExpression::~cDynamicExpression()
{
    delete[] elems;
}

void cDynamicExpression::copy(const cDynamicExpression& other)
{
    delete[] elems;
    size = other.size;
    elems = new Elem[size];
    for (int i = 0; i < size; i++)
        elems[i] = other.elems[i];
}

cDynamicExpression& cDynamicExpression::operator=(const cDynamicExpression& other)
{
    if (this == &other)
        return *this;
    cExpression::operator=(other);
    copy(other);
    return *this;
}

void cDynamicExpression::setExpression(Elem e[], int n)
{
    delete[] elems;
    elems = e;
    size = n;
}

void cDynamicExpression::parse(const char *text)
{
    // throws exception if something goes wrong
    ::doParseExpression(text, elems, size);
}

int cDynamicExpression::compare(const cExpression *other) const
{
    const cDynamicExpression *o = dynamic_cast<const cDynamicExpression *>(other);
    if (!o)
        return 1;  // for lack of a better option

    if (size != o->size)
        return o->size - size;

    for (int i = 0; i < size; i++) {
        int c = elems[i].compare(o->elems[i]);
        if (c != 0)
            return c;
    }
    return 0;
}

double cDynamicExpression::convertUnit(double d, const char *unit, const char *targetUnit)
{
    // not inline because simkernel header files cannot refer to common/ headers (unitconversion.h)
    return UnitConversion::convertUnit(d, unit, targetUnit);
}

bool cDynamicExpression::isAConstant() const
{
    for (int i = 0; i < size; i++) {
        switch (elems[i].type) {
            // literals and anything calculated from them are OK
            case Elem::BOOL:
            case Elem::DBL:
            case Elem::STR:
            case Elem::XML:
            case Elem::OP:
                continue;  // OK

            default:
                return false;
        }
    }
    return true;
}

bool cDynamicExpression::containsConstSubexpressions() const
{
    for (int i = 0; i < size; i++)
        if (elems[i].type == Elem::CONSTSUBEXPR)
            return true;
    return false;
}

void cDynamicExpression::evaluateConstSubexpressions(cComponent *context)
{
    throw cRuntimeError(this, "Const subexpressions not yet implemented");  // TODO implement
}

bool cDynamicExpression::boolValue(cComponent *context)
{
    cNEDValue v = evaluate(context);
    if (v.type != cNEDValue::BOOL)
        throw cRuntimeError(E_ECANTCAST, "bool");
    return v.bl;
}

long cDynamicExpression::longValue(cComponent *context, const char *expectedUnit)
{
    cNEDValue v = evaluate(context);
    if (v.type != cNEDValue::DBL)
        throw cRuntimeError(E_ECANTCAST, "long");
    return double_to_long(UnitConversion::convertUnit(v.dbl, v.dblunit, expectedUnit));
}

double cDynamicExpression::doubleValue(cComponent *context, const char *expectedUnit)
{
    cNEDValue v = evaluate(context);
    if (v.type != cNEDValue::DBL)
        throw cRuntimeError(E_ECANTCAST, "double");
    return UnitConversion::convertUnit(v.dbl, v.dblunit, expectedUnit);
}

std::string cDynamicExpression::stringValue(cComponent *context)
{
    cNEDValue v = evaluate(context);
    if (v.type != cNEDValue::STR)
        throw cRuntimeError(E_ECANTCAST, "string");
    return v.s;
}

cXMLElement *cDynamicExpression::xmlValue(cComponent *context)
{
    cNEDValue v = evaluate(context);
    if (v.type != cNEDValue::XML)
        throw cRuntimeError(E_ECANTCAST, "XML element");
    return v.xml;
}

#define ulong(x)    ((unsigned long)(x))

inline double trunc(double x)
{
    return x < 0.0 ? ceil(x) : floor(x);
}

static const int stksize = 20;

// we have a static stack to avoid new[] and call to cNEDValue ctor stksize times
static cNEDValue _stk[stksize];
static bool _stkinuse = false;

cNEDValue cDynamicExpression::evaluate(cComponent *context) const
{
    // use static _stk[] if possible, or allocate another one if that's in use.
    // Note: this will be reentrant but NOT thread safe
    cNEDValue *stk;
    if (_stkinuse)
        stk = new cNEDValue[stksize];
    else {
        _stkinuse = true;
        stk = _stk;
    }
    struct Finally
    {
        cNEDValue *stk;
        Finally(cNEDValue *stk) { this->stk = stk; }
        ~Finally() { if (stk == _stk) _stkinuse = false; else delete[] stk; }
    } f(stk);

    // printf("    evaluating: %s\n", str().c_str()); //XXX
    int tos = -1;
    for (int i = 0; i < size; i++) {
        Elem& e = elems[i];
        switch (e.type) {
            case Elem::BOOL:
                if (tos >= stksize-1)
                    throw cRuntimeError(E_ESTKOFLOW);
                stk[++tos] = e.b;
                break;

            case Elem::DBL:
                if (tos >= stksize-1)
                    throw cRuntimeError(E_ESTKOFLOW);
                stk[++tos].set(e.d.d, e.d.unit);
                break;

            case Elem::STR:
                if (tos >= stksize-1)
                    throw cRuntimeError(E_ESTKOFLOW);
                stk[++tos] = e.s;
                break;

            case Elem::XML:
                if (tos >= stksize-1)
                    throw cRuntimeError(E_ESTKOFLOW);
                stk[++tos] = e.x;
                break;

            case Elem::MATHFUNC:
                switch (e.f->getNumArgs()) {
                    case 0:
                        stk[++tos] = e.f->getMathFuncNoArg()();
                        break;

                    case 1:
                        if (tos < 0)
                            throw cRuntimeError(E_ESTKUFLOW);
                        if (stk[tos].type != cNEDValue::DBL)
                            throw cRuntimeError(E_EBADARGS, e.f->getName());
                        if (!opp_isempty(stk[tos].dblunit))
                            throw cRuntimeError(E_DIMLESS, e.f->getName());
                        stk[tos] = e.f->getMathFunc1Arg()(stk[tos].dbl);
                        break;

                    case 2:
                        if (tos < 1)
                            throw cRuntimeError(E_ESTKUFLOW);
                        if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL)
                            throw cRuntimeError(E_EBADARGS, e.f->getName());
                        if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                            throw cRuntimeError(E_DIMLESS, e.f->getName());
                        stk[tos-1] = e.f->getMathFunc2Args()(stk[tos-1].dbl, stk[tos].dbl);
                        tos--;
                        break;

                    case 3:
                        if (tos < 2)
                            throw cRuntimeError(E_ESTKUFLOW);
                        if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL || stk[tos-2].type != cNEDValue::DBL)
                            throw cRuntimeError(E_EBADARGS, e.f->getName());
                        if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit) || !opp_isempty(stk[tos-2].dblunit))
                            throw cRuntimeError(E_DIMLESS, e.f->getName());
                        stk[tos-2] = e.f->getMathFunc3Args()(stk[tos-2].dbl, stk[tos-1].dbl, stk[tos].dbl);
                        tos -= 2;
                        break;

                    case 4:
                        if (tos < 3)
                            throw cRuntimeError(E_ESTKUFLOW);
                        if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL || stk[tos-2].type != cNEDValue::DBL || stk[tos-3].type != cNEDValue::DBL)
                            throw cRuntimeError(E_EBADARGS, e.f->getName());
                        if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit) || !opp_isempty(stk[tos-2].dblunit) || !opp_isempty(stk[tos-3].dblunit))
                            throw cRuntimeError(E_DIMLESS, e.f->getName());
                        stk[tos-3] = e.f->getMathFunc4Args()(stk[tos-3].dbl, stk[tos-2].dbl, stk[tos-1].dbl, stk[tos].dbl);
                        tos -= 3;
                        break;

                    default:
                        throw cRuntimeError(E_BADEXP);
                }
                break;

            case Elem::NEDFUNC: {
                int numArgs = e.nf.argc;
                int argPos = tos-numArgs+1;  // stk[] index of 1st arg to pass
                if (argPos < 0)
                    throw cRuntimeError(E_ESTKUFLOW);
                // note: args checking is left to the function itself
                stk[argPos] = e.nf.f->invoke(context, stk+argPos, numArgs);
                tos = argPos;
                break;
            }

            case Elem::FUNCTOR: {
                int numArgs = e.fu->getNumArgs();
                int argPos = tos-numArgs+1;  // stk[] index of 1st arg to pass
                if (argPos < 0)
                    throw cRuntimeError(E_ESTKUFLOW);
                const char *argtypes = e.fu->getArgTypes();
                for (int i = 0; i < numArgs; i++)
                    if (argtypes[i] != '*' && stk[argPos+i].type != (argtypes[i] == 'L' ? 'D' : argtypes[i]))
                        throw cRuntimeError(E_EBADARGS, e.fu->getFullName());

                // note: unit checking is left to the function itself
                stk[argPos] = e.fu->evaluate(context, stk+argPos, numArgs);
                tos = argPos;
                break;
            }

            case Elem::CONSTSUBEXPR:
                // this should not occur
                throw cRuntimeError("evaluate: Constant subexpressions must have already been evaluated");

            case Elem::OP:
                if (e.op == NEG || e.op == NOT || e.op == BIN_NOT) {
                    // unary
                    if (tos < 0)
                        throw cRuntimeError(E_ESTKUFLOW);
                    switch (e.op) {
                        case NEG:
                            if (stk[tos].type != cNEDValue::DBL)
                                throw cRuntimeError(E_EBADARGS, "-");
                            stk[tos].dbl = -stk[tos].dbl;
                            break;

                        case NOT:
                            if (stk[tos].type != cNEDValue::BOOL)
                                throw cRuntimeError(E_EBADARGS, "!");
                            stk[tos].bl = !stk[tos].bl;
                            break;

                        case BIN_NOT:
                            if (stk[tos].type != cNEDValue::DBL)
                                throw cRuntimeError(E_EBADARGS, "~");
                            if (!opp_isempty(stk[tos].dblunit))
                                throw cRuntimeError(E_DIMLESS, "~");
                            stk[tos].dbl = ~ulong(stk[tos].dbl);
                            break;

                        default:
                            ASSERT(false);
                    }
                }
                else if (e.op == IIF) {
                    // tertiary
                    if (tos < 2)
                        throw cRuntimeError(E_ESTKUFLOW);
                    // 1st arg must be bool, others 2nd and 3rd can be anything
                    if (stk[tos-2].type != cNEDValue::BOOL)
                        throw cRuntimeError(E_EBADARGS, "?:");
                    stk[tos-2] = (stk[tos-2].bl ? stk[tos-1] : stk[tos]);
                    tos -= 2;
                }
                else {
                    // binary
                    if (tos < 1)
                        throw cRuntimeError(E_ESTKUFLOW);
                    switch (e.op) {
                        case ADD:
                            // double addition or string concatenation
                            if (stk[tos-1].type == cNEDValue::DBL && stk[tos].type == cNEDValue::DBL) {
                                stk[tos].dbl = UnitConversion::convertUnit(stk[tos].dbl, stk[tos].dblunit, stk[tos-1].dblunit);
                                stk[tos-1].dbl = stk[tos-1].dbl + stk[tos].dbl;
                            }
                            else if (stk[tos-1].type == cNEDValue::STR && stk[tos].type == cNEDValue::STR)
                                stk[tos-1].s = stk[tos-1].s + stk[tos].s;
                            else
                                throw cRuntimeError(E_EBADARGS, "+");
                            tos--;
                            break;

                        case SUB:
                            if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL)
                                throw cRuntimeError(E_EBADARGS, "-");
                            stk[tos].dbl = UnitConversion::convertUnit(stk[tos].dbl, stk[tos].dblunit, stk[tos-1].dblunit);
                            stk[tos-1].dbl = stk[tos-1].dbl - stk[tos].dbl;
                            tos--;
                            break;

                        case MUL:
                            if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL)
                                throw cRuntimeError(E_EBADARGS, "*");
                            if (!opp_isempty(stk[tos].dblunit) && !opp_isempty(stk[tos-1].dblunit))
                                throw cRuntimeError("Multiplying two quantities with units is not supported");
                            stk[tos-1].dbl = stk[tos-1].dbl * stk[tos].dbl;
                            if (opp_isempty(stk[tos-1].dblunit))
                                stk[tos-1].dblunit = stk[tos].dblunit;
                            tos--;
                            break;

                        case DIV:
                            if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL)
                                throw cRuntimeError(E_EBADARGS, "/");
                            // for now we only support num/num, unit/num, and unit/unit if the two units are convertible
                            if (!opp_isempty(stk[tos].dblunit))
                                stk[tos].dbl = UnitConversion::convertUnit(stk[tos].dbl, stk[tos].dblunit, stk[tos-1].dblunit);
                            stk[tos-1].dbl = stk[tos-1].dbl / stk[tos].dbl;
                            if (!opp_isempty(stk[tos].dblunit))
                                stk[tos-1].dblunit = nullptr;
                            tos--;
                            break;

                        case MOD:
                            if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL)
                                throw cRuntimeError(E_EBADARGS, "%");
                            if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                                throw cRuntimeError(E_DIMLESS, "%");
                            stk[tos-1].dbl = fmod(trunc(stk[tos-1].dbl), trunc(stk[tos].dbl));
                            tos--;
                            break;

                        case POW:
                            if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL)
                                throw cRuntimeError(E_EBADARGS, "^");
                            if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                                throw cRuntimeError(E_DIMLESS, "^");
                            stk[tos-1].dbl = pow(stk[tos-1].dbl, stk[tos].dbl);
                            tos--;
                            break;

                        case AND:
                            if (stk[tos].type != cNEDValue::BOOL || stk[tos-1].type != cNEDValue::BOOL)
                                throw cRuntimeError(E_EBADARGS, "&&");
                            stk[tos-1].bl = stk[tos-1].bl && stk[tos].bl;
                            tos--;
                            break;

                        case OR:
                            if (stk[tos].type != cNEDValue::BOOL || stk[tos-1].type != cNEDValue::BOOL)
                                throw cRuntimeError(E_EBADARGS, "||");
                            stk[tos-1].bl = stk[tos-1].bl || stk[tos].bl;
                            tos--;
                            break;

                        case XOR:
                            if (stk[tos].type != cNEDValue::BOOL || stk[tos-1].type != cNEDValue::BOOL)
                                throw cRuntimeError(E_EBADARGS, "##");
                            stk[tos-1].bl = stk[tos-1].bl != stk[tos].bl;
                            tos--;
                            break;

                        case BIN_AND:
                            if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL)
                                throw cRuntimeError(E_EBADARGS, "&");
                            if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                                throw cRuntimeError(E_DIMLESS, "&");
                            stk[tos-1].dbl = (double)(ulong(stk[tos-1].dbl) & ulong(stk[tos].dbl));
                            tos--;
                            break;

                        case BIN_OR:
                            if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL)
                                throw cRuntimeError(E_EBADARGS, "|");
                            if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                                throw cRuntimeError(E_DIMLESS, "|");
                            stk[tos-1].dbl = (double)(ulong(stk[tos-1].dbl) | ulong(stk[tos].dbl));
                            tos--;
                            break;

                        case BIN_XOR:
                            if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL)
                                throw cRuntimeError(E_EBADARGS, "#");
                            if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                                throw cRuntimeError(E_DIMLESS, "#");
                            stk[tos-1].dbl = (double)(ulong(stk[tos-1].dbl) ^ ulong(stk[tos].dbl));
                            tos--;
                            break;

                        case LSHIFT:
                            if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL)
                                throw cRuntimeError(E_EBADARGS, "<<");
                            if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                                throw cRuntimeError(E_DIMLESS, "<<");
                            stk[tos-1].dbl = (double)(ulong(stk[tos-1].dbl) << ulong(stk[tos].dbl));
                            tos--;
                            break;

                        case RSHIFT:
                            if (stk[tos].type != cNEDValue::DBL || stk[tos-1].type != cNEDValue::DBL)
                                throw cRuntimeError(E_EBADARGS, ">>");
                            if (!opp_isempty(stk[tos].dblunit) || !opp_isempty(stk[tos-1].dblunit))
                                throw cRuntimeError(E_DIMLESS, ">>");
                            stk[tos-1].dbl = (double)(ulong(stk[tos-1].dbl) >> ulong(stk[tos].dbl));
                            tos--;
                            break;

#define COMPARISON(RELATION) \
                                 if (stk[tos-1].type==cNEDValue::DBL && stk[tos].type==cNEDValue::DBL) { \
                                     stk[tos].dbl = UnitConversion::convertUnit(stk[tos].dbl, stk[tos].dblunit, stk[tos-1].dblunit); \
                                     stk[tos-1] = (stk[tos-1].dbl RELATION stk[tos].dbl); \
                                 } else if (stk[tos-1].type==cNEDValue::STR && stk[tos].type==cNEDValue::STR) \
                                     stk[tos-1] = (stk[tos-1].s RELATION stk[tos].s); \
                                 else if (stk[tos-1].type==cNEDValue::BOOL && stk[tos].type==cNEDValue::BOOL) \
                                     stk[tos-1] = (stk[tos-1].bl RELATION stk[tos].bl); \
                                 else \
                                     throw cRuntimeError(E_EBADARGS,#RELATION); \
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
                            throw cRuntimeError(E_BADEXP);
                    }
                }
                break;

            default:
                throw cRuntimeError(E_BADEXP);
        }
    }
    if (tos != 0)
        throw cRuntimeError(E_BADEXP);

    // printf("        ==> returning %s\n", stk[tos].str().c_str()); //XXX

    return stk[tos];
}

std::string cDynamicExpression::str() const
{
    // We perform the same algorithm as during evaluation (i.e. stack machine),
    // only instead of actual calculations we store the result as string.
    // We need to keep track of operator precendences to be able to add parens where needed.

    try {
        const int stksize = 20;
        std::string strstk[stksize];
        int pristk[stksize];

        int tos = -1;
        for (int i = 0; i < size; i++) {
            Elem& e = elems[i];
            switch (e.type) {
                case Elem::BOOL:
                    if (tos >= stksize-1)
                        throw cRuntimeError(E_ESTKOFLOW);
                    strstk[++tos] = (e.b ? "true" : "false");
                    pristk[tos] = 0;
                    break;

                case Elem::DBL: {
                    if (tos >= stksize-1)
                        throw cRuntimeError(E_ESTKOFLOW);
                    char buf[32];
                    sprintf(buf, "%g%s", e.d.d, opp_nulltoempty(e.d.unit));
                    strstk[++tos] = buf;
                    pristk[tos] = 0;
                    break;
                }

                case Elem::STR:
                    if (tos >= stksize-1)
                        throw cRuntimeError(E_ESTKOFLOW);
                    strstk[++tos] = opp_quotestr(e.s ? e.s : "");
                    pristk[tos] = 0;
                    break;

                case Elem::XML:
                    if (tos >= stksize-1)
                        throw cRuntimeError(E_ESTKOFLOW);
                    strstk[++tos] = std::string("<") + (e.x ? e.x->getTagName() : "null") +">";  // FIXME plus location info?
                    pristk[tos] = 0;
                    break;

                case Elem::MATHFUNC:
                case Elem::NEDFUNC: {
                    int numargs = (e.type == Elem::MATHFUNC) ? e.f->getNumArgs() : e.nf.argc;
                    std::string name = (e.type == Elem::MATHFUNC) ? e.f->getName() : e.nf.f->getName();
                    int argpos = tos-numargs+1;  // strstk[] index of 1st arg to pass
                    if (argpos < 0)
                        throw cRuntimeError(E_ESTKUFLOW);
                    std::string tmp = name+"(";
                    for (int i = 0; i < numargs; i++)
                        tmp += (i == 0 ? "" : ", ") + strstk[argpos+i];
                    tmp += ")";
                    strstk[argpos] = tmp;
                    tos = argpos;
                    pristk[tos] = 0;
                    break;
                }

                case Elem::FUNCTOR: {
                    int numargs = e.fu->getNumArgs();
                    int argpos = tos-numargs+1;  // strstk[] index of 1st arg to pass
                    if (argpos < 0)
                        throw cRuntimeError(E_ESTKUFLOW);
                    strstk[argpos] = e.fu->str(strstk+argpos, numargs);
                    tos = argpos;
                    break;
                }

                case Elem::CONSTSUBEXPR:
                    strstk[++tos] = std::string("const(")+e.constExpr->str()+")";
                    break;

                case Elem::OP:
                    if (e.op == NEG || e.op == NOT || e.op == BIN_NOT) {
                        if (tos < 0)
                            throw cRuntimeError(E_ESTKUFLOW);
                        const char *op;
                        switch (e.op) {
                            case NEG: op = " -"; break;
                            case NOT: op = " !"; break;
                            case BIN_NOT: op = " ~"; break;
                            default: op = " ???";
                        }
                        strstk[tos] = std::string(op) + strstk[tos];  // pri=0: never needs parens
                        pristk[tos] = 0;
                    }
                    else if (e.op == IIF) {  // conditional (tertiary)
                        if (tos < 2)
                            throw cRuntimeError(E_ESTKUFLOW);
                        strstk[tos-2] = strstk[tos-2] + " ? " + strstk[tos-1] + " : " + strstk[tos];
                        tos -= 2;
                        pristk[tos] = 8;
                    }
                    else {
                        // binary
                        if (tos < 1)
                            throw cRuntimeError(E_ESTKUFLOW);
                        int pri;
                        const char *op;
                        switch (e.op) {
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
                            case ADD: op = " + "; pri = 3; break;
                            case SUB: op = " - "; pri = 3; break;
                            case MUL: op = " * "; pri = 2; break;
                            case DIV: op = " / "; pri = 2; break;
                            case MOD: op = " % "; pri = 2; break;
                            case POW: op = " ^ "; pri = 1; break;
                            case EQ: op = " == "; pri = 6; break;
                            case NE: op = " != "; pri = 6; break;
                            case LT: op = " < "; pri = 6; break;
                            case GT: op = " > "; pri = 6; break;
                            case LE: op = " <= "; pri = 6; break;
                            case GE: op = " >= "; pri = 6; break;
                            case AND: op = " && "; pri = 7; break;
                            case OR: op = " || "; pri = 7; break;
                            case XOR: op = " ## "; pri = 7; break;
                            case BIN_AND: op = " & "; pri = 5; break;
                            case BIN_OR: op = " | "; pri = 5; break;
                            case BIN_XOR: op = " # "; pri = 5; break;
                            case LSHIFT: op = " << "; pri = 4; break;
                            case RSHIFT: op = " >> "; pri = 4; break;
                            default: op = " ??? "; pri = 10; break;
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
                    throw cRuntimeError(E_BADEXP);
            }
        }
        if (tos != 0)
            throw cRuntimeError(E_BADEXP);
        return strstk[tos];
    }
    catch (std::exception& e) {
        std::string ret = std::string("[[ ") + e.what() + " ]]";
        return ret;
    }
}

}  // namespace omnetpp

