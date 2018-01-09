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
 * @brief A function that can be used with cDynamicExpression.
 *
 * @see cNedFunction, Define_NED_Function().
 * @ingroup SimSupport
 */
typedef cNedValue (*NedFunction)(cComponent *context, cNedValue argv[], int argc);

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
  private:
    std::string signature; // function signature, as passed to the ctor
    std::string argTypes;  // sequence of B,L,T,D,Q,S,X,*
    bool hasVarargs_;      // if true, signature contains "..." after the last typed arg
    char returnType;       // one of B,L,T,D,Q,S,X,*
    int minArgc, maxArgc;  // minimum and maximum argument count
    NedFunction f;         // function ptr
    std::string category;  // category string; only used when listing all functions
    std::string description;  // optional documentation string

  protected:
    void parseSignature(const char *signature);
    void checkArgs(cNedValue argv[], int argc);

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
    cNedValue invoke(cComponent *context, cNedValue argv[], int argc);

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
     * Returns the function return type, one of the characters B,L,D,Q,S,X,*
     * for bool, long, double, quantity, string, xml and any, respectively.
     */
    char getReturnType() const  {return returnType;}

    /**
     * Returns the type of the kth argument; result is one of the characters
     * B,L,D,Q,S,X,* for bool, long, double, quantity, string, xml and any,
     * respectively.
     */
    char getArgType(int k) const  {return argTypes[k];}

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
     * Finds a registered function by name. Throws an error if not found.
     */
    static cNedFunction *get(const char *name);

    /**
     * Finds a registered function by function pointer.
     */
    static cNedFunction *findByPointer(NedFunction f);
};

// cNEDFunction was renamed cNedFunction in OMNeT++ 5.3; typedef added for compatibility
typedef cNedFunction cNEDFunction;

}  // namespace omnetpp


#endif


