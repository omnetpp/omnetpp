//==========================================================================
//  EXPRESSION.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_EXPRESSION_H
#define __OMNETPP_COMMON_EXPRESSION_H

#include <string>
#include "commondefs.h"
#include "commonutil.h"
#include "stringutil.h"
#include "stringpool.h"

namespace omnetpp {
namespace common {

/**
 * This is an extensible arithmetic expression evaluator class. Supports the
 * following data types: long, double, string, bool.
 *
 * The code is based on the simkernel's expression evaluator, with a number of
 * modifications:
 *   - no dependence on simkernel classes (cPar, cComponent, cRuntimeError, etc)
 *   - no support for NED constructs (sizeof(), index, this, default(), etc)
 *   - no support for xmldoc() and the XML data type
 *   - "unit" support for numeric values removed
 *   - does not use stringpool
 *   - added resolver
 *   - $ and @ accepted in identifier names
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
    class COMMON_API Elem
    {
      friend class Expression;
      public:
        // Types:
        //  - bool
        //  - double (there is no long -- we calculate everything in double)
        //  - string
        //  - math operator (+-*/%^...)
        //  - functor
        //
        enum Type {UNDEF, BOOL, DBL, STR, FUNCTOR, OP};
        static StringPool stringPool;
      private:
        Type type;
        union {
            bool b;
            struct {double d; const char *unit;} d;
            char *s;
            Functor *fu;
            OpType op;
        };

      private:
        void deleteOld() { // note: when defined in .cc file, VC++ linker won't find it from the envir lib
            if (type==STR)
                delete [] s;
            else if (type==FUNCTOR)
                delete fu;
        }

      public:
        Elem()  {type=UNDEF;}
        Elem(const Elem& other)  {type=UNDEF; operator=(other);}
        ~Elem() {deleteOld();}

        /** @name Getters */
        //@{
        Type getType() const {return type;}
        bool getBool() const {Assert(type==BOOL); return b;}
        double getDouble() const {Assert(type==DBL); return d.d;}
        const char *getString() const {Assert(type==STR); return s;}
        Functor *getFunctor() const {Assert(type==FUNCTOR); return fu;}
        OpType getOp() const {Assert(type==OP); return op;}
        //@}

        /**
         * Assignment operator -- we need to copy Elem at a hundred places
         */
        void operator=(const Elem& other) {Elem_eq(*this, other);}

        /**
         * Effect during evaluation of the expression: pushes the given boolean
         * constant to the evaluation stack.
         */
        void operator=(bool _b);

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(int _i);

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(short _i);

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(long _l);

        /**
         * Effect during evaluation of the expression: pushes the given number
         * to the evaluation stack.
         */
        void operator=(double _d);

        /**
         * Effect during evaluation of the expression: pushes the given string
         * to the evaluation stack.
         */
        void operator=(const char* _s);

        /**
         * Function object, with an interface not unlike cNedFunction.
         * This object will be deleted by expression's destructor.
         */
        void operator=(Functor* _f);

        /**
         * Unary, binary or tertiary (?: only) operations.
         */
        void operator=(OpType _op);

        /**
         * Sets the unit of an Elem previously set to a double value.
         * The type must already be DBL, or an error gets thrown.
         */
        void setUnit(const char *s)  {Assert(type==DBL); d.unit = stringPool.get(s);}

        /**
         * Utility function, returns the argument count of this element.
         * I.e. returns 0 for BOOL, DBL, STRING and zero-arg Functors,
         * returns 1 for a one-arg Functors, and returns 2 for most operators.
         */
        int getNumArgs() const {return Elem_getNumArgs(*this);}

        /**
         * Debug string.
         */
        std::string str() const { return Elem_str(type,b,d.d,s,fu,op);}
    };

    /**
     * The dynamic expression evaluator calculates in Values.
     * There is no "long" field in it: all numeric calculations are performed
     * in double. XXX This is fine for 32-bit longs, but not for 64-bit ones,
     * as double's mantissa is only 53 bits.
     */
    struct COMMON_API Value
    {
        enum {UNDEF=0, BOOL='B', DBL='D', STR='S'} type;
        bool bl;
        double dbl;
        const char *dblunit=nullptr; // stringpooled, may be nullptr
        std::string s;

        Value()  {type=UNDEF;}
        Value(bool b)  {*this=b;}
        Value(long l)  {*this=l;}
        Value(double d)  {*this=d;}
        Value(double d, const char *unit)  {set(d,unit);}
        Value(const char *s)  {*this=s;}
        Value(const std::string& s)  {*this=s;}
        void operator=(bool b)  {type=BOOL; bl=b;}
        void operator=(long l)  {type=DBL; dbl=l; dblunit=nullptr;}
        void operator=(double d)  {type=DBL; dbl=d; dblunit=nullptr;}
        void operator=(const char *s)  {type=STR; this->s=s?s:"";}
        void operator=(const std::string& s)  {type=STR; this->s=s;}
        void set(double d, const char *unit) {type=DBL; dbl=d; dblunit=unit;}
        std::string str();
    };

    /**
     * Function object base class. They can be used to implement variables, etc.
     */
    class COMMON_API Functor
    {
      public:
        virtual ~Functor() {}
        virtual Functor *dup() const = 0;
        virtual const char *getName() const = 0;
        virtual const char *getArgTypes() const = 0;
        virtual int getNumArgs() const {return strlen(getArgTypes());}
        virtual char getReturnType() const = 0;
        virtual Value evaluate(Value args[], int numArgs) = 0;
        virtual std::string str(std::string args[], int numArgs) = 0;
    };

    /**
     * A functor subclass that implements a variable; still an abstract class.
     */
    class COMMON_API Variable : public Functor
    {
      public:
        virtual const char *getArgTypes() const override {return "";}
        virtual int getNumArgs() const override {return 0;}
        virtual std::string str(std::string args[], int numArgs) override {return getName();}
    };

    /**
     * A functor subclass that implements a function; still an abstract class.
     */
    class COMMON_API Function : public Functor
    {
      public:
        // returns name(arg1,arg2,...)
        virtual std::string str(std::string args[], int numArgs) override {return Function_str(getName(), args, numArgs);}
    };

    /**
     * Abstract base class for variable and function resolvers. A resolver
     * is used during parsing, and tells the parser how to convert variable
     * references and functions into Functor objects for the stored expression.
     * Methods should return nullptr or throw an exception with a human-readable
     * description when the variable or function cannot be resolved; this will
     * be converted to an error message.
     */
    class COMMON_API Resolver
    {
      public:
        virtual ~Resolver() {}
        /**
         * Should return nullptr or throw exception if variable is not found
         */
        virtual Functor *resolveVariable(const char *varName) = 0;
        /**
         * Should return nullptr or throw exception if variable is not found.
         * Does not need to check the argCount, because it is also done by the caller.
         */
        virtual Functor *resolveFunction(const char *functionName, int argCount) = 0;
    };

  protected:
    Elem *elems;
    int nelems;

    // workaround: when calling out-of-line methods of inner classes (Elem, Function etc)
    // from the Envir lib, VC++ 9.0 linker reports them as undefined symbols; workaround
    // is to delegate them to Expression.
    static void Elem_eq(Elem& e, const Elem& other);
    static int Elem_getNumArgs(const Elem& e);
    static std::string Elem_str(int type, bool b, double d, const char *s, Functor *fu, int op);
    static std::string Function_str(const char *name, std::string args[], int numArgs);

  private:
    void copy(const Expression& other);

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
    Expression(const Expression& other) {elems=nullptr; copy(other);}

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
     * Returns the Reverse Polish expression as an array of Elems.
     * The array size is getExpressionLength().
     */
    virtual const Elem *getExpression() const {return elems;}

    /**
     * Returns the length of the array that contains the Reverse Polish
     * expression. The array is returned by getExpression().
     */
    virtual int getExpressionLength() const {return nelems;}

    /**
     * Evaluate the expression, and return the results as a Value.
     * Throws an error if the expression has some problem (i.e. stack
     * overflow/underflow, "cannot cast", "function not found", etc.)
     */
    virtual Value evaluate() const;

    /**
     * Evaluate the expression and convert the result to bool if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual bool boolValue() const;

    /**
     * Evaluate the expression and convert the result to long if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual long longValue(const char *expectedUnit=nullptr) const;

    /**
     * Evaluate the expression and convert the result to double if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual double doubleValue(const char *expectedUnit=nullptr) const;

    /**
     * Evaluate the expression and convert the result to string if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual std::string stringValue() const;
    //@}

    /** @name Miscellaneous utility functions. */
    //@{
    /**
     * Converts the expression to string.
     */
    virtual std::string str() const;

    /**
     * Interprets the string as an expression, and stores it. Resolver
     * is used for translating variable and function references during
     * parsing. Throws exception on parse errors.
     */
    virtual void parse(const char *text, Resolver *resolver = nullptr);

    /**
     * Returns true if the expression is just a literal (or equivalent to one,
     * like "2+2").
     */
    virtual bool isAConstant() const;
    //@}
};


/**
 * Function object to implement all math.h functions.
 */
class COMMON_API MathFunction : public Expression::Function
{
  private:
    std::string funcname;
    double (*f)(...);
    int argcount;
  public:
    struct FuncDesc {const char *name; double (*f)(...); int argcount;};
  private:
    static FuncDesc functable[];
    static FuncDesc *lookup(const char *name);
  public:
    static bool supports(const char *name);
    MathFunction(const char *name);
    virtual ~MathFunction();
    virtual Functor *dup() const override;
    virtual const char *getName() const override;
    virtual const char *getArgTypes() const override;
    virtual char getReturnType() const override;
    virtual Expression::Value evaluate(Expression::Value args[], int numArgs) override;
};

} // namespace common
}  // namespace omnetpp


#endif


