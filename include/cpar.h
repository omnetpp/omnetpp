//==========================================================================
//   CPAR.H   - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cPar       : general value holding class
//    cModulePar : specialized cPar that serves as module parameter
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPAR_H
#define __CPAR_H

#include "cobject.h"

#define SHORTSTR  27


//=== classes declared here:
class  cPar;
class  cModulePar;

//=== class mentioned
class  cStatistic;


//==========================================================================

/**
 * Abstract base class for expressions, to be used with cPar.
 *
 * @see cPar
 */
class cExpression
{
  public:
    virtual ~cExpression() {}
    virtual void getAsText(char *buf, int maxlen) = 0;
    virtual bool parseText(const char *text) = 0;
    virtual cExpression *dup() = 0;
};

/**
 * Abstract base class for double-valued expressions. Currently
 * used by the new, nedxml-based nedc, which generates
 * cDoubleExpression-based compiled expressions (used via cPar)
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
 *           cPar& p2;
 *       public:
 *           Expr12(long ap1, cPar& ap2) : p1(ap1), p2(ap2) {}
 *           virtual double evaluate() {return 3*p1+p2;}
 *   };
 *   </pre>
 *
 * @see cPar
 */
class cDoubleExpression : public cExpression
{
  public:
    virtual void getAsText(char *buf, int maxlen);
    virtual bool parseText(const char *text);
    virtual double evaluate() = 0;
};

//==========================================================================

/**
 * Parameter class are designed to hold a value.
 *
 * Types and type characters:
 *
 * <UL>
 *   <LI> basic types: C char, S string, L long, D double
 *   <LI> F math function (MathFuncNoArgs,MathFunc1Args,etc),
 *   <LI> X expression (table of ExprElems),
 *   <LI> C compiled expression (subclassed from cDoubleExpression),
 *   <LI> T distribution from a cStatistic,
 *   <LI> P pointer to cObject,
 *   <LI> I indirection (refers to another cPar)
 * </UL>
 *
 * For all types, an input flag can be set. In this case,
 * the user will be asked to enter the value when the object's value
 * is first used. The prompt string can also be specified
 * for cPar. If no prompt string is given, the object's
 * name will be displayed as prompt text.
 *
 * NOTE: forEach() ignores objects stored here such as cPars in eXpressions,
 * cObject pointed to in 'P' type, cStatistic in disTribution type
 *
 * @ingroup SimCore
 * @see ExprElem
 */
class SIM_API cPar : public cObject
{
  public:
    /**
     * One component in a (reversed Polish) expression in a cPar.
     *
     * If the value of the cPar is of expression type, the expression
     * must be converted to reversed Polish form. The reversed Polish
     * form expression is stored in a vector of ExprElem structures.
     */
    struct ExprElem
    {
        // Type chars:
        //   D  double constant
        //   P  pointer to "external" cPar (owned by someone else)
        //   R  "reference": the cPar will be dup()'ped and the copy kept
        //   0/1/2/3  function with 0/1/2/3 arguments
        //   @  math operator (+-*%/^=!<{>}?); see cPar::evaluate()
        //
        char type;    // D/P/R/0/1/2/3/@ (see above)
        union {
            double d;           // D
            cPar *p;            // P/R
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
        void operator=(long _l)           {type='D'; d=_l;  }

        /**
         * Effect during evaluation of the expression: pushes the given number
         * (which is converted to double) on the evaluation stack.
         */
        void operator=(double _d)         {type='D'; d=_d;  }

        /**
         * Effect during evaluation of the expression: takes the value of
         * the cPar object  (a double) and pushes the value
         * on the evaluation stack. The cPar is an "external"
         * one: its ownership does not change. This is how NED-language REF
         * parameters in expressions are handled.
         */
        void operator=(cPar *_p)          {type='P'; p=_p;  }

        /**
         * Effect during evaluation of the expression: takes the value of
         * the cPar object  (a double) and pushes the value
         * on the evaluation stack. The cPar which evaluates this
         * expression will copy the cPar for itself.
         */
        void operator=(cPar& _r);         //{type='R'; p=(cPar *)_r.dup();} See after cPar!

        /**
         * The argument can be a pointer to a function that takes no arguments
         * and returns a double. Effect during evaluation of the expression:
         * the return value is pushed on the stack. See also the cFunctionType
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
         * value is pushed back on the stack. See also the cFunctionType
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
         * value is pushed back on the stack. See also the cFunctionType
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
         * value is pushed back on the stack. See also the cFunctionType
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
         * value is pushed back on the stack. See also the cFunctionType
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
         * <UL>
         *   <LI> + - * / add, subtract, multiply, divide
         *   <LI> % ^  modulo, power of
         *   <LI> = !  equal, not equal
         *   <LI> > }  greater, greater or equal
         *   <LI> < {  less, less or equal
         *   <LI> ?  inline if (the C/C++ ?: operator)
         * </UL>
         */
        void operator=(char _op)          {type='@'; op=_op;}
    };

  protected:
    static char *possibletypes;
  private:
    char typechar;     // S/B/L/D/F/T/X/C/P/O/I
    bool inputflag;
    bool changedflag;
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
       struct { cPar *par;                      } ind;  // I:indirection
       struct { void *ptr;
                VoidDelFunc delfunc;
                VoidDupFunc dupfunc;
                size_t itemsize;                } ptr;  // P:void* pointer
       struct { cObject *obj;                   } obj;  // O:object pointer
    };

  private:
    // helper func: destruct old value
    void deleteold();

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
    cPar(const cPar& other);

    /**
     * Constructor, creates a cPar with the given name and long
     * ('L') as default type.
     */
    explicit cPar(const char *name=NULL);

    /**
     * Constructor, creates a copy of the second argument with another
     * name.
     */
    explicit cPar(const char *name, cPar& other);

    /**
     * Destructor.
     */
    virtual ~cPar();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's
     * operator=() for more details.
     *
     * The behavior with redirected cPar objects is the following. This function
     * copies the contents of the other object (whether it is redirected or not)
     * into this object, <b>or,</b> if this object is redirected, into the object
     * this object refers to. This means that if you want to overwrite this
     * very object (and not the one it points to), you have to use
     * cancelRedirection() first.
     */
    cPar& operator=(const cPar& otherpar);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const   {return new cPar(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(ostream& os);

    /**
     * Calls the given function for contained object, if there's any.
     */
    virtual void forEach(ForeachFunc f);

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netUnpack();
    //@}

    /** @name Setter functions. Note that overloaded assignment operators also exist. */
    //@{

    /**
     * Sets the value to the given bool value.
     */
    cPar& setBoolValue(bool b);

    /**
     * Sets the value to the given long value.
     */
    cPar& setLongValue(long l);

    /**
     * Sets the value to the given string value.
     * The cPar will make its own copy of the string. NULL is also accepted
     * and treated as an empty string.
     */
    cPar& setStringValue(const char *s);

    /**
     * Sets the value to the given double value.
     */
    cPar& setDoubleValue(double d);

    /**
     * Sets the value to the given distribution.
     * Every time the cPar's value is asked a random number produced
     * by res.random() will be returned.
     */
    cPar& setDoubleValue(cStatistic *res);

    /**
     * Sets the value to the given expression.
     * Every time the cPar's value is asked the expression will be
     * evaluated.
     */
    cPar& setDoubleValue(ExprElem *x, int n);

    /**
     * Sets the value to the given compiled expression.
     * Every time the cPar's value is asked, the evaluate() function of
     * cDoubleExpression will be called. The passed object will be
     * deallocated (using operator delete) from the cPar destructor, and
     * also when the cPar object is assigned another value.
     */
    cPar& setDoubleValue(cDoubleExpression *expr);

    /**
     * Sets the value to the given math function with no arguments.
     * Every time the cPar's value is asked the function will be called.
     */
    cPar& setDoubleValue(MathFuncNoArg f);

    /**
     * Sets the value to the given math function with one argument.
     * Every time the cPar's value is asked the function will be called
     * with p1 as an argument.
     */
    cPar& setDoubleValue(MathFunc1Arg  f, double p1);

    /**
     * Sets the value to the given math function with two arguments.
     * Every time the cPar's value is asked the function will be called
     * with p1 and p2 as an arguments.
     */
    cPar& setDoubleValue(MathFunc2Args f, double p1, double p2);

    /**
     * Sets the value to the given math function with three arguments.
     * Every time the cPar's value is asked the function will be called
     * with p1, p2 and p3 as an arguments.
     */
    cPar& setDoubleValue(MathFunc3Args f, double p1, double p2, double p3);

    /**
     * Sets the value to the given math function with four arguments.
     * Every time the cPar's value is asked the function will be called
     * with p1, p2, p3 and p4 as an arguments.
     */
    cPar& setDoubleValue(MathFunc4Args f, double p1, double p2, double p3, double p4);

    /**
     * Sets the value to the given pointer. The ownership of the block
     * pointer to can be controlled using configPointer().
     *
     * @see configPointer
     */
    cPar& setPointerValue(void *ptr);

    /**
     * Sets the value to the given object. Whether cPar will take the
     * ownership of the object depends on the takeOwnership() flag
     * (see cObject).
     */
    cPar& setObjectValue(cObject *obj);

    /**
     * Configures memory management for the void* pointer ('P') type.
     * Similar to cLinkedList's configPointer() function.
     *
     * <TABLE BORDER=1>
     *   <TR>
     *     <TD><b>delete func.</b></TD><TD><b>dupl.func.</b></TD><TD><b>itemsize</b></TD><TD><b>behavior</b></TD>
     *   </TR>
     *   <TR>
     *     <TD>NULL</TD><TD>NULL</TD><TD>0</TD><TD>Pointer is treated as mere pointer - no memory management. Duplication copies the pointer, and deletion does nothing.</TD>
     *   </TR>
     *   <TR>
     *     <TD>NULL</TD><TD>NULL</TD><TD>0 size</TD><TD>Plain memory management. Duplication is done with new char[size]+memcpy(), and deletion is done via delete.</TD>
     *   </TR>
     *   <TR>
     *     <TD>NULL or user's delete func.</TD><TD>user's dupfunc.</TD><TD>indifferent</TD><TD WIDTH=317>Sophisticated memory management. Duplication is done by calling the user-supplied duplication function, which should do the allocation and the appropriate copying. Deletion is done by calling the user-supplied delete function, or the delete operator if it is not supplied.</TD>
     *   </TR>
     * </TABLE>
     */
    void configPointer( VoidDelFunc delfunc, VoidDupFunc dupfunc, size_t itemsize=0);
    //@}

    /** @name Getter functions. Note that overloaded conversion operators also exist. */
    //@{

    /**
     * Returns value as a boolean. The cPar type must be bool (B) or a numeric type.
     */
    bool boolValue();

    /**
     * Returns value as long. The cPar type must be types long (L),
     * double (D), Boolean (B), function (F), distribution (T),
     * compiled expression (C) or expression (X).
     */
    long longValue();

    /**
     * Returns value as const char *. Only for string (S) type.
     */
    const char *stringValue();

    /**
     * Returns value as double. The cPar type must be types long (L),
     * double (D), function (F), Boolean (B), distribution (T),
     * compiled expression (C) or expression (X).
     */
    double doubleValue();

    /**
     * Returns value as a void * pointer. The cPar type must be pointer (P).
     */
    void *pointerValue();

    /**
     * Returns value as pointer to cObject. The cPar type must be pointer (O).
     */
    cObject *objectValue();
    //@}

    /** @name Redirection */
    //@{

    /**
     * Creates a redirection to another cPar. A cPar object can be set
     * to stand for a value actually stored in another cPar object.
     * This is called indirect or redirected value. When using redirection,
     * every operation on the value (i.e. reading or changing it)
     * will be actually done to the other cPar object.
     */
    cPar& setRedirection(cPar *par);

    /**
     * Returns true if this object is redirected to another cPar.
     */
    bool isRedirected() const {return typechar=='I';}

    /**
     * Returns NULL if the cPar's value is not redirected to another cPar;
     * otherwise it returns the pointer of that cPar.
     * This function and isRedirected() are the only ways to determine
     * if an object is redirected or not (type() returns the type of
     * the other cPar: 'D', 'L' etc).
     */
    cPar *redirection();

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
    cPar& read();

    /**
     * Replaces the object value with its evaluation (a double).
     * Implemented as something like setValue('D', this->doubleValue()).
     */
    void convertToConst();

    /**
     * Compares the stored values. The two objects must have the same type character
     * and the same value to be equal.
     */
    bool equalsTo(cPar *par);
    //@}

    /** @name Convert to/from text representation. */
    //@{

    /**
     * Places the value in text format it into buffer buf which
     * is maxlen characters long.
     */
    virtual void getAsText(char *buf, int maxlen);

    /**
     * This function tries to interpret the argument text as a type
     * typed value. type=='?' means that the type is to be auto-selected.
     * On success, cPar is updated with the new value and true
     * is returned, otherwise the function returns false. No
     * error message is generated.
     */
    virtual bool setFromText(const char *text, char tp);
    //@}

    /** @name Overloaded assignment and conversion operators. */
    //@{

    /**
     * Equivalent to setBoolValue().
     */
    cPar& operator=(bool b)          {return setBoolValue(b);}

    /**
     * Equivalent to setStringValue().
     */
    cPar& operator=(const char *s)   {return setStringValue(s);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cPar& operator=(char c)          {return setLongValue((long)c);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cPar& operator=(unsigned char c) {return setLongValue((long)c);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cPar& operator=(int i)           {return setLongValue((long)i);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cPar& operator=(unsigned int i)  {return setLongValue((long)i);}

    /**
     * Equivalent to setLongValue().
     */
    cPar& operator=(long l)          {return setLongValue(l);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cPar& operator=(unsigned long l) {return setLongValue((long)l);}

    /**
     * Equivalent to setDoubleValue().
     */
    cPar& operator=(double d)        {return setDoubleValue(d);}

    /**
     * Converts the argument to double, and calls setDoubleValue().
     */
    cPar& operator=(long double d)   {return setDoubleValue((double)d);}

    /**
     * Equivalent to setPointerValue().
     */
    cPar& operator=(void *ptr)       {return setPointerValue(ptr);}

    /**
     * Equivalent to setObjectValue().
     */
    cPar& operator=(cObject *obj)    {return setObjectValue(obj);}

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
    //@}

    /** @name Compare function */
    //@{

    /**
     * Compares two cPars by their value if they are numeric.
     * This function can be used to sort cPar objects in a priority
     * queue.
     */
    static int cmpbyvalue(cObject *one, cObject *other);
    //@}
};

// this function cannot be defined within ExprElem because of declaration order
inline void cPar::ExprElem::operator=(cPar& _r)  {type='R'; p=(cPar *)_r.dup();}

//==========================================================================

/**
 * Module parameter object. This is specialized version of cPar: it is capable
 * of logging parameter changes to file.
 *
 * NOTE: dup() creates only a cPar, NOT a cModulePar!
 *
 * @ingroup Internals
 */
class SIM_API cModulePar : public cPar
{
    friend class cModule;
  private:
    cModule *omodp;              // owner module

  private:
    // helper function
    void _construct();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    cModulePar(const cPar& other);

    /**
     * Constructor.
     */
    explicit cModulePar(const char *name=NULL);

    /**
     * Constructor.
     */
    explicit cModulePar(const char *name, cPar& other);

    /**
     * Destructor.
     */
    virtual ~cModulePar();

    /**
     * Assignment operator. The name member doesn't get copied;
     * see cObject's operator=() for more details.
     */
    cModulePar& operator=(const cModulePar& otherpar);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const  {return new cPar(*this);}

    /**
     * Redefined. (Reason: a C++ rule that overloaded virtual methods must be redefined together.)
     */
    virtual const char *fullPath() const;

    /**
     * The original fullPath() method is redefined to hide the
     * internal array (a cArray) used to store the parameter objects.
     */
    virtual const char *fullPath(char *buffer, int bufsize) const;
    //@}

    /** @name Set/get owner module. */
    //@{

    /**
     * Sets a pointer to the module which has this cPar as module parameter.
     */
    void setOwnerModule(cModule *om)   {omodp=om;}

    /**
     * Returns a pointer to the module which has this cPar as module parameter.
     */
    cModule *ownerModule() const        {return omodp;}
    //@}
};


//=== operators dealing with cPars
//
// These operators were removed -- see ChangeLog, Jan 17 2000 entry.
//

//#ifndef NO_CPAR_OPERATIONS
//#ifdef  LONG_CPAR_OPERATIONS
// inline int operator<(cPar& p, cPar& q)  {return (long)p<(long)q;}
// inline int operator<(long d, cPar& p)   {return d<(long)p;}
// inline int operator<(cPar& p, long d)   {return (long)p<d;}
// inline int operator>(cPar& p, cPar& q)  {return (long)p>(long)q;}
// inline int operator>(long d, cPar& p)   {return d>(long)p;}
// inline int operator>(cPar& p, long d)   {return (long)p>d;}
//
// inline int operator<=(cPar& p, cPar& q) {return (long)p<=(long)q;}
// inline int operator<=(long d, cPar& p)  {return d<=(long)p;}
// inline int operator<=(cPar& p, long d)  {return (long)p<=d;}
// inline int operator>=(cPar& p, cPar& q) {return (long)p>=(long)q;}
// inline int operator>=(long d, cPar& p)  {return d>=(long)p;}
// inline int operator>=(cPar& p, long d)  {return (long)p>=d;}
//
// inline long operator+(cPar& p, cPar& q) {return (long)p+(long)q;}
// inline long operator+(long d, cPar& p)  {return d+(long)p;}
// inline long operator+(cPar& p, long d)  {return (long)p+d;}
// inline long operator-(cPar& p, cPar& q) {return (long)p-(long)q;}
// inline long operator-(long d, cPar& p)  {return d-(long)p;}
// inline long operator-(cPar& p, long d)  {return (long)p-d;}
//
// inline long operator*(cPar& p, cPar& q)  {return (long)p*(long)q;}
// inline long operator*(long d, cPar& p)   {return d*(long)p;}
// inline long operator*(cPar& p, long d)   {return (long)p*d;}
// inline long operator/(cPar& p, cPar& q)  {return (long)p/(long)q;}
// inline long operator/(long d, cPar& p)   {return d/(long)p;}
// inline long operator/(cPar& p, long d)   {return (long)p/d;}
//
//#else
//
// inline int operator<(cPar& p, cPar& q)  {return (double)p<(double)q;}
// inline int operator<(double d, cPar& p) {return d<(double)p;}
// inline int operator<(cPar& p, double d) {return (double)p<d;}
// inline int operator>(cPar& p, cPar& q)  {return (double)p>(double)q;}
// inline int operator>(double d, cPar& p) {return d>(double)p;}
// inline int operator>(cPar& p, double d) {return (double)p>d;}
//
// inline int operator<=(cPar& p, cPar& q)  {return (double)p<=(double)q;}
// inline int operator<=(double d, cPar& p) {return d<=(double)p;}
// inline int operator<=(cPar& p, double d) {return (double)p<=d;}
// inline int operator>=(cPar& p, cPar& q)  {return (double)p>=(double)q;}
// inline int operator>=(double d, cPar& p) {return d>=(double)p;}
// inline int operator>=(cPar& p, double d) {return (double)p>=d;}
//
// inline double operator+(cPar& p, cPar& q)  {return (double)p+(double)q;}
// inline double operator+(double d, cPar& p) {return d+(double)p;}
// inline double operator+(cPar& p, double d) {return (double)p+d;}
// inline double operator-(cPar& p, cPar& q)  {return (double)p-(double)q;}
// inline double operator-(double d, cPar& p) {return d-(double)p;}
// inline double operator-(cPar& p, double d) {return (double)p-d;}
//
// inline double operator*(cPar& p, cPar& q)  {return (double)p*(double)q;}
// inline double operator*(double d, cPar& p) {return d*(double)p;}
// inline double operator*(cPar& p, double d) {return (double)p*d;}
// inline double operator/(cPar& p, cPar& q)  {return (double)p/(double)q;}
// inline double operator/(double d, cPar& p) {return d/(double)p;}
// inline double operator/(cPar& p, double d) {return (double)p/d;}
//#endif
//#endif

#endif


