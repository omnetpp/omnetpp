//==========================================================================
//  CNEDFUNCTION.H - part of
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

#ifndef __OMNETPP_CNEDFUNCTION_H
#define __OMNETPP_CNEDFUNCTION_H

#include <cstdarg>
#include "simkerneldefs.h"
#include "globals.h"
#include "cownedobject.h"
#include "cdynamicexpression.h"

namespace omnetpp {


/**
 * @brief One of the two function signatures that can be used with cDynamicExpression.
 * This one can be used with functions that do not need context information other
 * than contextComponent for the evaluation.
 *
 * @see cNedFunction, Define_NED_Function().
 * @ingroup Misc
 */
typedef cValue (*NedFunction)(cComponent *context, cValue argv[], int argc);

/**
 * @brief One of the two function signatures that can be used with cDynamicExpression.
 * This one is more generic, as it receives all context information in the context
 * argument.
 *
 * @see cNedFunction, Define_NED_Function().
 * @ingroup Misc
 */
typedef cValue (*NedFunctionExt)(cExpression::Context *context, cValue argv[], int argc);

// NEDFunction was renamed NedFunction in OMNeT++ 5.3; typedef added for compatibility
typedef NedFunction NEDFunction;



/**
 * @brief Registration class for extending NED with arbitrary new functions.
 *
 * Objects of this class are usually created via the Define_NED_Function() macro.
 *
 * @ingroup Internals
 */
class SIM_API cNedFunction : public cNoncopyableOwnedObject
{
  public:
    enum Type { UNDEF=0, BOOL, INT, INTQ, DOUBLE, DOUBLEQ, STRING, OBJECT, XML, ANY };

  private:
    std::string signature; // function signature, as passed to the ctor
    std::vector<Type> argTypes;  // argument types
    bool hasVarargs_;      // if true, signature contains "..." after the last typed arg
    Type returnType;       // return type
    int minArgc, maxArgc;  // minimum and maximum argument count
    NedFunction f = nullptr; // implementation function
    NedFunctionExt fext = nullptr; // alternative function that takes cExpression::Context as arg
    std::string category;  // category string; only used when listing all functions
    std::string description;  // optional documentation string

  protected:
    void parseSignature(const char *signature);
    void checkArgs(cValue argv[], int argc);

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor. Signature is expected in the following syntax:
     *     returntype name(argtype argname,...),
     * where types can be bool, long, double, quantity, intquantity, string, xml, any;
     * names of optional arguments end in '?'. The object name will be the
     * function name, as extracted from the signature string. The signature
     * may end in an ellipsis, i.e. "...", to mean that any number of
     * extra args of unspecified types should be accepted. (When there are
     * both optional args and an ellipsis, then extra arguments can only be
     * passed when all optional arguments are all supplied.)
     *
     * Examples:
     *    "quantity uniform(quantity a, quantity b, long rng?)"
     *    "string sprintf(format, ...)"
     *    "any max(...)"
     */
    cNedFunction(NedFunction f, const char *signature, const char *category=nullptr, const char *description=nullptr);

    /**
     * Alternative constructor. Usage is similar to the first constructor, except that it
     * takes a function with a slightly different signature (NedFunctionExt instead of
     * NedFunction).
     */
    cNedFunction(NedFunctionExt f, const char *signature, const char *category=nullptr, const char *description=nullptr);

    /**
     * Destructor.
     */
    virtual ~cNedFunction() {}
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
     * Performs argument type checking, and invokes the function.
     */
    cValue invoke(cExpression::Context *context, cValue argv[], int argc);

    /**
     * Returns the function pointer. Do not call the function
     * directly, because that would bypass argument type validation.
     */
    NedFunction getFunctionPointer() const  {return f;}

    /**
     * Returns the functions signature, as passed to the constructor
     */
    const char *getSignature() const {return signature.c_str();}

    /**
     * Returns the function return type.
     */
    Type getReturnType() const  {return returnType;}

    /**
     * Returns the type of the kth argument.
     */
    Type getArgType(int k) const  {return argTypes[k];}

    /**
     * Returns the minimum number of arguments (i.e. the number
     * of mandatory arguments).
     */
    int getMinArgs() const  {return minArgc;}

    /**
     * Returns the maximum number of typed arguments (i.e. the last max-min
     * args are optional). If hasVarArgs() is true, the function actually
     * accepts more than getMaxArgs() arguments.
     */
    int getMaxArgs() const  {return maxArgc;}

    /**
     * Returns true if the function signature ends in an ellipsis ("..."),
     * that is, the function supports varargs.
     */
    bool hasVarArgs() const  {return hasVarargs_;}

    /**
     * Convenience method: Returns true if the function accepts the given
     * number of arguments.
     */
    bool acceptsArgCount(int argCount) {return argCount >= minArgc && (hasVarargs_ || argCount <= maxArgc);}

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
    static cNedFunction *find(const char *name);

    /**
     * Finds a registered function with the given name that accepts the given
     * number or arguments. Returns nullptr if not found.
     */
    static cNedFunction *find(const char *name, int argCount);

    /**
     * Finds a registered function by name. Throws an error if not found.
     */
    static cNedFunction *get(const char *name);

    /**
     * Finds a registered function with the given name that accepts the given
     * number or arguments. Throws an error if not found.
     */
    static cNedFunction *get(const char *name, int argCount);

    /**
     * Finds a registered function by function pointer.
     */
    static cNedFunction *findByPointer(NedFunction f);
};

// cNEDFunction was renamed cNedFunction in OMNeT++ 5.3; typedef added for compatibility
typedef cNedFunction cNEDFunction;

}  // namespace omnetpp


#endif


