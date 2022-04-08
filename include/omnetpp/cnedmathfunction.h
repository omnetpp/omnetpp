//==========================================================================
//  CMATHFUNCTION.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CMATHFUNCTION_H
#define __OMNETPP_CMATHFUNCTION_H

#include <cstdarg>
#include "simkerneldefs.h"
#include "globals.h"
#include "cownedobject.h"

namespace omnetpp {


/**
 * @brief Prototype for mathematical functions.
 *
 * @see cNedMathFunction, Define_NED_Math_Function().
 * @ingroup Misc
 */
typedef double (*MathFunc)(...);

/**
 * @brief Prototype for mathematical functions taking no arguments.
 *
 * @see cNedMathFunction, Define_NED_Math_Function().
 * @ingroup Misc
 */
typedef double (*MathFuncNoArg)();

/**
 * @brief Prototype for mathematical functions taking one argument.
 *
 * @see cNedMathFunction, Define_NED_Math_Function().
 * @ingroup Misc
 */
typedef double (*MathFunc1Arg)(double);

/**
 * @brief Prototype for mathematical functions taking two arguments.
 *
 * @see cNedMathFunction, Define_NED_Math_Function().
 * @ingroup Misc
 */
typedef double (*MathFunc2Args)(double,double);

/**
 * @brief Prototype for mathematical functions taking three arguments.
 *
 * @see cNedMathFunction, Define_NED_Math_Function().
 * @ingroup Misc
 */
typedef double (*MathFunc3Args)(double,double,double);

/**
 * @brief Prototype for mathematical functions taking four arguments.
 *
 * @see cNedMathFunction, Define_NED_Math_Function().
 * @ingroup Misc
 */
typedef double (*MathFunc4Args)(double,double,double,double);


/**
 * @brief Registration class for extending NED with new mathematical functions.
 *
 * Objects of this class are usually created via the Define_NED_Math_Function()
 * macro. It stores a pointer to a function taking up to 4 doubles as args
 * and returning a double.
 *
 * @ingroup Internals
 */
class SIM_API cNedMathFunction : public cNoncopyableOwnedObject
{
  private:
    MathFunc f;            // pointer to the function
    int argc;              // argument count (up to 4 doubles)
    std::string category;  // category string; only used when listing all functions
    std::string description;  // optional documentation string

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor.
     */
    cNedMathFunction(const char *name, MathFuncNoArg f, int argc=-1, const char *category=nullptr, const char *description=nullptr);

    /**
     * Constructor.
     */
    cNedMathFunction(const char *name, MathFunc1Arg f, int argc=-1, const char *category=nullptr, const char *description=nullptr);

    /**
     * Constructor.
     */

    cNedMathFunction(const char *name, MathFunc2Args f, int argc=-1, const char *category=nullptr, const char *description=nullptr);

    /**
     * Constructor.
     */
    cNedMathFunction(const char *name, MathFunc3Args f, int argc=-1, const char *category=nullptr, const char *description=nullptr);

    /**
     * Constructor.
     */
    cNedMathFunction(const char *name, MathFunc4Args f, int argc=-1, const char *category=nullptr, const char *description=nullptr);

    /**
     * Destructor.
     */
    virtual ~cNedMathFunction() {}
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of the object's contents.
     */
    virtual std::string str() const override;
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
     * Throws exception if actual arg count is different.
     */
    MathFuncNoArg getMathFuncNoArg() const;

    /**
     * Returns function pointer as double function with 1 double arg.
     * Throws exception if actual arg count is different.
     */
    MathFunc1Arg getMathFunc1Arg() const;

    /**
     * Returns function pointer as double function with 2 double args.
     * Throws exception if actual arg count is different.
     */
    MathFunc2Args getMathFunc2Args() const;

    /**
     * Returns function pointer as double function with 3 double args.
     * Throws exception if actual arg count is different.
     */
    MathFunc3Args getMathFunc3Args() const;

    /**
     * Returns function pointer as double function with 4 double args.
     * Throws exception if actual arg count is different.
     */
    MathFunc4Args getMathFunc4Args() const;

    /**
     * Returns a string that can be useful in classifying NED functions,
     * e.g. "trigonometric".
     */
    const char *getCategory() const  {return category.c_str();}

    /**
     * Returns the function's documentation as a string.
     */
    const char *getDescription() const  {return description.c_str();}
    //@}

    /**
     * Finds a registered function by name. Returns nullptr if not found.
     */
    static cNedMathFunction *find(const char *name, int numArgs);

    /**
     * Finds a registered function by name. Throws an error if not found.
     */
    static cNedMathFunction *get(const char *name, int numArgs);

    /**
     * Finds a registered function by function pointer.
     */
    static cNedMathFunction *findByPointer(MathFunc f);

};

// cNEDMathFunction was renamed cNedMathFunction in OMNeT++ 5.3; typedef added for compatibility
typedef cNedMathFunction cNEDMathFunction;

}  // namespace omnetpp


#endif


