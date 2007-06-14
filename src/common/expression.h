//==========================================================================
//   EXPRESSION.H  - part of
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

#ifndef __EXPRESSION_H
#define __EXPRESSION_H

#include <string>
#include "commondefs.h"
#include "commonutil.h"


/**
 * FIXME revise docu in the whole class!!!!!!
 *
 * This is a variant of the simkernel's expression evaluator, with several
 * simplifications:
 *   - no dependence on simkernel classes (cPar, cComponent, cRuntimeError, etc)
 *   - fewer data types (cXMLElement support removed)
 *   - "unit" of numeric values removed
 *   - doesn't use stringpool
 */
class COMMON_API Expression
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
    class Elem
    {
      friend class Expression;
      private:
        // Types:
        //  - bool
        //  - double (there's no long -- we calculate everything in double)
        //  - string
        //  - math operator (+-*/%^...)
        //  - functor
        //
        enum Type {UNDEF, BOOL, DBL, STR, FUNCTOR, OP} type;
        union {
            bool b;
            double d;
            char *s;
            Functor *fu;
            OpType op;
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
        void operator=(int _i)  {type=DBL; d=_i;}

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(short _i)  {type=DBL; d=_i;}

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(long _l)  {type=DBL; d=_l;}

        /**
         * Effect during evaluation of the expression: pushes the given number
         * to the evaluation stack.
         */
        void operator=(double _d)  {type=DBL; d=_d;}

        /**
         * Effect during evaluation of the expression: pushes the given string
         * to the evaluation stack.
         */
        void operator=(const char *_s)  {type=STR; Assert(_s); s=new char[strlen(_s)+1]; strcpy(s,_s);}

        /**
         * Function object, with an interface not unlike cNEDFunction.
         * This object will be deleted by expression's destructor.
         */
        void operator=(Functor *_f)  {type=FUNCTOR; Assert(_f); fu=_f;}

        /**
         * Unary, binary or tertiary (?: only) operations.
         */
        void operator=(OpType _op)  {type=OP; op=_op;}
    };

    /**
     * The dynamic expression evaluator calculates in StkValues.
     * There's no "long" field in it: all numeric calculations are performed
     * in double. XXX This is fine for 32-bit longs, but not for 64-bit ones,
     * as double's mantissa is only 53 bits.
     */
    struct StkValue
    {
        enum {UNDEF=0, BOOL='B', DBL='D', STR='S'} type;
        bool bl;
        double dbl;
        std::string str;

        StkValue()  {type=UNDEF;}
        StkValue(bool b)  {*this=b;}
        StkValue(long l)  {*this=l;}
        StkValue(double d)  {*this=d;}
        StkValue(const char *s)  {*this=s;}
        StkValue(const std::string& s)  {*this=s;}
        void operator=(bool b)  {type=BOOL; bl=b;}
        void operator=(long l)  {type=DBL; dbl=l;}
        void operator=(double d)  {type=DBL; dbl=d;}
        void operator=(const char *s)  {type=STR; str=s?s:"";}
        void operator=(const std::string& s)  {type=STR; str=s;}
        std::string toString();
    };

    /**
     * Function object base class. They can be used to implement variables, etc.
     */
    class Functor
    {
      public:
        virtual ~Functor() {}
        virtual const char *name() const = 0;
        virtual const char *argTypes() const = 0;
        virtual int numArgs() const {return strlen(argTypes());}
        virtual char returnType() const = 0;
        virtual StkValue evaluate(StkValue args[], int numargs) = 0;
        virtual std::string toString(std::string args[], int numargs) = 0;
    };

  protected:
    Elem *elems;
    int nelems;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    Expression();

    /**
     * Copy constructor.
     */
    Expression(const Expression& other) {elems=NULL; operator=(other);}

    /**
     * Destructor.
     */
    virtual ~Expression();

    /**
     * Assignment operator.
     */
    Expression& operator=(const Expression& other);
    //@}

    /** @name Setter and evaluator methods. */
    //@{
    /**
     * Sets the Reverse Polish expression to evaluate. The array must be a
     * dynamically allocated one, and will be deleted by this object.
     * No verification is performed on the expression at this time.
     */
    virtual void setExpression(Elem e[], int nelems);

    /**
     * Evaluate the expression, and return the results as a StkValue.
     * Throws an error if the expression has some problem (i.e. stack
     * overflow/underflow, "cannot cast", "function not found", etc.)
     */
    virtual StkValue evaluate() const;

    /**
     * Evaluate the expression and convert the result to bool if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual bool boolValue();

    /**
     * Evaluate the expression and convert the result to long if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual long longValue();

    /**
     * Evaluate the expression and convert the result to double if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual double doubleValue();

    /**
     * Evaluate the expression and convert the result to string if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual std::string stringValue();
    //@}

    /** @name Miscellaneous utility functions. */
    //@{
    /**
     * Converts the expression to string.
     */
    virtual std::string toString() const;

    /**
     * Interprets the string as an expression, and stores it.
     */
    virtual void parse(const char *text);

    /**
     * Returns true if the expression is just a literal (or equivalent to one,
     * like "2+2").
     */
    virtual bool isAConstant() const;
    //@}
};

#endif


