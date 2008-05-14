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
 * @see cMathFunction, Define_Function().
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc)(...);

/**
 * Prototype for mathematical functions taking no arguments.
 *
 * @see cMathFunction, Define_Function().
 * @ingroup EnumsTypes
 */
typedef double (*MathFuncNoArg)();

/**
 * Prototype for mathematical functions taking one argument.
 *
 * @see cMathFunction, Define_Function().
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc1Arg)(double);

/**
 * Prototype for mathematical functions taking two arguments.
 *
 * @see cMathFunction, Define_Function().
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc2Args)(double,double);

/**
 * Prototype for mathematical functions taking three arguments.
 *
 * @see cMathFunction, Define_Function().
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc3Args)(double,double,double);

/**
 * Prototype for mathematical functions taking four arguments.
 *
 * @see cMathFunction, Define_Function().
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc4Args)(double,double,double,double);


/**
 * Registration class for extending NED with new functions. Stores a pointer to
 * a function taking up to 4 doubles as args and returning a double.
 *
 * Objects of this class are usually created via the Define_Function() macro.
 *
 * @ingroup Internals
 */
class SIM_API cMathFunction : public cNoncopyableOwnedObject
{
  private:
    MathFunc f;
    int argc;
  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor.
     */
    cMathFunction(const char *name, MathFuncNoArg f, int argc=-1);

    /**
     * Constructor.
     */
    cMathFunction(const char *name, MathFunc1Arg f, int argc=-1);

    /**
     * Constructor.
     */

    cMathFunction(const char *name, MathFunc2Args f, int argc=-1);

    /**
     * Constructor.
     */
    cMathFunction(const char *name, MathFunc3Args f, int argc=-1);

    /**
     * Constructor.
     */
    cMathFunction(const char *name, MathFunc4Args f, int argc=-1);

    /**
     * Destructor.
     */
    virtual ~cMathFunction() {}
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of object contents.
     */
    virtual std::string info() const;
    //@}

    /** @name Member access. */
    //@{
    /**
     * Argument count to function.
     */
    int numArgs() const  {return argc;}

    /**
     * Returns function pointer as double function with unchecked arg list
     * (no type safety!)
     */
    MathFunc mathFunc()  {return f;}

    /**
     * Returns function pointer as double function with no args.
     * Throws exception is actual arg count is different.
     */
    MathFuncNoArg mathFuncNoArg();

    /**
     * Returns function pointer as double function with 1 double arg.
     * Throws exception is actual arg count is different.
     */
    MathFunc1Arg mathFunc1Arg();

    /**
     * Returns function pointer as double function with 2 double args.
     * Throws exception is actual arg count is different.
     */
    MathFunc2Args mathFunc2Args();

    /**
     * Returns function pointer as double function with 3 double args.
     * Throws exception is actual arg count is different.
     */
    MathFunc3Args mathFunc3Args();

    /**
     * Returns function pointer as double function with 4 double args.
     * Throws exception is actual arg count is different.
     */
    MathFunc4Args mathFunc4Args();
    //@}

    /**
     * Finds a registered function by name.
     */
    static cMathFunction *find(const char *name, int numArgs);

    /**
     * Finds a registered function by function pointer.
     */
    static cMathFunction *findByPointer(MathFunc f);

};

NAMESPACE_END


#endif


