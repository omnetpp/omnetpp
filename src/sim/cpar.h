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
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPAR_H
#define __CPAR_H

#include "cobject.h"
#include "carray.h"   //NL

#define SHORTSTR  27

#define NOPAR  NO(cPar)

//=== classes declared here:
class  cPar;
class  cModulePar;

//=== class mentioned
class  cStatistic;
class  cArray;

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
   void operator=(int _i)            {type='D'; d=_i;  }
   void operator=(long _l)           {type='D'; d=_l;  }
   void operator=(double _d)         {type='D'; d=_d;  }
   void operator=(cPar *_p)          {type='P'; p=_p;  }
   void operator=(cPar& _r);         //{type='R'; p=(cPar *)_r.dup();} See after cPar!
   void operator=(MathFuncNoArg _f)  {type='0'; f0=_f;}
   void operator=(MathFunc1Arg  _f)  {type='1'; f1=_f;}
   void operator=(MathFunc2Args _f)  {type='2'; f2=_f;}
   void operator=(MathFunc3Args _f)  {type='3'; f3=_f;}
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

class cPar : public cObject
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
     void deleteold();          // helper func: destruct old value
     double evaluate();         // helper func: evaluates expression (X)
     double fromstat();         // helper func: rand.num with given distr.(T)
     bool setfunction(char *w); // setFromText() helper func.

   protected:
     virtual void valueChanges();

   public:
     cPar(cPar& other);
     explicit cPar(char *namestr=NULL);
     explicit cPar(char *namestr, cPar& other);

     virtual ~cPar();

     // redefined functions
     virtual char *className()  {return "cPar";}
     virtual cObject *dup()   {return new cPar(*this);}
     virtual void info(char *buf);
     virtual char *inspectorFactoryName() {return "cParIFC";}
     virtual void writeContents(ostream& os);
     cPar& operator=(cPar& otherpar);
     virtual int netPack();
     virtual int netUnpack();

     // new functions:

     // functions to set value (assignment operators also exist, see later)
     cPar& setBoolValue(bool b);              // bool
     cPar& setLongValue(long l);              // long
     cPar& setStringValue(char *s);           // string
     cPar& setDoubleValue(double d);          // double
     cPar& setDoubleValue(cStatistic *res);   // distribution
     cPar& setDoubleValue(sXElem *x, int n);  // expression
     cPar& setDoubleValue(MathFuncNoArg f);   // math func: 0,1,2,3 args
     cPar& setDoubleValue(MathFunc1Arg  f, double p1);
     cPar& setDoubleValue(MathFunc2Args f, double p1, double p2);
     cPar& setDoubleValue(MathFunc3Args f, double p1, double p2, double p3);
     cPar& setPointerValue(void *ptr);        // void* pointer
     cPar& setObjectValue(cObject *obj);      // cObject* pointer

     // memory management to void* pointer type
     void configPointer( VoidDelFunc delfunc, VoidDupFunc dupfunc, size_t itemsize=0);

     // functions to return value
     //   conversion operators also exist; see later
     bool   boolValue();
     long   longValue();
     char * stringValue();
     double doubleValue();
     void * pointerValue();
     cObject *objectValue();

     // compares stored value
     bool equalsTo(cPar *par);

     // query/set value as string (e.g. "uniform(10,3)")
     virtual void getAsText(char *buf, int maxlen);
     virtual bool setFromText(char *text, char tp);

     // redirection
     cPar& setRedirection(cPar *par);
     bool isRedirected() {return typechar=='I';}
     cPar *redirection();
     void cancelRedirection();

     cPar& read();
     void convertToConst();

     // functions to get/set different flags
     char type();                   // type char

     char *prompt();                // prompt text
     void setPrompt(char *s);       // set prompt str

     void setInput(bool ip);        // set input flag
     bool isInput();                // is an input value?

     bool changed();                // has changed? (clears `changed' flag)

     // redefined operators to set/get value
     cPar& operator=(bool b)          {return setBoolValue(b);}
     cPar& operator=(char *s)         {return setStringValue(s);}
     cPar& operator=(char c)          {return setLongValue((long)c);}
     cPar& operator=(unsigned char c) {return setLongValue((long)c);}
     cPar& operator=(int i)           {return setLongValue((long)i);}
     cPar& operator=(unsigned int i)  {return setLongValue((long)i);}
     cPar& operator=(long l)          {return setLongValue(l);}
     cPar& operator=(unsigned long l) {return setLongValue((long)l);}
     cPar& operator=(double d)        {return setDoubleValue(d);}
     cPar& operator=(long double d)   {return setDoubleValue((double)d);}
     cPar& operator=(void *ptr)       {return setPointerValue(ptr);}
     cPar& operator=(cObject *obj)    {return setObjectValue(obj);}

     operator bool()          {return boolValue();}
     operator char*()         {return stringValue();}
     operator char()          {return (char)longValue();}
     operator unsigned char() {return (unsigned char)longValue();}
     operator int()           {return (int)longValue();}
     operator unsigned int()  {return (unsigned int)longValue();}
     operator long()          {return longValue();}
     operator unsigned long() {return longValue();}
     operator double()        {return doubleValue();}
     operator long double()   {return doubleValue();}
     operator void *()        {return pointerValue();}
     operator cObject *()     {return objectValue();}

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

class cModulePar : public cPar
{
     friend class cModule;
   private:
     cModule *omodp;        // owner module
     bool log_initialised;  // logging: TRUE if the "label" line already written out
     long log_ID;           // logging: ID of the data lines
     simtime_t lastchange;  // logging: time of last value change
   private:
     void _construct();     // helper function

   public:
     cModulePar(cPar& other);
     explicit cModulePar(char *namestr=NULL);
     explicit cModulePar(char *namestr, cPar& other);

     virtual ~cModulePar();

     // redefined functions
     virtual char *className()  {return "cModulePar";}
     virtual cObject *dup()  {return new cPar(*this);}
     virtual char *inspectorFactoryName() {return "cModuleParIFC";}
     virtual char *fullPath();
     cModulePar& operator=(cModulePar& otherpar);

     // redefined cPar function
     virtual void valueChanges();    // does parameter logging

     // new functions
     void setOwnerModule(cModule *om)   {omodp=om;}
     cModule *ownerModule()             {return omodp;}
};

//=== operators dealing with cPars
// Two different sets of overloaded operators are provided, working
// through converting to double / long. Default is the double version,
// you can choose the other set by defining LONG_CPAR_OPERATIONS.
// If you don't want either, define NO_CPAR_OPERATIONS!

#ifndef NO_CPAR_OPERATIONS
#ifdef  LONG_CPAR_OPERATIONS
inline int operator<(cPar& p, cPar& q)  {return (long)p<(long)q;}
inline int operator<(long d, cPar& p)   {return d<(long)p;}
inline int operator<(cPar& p, long d)   {return (long)p<d;}
inline int operator>(cPar& p, cPar& q)  {return (long)p>(long)q;}
inline int operator>(long d, cPar& p)   {return d>(long)p;}
inline int operator>(cPar& p, long d)   {return (long)p>d;}

inline int operator<=(cPar& p, cPar& q) {return (long)p<=(long)q;}
inline int operator<=(long d, cPar& p)  {return d<=(long)p;}
inline int operator<=(cPar& p, long d)  {return (long)p<=d;}
inline int operator>=(cPar& p, cPar& q) {return (long)p>=(long)q;}
inline int operator>=(long d, cPar& p)  {return d>=(long)p;}
inline int operator>=(cPar& p, long d)  {return (long)p>=d;}

inline long operator+(cPar& p, cPar& q) {return (long)p+(long)q;}
inline long operator+(long d, cPar& p)  {return d+(long)p;}
inline long operator+(cPar& p, long d)  {return (long)p+d;}
inline long operator-(cPar& p, cPar& q) {return (long)p-(long)q;}
inline long operator-(long d, cPar& p)  {return d-(long)p;}
inline long operator-(cPar& p, long d)  {return (long)p-d;}

inline long operator*(cPar& p, cPar& q)  {return (long)p*(long)q;}
inline long operator*(long d, cPar& p)   {return d*(long)p;}
inline long operator*(cPar& p, long d)   {return (long)p*d;}
inline long operator/(cPar& p, cPar& q)  {return (long)p/(long)q;}
inline long operator/(long d, cPar& p)   {return d/(long)p;}
inline long operator/(cPar& p, long d)   {return (long)p/d;}

#else

inline int operator<(cPar& p, cPar& q)  {return (double)p<(double)q;}
inline int operator<(double d, cPar& p) {return d<(double)p;}
inline int operator<(cPar& p, double d) {return (double)p<d;}
inline int operator>(cPar& p, cPar& q)  {return (double)p>(double)q;}
inline int operator>(double d, cPar& p) {return d>(double)p;}
inline int operator>(cPar& p, double d) {return (double)p>d;}

inline int operator<=(cPar& p, cPar& q)  {return (double)p<=(double)q;}
inline int operator<=(double d, cPar& p) {return d<=(double)p;}
inline int operator<=(cPar& p, double d) {return (double)p<=d;}
inline int operator>=(cPar& p, cPar& q)  {return (double)p>=(double)q;}
inline int operator>=(double d, cPar& p) {return d>=(double)p;}
inline int operator>=(cPar& p, double d) {return (double)p>=d;}

inline double operator+(cPar& p, cPar& q)  {return (double)p+(double)q;}
inline double operator+(double d, cPar& p) {return d+(double)p;}
inline double operator+(cPar& p, double d) {return (double)p+d;}
inline double operator-(cPar& p, cPar& q)  {return (double)p-(double)q;}
inline double operator-(double d, cPar& p) {return d-(double)p;}
inline double operator-(cPar& p, double d) {return (double)p-d;}

inline double operator*(cPar& p, cPar& q)  {return (double)p*(double)q;}
inline double operator*(double d, cPar& p) {return d*(double)p;}
inline double operator*(cPar& p, double d) {return (double)p*d;}
inline double operator/(cPar& p, cPar& q)  {return (double)p/(double)q;}
inline double operator/(double d, cPar& p) {return d/(double)p;}
inline double operator/(cPar& p, double d) {return (double)p/d;}
#endif
#endif

#endif

