//==========================================================================
//   CMESSAGEPAR.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMESSAGEPAR_H
#define __CMESSAGEPAR_H

#include "cobject.h"
#include "cfunction.h"

#define SHORTSTR  27

class  cStatistic;
class  cXMLElement;


/**
 * Abstract base class for expressions, to be used with cMessagePar.
 *
 * @see cMessagePar
 */
class SIM_API cMPExpression  // FIXME do we still need this????
{
  public:
    virtual ~cMPExpression() {}
    virtual std::string getAsText() = 0;
    virtual bool parseText(const char *text) = 0;
    virtual cMPExpression *dup() = 0;
};

/**
 * Abstract base class for double-valued expressions. Currently
 * used by nedtool-generated C++ code, which contains
 * cDoubleExpression-based compiled expressions (used via cMessagePar)
 * for e.g. submodule parameters.
 *
 * The actual expression should be supplied by creating a subclass
 * and overriding evaluate().  The expression may use parameters
 * stored in data members of the class; parameters can be initialized
 * e.g. in the constructor.
 *
 * Example:
 *   <pre>
 *   class Expr12 : public cDoubleExpression {
 *       private:
 *           long p1;
 *           cMessagePar& p2;
 *       public:
 *           Expr12(long ap1, cMessagePar& ap2) : p1(ap1), p2(ap2) {}
 *           virtual double evaluate() {return 3*p1+p2;}
 *   };
 *   </pre>
 *
 * @see cMessagePar
 */
class SIM_API cDoubleExpression : public cMPExpression
{
  public:
    virtual std::string getAsText();
    virtual bool parseText(const char *text);
    virtual double evaluate() = 0;
};



/**
 * FIXME revise doc!!!
 *
 * cMessagePar instances can be attached to cMessage objects.
 *
 * cMessagePar supports several data types. Data types are identified by type
 * characters. The current data type is returned by type().
 *
 *     - basic types: <b>S</b> string, <b>B</b> bool, <b>L</b> long, <b>D</b> double
 *     - <b>F</b> math function (MathFuncNoArgs,MathFunc1Args,etc),
 *     - <b>X</b> reverse Polish expression (array of ExprElem objects),
 *     - <b>C</b> compiled expression (subclassed from cDoubleExpression),
 *     - <b>T</b> distribution from a cStatistic,
 *     - <b>P</b> pointer to cObject,
 *     - <b>I</b> indirection (refers to another cMessagePar)
 *     - <b>M</b> XML element (pointer to a cXMLElement)
 *
 * For all types, an input flag can be set. In this case,
 * the user will be asked to enter the value when the object's value
 * is first used. The prompt string can also be specified
 * for cMessagePar. If no prompt string is given, the object's
 * name will be displayed as prompt text.
 *
 * NOTE: forEachChild() ignores objects stored here such as cMessagePars in ExprElem
 * structs (type X), cObject pointed to (type P), cStatistic (type T)
 *
 * @ingroup SimCore
 * @see ExprElem
 */
class SIM_API cMessagePar : public cObject   // FIXME simplify and DEPRECATE this class!!!
{
  public:
    /**
     * One component in a (reversed Polish) expression in a cMessagePar;
     * see cMessagePar::setDoubleValue(ExprElem *,int).
     */
    struct ExprElem
    {
        // Type chars:
        //   D  double constant
        //   P  pointer to "external" cMessagePar (owned by someone else)
        //   R  "reference": the cMessagePar will be dup()'ped and the copy kept
        //   0/1/2/3  function with 0/1/2/3 arguments
        //   @  math operator (+-*%/^=!<{>}?); see cMessagePar::evaluate()
        //
        char type;    // D/P/R/0/1/2/3/@ (see above)
        union {
            double d;           // D
            cMessagePar *p;     // P/R
            MathFuncNoArg f0;   // 0
            MathFunc1Arg  f1;   // 1
            MathFunc2Args f2;   // 2
            MathFunc3Args f3;   // 3
            MathFunc4Args f4;   // 4
            char op;            // @, op = +-*/%^=!<{>}?
        };

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) on the evaluation stack.
         */
        void operator=(int _i)            {type='D'; d=_i;  }


        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) on the evaluation stack.
         */
        void operator=(short _i)            {type='D'; d=_i;  }

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) on the evaluation stack.
         */
        void operator=(long _l)           {type='D'; d=_l;  }

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) on the evaluation stack.
         */
        void operator=(double _d)         {type='D'; d=_d;  }

        /**
         * Effect during evaluation of the expression: takes the value of
         * the cMessagePar object  (a double) and pushes the value
         * on the evaluation stack. The cMessagePar is an "external"
         * one: its ownership does not change. This is how NED-language REF
         * parameters in expressions are handled.
         */
        void operator=(cMessagePar *_p)          {type='P'; p=_p;  }

        /**
         * Effect during evaluation of the expression: takes the value of
         * the cMessagePar object  (a double) and pushes the value
         * on the evaluation stack. The cMessagePar which evaluates this
         * expression will copy the cMessagePar for itself.
         */
        void operator=(cMessagePar& _r);         //{type='R'; p=(cMessagePar *)_r.dup();} See after cMessagePar!

        /**
         * The argument can be a pointer to a function that takes no arguments
         * and returns a double. Effect during evaluation of the expression:
         * the return value is pushed on the stack. See also the cMathFunction
         * class and the Define_Function() macro.
         *
         * The OMNeT++ functions generating random variables of different
         * distributions can also be used in ExprElem expressions.
         */
        void operator=(MathFuncNoArg _f)  {type='0'; f0=_f;}

        /**
         * The argument can be a pointer to a function that takes 1
         * double argument and returns a double (e.g. sqrt()).
         * Effect during evaluation of the expression: 1 double is popped from the
         * stack, the given function is called with them as arguments, and the return
         * value is pushed back on the stack. See also the cMathFunction
         * class and the Define_Function() macro.
         *
         * The OMNeT++ functions generating random variables of different
         * distributions can also be used in ExprElem expressions.
         */
        void operator=(MathFunc1Arg  _f)  {type='1'; f1=_f;}

        /**
         * The argument can be a pointer to a function that takes 2
         * double arguments and returns a double.
         * Effect during evaluation of the expression:
         * 2 doubles are popped from the stack,
         * the given function is called with them as arguments, and the return
         * value is pushed back on the stack. See also the cMathFunction
         * class and the Define_Function() macro.
         *
         * The OMNeT++ functions generating random variables of different
         * distributions can also be used in ExprElem expressions.
         */
        void operator=(MathFunc2Args _f)  {type='2'; f2=_f;}

        /**
         * The argument can be a pointer to a function that takes 3
         * double arguments and returns a double.
         * Effect during evaluation of the expression:
         * 3 doubles are popped from the stack,
         * the given function is called with them as arguments, and the return
         * value is pushed back on the stack. See also the cMathFunction
         * class and the Define_Function() macro.
         *
         * The OMNeT++ functions generating random variables of different
         * distributions can also be used in ExprElem expressions.
         */
        void operator=(MathFunc3Args _f)  {type='3'; f3=_f;}

        /**
         * The argument can be a pointer to a function that takes 4
         * double arguments and returns a double.
         * Effect during evaluation of the expression:
         * 4 doubles are popped from the stack,
         * the given function is called with them as arguments, and the return
         * value is pushed back on the stack. See also the cMathFunction
         * class and the Define_Function() macro.
         *
         * The OMNeT++ functions generating random variables of different
         * distributions can also be used in ExprElem expressions.
         */
        void operator=(MathFunc4Args _f)  {type='4'; f4=_f;}

        /**
         * Operation. During evaluation of the expression, two items (or three,
         * with '?') are popped out of the stack, the given operator
         * is applied to them and the result is pushed back on the stack.
         *
         * The operation can be:
         *     - + - * / add, subtract, multiply, divide
         *     - \% ^  modulo, power of
         *     - = !  equal, not equal
         *     - > }  greater, greater or equal
         *     - < {  less, less or equal
         *     - ?  inline if (the C/C++ ?: operator)
         */
        void operator=(char _op)          {type='@'; op=_op;}
    };

  protected:
    static char *possibletypes;
  private:
    char typechar;     // S/B/L/D/F/T/X/C/P/O/I
    bool inputflag;
    bool changedflag;
    bool tkownership;
    opp_string promptstr; // prompt text used when the value is being input

    union {    // Take care of 'operator=()' when changing this!!!
       struct { bool sht; char *str;            } ls;   // S:long string
       struct { bool sht; char str[SHORTSTR+1]; } ss;   // S:short str
       struct { long val;                       } lng;  // L:long,B:bool
       struct { double val;                     } dbl;  // D:double
       struct { MathFunc f; int argc;
                double p1,p2,p3,p4;             } func; // F:math function
       struct { cStatistic *res;                } dtr;  // T:distribution
       struct { cDoubleExpression *expr;        } cexpr;// C:compiled expression
       struct { ExprElem *xelem; int n;         } expr; // X:expression
       struct { cMessagePar *par;               } ind;  // I:indirection
       struct { void *ptr;
                VoidDelFunc delfunc;
                VoidDupFunc dupfunc;
                size_t itemsize;                } ptr;  // P:void* pointer
       struct { cObject *obj;                   } obj;  // O:object pointer
       struct { cXMLElement *node;              } xmlp; // M:XML element pointer
    };

  private:
    // helper func: destruct old value
    void deleteOld();

    // helper func: evaluates expression (X)
    double evaluate();

    // helper func: rand.num with given distr.(T)
    double fromstat();

    // setFromText() helper func.
    bool setfunction(char *w);

  protected:
    /** @name Event hooks */
    //@{

    /**
     * Called each time before the value of this object changes.
     * It can be used for tracking parameter changes.
     * This default implementation does nothing.
     */
    virtual void beforeChange();

    /**
     * Called each time after the value of this object changed.
     * It can be used for tracking parameter changes.
     * This default implementation does nothing.
     */
    virtual void afterChange();
    //@}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor, creates an exact copy of the argument.
     */
    cMessagePar(const cMessagePar& other);

    /**
     * Constructor, creates a cMessagePar with the given name and long
     * ('L') as default type.
     */
    explicit cMessagePar(const char *name=NULL);

    /**
     * Constructor, creates a copy of the second argument with another
     * name.
     */
    explicit cMessagePar(const char *name, cMessagePar& other);

    /**
     * Destructor.
     */
    virtual ~cMessagePar();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's
     * operator=() for more details.
     *
     * The behavior with redirected cMessagePar objects is the following. This function
     * copies the contents of the other object (whether it is redirected or not)
     * into this object, <b>or,</b> if this object is redirected, into the object
     * this object refers to. This means that if you want to overwrite this
     * very object (and not the one it points to), you have to use
     * cancelRedirection() first.
     */
    cMessagePar& operator=(const cMessagePar& otherpar);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cMessagePar *dup() const  {return new cMessagePar(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(std::ostream& os);

    /**
     * Calls v->visit(this) for the contained object, if there's any.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Setter functions. Note that overloaded assignment operators also exist. */
    //@{

    /**
     * Sets the value to the given bool value.
     */
    cMessagePar& setBoolValue(bool b);

    /**
     * Sets the value to the given long value.
     */
    cMessagePar& setLongValue(long l);

    /**
     * Sets the value to the given string value.
     * The cMessagePar will make its own copy of the string. NULL is also accepted
     * and treated as an empty string.
     */
    cMessagePar& setStringValue(const char *s);

    /**
     * Sets the value to the given double value.
     */
    cMessagePar& setDoubleValue(double d);

    /**
     * Sets the value to the given distribution.
     * Every time the cMessagePar's value is asked a random number produced
     * by res.random() will be returned.
     */
    cMessagePar& setDoubleValue(cStatistic *res);

    /**
     * Sets the value to the given Reverse Polish expression, specified as
     * an array of ExprElem structs.
     * Every time the cMessagePar's value is asked the expression will be
     * evaluated using a stack machine. The stack machine calculates
     * in doubles.
     *
     * Example: the NED expression <i>(count+1)/2</i> would be represented
     * in the following way:
     *
     * \code
     * cMessagePar::ExprElem *expression = new cMessagePar::ExprElem[5];
     * expression[0] = &(mod->par("count")); // ptr to module parameter
     * expression[1] = 1;
     * expression[2] = '+';
     * expression[3] = 2;
     * expression[4] = '/';
     * param.setDoubleValue(expression,5);
     * \endcode
     */
    cMessagePar& setDoubleValue(ExprElem *x, int n);

    /**
     * Sets the value to the given compiled expression, subclassed from
     * cDoubleExpression.
     * Every time the cMessagePar's value is asked, the evaluate() function of
     * cDoubleExpression will be called. The passed object will be
     * deallocated (using operator delete) from the cMessagePar destructor, and
     * also when the cMessagePar object is assigned another value.
     */
    cMessagePar& setDoubleValue(cDoubleExpression *expr);

    /**
     * Sets the value to the given math function with no arguments.
     * Every time the cMessagePar's value is asked the function will be called.
     */
    cMessagePar& setDoubleValue(MathFuncNoArg f);

    /**
     * Sets the value to the given math function with one argument.
     * Every time the cMessagePar's value is asked the function will be called
     * with p1 as an argument.
     */
    cMessagePar& setDoubleValue(MathFunc1Arg  f, double p1);

    /**
     * Sets the value to the given math function with two arguments.
     * Every time the cMessagePar's value is asked the function will be called
     * with p1 and p2 as an arguments.
     */
    cMessagePar& setDoubleValue(MathFunc2Args f, double p1, double p2);

    /**
     * Sets the value to the given math function with three arguments.
     * Every time the cMessagePar's value is asked the function will be called
     * with p1, p2 and p3 as an arguments.
     */
    cMessagePar& setDoubleValue(MathFunc3Args f, double p1, double p2, double p3);

    /**
     * Sets the value to the given math function with four arguments.
     * Every time the cMessagePar's value is asked the function will be called
     * with p1, p2, p3 and p4 as an arguments.
     */
    cMessagePar& setDoubleValue(MathFunc4Args f, double p1, double p2, double p3, double p4);

    /**
     * Sets the value to the given pointer. The ownership of the block
     * pointer to can be controlled using configPointer().
     *
     * @see configPointer
     */
    cMessagePar& setPointerValue(void *ptr);

    /**
     * Sets the value to the given object. Whether cMessagePar will take the
     * ownership of the object depends on the takeOwnership() flag.
     */
    cMessagePar& setObjectValue(cObject *obj);

    /**
     * Sets the value to the given cXMLElement.
     */
    cMessagePar& setXMLValue(cXMLElement *node);

    /**
     * Configures memory management for the void* pointer ('P') type.
     * Similar to cLinkedList::configPointer().
     *
     * <TABLE BORDER=1>
     *   <TR>
     *     <TD><b>delfunc</b></TD><TD><b>dupfunc.</b></TD><TD><b>itemsize</b></TD><TD><b>behavior</b></TD>
     *   </TR>
     *   <TR>
     *     <TD>NULL</TD><TD>NULL</TD><TD>0</TD><TD>Pointer is treated as mere pointer - no memory management. Duplication copies the pointer, and deletion does nothing.</TD>
     *   </TR>
     *   <TR>
     *     <TD>NULL</TD><TD>NULL</TD><TD>!=0</TD><TD>Plain memory management. Duplication is done with new char[size]+memcpy(), and deletion is done via delete.</TD>
     *   </TR>
     *   <TR>
     *     <TD>NULL or user's delete func.</TD><TD>user's dupfunc.</TD><TD>indifferent</TD><TD WIDTH=317>Sophisticated memory management. Duplication is done by calling the user-supplied duplication function, which should do the allocation and the appropriate copying. Deletion is done by calling the user-supplied delete function, or the delete operator if it is not supplied.</TD>
     *   </TR>
     * </TABLE>
     */
    void configPointer( VoidDelFunc delfunc, VoidDupFunc dupfunc, size_t itemsize=0);

    /**
     * Sets the flag that determines whether setObjectValue(cObject *) and
     * setDoubleValue(cStatistic *) should automatically take ownership of
     * the objects.
     */
    void takeOwnership(bool tk) {tkownership=tk;}

    /**
     * Returns the takeOwnership flag, see takeOwnership().
     */
    bool takeOwnership() const   {return tkownership;}
    //@}

    /** @name Getter functions. Note that overloaded conversion operators also exist. */
    //@{

    /**
     * Returns value as a boolean. The cMessagePar type must be bool (B) or a numeric type.
     */
    bool boolValue();

    /**
     * Returns value as long. The cMessagePar type must be types long (L),
     * double (D), Boolean (B), function (F), distribution (T),
     * compiled expression (C) or expression (X).
     */
    long longValue();

    /**
     * Returns value as const char *. Only for string (S) type.
     */
    const char *stringValue();

    /**
     * Returns value as double. The cMessagePar type must be types long (L),
     * double (D), function (F), Boolean (B), distribution (T),
     * compiled expression (C) or expression (X).
     */
    double doubleValue();

    /**
     * Returns value as a void * pointer. The cMessagePar type must be pointer (P).
     */
    void *pointerValue();

    /**
     * Returns value as pointer to cObject. The cMessagePar type must be pointer (O).
     */
    cObject *objectValue();

    /**
     * Returns value as pointer to cXMLElement. The cMessagePar type must be XML (M).
     */
    cXMLElement *xmlValue();
    //@}

    /** @name Redirection */
    //@{

    /**
     * Creates a redirection to another cMessagePar. A cMessagePar object can be set
     * to stand for a value actually stored in another cMessagePar object.
     * This is called indirect or redirected value. When using redirection,
     * every operation on the value (i.e. reading or changing it)
     * will be actually done to the other cMessagePar object.
     */
    cMessagePar& setRedirection(cMessagePar *par);

    /**
     * Returns true if this object is redirected to another cMessagePar.
     */
    bool isRedirected() const {return typechar=='I';}

    /**
     * Returns NULL if the cMessagePar's value is not redirected to another cMessagePar;
     * otherwise it returns the pointer of that cMessagePar.
     * This function and isRedirected() are the only ways to determine
     * if an object is redirected or not (type() returns the type of
     * the other cMessagePar: 'D', 'L' etc).
     */
    cMessagePar *redirection();

    /**
     * Break the redirection. The new type will be long ('L').
     */
    void cancelRedirection();
    //@}

    /** @name Type, prompt text, input flag, change flag. */
    //@{

    /**
     * Returns type character. If the "real" type is 'I',
     * it returns the type of the object it is redirected to (for example,
     * 'D', 'L', etc.)
     */
    char type() const;

    /**
     * Returns true if the stored value is of a numeric type.
     */
    bool isNumeric() const;

    /**
     * Returns true if the value is constant. It returns true if
     * the type is 'L', 'D', 'B', or 'S', that is, the object stores
     * a long, double, boolean or string constant.
     */
    bool isConstant() const;

    /**
     * Returns the prompt text or NULL.
     */
    const char *prompt() ;

    /**
     * Sets the prompt text.
     */
    void setPrompt(const char *s);

    /**
     * Sets (ip=true) or clears (ip=false) the input flag.
     */
    void setInput(bool ip);

    /**
     * Returns true if the parameter is of input type (the input
     * flag is set).
     */
    bool isInput() const;

    /**
     * Returns true if the value has changed since the last changed() call.
     * Side effect: clears the 'changed' flag, so a next call will return
     * false.
     */
    bool changed();
    //@}

    /** @name Utility functions. */
    //@{

    /**
     * Reads the object value from the ini file or from the user.
     */
    cMessagePar& read();

    /**
     * Replaces the object value with its evaluation (a double).
     * Equivalent to <tt>setValue('D', this->doubleValue())</tt>.
     */
    void convertToConst();

    /**
     * Compares the stored values. The two objects must have the same type character
     * and the same value to be equal.
     */
    bool equalsTo(cMessagePar *par);
    //@}

    /** @name Convert to/from text representation. */
    //@{

    /**
     * Returns the value in text form.
     */
    virtual std::string getAsText() const;

    /**
     * This function tries to interpret the argument text as a type
     * typed value (L=long, S=string, etc). type=='?' (the default)
     * means that the type is to be auto-selected. On success,
     * cMessagePar is updated with the new value and true is returned,
     * otherwise the function returns false. No error message is generated.
     */
    virtual bool setFromText(const char *text, char type='?');  // FIXME remove this... no one uses it
    //@}

    /** @name Overloaded assignment and conversion operators. */
    //@{

    /**
     * Equivalent to setBoolValue().
     */
    cMessagePar& operator=(bool b)          {return setBoolValue(b);}

    /**
     * Equivalent to setStringValue().
     */
    cMessagePar& operator=(const char *s)   {return setStringValue(s);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMessagePar& operator=(char c)          {return setLongValue((long)c);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMessagePar& operator=(unsigned char c) {return setLongValue((long)c);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMessagePar& operator=(int i)           {return setLongValue((long)i);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMessagePar& operator=(unsigned int i)  {return setLongValue((long)i);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMessagePar& operator=(short i)  {return setLongValue((long)i);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMessagePar& operator=(unsigned short i)  {return setLongValue((long)i);}

    /**
     * Equivalent to setLongValue().
     */
    cMessagePar& operator=(long l)          {return setLongValue(l);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cMessagePar& operator=(unsigned long l) {return setLongValue((long)l);}

    /**
     * Equivalent to setDoubleValue().
     */
    cMessagePar& operator=(double d)        {return setDoubleValue(d);}

    /**
     * Converts the argument to double, and calls setDoubleValue().
     */
    cMessagePar& operator=(long double d)   {return setDoubleValue((double)d);}

    /**
     * Equivalent to setPointerValue().
     */
    cMessagePar& operator=(void *ptr)       {return setPointerValue(ptr);}

    /**
     * Equivalent to setObjectValue().
     */
    cMessagePar& operator=(cObject *obj)    {return setObjectValue(obj);}

    /**
     * Equivalent to setXMLValue().
     */
    cMessagePar& operator=(cXMLElement *node) {return setXMLValue(node);}

    /**
     * Equivalent to boolValue().
     */
    operator bool()          {return boolValue();}

    /**
     * Equivalent to stringValue().
     */
    operator const char *()  {return stringValue();}

    /**
     * Calls longValue() and converts the result to char.
     */
    operator char()          {return (char)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned char.
     */
    operator unsigned char() {return (unsigned char)longValue();}

    /**
     * Calls longValue() and converts the result to int.
     */
    operator int()           {return (int)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned int.
     */
    operator unsigned int()  {return (unsigned int)longValue();}

    /**
     * Calls longValue() and converts the result to short.
     */
    operator short()  {return (short)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned short.
     */
    operator unsigned short()  {return (unsigned short)longValue();}

    /**
     * Equivalent to longValue().
     */
    operator long()          {return longValue();}

    /**
     * Calls longValue() and converts the result to unsigned long.
     */
    operator unsigned long() {return longValue();}

    /**
     * Equivalent to doubleValue().
     */
    operator double()        {return doubleValue();}

    /**
     * Calls doubleValue() and converts the result to long double.
     */
    operator long double()   {return doubleValue();}

    /**
     * Equivalent to pointerValue().
     */
    operator void *()        {return pointerValue();}

    /**
     * Equivalent to objectValue().
     */
    operator cObject *()     {return objectValue();}

    /**
     * Equivalent to xmlValue().
     */
    operator cXMLElement *() {return xmlValue();}

    //@}

    /** @name Compare function */
    //@{

    /**
     * Compares two cMessagePars by their value if they are numeric.
     * This function can be used to sort cMessagePar objects in a priority
     * queue.
     */
    static int cmpbyvalue(cObject *one, cObject *other);
    //@}
};

// this function cannot be defined within ExprElem because of declaration order
inline void cMessagePar::ExprElem::operator=(cMessagePar& _r)  {
    type='R'; p=(cMessagePar *)_r.dup();
}

inline std::ostream& operator<< (std::ostream& os, const cMessagePar& o) {
    return os << o.getAsText();
}

inline std::ostream& operator<< (std::ostream& os, cMessagePar& o) {
    return os << o.getAsText();
}

#endif


