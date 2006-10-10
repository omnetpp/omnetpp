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
     * One component in a (reversed Polish) expression
     */
    class Elem
    {
      friend class cDynamicExpression;
      private:
        // Type chars:   FIXME use enum
        //   B  bool
        //   D  double (there's no long -- we calculate everything in double)
        //   S  string
        //   X  pointer to an "external" cXMLElement
        //   P  pointer to an "external" parameter object
        //   F  cMathFunction: function with 0/1/2/3/4 double arguments
        //   A  cNEDFunction: function taking/returning StkValue (NEDFunction)
        //   @  math operator (+-*%/^=!<{>}?); see operator=(char)
        //
        char type;    // B,D,S... (see above)
        union {
            bool b;           // B
            double d;         // D
            const char *s;    // S
            cXMLElement *x;   // X
            cPar *p;          // P
            cMathFunction *f; // F
            cNEDFunction *af; // A
            char op;          // @, op = +-*/%^=!<{>}?...
        };

      public:
        Elem()  {type=0;}
        Elem(const Elem& other)  {type='0'; operator=(other);}
        ~Elem()  {if (type=='S') delete [] s;}

        /**
         * Assignment operator -- we need to copy Elem at a hundred places
         */
        void operator=(const Elem& other);

        /**
         * Effect during evaluation of the expression: pushes the given boolean
         * constant to the evaluation stack.
         */
        void operator=(bool _b)  {type='B'; b=_b;}

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(int _i)  {type='D'; d=_i;}

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(short _i)  {type='D'; d=_i;}

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(long _l)  {type='D'; d=_l;}

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) to the evaluation stack.
         */
        void operator=(double _d)  {type='D'; d=_d;}

        /**
         * Effect during evaluation of the expression: pushes the given string
         * to the evaluation stack.
         */
        void operator=(const char *_s)  {type='S'; s=opp_strdup(_s);}

        /**
         * Effect during evaluation of the expression: pushes the given
         * cXMLElement pointer to the evaluation stack.
         */
        void operator=(cXMLElement *_x)  {type='X'; x=_x;}

        /**
         * Effect during evaluation of the expression: takes the value of
         * the cMessagePar object and pushes the value to the evaluation stack.
         * The cMessagePar is an "external" one: its ownership does not change.
         * This is how NED-language parameter references in expressions
         * are handled.
         */
        void operator=(cPar *_p)  {type='P'; ASSERT(_p); p=_p;}

        /**
         * Effect during evaluation of the expression: Call a function
         * taking 0..4 doubles and returning a double.
         */
        void operator=(cMathFunction *_f)  {type='F'; ASSERT(_f); f=_f;}

        /**
         * Effect during evaluation of the expression: call a function
         * that function takes an array of StkValues and returns a StkValue.
         */
        void operator=(cNEDFunction *_f)  {type='A'; ASSERT(_f); af=_f;}

        /**
         * Operation. During evaluation of the expression, two items (or three,
         * with '?') are popped out of the stack, the given operator
         * is applied to them and the result is pushed back on the stack.
         *
         * Supported operations:
         *     - + - * /  add, subtract, multiply, divide ("+" is also string concatenation)
         *     - \% ^     modulo, power of
         *     - = !      equal, not equal
         *     - > }      greater, greater or equal
         *     - < {      less, less or equal
         *     - ?        inline if (the C/C++ ?: operator)
         *     - A O X N  logical and, or, xor, not
         *     - & | # ~  bitwise and, or, xor, not (1's complement)
         *     - M        negation (-1)
         *     - L R      left shift, right shift
         */
        void operator=(char _op)  {type='@'; op=_op;}
    };

    /**
     * The dynamic expression evaluator calculates in StkValues.
     * There's no "long" field in it: all numeric calculations are performed
     * in double. XXX This is fine for 32-bit longs, but not for 64-bit ones,
     * as double's mantissa is only 53 bits.
     */
    struct StkValue
    {
        enum {UNDEF=0, BOOL='B', DBL='D', STR='S', XML='X'} type;
        bool bl;
        double dbl;
        std::string str;
        cXMLElement *xml;

        StkValue()  {type=UNDEF;}
        StkValue(bool b)  {*this=b;}
        StkValue(long l)  {*this=l;}
        StkValue(double d)  {*this=d;}
        StkValue(std::string s)  {*this=s;}
        StkValue(cXMLElement *x)  {*this=x;}
        StkValue(const cPar& par) {*this=par;}
        void operator=(bool b)  {type=BOOL; bl=b;}
        void operator=(long l)  {type=DBL; dbl=l;}
        void operator=(double d)  {type=DBL; dbl=d;}
        void operator=(std::string s)  {type=STR; str=s;}
        void operator=(cXMLElement *x)  {type=XML; xml=x;}
        void operator=(const cPar& par);
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
     * Reads the object value from the ini file or from the user.
     * FIXME clarify this.... (or maybe remove this function)
     */
    virtual cDynamicExpression& read();

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

#endif


