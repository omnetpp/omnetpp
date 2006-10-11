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

class cXMLElement;
class cPar;
class cMathFunction;
class cNEDFunction;

/**
 * FIXME revise docu in the whole class!!!!!!
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
    class Elem
    {
      friend class cDynamicExpression;
      private:
        // Types:
        //  - bool
        //  - double (there's no long -- we calculate everything in double)
        //  - string
        //  - pointer to an "external" cXMLElement
        //  - pointer to an "external" cPar object
        //  - cMathFunction: function with 0/1/2/3/4 double arguments
        //  - cNEDFunction: function taking/returning StkValue (NEDFunction)
        //  - math operator (+-*/%^...)
        //
        enum {UNDEF, BOOL, DBL, STR, XML, CPAR, MATHFUNC, NEDFUNC, FUNCTOR, OP} type;
        union {
            bool b;
            double d;
            const char *s;
            cXMLElement *x;
            cPar *p;
            cMathFunction *f;
            cNEDFunction *af;
            Functor *fu;
            OpType op;
        };

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
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(double _d)  {type=DBL; d=_d;}

        /**
         * Effect during evaluation of the expression: pushes the given string
         * to the evaluation stack.
         */
        void operator=(const char *_s)  {type=STR; s=opp_strdup(_s);}

        /**
         * Effect during evaluation of the expression: pushes the given
         * cXMLElement pointer to the evaluation stack.
         */
        void operator=(cXMLElement *_x)  {type=XML; x=_x;}

        /**
         * Effect during evaluation of the expression: takes the value of
         * the cMessagePar object and pushes the value to the evaluation stack.
         * The cMessagePar is an "external" one: its ownership does not change.
         * This is how NED-language parameter references in expressions
         * are handled.
         */
        void operator=(cPar *_p)  {type=CPAR; ASSERT(_p); p=_p;}

        /**
         * Effect during evaluation of the expression: Call a function
         * taking 0..4 doubles and returning a double.
         */
        void operator=(cMathFunction *_f)  {type=MATHFUNC; ASSERT(_f); f=_f;}

        /**
         * Effect during evaluation of the expression: call a function
         * that function takes an array of StkValues and returns a StkValue.
         */
        void operator=(cNEDFunction *_f)  {type=NEDFUNC; ASSERT(_f); af=_f;}

        /**
         * Function object, with an interface not unlike cNEDFunction.
         * This object will be deleted by expression's destructor.
         */
        void operator=(Functor *_f)  {type=FUNCTOR; ASSERT(_f); fu=_f;}

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
        // Note: char codes need to be present and be consistent with cNEDFunction::argTypes()
        enum {UNDEF=0, BOOL='B', DBL='D', STR='S', XML='X'} type;
        bool bl;
        double dbl;
        std::string str;
        cXMLElement *xml;

        StkValue()  {type=UNDEF;}
        StkValue(bool b)  {*this=b;}
        StkValue(long l)  {*this=l;}
        StkValue(double d)  {*this=d;}
        StkValue(const char *s)  {*this=s;}
        StkValue(std::string s)  {*this=s;}
        StkValue(cXMLElement *x)  {*this=x;}
        StkValue(const cPar& par) {*this=par;}
        void operator=(bool b)  {type=BOOL; bl=b;}
        void operator=(long l)  {type=DBL; dbl=l;}
        void operator=(double d)  {type=DBL; dbl=d;}
        void operator=(const char *s)  {type=STR; str=s?s:"";}
        void operator=(std::string s)  {type=STR; str=s;}
        void operator=(cXMLElement *x)  {type=XML; xml=x;}
        void operator=(const cPar& par);
    };

    /**
     * Function object base class. We use function objects to handle NED parameter
     * references, "index" and "sizeof" operators, and references to NED "for" loop
     * variables.
     */
    class Functor : public cPolymorphic
    {
      public:
        int numArgs() {return strlen(argTypes());}
        virtual const char *argTypes() const = 0;
        virtual char returnType() const = 0;
        virtual StkValue evaluate(cComponent *context, StkValue args[], int numargs) = 0;
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
    explicit cDynamicExpression();

    /**
     * Copy constructor.
     */
    cDynamicExpression(const cDynamicExpression& other) {operator=(other);}

    /**
     * Destructor.
     */
    virtual ~cDynamicExpression();

    /**
     * Assignment operator.
     */
    cDynamicExpression& operator=(const cDynamicExpression& other);
    //@}

    /** @name Redefined cPolymorphic functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cDynamicExpression *dup() const  {return new cDynamicExpression(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
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
    virtual void setExpression(Elem e[], int nelems);

    /**
     * Evaluate the expression, and return the results as a StkValue.
     * Throws an error if the expression has some problem (i.e. stack
     * overflow/underflow, "cannot cast", "function not found", etc.)
     */
    virtual StkValue evaluate(cComponent *context=NULL) const;

    /**
     * Evaluate the expression and convert the result to bool if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual bool boolValue(cComponent *context=NULL);

    /**
     * Evaluate the expression and convert the result to long if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual long longValue(cComponent *context=NULL);

    /**
     * Evaluate the expression and convert the result to double if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual double doubleValue(cComponent *context=NULL);

    /**
     * Evaluate the expression and convert the result to string if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual std::string stringValue(cComponent *context=NULL);

    /**
     * Evaluate the expression and convert the result to an XML tree if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual cXMLElement *xmlValue(cComponent *context=NULL);
    //@}

    /** @name Miscellaneous utility functions. */
    //@{
    /**
     * Converts the expression to string.
     */
    virtual std::string toString() const;

    /**
     * Interprets the string as an expression, and stores it.
     * This may not be possible with all subclasses.
     */
    virtual bool parse(const char *text);
    //@}
};

namespace NEDSupport
{

typedef cDynamicExpression::StkValue StkValue; // abbreviation for local use

class ModuleIndex : public cDynamicExpression::Functor
{
  public:
    ModuleIndex();
    ModuleIndex *dup() const {return new ModuleIndex();}
    virtual const char *fullName() const {return "index";}
    virtual const char *argTypes() const {return "";}
    virtual char returnType() const {return 'L';}
    virtual StkValue evaluate(cComponent *context, StkValue args[], int numargs);
    virtual std::string toString(std::string args[], int numargs);
};

/**
 * Variations: parameter, parentParameter
 */
class ParameterRef : public cDynamicExpression::Functor
{
  protected:
    bool ofParent;  // if true, return parentModule->par(paramname)
    bool printThis; // whether toString() should prefix paramName with "this."
    std::string paramName;
  public:
    ParameterRef(const char *paramName, bool ofParent, bool printThis);
    ParameterRef *dup() const {return new ParameterRef(paramName.c_str(), ofParent, printThis);}
    virtual const char *fullName() const {return paramName.c_str();}
    virtual const char *argTypes() const {return "";}
    virtual char returnType() const {return '*';}
    virtual StkValue evaluate(cComponent *context, StkValue args[], int numargs);
    virtual std::string toString(std::string args[], int numargs);
};

/**
 * siblingModuleParameter, indexedSiblingModuleParameter
 */
class SiblingModuleParameterRef : public cDynamicExpression::Functor
{
  protected:
    bool ofParent;  // if true, return parentModule->submodule(...)->par(parname)
    std::string moduleName;
    bool withModuleIndex;
    std::string paramName;
  public:
    SiblingModuleParameterRef(const char *moduleName, const char *paramName, bool ofParent, bool withModuleIndex);
    SiblingModuleParameterRef *dup() const {return new SiblingModuleParameterRef(moduleName.c_str(), paramName.c_str(), ofParent, withModuleIndex);}
    virtual const char *fullName() const {return paramName.c_str();}
    virtual const char *argTypes() const {return withModuleIndex ? "L" : "";}
    virtual char returnType() const {return '*';}
    virtual StkValue evaluate(cComponent *context, StkValue args[], int numargs);
    virtual std::string toString(std::string args[], int numargs);
};

/*XXX TODO
static StkValue sizeofIdent(cComponent *context, StkValue args[], int numargs);
static StkValue sizeofGate(cComponent *context, StkValue args[], int numargs);
static StkValue sizeofParentModuleGate(cComponent *context, StkValue args[], int numargs);
static StkValue sizeofSiblingModuleGate(cComponent *context, StkValue args[], int numargs);
static StkValue sizeofIndexedSiblingModuleGate(cComponent *context, StkValue args[], int numargs);

class Sizeof : public Functor
{
  protected:
    bool ofParent;
    bool bstd::string paramName;
    std::string moduleName;
  public:
    virtual const char *argTypes() const {return "";}
    virtual char returnType() const {return 'L';}
    virtual StkValue evaluate(cComponent *context, StkValue args[], int numargs);
    virtual std::string toString(std::string args[], int numargs) {return "index";}
};
*/

};

#endif


