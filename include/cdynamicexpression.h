//==========================================================================
//   CDYNAMICEXPRESSION.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDYNAMICEXPRESSION_H
#define __CDYNAMICEXPRESSION_H

#include "cnedvalue.h"
#include "cexpression.h"
#include "cstringpool.h"

NAMESPACE_BEGIN

class cXMLElement;
class cPar;
class cNEDMathFunction;
class cNEDFunction;

/**
 * A stack-based expression evaluator class, for dynamically created
 * expressions.
 *
 * NOTE: Experimental class -- API is subject to change.
 *
 * @ingroup SimCore
 */
class SIM_API cDynamicExpression : public cExpression
{
  public:
    /**
     * Operations supported by this class:
     *  - add, subtract, multiply, divide ("+" is also string concatenation)
     *  - modulo, power of, negation (-1)
     *  - equal, not equal, greater, greater or equal, less, less or equal
     *  - inline if (the C/C++ ?: operator)
     *  - logical and, or, xor, not
     *  - bitwise and, or, xor, not (1's complement)
     *  - left shift, right shift
     */
    enum OpType {
        ADD, SUB, MUL, DIV, MOD, POW, NEG,
        EQ, NE, GT, GE, LT, LE, IIF, AND, OR, XOR, NOT,
        BIN_AND, BIN_OR, BIN_XOR, BIN_NOT, LSHIFT, RSHIFT
    };

    class Functor; // forward decl

    /**
     * One element in a (reverse Polish) expression
     */
    class SIM_API Elem
    {
      friend class cDynamicExpression;
      private:
        // Types:
        //  - bool
        //  - double (there is no long -- we calculate everything in double)
        //  - string
        //  - pointer to an "external" cXMLElement
        //  - cNEDMathFunction: function with 0/1/2/3/4 double arguments
        //  - cNEDFunction: function taking/returning cNEDValue (NEDFunction)
        //  - functor
        //  - math operator (+-*/%^...)
        //  - constant subexpression
        //
        enum Type {UNDEF, BOOL, DBL, STR, XML, MATHFUNC, NEDFUNC, FUNCTOR, OP, CONSTSUBEXPR} type;
        static cStringPool stringPool;
        union {
            bool b;
            struct {double d; const char *unit;} d;
            const char *s; // points into stringPool
            cXMLElement *x;
            cNEDMathFunction *f;
            struct {cNEDFunction *f; int argc;} nf;
            Functor *fu;
            OpType op;
            cExpression *constExpr;
        };

      private:
        void copy(const Elem& other);

        void deleteOld();

      public:
        Elem()  {type=UNDEF;}
        Elem(const Elem& other)  {type=UNDEF; copy(other);}
        ~Elem();

        /**
         * Assignment operator -- we need to copy Elem at a hundred places
         */
        void operator=(const Elem& other);

        /**
         * Effect during evaluation of the expression: pushes the given boolean
         * constant to the evaluation stack.
         */
        void operator=(bool _b)  {type=BOOL; b=_b;}

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(int _i)  {type=DBL; d.d=_i; d.unit=NULL;}

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(short _i)  {type=DBL; d.d=_i; d.unit=NULL;}

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(long _l)  {type=DBL; d.d=_l; d.unit=NULL;}

        /**
         * Effect during evaluation of the expression: pushes the given number
         * to the evaluation stack.
         */
        void operator=(double _d)  {type=DBL; d.d=_d; d.unit=NULL;}

        /**
         * Sets the unit of an Elem previously set to a double value.
         * The type must already be DBL, or an error gets thrown.
         */
        void setUnit(const char *s)  {ASSERT(type==DBL); d.unit = stringPool.get(s);}

        /**
         * Effect during evaluation of the expression: pushes the given string
         * to the evaluation stack.
         */
        void operator=(const char *_s)  {type=STR; s = stringPool.get(_s);}

        /**
         * Effect during evaluation of the expression: pushes the given
         * cXMLElement pointer to the evaluation stack.
         */
        void operator=(cXMLElement *_x)  {type=XML; x=_x;}

        /**
         * Effect during evaluation of the expression: Call a function
         * taking 0..4 doubles and returning a double.
         */
        void operator=(cNEDMathFunction *_f)  {type=MATHFUNC; ASSERT(_f); f=_f;}

        /**
         * Effect during evaluation of the expression: call a function
         * that function takes an array of cNEDValues and returns a cNEDValue.
         */
        void set(cNEDFunction *f, int argc)  {type=NEDFUNC; ASSERT(f); nf.f=f; nf.argc=argc;}

        /**
         * Function object, with an interface not unlike cNEDFunction.
         * This object will be deleted by expression's destructor.
         */
        void operator=(Functor *f)  {type=FUNCTOR; ASSERT(f); fu=f;}

        /**
         * Unary, binary or tertiary (?:) operations.
         */
        void operator=(OpType _op)  {type=OP; op=_op;}

        /**
         * Constant subexpression.
         */
        void operator=(cExpression *_expr)  {type=CONSTSUBEXPR; constExpr=_expr;}

        /**
         * For cDynamicExpression::compare()
         */
        int compare(const Elem& other) const;
    };

    /**
     * Function object base class. We use function objects to handle NED parameter
     * references, "index" and "sizeof" operators, and references to NED "for" loop
     * variables.
     */
    class SIM_API Functor : public cObject
    {
      public:
        virtual const char *getArgTypes() const = 0;
        virtual int getNumArgs() const {return strlen(getArgTypes());}
        virtual char getReturnType() const = 0;
        virtual cNEDValue evaluate(cComponent *context, cNEDValue args[], int numargs) = 0;
        virtual std::string str(std::string args[], int numargs) = 0;
    };

  protected:
    Elem *elems;
    int size;

  private:
    void copy(const cDynamicExpression& other);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cDynamicExpression();

    /**
     * Copy constructor.
     */
    cDynamicExpression(const cDynamicExpression& other) : cExpression(other) {elems=NULL; copy(other);}

    /**
     * Destructor.
     */
    virtual ~cDynamicExpression();

    /**
     * Assignment operator.
     */
    cDynamicExpression& operator=(const cDynamicExpression& other);
    //@}

    /** @name Redefined cObject functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cDynamicExpression *dup() const  {return new cDynamicExpression(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    // Note: parsimPack()/parsimUnpack() de-inherited in cExpression.
    //@}

    /** @name Setter and evaluator methods. */
    //@{
    /**
     * Sets the Reverse Polish expression to evaluate. The array must be a
     * dynamically allocated one, and will be deleted by this object.
     * No verification is performed on the expression at this time.
     */
    virtual void setExpression(Elem e[], int size);

    /**
     * Evaluate the expression, and return the results as a cNEDValue.
     * Throws an error if the expression has some problem (i.e. stack
     * overflow/underflow, "cannot cast", "function not found", etc.)
     */
    virtual cNEDValue evaluate(cComponent *context) const;

    /**
     * Evaluate the expression and convert the result to bool if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual bool boolValue(cComponent *context);

    /**
     * Evaluate the expression and convert the result to long if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual long longValue(cComponent *context, const char *expectedUnit=NULL);

    /**
     * Evaluate the expression and convert the result to double if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual double doubleValue(cComponent *context, const char *expectedUnit=NULL);

    /**
     * Evaluate the expression and convert the result to string if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual std::string stringValue(cComponent *context);

    /**
     * Evaluate the expression and convert the result to an XML tree if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual cXMLElement *xmlValue(cComponent *context);
    //@}

    /** @name Miscellaneous utility functions. */
    //@{
    /**
     * Converts the expression to string.
     */
    virtual std::string str() const;

    /**
     * Interprets the string as an expression, and stores it.
     */
    virtual void parse(const char *text);

    /**
     * Compares two expressions.
     */
    virtual int compare(const cExpression *other) const;

    /**
     * Returns true if the expression is just a literal (or equivalent to one,
     * like "2+2").
     */
    virtual bool isAConstant() const;

    /**
     * Returns true if this expression contains const subexpressions.
     */
    virtual bool containsConstSubexpressions() const;

    /**
     * Evaluates const subexpressions, and replaces them with their values.
     * See cDynamicExpression::Elem::CONSTSUBEXPR.
     */
    virtual void evaluateConstSubexpressions(cComponent *context);

    /**
     * Convert the given number into the target unit (e.g. milliwatt to watt).
     * Throws an exception if conversion is not possible (unknown/unrelated units).
     */
    static double convertUnit(double d, const char *unit, const char *targetUnit);

    //@}
};

NAMESPACE_END


#endif


