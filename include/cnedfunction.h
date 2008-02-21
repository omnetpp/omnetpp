//==========================================================================
//  CNEDFUNCTION.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

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
typedef cDynamicExpression::Value (*NEDFunction)(cComponent *context,
                                                 cDynamicExpression::Value argv[],
                                                 int argc);


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
    char rettype;          // one of B,L,D,Q,S,X,*
    int minargc, maxargc;  // minimum and maximum argument count
    NEDFunction f;         // function ptr

  protected:
      void parseSignature(const char *signature);
      void checkArgs(cDynamicExpression::Value argv[], int argc);

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor. Signature is expected in the following syntax:
     *  - argument list and return type are separated with "->"
     *  - argument types and return types are one of: B,L,D,Q,S,X,*
     *    (for Bool, Long, Double, Quantity, String, XML, any)
     *  - "/" marks end of mandatory args and start of optional args
     * Examples: "QQ->D"; "SD/D->S"
     */
    cNEDFunction(const char *name, NEDFunction f, const char *signature);

    /**
     * Destructor.
     */
    virtual ~cNEDFunction() {}
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of object contents.
     */
    virtual std::string info() const;

    /**
     * Produces more detailed description of object contents.
     */
    virtual std::string detailedInfo() const;

    //@}

    /** @name Member access. */
    //@{
    /**
     * Performs argument type checking, and invokes the function.
     */
    cDynamicExpression::Value invoke(cComponent *context, cDynamicExpression::Value argv[], int argc);

    /**
     * Returns the function pointer. Do not call the function
     * directly, because that would bypass argument type validation.
     */
    NEDFunction functionPointer() const  {return f;}

    /**
     * Returns the functions signature, as passed to the constructor
     */
    const char *signature() const {return sign.c_str();}

    /**
     * Returns the function return type, one of: B,L,D,Q,S,X,*
     */
    char returnType() const  {return rettype;}

    /**
     * Returns the type of the kth argument; result is
     * one of: B,L,D,Q,S,X,*
     */
    char argType(int k) const  {return argtypes[k];}

    /**
     * Returns the minimum number of arguments (i.e. the number
     * of mandatory arguments).
     */
    int minArgs() const  {return minargc;}

    /**
     * Returns the maximum number of arguments (i.e. the last max-min
     * args are optional).
     */
    int maxArgs() const  {return maxargc;}
    //@}

    /**
     * Finds a registered function by name.
     */
    static cNEDFunction *find(const char *name, int numArgs);

    /**
     * Finds a registered function by function pointer.
     */
    static cNEDFunction *findByPointer(NEDFunction f);
};

NAMESPACE_END


#endif


