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
#include <limits>
#include <cinttypes>
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

void cDynamicExpression::Elem::set(cNedFunction* f, int argc)
{
    deleteOld();
    type = NEDFUNC;
    ASSERT(f);
    nf.f = f;
    nf.argc = argc;
}

void cDynamicExpression::Elem::operator=(bool _b)
{
    deleteOld();
    type = BOOL;
    b = _b;
}

void cDynamicExpression::Elem::operator=(intpar_t _i)
{
    deleteOld();
    type = INT;
    i.i = _i;
    i.unit = nullptr;
}

void cDynamicExpression::Elem::operator=(double _d)
{
    deleteOld();
    type = DBL;
    d.d = _d;
    d.unit = nullptr;
}

void cDynamicExpression::Elem::operator=(cNedMathFunction* _f)
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

void cDynamicExpression::Elem::setUnit(const char *s)
{
    switch(type) {
    case INT: i.unit = stringPool.get(s); break;
    case DBL: d.unit = stringPool.get(s); break;
    default: throw cRuntimeError("Cannot set unit for this Elem type");
    }
}

void cDynamicExpression::Elem::negate()
{
    switch(type) {
    case INT: i.i = -i.i; break;
    case DBL: d.d = -d.d; break;
    default: throw cRuntimeError("Cannot negate this Elem type");
    }
}

int cDynamicExpression::Elem::compare(const Elem& other) const
{
    if (type != other.type)
        return type - other.type;

    switch (type) {
#define CMP(x)    (x == other.x ? 0 : x < other.x ? -1 : 1)
        case BOOL: return (int)other.b - (int)b;
        case INT: return i.i == other.i.i ? omnetpp::opp_strcmp(i.unit, other.i.unit) : i.i < other.i.i ? -1 : 1;
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

const char *cDynamicExpression::Elem::getOpName(OpType op)
{
    switch (op) {
        case ADD: return "+";
        case SUB: return "-";
        case MUL: return "*";
        case DIV: return "/";
        case MOD: return "%";
        case POW: return "^";
        case NEG: return "-";
        case EQ: return "==";
        case NE: return "!=";
        case LT: return "<";
        case GT: return ">";
        case LE: return "<=";
        case GE: return ">=";
        case IIF: return "?:";
        case AND: return "&&";
        case OR: return "||";
        case XOR: return "##";
        case NOT: return "!";
        case BIN_AND: return "&";
        case BIN_OR: return "|";
        case BIN_XOR: return "#";
        case BIN_NOT: return "~";
        case LSHIFT: return "<<";
        case RSHIFT: return ">>";
        default: return "<unknown operator>";
    }

}

std::string cDynamicExpression::Elem::str() const
{
    char buf[32];
    switch (type) {
        case BOOL: return b ? "true" : "false";
        case INT: sprintf(buf, "%" PRId64 "%s", (int64_t) i.i, opp_nulltoempty(i.unit)); return buf;
        case DBL: sprintf(buf, "%g%s", d.d, opp_nulltoempty(d.unit)); return buf;
        case STR: return opp_quotestr(s ? s : "");;
        case XML: return x==nullptr ? "<nullptr>" : std::string("<") + x->getTagName() +">@" + x->getSourceLocation();
        case MATHFUNC: return std::string(f->getName()) + "()";
        case NEDFUNC: return std::string(nf.f->getName()) + "()";
        case FUNCTOR: return "";
        case CONSTSUBEXPR: return "const()";
        case OP: return std::string("operator \"") + getOpName(op) + "\"";
        default: return "<unknown expression element>";
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
            case Elem::INT:
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

void cDynamicExpression::evaluateConstSubexpressions(Context *context)
{
    throw cRuntimeError(this, "Const subexpressions not yet implemented");  // TODO implement
}

bool cDynamicExpression::boolValue(Context *context) const
{
    cNedValue v = evaluate(context);
    return v.boolValue();
}

intpar_t cDynamicExpression::intValue(Context *context, const char *expectedUnit) const
{
    cNedValue v = evaluate(context);
    return expectedUnit == nullptr ? v.intValue() : (intpar_t)v.doubleValueInUnit(expectedUnit);
}

double cDynamicExpression::doubleValue(Context *context, const char *expectedUnit) const
{
    cNedValue v = evaluate(context);
    return expectedUnit == nullptr ? v.doubleValue() : v.doubleValueInUnit(expectedUnit);
}

std::string cDynamicExpression::stringValue(Context *context) const
{
    cNedValue v = evaluate(context);
    return v.stringValue();
}

cXMLElement *cDynamicExpression::xmlValue(Context *context) const
{
    cNedValue v = evaluate(context);
    return v.xmlValue();
}

inline double trunc(double x)
{
    return x < 0.0 ? ceil(x) : floor(x);
}

inline intpar_t shift(intpar_t a, intpar_t b)
{
    // positive b = left shift, negative b = (arithmetic) right shift
    // note: result of ">>" and "<<" is undefined if shift is larger or equal to the width of the integer
    const int width = 8*sizeof(a);
    if (b > 0)
        return b < width ? (a << b) : 0;
    else
        return -width < b ? (a >> -b) : a > 0 ? 0 : ~(intpar_t)0;
}

#ifndef __has_builtin
  #define __has_builtin(x) 0  // Compatibility for compilers without the __has_builtin macro
#endif

inline intpar_t safeAdd(intpar_t a, intpar_t b)
{
#if (__has_builtin(__builtin_add_overflow) && !defined(__c2__)) || (defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 5)
    intpar_t res;
    if (__builtin_add_overflow(a, b, &res))
        throw cRuntimeError("Integer overflow adding %" PRId64 " and %" PRId64 ", try casting operands to double", (int64_t)a, (int64_t)b);
    return res;
#else
    return a + b;  // unchecked
#endif
}

inline intpar_t safeMul(intpar_t a, intpar_t b)
{
#if (__has_builtin(__builtin_mul_overflow) && !defined(__c2__)) || (defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 5)
    intpar_t res;
    if (__builtin_mul_overflow(a, b, &res))
        throw cRuntimeError("Integer overflow multiplying %" PRId64 " and %" PRId64 ", try casting operands to double", (int64_t)a, (int64_t)b);
    return res;
#else
    const intpar_t int32max = std::numeric_limits<int32_t>::max();
    if ((a & ~int32max) == 0 && (b & ~int32max) == 0)
        return a * b;
    intpar_t res = a * b;
    if (res / a != b)
        throw cRuntimeError("Integer overflow multiplying %" PRId64 " and %" PRId64 ", try casting operands to double", (int64_t)a, (int64_t)b);
    return res;
#endif
}

inline intpar_t intPow(intpar_t base, intpar_t exp)
{
    ASSERT(exp >= 0);
    if (exp == 0)
        return 1;
    try {
        intpar_t result = 1;
        while (true) {
            if (exp & 1)
                result = safeMul(result, base);
            exp >>= 1;
            if (exp == 0)
                break;
            base = safeMul(base, base);
        }
        return result;
    }
    catch (const cRuntimeError& e) {
        throw cRuntimeError("Overflow during integer exponentiation, try casting operands to double");
    }
}

void cDynamicExpression::bringToCommonTypeAndUnit(cNedValue& a, cNedValue& b) const
{
    if (a.getType() == cNEDValue::INT && b.getType() == cNEDValue::INT) {
        // if conversion factor is an integer, try doing it using integer arithmetic
        // by converting values into the smaller unit; e.g. if one is KiB and the
        // other is MiB, convert MiB value into KiB.
        double c = UnitConversion::getConversionFactor(a.getUnit(), b.getUnit());
        double c2 = UnitConversion::getConversionFactor(b.getUnit(), a.getUnit());
        if (c == 1)
            return; // done
        else if (c > 1 && c == floor(c)) {
            a.intv = safeMul((intpar_t)c, a.intv);
            a.unit = b.unit;
            return;
        }
        else if (c2 > 1 && c2 == floor(c2)) {
            b.intv = safeMul((intpar_t)c2, b.intv);
            b.unit = a.unit;
            return;
        }
        // else fall through to computing the result in doubles
    }

    // do it in double, or fail due to incompatible units
    a.convertToDouble();
    b.convertToDouble();
    a.dbl = UnitConversion::convertUnit(a.dbl, a.unit, b.unit);
    a.unit = b.unit;
}

static void errorBooleanArgExpected(const cNedValue& actual)
{
    throw cRuntimeError("Boolean argument expected, got %s", cNEDValue::getTypeName(actual.getType()));
}

static void errorBooleanArgsExpected(const cNedValue& actual1, const cNedValue& actual2)
{
    throw cRuntimeError("Boolean arguments expected, got %s and %s", cNEDValue::getTypeName(actual1.getType()), cNEDValue::getTypeName(actual2.getType()));
}

static void errorNumericArgExpected(const cNedValue& actual)
{
    throw cRuntimeError("Numeric argument expected, got %s", cNEDValue::getTypeName(actual.getType()));
}

static void errorNumericArgsExpected(const cNedValue& actual1, const cNedValue& actual2)
{
    throw cRuntimeError("Numeric arguments expected, got %s and %s", cNEDValue::getTypeName(actual1.getType()), cNEDValue::getTypeName(actual2.getType()));
}

static void errorIntegerArgExpected(const cNedValue& actual)
{
    const char *hint = actual.getType() == cNedValue::DOUBLE ? " (note: no implicit conversion from double to int)" : "";
    throw cRuntimeError("Integer argument expected, got %s%s", cNEDValue::getTypeName(actual.getType()), hint);
}

static void errorIntegerArgsExpected(const cNedValue& actual1, const cNedValue& actual2)
{
    bool hasDouble = actual1.getType() == cNedValue::DOUBLE || actual2.getType() == cNedValue::DOUBLE;
    const char *hint = hasDouble ? " (note: no implicit conversion from double to int)" : "";
    throw cRuntimeError("Integer argument expected, got %s and %s%s", cNEDValue::getTypeName(actual1.getType()), cNEDValue::getTypeName(actual2.getType()), hint);
}

static void errorDimlessArgExpected(const cNedValue& actual)
{
    throw cRuntimeError("Dimensionless argument expected, got %s", actual.str().c_str());
}

static void errorDimlessArgsExpected(const cNedValue& actual1, const cNedValue& actual2)
{
    throw cRuntimeError("Dimensionless argument expected, got %s and %s", actual1.str().c_str(), actual2.str().c_str());
}

void cDynamicExpression::ensureNoLogarithmicUnit(const cNedValue& v)
{
    ASSERT(v.type == cNedValue::INT || v.type == cNedValue::DOUBLE);
    if (!opp_isempty(v.unit) && !UnitConversion::isLinearUnit(v.unit))
        throw cRuntimeError("Refusing to perform computations involving quantities with nonlinear units (%s)", v.str().c_str());
}


// we have a static stack to avoid new[] and call to cNedValue ctor stksize times
static const int stksize = 20;
static cNedValue _stk[stksize];
static bool _stkinuse = false;

cNedValue cDynamicExpression::evaluate(Context *context) const
{
    if (!context)
        throw cRuntimeError("cDynamicExpression::evaluate(): context cannot be nullptr");

    // use static _stk[] if possible, or allocate another one if that's in use.
    // Note: this will be reentrant but NOT thread safe
    cNedValue *stk;
    if (_stkinuse)
        stk = new cNedValue[stksize];
    else {
        _stkinuse = true;
        stk = _stk;
    }
    struct Finally
    {
        cNedValue *stk;
        Finally(cNedValue *stk) { this->stk = stk; }
        ~Finally() { if (stk == _stk) _stkinuse = false; else delete[] stk; }
    } f(stk);

    int tos = -1;
    int i;
    for (i = 0; i < size; i++) {
        Elem& e = elems[i];
        switch (e.type) {
            case Elem::BOOL:
                if (tos >= stksize-1)
                    throw cRuntimeError(E_ESTKOFLOW);
                stk[++tos] = e.b;
                break;

            case Elem::INT:
                if (tos >= stksize-1)
                    throw cRuntimeError(E_ESTKOFLOW);
                stk[++tos].set(e.i.i, e.i.unit);
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
                    case 0: {
                        stk[++tos] = e.f->getMathFuncNoArg()();
                        break;
                    }

                    case 1: {
                        if (tos < 0)
                            throw cRuntimeError(E_ESTKUFLOW);
                        double arg1 = stk[tos];
                        stk[tos] = e.f->getMathFunc1Arg()(arg1);
                        break;
                    }

                    case 2: {
                        if (tos < 1)
                            throw cRuntimeError(E_ESTKUFLOW);
                        double arg1 = stk[tos-1];
                        double arg2 = stk[tos];
                        stk[tos-1] = e.f->getMathFunc2Args()(arg1, arg2);
                        tos--;
                        break;
                    }

                    case 3: {
                        if (tos < 2)
                            throw cRuntimeError(E_ESTKUFLOW);
                        double arg1 = stk[tos-2];
                        double arg2 = stk[tos-1];
                        double arg3 = stk[tos];
                        stk[tos-2] = e.f->getMathFunc3Args()(arg1, arg2, arg3);
                        tos -= 2;
                        break;
                    }

                    case 4: {
                        if (tos < 3)
                            throw cRuntimeError(E_ESTKUFLOW);
                        double arg1 = stk[tos-3];
                        double arg2 = stk[tos-2];
                        double arg3 = stk[tos-1];
                        double arg4 = stk[tos];
                        stk[tos-3] = e.f->getMathFunc4Args()(arg1, arg2, arg3, arg4);
                        tos -= 3;
                        break;
                    }

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
                stk[argPos] = e.nf.f->invoke(context ? context->component : nullptr, stk+argPos, numArgs);
                tos = argPos;
                break;
            }

            case Elem::FUNCTOR: {
                int numArgs = e.fu->getNumArgs();
                int argPos = tos-numArgs+1;  // stk[] index of 1st arg to pass
                if (argPos < 0)
                    throw cRuntimeError(E_ESTKUFLOW);
                const char *argtypes = e.fu->getArgTypes();
                for (int i = 0; i < numArgs; i++) {
                    bool ok = argtypes[i] == '*' || argtypes[i] == stk[argPos+i].type || (argtypes[i] == 'L' && stk[argPos+i].type == cNedValue::DOUBLE); // allow int-to-double, but not double-to-int implicit conversion
                    if (!ok)
                        throw cRuntimeError(E_EBADARGS, e.fu->getFullName());
                }

                // note: unit checking is left to the function itself
                stk[argPos] = e.fu->evaluate(context, stk+argPos, numArgs);
                tos = argPos;
                break;
            }

            case Elem::CONSTSUBEXPR:
                // this should not occur
                throw cRuntimeError("evaluate: Constant subexpressions must have already been evaluated");

            case Elem::OP:
                try {
                    if (e.op == NEG || e.op == NOT || e.op == BIN_NOT) {
                        // unary
                        if (tos < 0)
                            throw cRuntimeError(E_ESTKUFLOW);
                        switch (e.op) {
                            case NEG:
                                if (stk[tos].type == cNedValue::INT) {
                                    ensureNoLogarithmicUnit(stk[tos]);
                                    stk[tos].intv = -stk[tos].intv;
                                }
                                else if (stk[tos].type == cNedValue::DOUBLE) {
                                    ensureNoLogarithmicUnit(stk[tos]);
                                    stk[tos].dbl = -stk[tos].dbl;
                                }
                                else
                                    errorNumericArgExpected(stk[tos]);
                                break;

                            case NOT:
                                if (stk[tos].type != cNedValue::BOOL)
                                    errorBooleanArgExpected(stk[tos]);
                                stk[tos].bl = !stk[tos].bl;
                                break;

                            case BIN_NOT:
                                if (stk[tos].type != cNedValue::INT)
                                    errorIntegerArgExpected(stk[tos]);
                                if (!opp_isempty(stk[tos].unit))
                                    errorDimlessArgExpected(stk[tos]);
                                stk[tos].intv = ~stk[tos].intv;
                                break;

                            default:
                                ASSERT(false);
                        }
                    }
                    else if (e.op == IIF) {
                        // ternary
                        if (tos < 2)
                            throw cRuntimeError(E_ESTKUFLOW);
                        if (stk[tos-2].type != cNedValue::BOOL)
                            errorBooleanArgExpected(stk[tos-2]);
                        stk[tos-2] = stk[tos-2].bl ? stk[tos-1] : stk[tos];
                        tos -= 2;
                    }
                    else {
                        // binary
                        if (tos < 1)
                            throw cRuntimeError(E_ESTKUFLOW);
                        switch (e.op) {
                            case SUB:
                                // negate second argument, then fall through to addition
                                if (stk[tos].type == cNedValue::DOUBLE)
                                    stk[tos].dbl = -stk[tos].dbl;
                                else if (stk[tos].type == cNedValue::INT) {
                                    if (stk[tos].intv == std::numeric_limits<intpar_t>::min())
                                        throw cRuntimeError("Integer overflow: cannot subtract -MAXINT");
                                    stk[tos].intv = -stk[tos].intv;
                                }
                                else
                                    errorNumericArgExpected(stk[tos]);
                                // [[fallthrough]]

                            case ADD:
                                // numeric addition or string concatenation
                                if (stk[tos-1].type == cNedValue::INT && stk[tos].type == cNedValue::INT) {  // both ints -> integer addition
                                    ensureNoLogarithmicUnit(stk[tos]);
                                    ensureNoLogarithmicUnit(stk[tos-1]);
                                    bringToCommonTypeAndUnit(stk[tos], stk[tos-1]);
                                    if (stk[tos].type == cNedValue::INT)
                                        stk[tos-1].intv = safeAdd(stk[tos-1].intv, stk[tos].intv);
                                    else //DOUBLE
                                        stk[tos-1].dbl = stk[tos-1].dbl + stk[tos].dbl;
                                }
                                else if (stk[tos-1].type == cNedValue::DOUBLE || stk[tos].type == cNedValue::DOUBLE) { // at least one is double -> double addition
                                    ensureNoLogarithmicUnit(stk[tos]);
                                    ensureNoLogarithmicUnit(stk[tos-1]);
                                    bringToCommonTypeAndUnit(stk[tos], stk[tos-1]);
                                    stk[tos-1].dbl = stk[tos-1].dbl + stk[tos].dbl;
                                }
                                else if (stk[tos-1].type == cNedValue::STRING && stk[tos].type == cNedValue::STRING)
                                    stk[tos-1].s = stk[tos-1].s + stk[tos].s;
                                else
                                    errorNumericArgsExpected(stk[tos-1], stk[tos]);
                                tos--;
                                break;

                            case MUL:
                                if (stk[tos-1].type == cNedValue::INT && stk[tos].type == cNedValue::INT) {  // both are integers -> integer multiplication
                                    if (!opp_isempty(stk[tos].unit) && !opp_isempty(stk[tos-1].unit))
                                        throw cRuntimeError("Multiplying two quantities with units is not supported");
                                    ensureNoLogarithmicUnit(stk[tos]);
                                    ensureNoLogarithmicUnit(stk[tos-1]);
                                    stk[tos-1].intv = safeMul(stk[tos-1].intv, stk[tos].intv);
                                    if (opp_isempty(stk[tos-1].unit))
                                        stk[tos-1].unit = stk[tos].unit;
                                }
                                else if (stk[tos-1].type == cNedValue::DOUBLE || stk[tos].type == cNedValue::DOUBLE) { // at least one is double -> double multiplication
                                    if (!opp_isempty(stk[tos].unit) && !opp_isempty(stk[tos-1].unit))
                                        throw cRuntimeError("Multiplying two quantities with units is not supported");
                                    ensureNoLogarithmicUnit(stk[tos]);
                                    ensureNoLogarithmicUnit(stk[tos-1]);
                                    stk[tos-1].convertToDouble();
                                    stk[tos].convertToDouble();
                                    stk[tos-1].dbl = stk[tos-1].dbl * stk[tos].dbl;
                                    if (opp_isempty(stk[tos-1].unit))
                                        stk[tos-1].unit = stk[tos].unit;
                                }
                                else
                                    errorNumericArgsExpected(stk[tos-1], stk[tos]);
                                tos--;
                                break;

                            case DIV:
                                // even if both args are integer, we perform the division in double, to reduce surprises;
                                // for now we only support num/num, unit/num, plus and unit/unit only if the two units are convertible
                                stk[tos-1].convertToDouble();
                                stk[tos].convertToDouble();
                                ensureNoLogarithmicUnit(stk[tos]);
                                if (stk[tos].dbl != 0)  // allow "0dB/0" as nan for compatibility with INET 3.x
                                    ensureNoLogarithmicUnit(stk[tos-1]);
                                if (!opp_isempty(stk[tos].unit))
                                    stk[tos].dbl = UnitConversion::convertUnit(stk[tos].dbl, stk[tos].unit, stk[tos-1].unit);
                                stk[tos-1].dbl = stk[tos-1].dbl / stk[tos].dbl;
                                if (!opp_isempty(stk[tos].unit))
                                    stk[tos-1].unit = nullptr;
                                tos--;
                                break;

                            case MOD:
                                if (stk[tos-1].type == cNedValue::INT && stk[tos].type == cNedValue::INT) {  // both ints -> integer modulo
                                    ensureNoLogarithmicUnit(stk[tos]);
                                    ensureNoLogarithmicUnit(stk[tos-1]);
                                    if (!opp_isempty(stk[tos].unit) || !opp_isempty(stk[tos-1].unit))
                                        errorDimlessArgsExpected(stk[tos-1], stk[tos]);
                                    stk[tos-1].intv = stk[tos-1].intv % stk[tos].intv;  //TODO result differs from fmod's result for negative numbers
                                }
                                else if (stk[tos-1].type == cNedValue::DOUBLE || stk[tos].type == cNedValue::DOUBLE) { // at least one is double -> double modulo
                                    ensureNoLogarithmicUnit(stk[tos]);
                                    ensureNoLogarithmicUnit(stk[tos-1]);
                                    stk[tos-1].convertToDouble();
                                    stk[tos].convertToDouble();
                                    if (!opp_isempty(stk[tos].unit) || !opp_isempty(stk[tos-1].unit))
                                        errorDimlessArgsExpected(stk[tos-1], stk[tos]);
                                    stk[tos-1].dbl = fmod(trunc(stk[tos-1].dbl), trunc(stk[tos].dbl));
                                }
                                else
                                    errorNumericArgsExpected(stk[tos-1], stk[tos]);
                                tos--;
                                break;

                            case POW:
                                if (stk[tos-1].type == cNedValue::INT && stk[tos].type == cNedValue::INT) {  // both ints -> integer power-of
                                    if (!opp_isempty(stk[tos].unit) || !opp_isempty(stk[tos-1].unit))
                                        errorDimlessArgsExpected(stk[tos-1], stk[tos]);
                                    if (stk[tos].intv < 0)
                                        throw cRuntimeError("Negative exponent in integer exponentiation, cast operands to double to allow it");
                                    stk[tos-1].intv = intPow(stk[tos-1].intv, stk[tos].intv);
                                }
                                else {
                                    stk[tos-1].convertToDouble();
                                    stk[tos].convertToDouble();
                                    if (!opp_isempty(stk[tos].unit) || !opp_isempty(stk[tos-1].unit))
                                        errorDimlessArgsExpected(stk[tos-1], stk[tos]);
                                    stk[tos-1].dbl = pow(stk[tos-1].dbl, stk[tos].dbl);
                                }
                                tos--;
                                break;

                            case AND:
                                if (stk[tos].type != cNedValue::BOOL || stk[tos-1].type != cNedValue::BOOL)
                                    errorBooleanArgsExpected(stk[tos-1], stk[tos]);
                                stk[tos-1].bl = stk[tos-1].bl && stk[tos].bl;
                                tos--;
                                break;

                            case OR:
                                if (stk[tos].type != cNedValue::BOOL || stk[tos-1].type != cNedValue::BOOL)
                                    errorBooleanArgsExpected(stk[tos-1], stk[tos]);
                                stk[tos-1].bl = stk[tos-1].bl || stk[tos].bl;
                                tos--;
                                break;

                            case XOR:
                                if (stk[tos].type != cNedValue::BOOL || stk[tos-1].type != cNedValue::BOOL)
                                    errorBooleanArgsExpected(stk[tos-1], stk[tos]);
                                stk[tos-1].bl = stk[tos-1].bl != stk[tos].bl;
                                tos--;
                                break;

                            case BIN_AND:
                                if (stk[tos].type != cNedValue::INT || stk[tos-1].type != cNedValue::INT)
                                    errorIntegerArgsExpected(stk[tos-1], stk[tos]);
                                if (!opp_isempty(stk[tos].unit) || !opp_isempty(stk[tos-1].unit))
                                    errorDimlessArgsExpected(stk[tos-1], stk[tos]);
                                stk[tos-1].intv = stk[tos-1].intv & stk[tos].intv;
                                tos--;
                                break;

                            case BIN_OR:
                                if (stk[tos].type != cNedValue::INT || stk[tos-1].type != cNedValue::INT)
                                    errorIntegerArgsExpected(stk[tos-1], stk[tos]);
                                if (!opp_isempty(stk[tos].unit) || !opp_isempty(stk[tos-1].unit))
                                    errorDimlessArgsExpected(stk[tos-1], stk[tos]);
                                stk[tos-1].intv = stk[tos-1].intv | stk[tos].intv;
                                tos--;
                                break;

                            case BIN_XOR:
                                if (stk[tos].type != cNedValue::INT || stk[tos-1].type != cNedValue::INT)
                                    errorIntegerArgsExpected(stk[tos-1], stk[tos]);
                                if (!opp_isempty(stk[tos].unit) || !opp_isempty(stk[tos-1].unit))
                                    errorDimlessArgsExpected(stk[tos-1], stk[tos]);
                                stk[tos-1].intv = stk[tos-1].intv ^ stk[tos].intv;
                                tos--;
                                break;

                            case LSHIFT:
                                if (stk[tos].type != cNedValue::INT || stk[tos-1].type != cNedValue::INT)
                                    errorIntegerArgsExpected(stk[tos-1], stk[tos]);
                                if (!opp_isempty(stk[tos].unit) || !opp_isempty(stk[tos-1].unit))
                                    errorDimlessArgsExpected(stk[tos-1], stk[tos]);
                                stk[tos-1].intv = shift(stk[tos-1].intv, stk[tos].intv);
                                tos--;
                                break;

                            case RSHIFT:
                                if (stk[tos].type != cNedValue::INT || stk[tos-1].type != cNedValue::INT)
                                    errorIntegerArgsExpected(stk[tos-1], stk[tos]);
                                if (!opp_isempty(stk[tos].unit) || !opp_isempty(stk[tos-1].unit))
                                    errorDimlessArgsExpected(stk[tos-1], stk[tos]);
                                if (stk[tos].intv < 0 && stk[tos].intv == -stk[tos].intv) // -MAXINT has no positive equivalent
                                    stk[tos-1].intv = 0;
                                else
                                    stk[tos-1].intv = shift(stk[tos-1].intv, -stk[tos].intv);
                                tos--;
                                break;

#define COMPARISON(RELATION) \
                                 if (stk[tos-1].type==cNedValue::INT && stk[tos].type==cNedValue::INT) { \
                                    bringToCommonTypeAndUnit(stk[tos], stk[tos-1]); \
                                    stk[tos-1] = (stk[tos-1].intv RELATION stk[tos].intv); \
                                 } else if (stk[tos-1].type==cNedValue::DOUBLE || stk[tos].type==cNedValue::DOUBLE) { \
                                     stk[tos-1].convertToDouble(); \
                                     stk[tos].convertToDouble(); \
                                     stk[tos].dbl = UnitConversion::convertUnit(stk[tos].dbl, stk[tos].unit, stk[tos-1].unit); \
                                     stk[tos-1] = (stk[tos-1].dbl RELATION stk[tos].dbl); \
                                 } else if (stk[tos-1].type==cNedValue::STRING && stk[tos].type==cNedValue::STRING) \
                                     stk[tos-1] = (stk[tos-1].s RELATION stk[tos].s); \
                                 else if (stk[tos-1].type==cNedValue::BOOL && stk[tos].type==cNedValue::BOOL) \
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
                }
                catch (std::exception& ex) {
                    throw cRuntimeError("%s: %s", e.str().c_str(), ex.what());
                }
                break;

            default:
                throw cRuntimeError(E_BADEXP);
        }
    } // for

    if (tos != 0)
        throw cRuntimeError(E_BADEXP);

    return stk[tos];
}

std::string cDynamicExpression::str() const
{
    // We perform the same algorithm as during evaluation (i.e. stack machine),
    // only instead of actual calculations we store the result as string.
    // We need to keep track of operator precedences to be able to add parens where needed.

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

                case Elem::INT: {
                    if (tos >= stksize-1)
                        throw cRuntimeError(E_ESTKOFLOW);
                    char buf[32];
                    sprintf(buf, "%" PRId64, (int64_t) e.i.i);
                    strstk[++tos] = buf;
                    pristk[tos] = 0;
                    break;
                }

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
                    strstk[++tos] = e.x==nullptr ? "<nullptr>" : std::string("<") + e.x->getTagName() + ">@" + e.x->getSourceLocation();
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

