//==========================================================================
//   CDYNAMICEXPRESSION.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CDYNAMICEXPRESSION_H
#define __OMNETPP_CDYNAMICEXPRESSION_H

#include "cnedvalue.h"
#include "cexpression.h"
#include "cstringpool.h"

namespace omnetpp {

class cXMLElement;
class cPar;
class cNedMathFunction;
class cNedFunction;

/**
 * @brief A stack-based expression evaluator class, for dynamically created
 * expressions.
 *
 * @ingroup SimSupport
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
     * @brief One element in a (reverse Polish) expression
     */
    class SIM_API Elem
    {
      friend class cDynamicExpression;
      private:
        // Types:
        //  - bool
        //  - intpar_t
        //  - double
        //  - string
        //  - pointer to an "external" cXMLElement
        //  - cNedMathFunction: function with 0/1/2/3/4 double arguments
        //  - cNedFunction: function taking/returning cNedValue (NedFunction)
        //  - functor
        //  - math operator (+-*/%^...)
        //  - constant subexpression
        //
        enum Type {UNDEF, BOOL, INT, DBL, STR, XML, MATHFUNC, NEDFUNC, FUNCTOR, OP, CONSTSUBEXPR} type;
        static cStringPool stringPool;
        union {
            bool b;
            struct {intpar_t i; const char *unit;} i;
            struct {double d; const char *unit;} d;
            const char *s; // points into stringPool
            cXMLElement *x;
            cNedMathFunction *f;
            struct {cNedFunction *f; int argc;} nf;
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
        void operator=(bool b);

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(intpar_t i);

        /**
         * Effect during evaluation of the expression: pushes the given number
         * to the evaluation stack.
         */
        void operator=(double d);

        /**
         * Sets the unit of an Elem previously set to a double or integer value.
         * The type must already be DBL or INT, or an error gets thrown.
         */
        void setUnit(const char *s);

        /**
         * Effect during evaluation of the expression: pushes the given string
         * to the evaluation stack.
         */
        void operator=(const char *s);

        /**
         * Effect during evaluation of the expression: pushes the given
         * cXMLElement pointer to the evaluation stack.
         */
        void operator=(cXMLElement *x);

        /**
         * Effect during evaluation of the expression: Call a function
         * taking 0..4 doubles and returning a double.
         */
        void operator=(cNedMathFunction *f);

        /**
         * Effect during evaluation of the expression: call a function
         * that function takes an array of cNedValues and returns a cNedValue.
         */
        void set(cNedFunction *f, int argc);

        /**
         * Function object, with an interface not unlike cNedFunction.
         * This object will be deleted by expression's destructor.
         */
        void operator=(Functor *f);

        /**
         * Unary, binary or tertiary (?:) operations.
         */
        void operator=(OpType op);

        /**
         * Constant subexpression.
         */
        void operator=(cExpression* expr);

        /**
         * For cDynamicExpression::compare()
         */
        int compare(const Elem& other) const;

        /**
         * Returns the string representation of the given operator.
         */
        static const char *getOpName(OpType op);

        /**
         * Returns the string representation of this element.
         */
        std::string str() const;
    };

    /**
     * @brief Function object base class. We use function objects to handle NED parameter
     * references, "index" and "sizeof" operators, and references to NED "for" loop
     * variables.
     */
    class SIM_API Functor : public cObject
    {
      public:
        virtual const char *getArgTypes() const = 0;
        virtual int getNumArgs() const {return strlen(getArgTypes());}
        virtual char getReturnType() const = 0;
        virtual cNedValue evaluate(Context *context, cNedValue args[], int numargs) = 0;
        virtual std::string str(std::string args[], int numargs) = 0;
        using cObject::str;
        typedef cDynamicExpression::Context Context;
    };


  protected:
    Elem *elems;
    int size;

  private:
    void copy(const cDynamicExpression& other);
    void bringToCommonTypeAndUnit(cNedValue& a, cNedValue& b) const;

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
    cDynamicExpression(const cDynamicExpression& other) : cExpression(other) {elems=nullptr; copy(other);}

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
    virtual cDynamicExpression *dup() const override  {return new cDynamicExpression(*this);}

    /**
     * Converts the expression to string.
     * See cObject for more details.
     */
    virtual std::string str() const override;

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
     * Evaluate the expression, and return the results as a cNedValue.
     * Throws an error if the expression has some problem (i.e. stack
     * overflow/underflow, "cannot cast", "function not found", etc.)
     */
    virtual cNedValue evaluate(Context *context) const override;

    /**
     * Evaluate the expression and convert the result to bool if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual bool boolValue(Context *context) override;

    /**
     * Evaluate the expression and convert the result to intpar_t if possible;
     * throw an error if conversion from that type is not supported, or
     * the value of out of the range of intpar_t.
     */
    virtual intpar_t intValue(Context *context, const char *expectedUnit=nullptr) override;

    /**
     * Evaluate the expression and convert the result to double if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual double doubleValue(Context *context, const char *expectedUnit=nullptr) override;

    /**
     * Evaluate the expression and convert the result to string if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual std::string stringValue(Context *context) override;

    /**
     * Evaluate the expression and convert the result to an XML tree if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual cXMLElement *xmlValue(Context *context) override;
    //@}

    /** @name Miscellaneous utility functions. */
    //@{
    /**
     * Interprets the string as an expression, and stores it.
     */
    virtual void parse(const char *text) override;

    /**
     * Compares two expressions.
     */
    virtual int compare(const cExpression *other) const override;

    /**
     * Returns true if the expression is just a literal (or equivalent to one,
     * like "2+2").
     */
    virtual bool isAConstant() const;

    /**
     * Returns true if this expression contains const subexpressions.
     */
    virtual bool containsConstSubexpressions() const override;

    /**
     * Evaluates const subexpressions, and replaces them with their values.
     * See cDynamicExpression::Elem::CONSTSUBEXPR.
     */
    virtual void evaluateConstSubexpressions(Context *context) override;

    /**
     * Convert the given number into the target unit (e.g. milliwatt to watt).
     * Throws an exception if conversion is not possible (unknown/unrelated units).
     */
    static double convertUnit(double d, const char *unit, const char *targetUnit);

    //@}
};


}  // namespace omnetpp


#endif


