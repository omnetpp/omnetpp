//==========================================================================
//  CNEDFUNCTION.H - part of
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

#ifndef __CNEDFUNCTION_H
#define __CNEDFUNCTION_H

#include <stdarg.h>
#include "simkerneldefs.h"
#include "globals.h"
#include "cownedobject.h"
#include "cdynamicexpression.h"

NAMESPACE_BEGIN


/**
 * A function that can be used with cDynamicExpression.
 *
 * @see cNEDFunction, Define_NED_Function().
 * @ingroup EnumsTypes
 */
typedef cNEDValue (*NEDFunction)(cComponent *context, cNEDValue argv[], int argc);


/**
 * Registration class for extending NED with new functions.
 *
 * Objects of this class are usually created via the Define_NED_Function() macro.
 *
 * @ingroup Internals
 */
class SIM_API cNEDFunction : public cNoncopyableOwnedObject
{
  private:
    std::string sign;      // function signature, as passed to the ctor
    std::string argtypes;  // sequence of B,L,D,Q,S,X,*
    bool hasvarargs;       // if true, signature contains "..." after the last typed arg
    char rettype;          // one of B,L,D,Q,S,X,*
    int minargc, maxargc;  // minimum and maximum argument count
    NEDFunction f;         // function ptr
    std::string categ;     // category string; only used when listing all functions
    std::string desc;      // optional documentation string

  protected:
    void parseSignature(const char *signature);
    void checkArgs(cNEDValue argv[], int argc);

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor. Signature is expected in the following syntax:
     *     returntype name(argtype argname,...),
     * where types can be bool, long, double, quantity, string, xml, any;
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
    cNEDFunction(NEDFunction f, const char *signature, const char *category=NULL, const char *description=NULL);

    /**
     * Destructor.
     */
    virtual ~cNEDFunction() {}
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
     * Performs argument type checking, and invokes the function.
     */
    cNEDValue invoke(cComponent *context, cNEDValue argv[], int argc);

    /**
     * Returns the function pointer. Do not call the function
     * directly, because that would bypass argument type validation.
     */
    NEDFunction getFunctionPointer() const  {return f;}

    /**
     * Returns the functions signature, as passed to the constructor
     */
    const char *getSignature() const {return sign.c_str();}

    /**
     * Returns the function return type, one of the characters B,L,D,Q,S,X,*
     * for bool, long, double, quantity, string, xml and any, respectively.
     */
    char getReturnType() const  {return rettype;}

    /**
     * Returns the type of the kth argument; result is one of the characters
     * B,L,D,Q,S,X,* for bool, long, double, quantity, string, xml and any,
     * respectively.
     */
    char getArgType(int k) const  {return argtypes[k];}

    /**
     * Returns the minimum number of arguments (i.e. the number
     * of mandatory arguments).
     */
    int getMinArgs() const  {return minargc;}

    /**
     * Returns the maximum number of typed arguments (i.e. the last max-min
     * args are optional). If hasVarArgs() is true, the function actually
     * accepts more than getMaxArgs() arguments.
     */
    int getMaxArgs() const  {return maxargc;}

    /**
     * Returns true if the function signature ends in an ellipsis ("..."),
     * that is, the function supports varargs.
     */
    bool hasVarArgs() const  {return hasvarargs;}

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
    static cNEDFunction *find(const char *name);

    /**
     * Finds a registered function by name. Throws an error if not found.
     */
    static cNEDFunction *get(const char *name);

    /**
     * Finds a registered function by function pointer.
     */
    static cNEDFunction *findByPointer(NEDFunction f);
};

NAMESPACE_END


#endif


