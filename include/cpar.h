//==========================================================================
//   CPAR.H   - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cPar      : general value
//    cModulePar   : module parameter
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPAR_H
#define __CPAR_H

#include "cobject.h"

#define SHORTSTR  27

#define NOPAR  NO(cPar)

//=== classes declared here:
class  cPar;
class  cModulePar;

//=== class mentioned
class  cStatistic;

//==========================================================================
// sXElem: one component in a (reversed Polish) expression in a cPar
//  NOTE: not a cObject descendant!
//   Types:
//      D  double constant
//      P  pointer to "external" cPar (owned by someone else)
//      R  "reference": the cPar will be dup()'ped and the copy kept
//      0/1/2/3  function with 0/1/2/3 arguments
//      @  math operator (+-*/%^=!<{>}?); see cPar::evaluate()
//

/**
 * FIXME: //=== classes declared here:
 * //=== class mentioned
 * 
 * sXElem: one component in a (reversed Polish) expression in a cPar
 *  NOTE: not a cObject descendant!
 *   Types:
 *      D  double constant
 *      P  pointer to "external" cPar (owned by someone else)
 *      R  "reference": the cPar will be dup()'ped and the copy kept
 *      0/1/2/3  function with 0/1/2/3 arguments
 *      @  math operator (+-* /%^=!<{>}?); see cPar::evaluate()
 * 
 */
struct sXElem
{
   char type;    // D/P/R/0/1/2/3/@
   union {
       double d;           // D
       cPar *p;            // P/R
       MathFuncNoArg f0;   // 0
       MathFunc1Arg  f1;   // 1
       MathFunc2Args f2;   // 2
       MathFunc3Args f3;   // 3
       char op;            // @, op = +-*/%^=!<{>}?
   };

   // A little help to create expressions:

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
     * The argument can be a pointer to a function that takes (0, 1,
     * 2, or 3) double arguments and returns a double
     * (e.g. sqrt()). Effect during evaluation of the expression:
     * the given number of doubles are popped from the stack,
     * the given function is called with them as arguments, and the return
     * value is pushed back on the stack. See also the cFunctionType
     * class and the Define_Function() macro.
     * 
     * The OMNeT++ functions generating random variables of different
     * distributions can also be used in sXElem expressions.
     */
   void operator=(MathFuncNoArg _f)  {type='0'; f0=_f;}

    /**
     * The argument can be a pointer to a function that takes (0, 1,
     * 2, or 3) double arguments and returns a double
     * (e.g. sqrt()). Effect during evaluation of the expression:
     * the given number of doubles are popped from the stack,
     * the given function is called with them as arguments, and the return
     * value is pushed back on the stack. See also the cFunctionType
     * class and the Define_Function() macro.
     * 
     * The OMNeT++ functions generating random variables of different
     * distributions can also be used in sXElem expressions.
     */
   void operator=(MathFunc1Arg  _f)  {type='1'; f1=_f;}

    /**
     * The argument can be a pointer to a function that takes (0, 1,
     * 2, or 3) double arguments and returns a double
     * (e.g. sqrt()). Effect during evaluation of the expression:
     * the given number of doubles are popped from the stack,
     * the given function is called with them as arguments, and the return
     * value is pushed back on the stack. See also the cFunctionType
     * class and the Define_Function() macro.
     * 
     * The OMNeT++ functions generating random variables of different
     * distributions can also be used in sXElem expressions.
     */
   void operator=(MathFunc2Args _f)  {type='2'; f2=_f;}

    /**
     * The argument can be a pointer to a function that takes (0, 1,
     * 2, or 3) double arguments and returns a double
     * (e.g. sqrt()). Effect during evaluation of the expression:
     * the given number of doubles are popped from the stack,
     * the given function is called with them as arguments, and the return
     * value is pushed back on the stack. See also the cFunctionType
     * class and the Define_Function() macro.
     * 
     * The OMNeT++ functions generating random variables of different
     * distributions can also be used in sXElem expressions.
     */
   void operator=(MathFunc3Args _f)  {type='3'; f3=_f;}

    /**
     * The argument can be:
     * <TABLE BORDER=1>
     * <TR><TD WIDTH=114><CENTER>+ - * /</CENTER></TD><TD WIDTH=330>add, subtract, multiply, divide
     * </TD></TR>
     * <TR><TD WIDTH=114><CENTER>% ^ </CENTER></TD><TD WIDTH=330>modulo, power of
     * </TD></TR>
     * <TR><TD WIDTH=114><CENTER>= ! </CENTER></TD><TD WIDTH=330>equal, not equal
     * </TD></TR>
     * <TR><TD WIDTH=114><CENTER>> }</CENTER></TD><TD WIDTH=330>greater, greater or equal
     * </TD></TR>
     * <TR><TD WIDTH=114><CENTER>< {</CENTER></TD><TD WIDTH=330>less, less or equal
     * </TD></TR>
     * <TR><TD WIDTH=114><CENTER>?</CENTER></TD><TD WIDTH=330>inline if (C language's (cond ? a : b)
     * </TD></TR>
     * </TABLE>
     * 
     * Effect during evaluation of the expression: two items (or three,
     * with '?') are popped out of the stack, the given operator
     * is applied to them and the result is pushed back on the stack.
     */
   void operator=(char _op)          {type='@'; op=_op;}
};

//==========================================================================
// cPar: stores a general value (char/string/long/double/expr..)
//  NOTE: forEach() ignores objects stored here such as cPars in eXpressions,
//        cObject pointed to in 'P' type, cStatistic in disTribution type
// Types and type characters:
//   C char, S string, L long, D double,
//   F math function (MathFuncNoArgs,MathFunc1Args,etc),
//   X expression (table of sXElems),
//   T distribution from a cStatistic,
//   P pointer to cObject,
//   I indirection (refers to another cPar)
//


/**
 * FIXME: 
 * cPar: stores a general value (char/string/long/double/expr..)
 *  NOTE: forEach() ignores objects stored here such as cPars in eXpressions,
 *        cObject pointed to in 'P' type, cStatistic in disTribution type
 * Types and type characters:
 *   C char, S string, L long, D double,
 *   F math function (MathFuncNoArgs,MathFunc1Args,etc),
 *   X expression (table of sXElems),
 *   T distribution from a cStatistic,
 *   P pointer to cObject,
 *   I indirection (refers to another cPar)
 * 
 */
class SIM_API cPar : public cObject
{
  protected:
    static char *possibletypes;
  private:
    char typechar;     // S/B/L/D/F/T/X/P/O/I
    bool inputflag;
    bool changedflag;
    opp_string promptstr; // prompt text used when the value is being input

    union {    // Take care of 'operator=()' when changing this!!!
       struct { bool sht; char *str;            } ls;   // S:long string
       struct { bool sht; char str[SHORTSTR+1]; } ss;   // S:short str
       struct { long val;                       } lng;  // L:long,B:bool
       struct { double val;                     } dbl;  // D:double
       struct { MathFunc f; int argc;
                double p1,p2,p3;                } func; // F:math function
       struct { cStatistic *res;                } dtr;  // T:distribution
       struct { sXElem *xelem; int n;           } expr; // X:expression
       struct { cPar *par;                      } ind;  // I:indirection
       struct { void *ptr;
                VoidDelFunc delfunc;
                VoidDupFunc dupfunc;
                size_t itemsize;                } ptr;  // P:void* pointer
       struct { cObject *obj;                   } obj;  // O:object pointer
    };

  private:

    /**
     * MISSINGDOC: cPar:void deleteold()
     */
    void deleteold();          // helper func: destruct old value

    /**
     * MISSINGDOC: cPar:double evaluate()
     */
    double evaluate();         // helper func: evaluates expression (X)

    /**
     * MISSINGDOC: cPar:double fromstat()
     */
    double fromstat();         // helper func: rand.num with given distr.(T)

    /**
     * MISSINGDOC: cPar:bool setfunction(char*)
     */
    bool setfunction(char *w); // setFromText() helper func.

  protected:

    /**
     * MISSINGDOC: cPar:void valueChanges()
     */
    virtual void valueChanges();

  public:

    /**
     * Copy constructor, creates an exact copy of the argument.
     */
    cPar(cPar& other);

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
     * MISSINGDOC: cPar:~cPar()
     */
    virtual ~cPar();

    // redefined functions

    /**
     * Returns pointer to the class name string, "cPar".
     */
    virtual const char *className() const {return "cPar";}

    /**
     * Duplicates the object and returns a pointer to the new one.
     */
    virtual cObject *dup()   {return new cPar(*this);}

    /**
     * MISSINGDOC: cPar:void info(char*)
     */
    virtual void info(char *buf);

    /**
     * MISSINGDOC: cPar:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cParIFC";}

    /**
     * MISSINGDOC: cPar:void writeContents(ostream&)
     */
    virtual void writeContents(ostream& os);

    /**
     * The assignment operator works with cPar objects.
     */
    cPar& operator=(cPar& otherpar);

    /**
     * MISSINGDOC: cPar:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cPar:int netUnpack()
     */
    virtual int netUnpack();

    // new functions:

    // functions to set value (assignment operators also exist, see later)

    /**
     * FIXME: new functions:
     * functions to set value (assignment operators also exist, see later)
     */
    cPar& setBoolValue(bool b);              // bool

    /**
     * MISSINGDOC: cPar:cPar&setLongValue(long)
     */
    cPar& setLongValue(long l);              // long

    /**
     * MISSINGDOC: cPar:cPar&setStringValue(char*)
     */
    cPar& setStringValue(const char *s);     // string

    /**
     * MISSINGDOC: cPar:cPar&setDoubleValue(double)
     */
    cPar& setDoubleValue(double d);          // double

    /**
     * MISSINGDOC: cPar:cPar&setDoubleValue(cStatistic*)
     */
    cPar& setDoubleValue(cStatistic *res);   // distribution

    /**
     * MISSINGDOC: cPar:cPar&setDoubleValue(sXElem*,int)
     */
    cPar& setDoubleValue(sXElem *x, int n);  // expression

    /**
     * MISSINGDOC: cPar:cPar&setDoubleValue(MathFuncNoArg)
     */
    cPar& setDoubleValue(MathFuncNoArg f);   // math func: 0,1,2,3 args

    /**
     * MISSINGDOC: cPar:cPar&setDoubleValue(MathFunc1Arg,double)
     */
    cPar& setDoubleValue(MathFunc1Arg  f, double p1);

    /**
     * MISSINGDOC: cPar:cPar&setDoubleValue(MathFunc2Args,double,double)
     */
    cPar& setDoubleValue(MathFunc2Args f, double p1, double p2);

    /**
     * MISSINGDOC: cPar:cPar&setDoubleValue(MathFunc3Args,double,double,double)
     */
    cPar& setDoubleValue(MathFunc3Args f, double p1, double p2, double p3);

    /**
     * MISSINGDOC: cPar:cPar&setPointerValue(void*)
     */
    cPar& setPointerValue(void *ptr);        // void* pointer

    /**
     * MISSINGDOC: cPar:cPar&setObjectValue(cObject*)
     */
    cPar& setObjectValue(cObject *obj);      // cObject* pointer

    // memory management to void* pointer type

    /**
     * Configures memory management for the void* pointer ('P') type.
     * Similar to cLinkedList's configPointer() function.
     * <TABLE BORDER=1>
     * <TR><TD WIDTH=96><B>delete func.</B></TD><TD WIDTH=89><B>dupl.func.</B>
     * </TD><TD WIDTH=92><B>itemsize</B></TD><TD WIDTH=317><B>behaviour</B>
     * </TD></TR>
     * <TR><TD WIDTH=96>NULL</TD><TD WIDTH=89>NULL
     * </TD><TD WIDTH=92>0</TD><TD WIDTH=317>Pointer is treated as mere pointer - no memory management. Duplication copies the pointer, and deletion does nothing.
     * </TD></TR>
     * <TR><TD WIDTH=96>NULL</TD><TD WIDTH=89>NULL
     * </TD><TD WIDTH=92>>0 size</TD><TD WIDTH=317>Plain memory management. Duplication is done with new char[size]+memcpy(), and deletion is done via delete.
     * </TD></TR>
     * <TR><TD WIDTH=96>NULL or user's delete func.</TD><TD WIDTH=89>user's dupfunc.
     * </TD><TD WIDTH=92>indifferent</TD><TD WIDTH=317>Sophisticated memory management. Duplication is done by calling the user-supplied duplication function, which should do the allocation and the appropriate copying. Deletion is done by calling the user-supplied delete function, or the delete operator if it is not supplied.
     * </TD></TR>
     * </TABLE>
     */
    void configPointer( VoidDelFunc delfunc, VoidDupFunc dupfunc, size_t itemsize=0);

    // functions to return value
    //   conversion operators also exist; see later

    /**
     * FIXME: functions to return value
     *   conversion operators also exist; see later
     */
    bool   boolValue();

    /**
     * Returns value as long. Converts from types long (L),
     * double (D), Boolean (B), function (F), distribution (T) and expression
     * (X).
     */
    long   longValue();

    /**
     * Returns value as char*. Only for string (S) type.
     */
    const char *stringValue();

    /**
     * Returns value as double. Converts from types long (L),
     * double (D), function (F), Boolean (B), distribution (T) and expression
     * (X).
     */
    double doubleValue();

    /**
     * Returns value as pointer to cObject. Type must be pointer
     * (P).
     */
    void *pointerValue();

    /**
     * Returns value as pointer to cObject. Type must be pointer
     * (O).
     */
    cObject *objectValue();

    // returns true if it is safe to call doubleValue()/longValue()/boolValue()

    /**
     * Returns true if the stored value is of a numeric type.
     */
    bool isNumeric();

    // compares the stored values; the two objects must have the same typechar, etc. to be equal

    /**
     * FIXME: compares the stored values; the two objects must have the same typechar, etc. to be equal
     */
    bool equalsTo(cPar *par);

    // query/set value as string (e.g. "uniform(10,3)")

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

    // redirection

    /**
     * TBD
     */
    cPar& setRedirection(cPar *par);

    /**
     * TBD
     */
    bool isRedirected() {return typechar=='I';}

    /**
     * Returns NULL if the cPar's value is not redirected
     * to another cPar; otherwise it returns the pointer of
     * that cPar. This function is the only way to determine
     * if an object is redirected or not (type() returns the
     * type of the other cPar: 'D', 'L' etc).
     */
    cPar *redirection();

    /**
     * Break the redirection. The new type will be long ('L').
     */
    void cancelRedirection();


    /**
     * Reads the object value from the ini file or from the user.
     */
    cPar& read();

    /**
     * Replaces the object value with its evaluation (a double).
     * Implemented as something like setValue('D', this->doubleValue()).
     */
    void convertToConst();

    // functions to get/set different flags

    /**
     * Returns type character. If the "real" type is 'I',
     * it returns the type of the object it is redirected to (for example,
     * 'D', 'L', etc.)
     */
    char type();                   // type char


    /**
     * Returns the prompt text or NULL.
     */
    const char *prompt();          // prompt text

    /**
     * Sets the prompt text.
     */
    void setPrompt(const char *s); // set prompt str


    /**
     * Sets (ip=true) or clears (ip=false) the input
     * flag.
     */
    void setInput(bool ip);        // set input flag

    /**
     * Returns true if the parameter is of input type (the input
     * flag is set).
     */
    bool isInput();                // is an input value?


    /**
     * Returns true if the value has changed since the last
     * changed() call.
     */
    bool changed();                // has changed? (clears `changed' flag)

    // redefined operators to set/get value

    /**
     * FIXME: redefined operators to set/get value
     */
    cPar& operator=(bool b)          {return setBoolValue(b);}

    /**
     * Equivalent to setValue('S',s).
     */
    cPar& operator=(const char *s)   {return setStringValue(s);}

    /**
     * Equivalent to setValue('C',(int)c).
     */
    cPar& operator=(char c)          {return setLongValue((long)c);}

    /**
     * Equivalent to setValue('C',(int)c).
     */
    cPar& operator=(unsigned char c) {return setLongValue((long)c);}

    /**
     * Equivalent to setValue('L',(long)i).
     */
    cPar& operator=(int i)           {return setLongValue((long)i);}

    /**
     * Equivalent to setValue('L',(long)i).
     */
    cPar& operator=(unsigned int i)  {return setLongValue((long)i);}

    /**
     * Equivalent to setValue('L',l).
     */
    cPar& operator=(long l)          {return setLongValue(l);}

    /**
     * Equivalent to setValue('L',(long)l).
     */
    cPar& operator=(unsigned long l) {return setLongValue((long)l);}

    /**
     * Equivalent to setValue('D',d).
     */
    cPar& operator=(double d)        {return setDoubleValue(d);}

    /**
     * Equivalent to setValue('D',(double)d).
     */
    cPar& operator=(long double d)   {return setDoubleValue((double)d);}

    /**
     * MISSINGDOC: cPar:cPar&operator=(void*)
     */
    cPar& operator=(void *ptr)       {return setPointerValue(ptr);}

    /**
     * Equivalent to setValue('P',obj).
     */
    cPar& operator=(cObject *obj)    {return setObjectValue(obj);}


    /**
     * MISSINGDOC: cPar:operator bool()
     */
    operator bool()          {return boolValue();}

    /**
     * Equivalent to stringValue().
     */
    operator const char *()  {return stringValue();}

    /**
     * MISSINGDOC: cPar:operator char()
     */
    operator char()          {return (char)longValue();}

    /**
     * MISSINGDOC: cPar:operator unsigned char()
     */
    operator unsigned char() {return (unsigned char)longValue();}

    /**
     * Equivalent to longValue().
     */
    operator int()           {return (int)longValue();}

    /**
     * Equivalent to longValue().
     */
    operator unsigned int()  {return (unsigned int)longValue();}

    /**
     * Equivalent to longValue().
     */
    operator long()          {return longValue();}

    /**
     * Equivalent to longValue().
     */
    operator unsigned long() {return longValue();}

    /**
     * Equivalent to doubleValue().
     */
    operator double()        {return doubleValue();}

    /**
     * Equivalent to doubleValue().
     */
    operator long double()   {return doubleValue();}

    /**
     * MISSINGDOC: cPar:operator void*()
     */
    operator void *()        {return pointerValue();}

    /**
     * Equivalent to pointerValue().
     */
    operator cObject *()     {return objectValue();}


    /**
     * Compares two cPars by their value if they are numeric.
     * This function can be used to sort cPar objects in a priority
     * queue.
     */
    static int cmpbyvalue(cObject *one, cObject *other);
};

// this function cannot be defined within sXElem because of declaration order
inline void sXElem::operator=(cPar& _r)  {type='R'; p=(cPar *)_r.dup();}

//==========================================================================
// cModulePar - module parameter object
//   essentially the same as cPar, but it is capable of logging
//   parameter changes to file
//
//   NOTE: dup() creates only a cPar NOT a cModulePar!


/**
 * FIXME: this function cannot be defined within sXElem because of declaration order
 * 
 * cModulePar - module parameter object
 *   essentially the same as cPar, but it is capable of logging
 *   parameter changes to file
 * 
 *   NOTE: dup() creates only a cPar NOT a cModulePar!
 */
class SIM_API cModulePar : public cPar
{
    friend class cModule;
  private:
    cModule *omodp;        // owner module
    bool log_initialised;  // logging: true if the "label" line already written out
    long log_ID;           // logging: ID of the data lines
    simtime_t lastchange;  // logging: time of last value change
  private:

    /**
     * MISSINGDOC: cModulePar:void _ruct()
     */
    void _construct();     // helper function

  public:

    /**
     * MISSINGDOC: cModulePar:cModulePar(cPar&)
     */
    cModulePar(cPar& other);

    /**
     * MISSINGDOC: cModulePar:cModulePar(char*)
     */
    explicit cModulePar(const char *name=NULL);

    /**
     * MISSINGDOC: cModulePar:cModulePar(char*,cPar&)
     */
    explicit cModulePar(const char *name, cPar& other);


    /**
     * MISSINGDOC: cModulePar:~cModulePar()
     */
    virtual ~cModulePar();

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cModulePar";}

    /**
     * MISSINGDOC: cModulePar:cObject*dup()
     */
    virtual cObject *dup()  {return new cPar(*this);}

    /**
     * MISSINGDOC: cModulePar:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cModuleParIFC";}

    /**
     * MISSINGDOC: cModulePar:char*fullPath()
     */
    virtual const char *fullPath() const;

    /**
     * MISSINGDOC: cModulePar:cModulePar&operator=(cModulePar&)
     */
    cModulePar& operator=(cModulePar& otherpar);

    // redefined cPar function

    /**
     * FIXME: redefined cPar function
     */
    virtual void valueChanges();    // does parameter logging

    // new functions

    /**
     * FIXME: new functions
     */
    void setOwnerModule(cModule *om)   {omodp=om;}

    /**
     * MISSINGDOC: cModulePar:cModule*ownerModule()
     */
    cModule *ownerModule()             {return omodp;}
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
