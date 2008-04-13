//==========================================================================
//   CDYNAMICEXPRESSION.H  - part of
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

#ifndef __CDYNAMICEXPRESSION_H
#define __CDYNAMICEXPRESSION_H

#include "cexpression.h"
#include "cstringpool.h"

NAMESPACE_BEGIN

class cXMLElement;
class cPar;
class cMathFunction;
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
        //  - double (there's no long -- we calculate everything in double)
        //  - string
        //  - pointer to an "external" cXMLElement
        //  - cMathFunction: function with 0/1/2/3/4 double arguments
        //  - cNEDFunction: function taking/returning Value (NEDFunction)
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
            cMathFunction *f;
            struct {cNEDFunction *f; int argc;} nf;
            Functor *fu;
            OpType op;
            cExpression *constexpr;
        };

      private:
        void deleteOld();

      public:
        Elem()  {type=UNDEF;}
        Elem(const Elem& other)  {type=UNDEF; operator=(other);}
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
        void operator=(cMathFunction *_f)  {type=MATHFUNC; ASSERT(_f); f=_f;}

        /**
         * Effect during evaluation of the expression: call a function
         * that function takes an array of Values and returns a Value.
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
        void operator=(cExpression *_expr)  {type=CONSTSUBEXPR; constexpr=_expr;}

        /**
         * For cDynamicExpression::compare()
         */
        int compare(const Elem& other) const;
    };

    /**
     * The dynamic expression evaluator calculates in Values.
     *
     * Note: There's no <tt>long</tt> field in it: all numeric calculations are
     * performed in floating point (<tt>double</tt>). While this is fine on 32-bit
     * architectures, on 64-bit architectures some precision will be lost
     * because IEEE <tt>double</tt>'s mantissa is only 53 bits.
     *
     * NOTE: Experimental class -- API is subject to change.
     */
    struct SIM_API Value
    {
        // Note: char codes need to be present and be consistent with cNEDFunction::argTypes()
        enum {UNDEF=0, BOOL='B', DBL='D', STR='S', XML='X'} type;
        bool bl;
        double dbl;
        const char *dblunit; // stringpooled, may be NULL
        std::string s;
        cXMLElement *xml;

        Value()  {type=UNDEF;}
        Value(bool b)  {*this=b;}
        Value(long l)  {*this=l;}
        Value(double d)  {*this=d;}
        Value(double d, const char *unit)  {set(d,unit);}
        Value(const char *s)  {*this=s;}
        Value(const std::string& s)  {*this=s;}
        Value(cXMLElement *x)  {*this=x;}
        Value(const cPar& par) {*this=par;}
        void operator=(bool b)  {type=BOOL; bl=b;}
        void operator=(long l)  {type=DBL; dbl=l; dblunit=NULL;}
        void operator=(double d)  {type=DBL; dbl=d; dblunit=NULL;}
        void set(double d, const char *unit) {type=DBL; dbl=d; dblunit=unit;}
        void operator=(const char *s)  {type=STR; this->s=s?s:"";}
        void operator=(const std::string& s)  {type=STR; this->s=s;}
        void operator=(cXMLElement *x)  {type=XML; xml=x;}
        void operator=(const cPar& par);
        std::string str() const;
        Value& convertTo(const char *unit) {dbl=convertUnit(dbl, dblunit, unit); dblunit=unit; return *this;}
    };

    /**
     * Function object base class. We use function objects to handle NED parameter
     * references, "index" and "sizeof" operators, and references to NED "for" loop
     * variables.
     */
    class SIM_API Functor : public cObject
    {
      public:
        virtual const char *argTypes() const = 0;
        virtual int numArgs() const {return strlen(argTypes());}
        virtual char returnType() const = 0;
        virtual Value evaluate(cComponent *context, Value args[], int numargs) = 0;
        virtual std::string str(std::string args[], int numargs) = 0;
    };

  protected:
    Elem *elems;
    int size;

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
    cDynamicExpression(const cDynamicExpression& other) {elems=NULL; operator=(other);}

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
     * Produces a one-line description of object contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    // Note: netPack()/netUnpack() de-inherited in cExpression.
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
     * Evaluate the expression, and return the results as a Value.
     * Throws an error if the expression has some problem (i.e. stack
     * overflow/underflow, "cannot cast", "function not found", etc.)
     */
    virtual Value evaluate(cComponent *context) const;

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
     * Convert the given number into the target unit (e.g. milliwatt to watt).
     * Throws an exception if conversion is not possible (unknown/unrelated units).
     */
    static double convertUnit(double d, const char *unit, const char *targetUnit);
    //@}
};

NAMESPACE_END


#endif


