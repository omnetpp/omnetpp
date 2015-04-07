//==========================================================================
//  CMATHFUNCTION.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMATHFUNCTION_H
#define __CMATHFUNCTION_H

#include <stdarg.h>
#include "simkerneldefs.h"
#include "globals.h"
#include "cownedobject.h"

NAMESPACE_BEGIN


/**
 * Prototype for mathematical functions.
 *
 * @see cNEDMathFunction, Define_NED_Math_Function().
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc)(...);

/**
 * Prototype for mathematical functions taking no arguments.
 *
 * @see cNEDMathFunction, Define_NED_Math_Function().
 * @ingroup EnumsTypes
 */
typedef double (*MathFuncNoArg)();

/**
 * Prototype for mathematical functions taking one argument.
 *
 * @see cNEDMathFunction, Define_NED_Math_Function().
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc1Arg)(double);

/**
 * Prototype for mathematical functions taking two arguments.
 *
 * @see cNEDMathFunction, Define_NED_Math_Function().
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc2Args)(double,double);

/**
 * Prototype for mathematical functions taking three arguments.
 *
 * @see cNEDMathFunction, Define_NED_Math_Function().
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc3Args)(double,double,double);

/**
 * Prototype for mathematical functions taking four arguments.
 *
 * @see cNEDMathFunction, Define_NED_Math_Function().
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc4Args)(double,double,double,double);


/**
 * Registration class for extending NED with new functions. Stores a pointer to
 * a function taking up to 4 doubles as args and returning a double.
 *
 * Objects of this class are usually created via the Define_NED_Math_Function() macro.
 *
 * @ingroup Internals
 */
class SIM_API cNEDMathFunction : public cNoncopyableOwnedObject
{
  private:
    MathFunc f;            // pointer to the function
    int argc;              // argument count (up to 4 doubles)
    std::string categ;     // category string; only used when listing all functions
    std::string desc;      // optional documentation string

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor.
     */
    cNEDMathFunction(const char *name, MathFuncNoArg f, int argc=-1, const char *category=NULL, const char *description=NULL);

    /**
     * Constructor.
     */
    cNEDMathFunction(const char *name, MathFunc1Arg f, int argc=-1, const char *category=NULL, const char *description=NULL);

    /**
     * Constructor.
     */

    cNEDMathFunction(const char *name, MathFunc2Args f, int argc=-1, const char *category=NULL, const char *description=NULL);

    /**
     * Constructor.
     */
    cNEDMathFunction(const char *name, MathFunc3Args f, int argc=-1, const char *category=NULL, const char *description=NULL);

    /**
     * Constructor.
     */
    cNEDMathFunction(const char *name, MathFunc4Args f, int argc=-1, const char *category=NULL, const char *description=NULL);

    /**
     * Destructor.
     */
    virtual ~cNEDMathFunction() {}
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of the object's contents.
     */
    virtual std::string info() const;
    //@}

    /** @name Member access. */
    //@{
    /**
     * Argument count to function.
     */
    int getNumArgs() const  {return argc;}

    /**
     * Returns function pointer as double function with unchecked arg list
     * (no type safety!)
     */
    MathFunc getMathFunc() const {return f;}

    /**
     * Returns function pointer as double function with no args.
     * Throws exception is actual arg count is different.
     */
    MathFuncNoArg getMathFuncNoArg() const;

    /**
     * Returns function pointer as double function with 1 double arg.
     * Throws exception is actual arg count is different.
     */
    MathFunc1Arg getMathFunc1Arg() const;

    /**
     * Returns function pointer as double function with 2 double args.
     * Throws exception is actual arg count is different.
     */
    MathFunc2Args getMathFunc2Args() const;

    /**
     * Returns function pointer as double function with 3 double args.
     * Throws exception is actual arg count is different.
     */
    MathFunc3Args getMathFunc3Args() const;

    /**
     * Returns function pointer as double function with 4 double args.
     * Throws exception is actual arg count is different.
     */
    MathFunc4Args getMathFunc4Args() const;

    /**
     * Returns a string that can be useful in classifying NED functions,
     * e.g. "trigonometric".
     */
    const char *getCategory() const  {return categ.c_str();}

    /**
     * Returns the function's documentation as a string.
     */
    const char *getDescription() const  {return desc.c_str();}
    //@}

    /**
     * Finds a registered function by name. Returns NULL if not found.
     */
    static cNEDMathFunction *find(const char *name, int numArgs);

    /**
     * Finds a registered function by name. Throws an error if not found.
     */
    static cNEDMathFunction *get(const char *name, int numArgs);

    /**
     * Finds a registered function by function pointer.
     */
    static cNEDMathFunction *findByPointer(MathFunc f);

};

NAMESPACE_END


#endif


