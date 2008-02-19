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
                                                 cDynamicExpression::Value args[],
                                                 int numargs);


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
    NEDFunction f;         // function ptr
    char rettype;          // one of B,L,D,S,X,*  FIXME document that "*" is for any/unknown
    std::string argtypes;  // sequence of type characters B,L,D,S,X,*
    int numargs;           // cached strlen(argtypes)

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor. FIXME document
     */
    cNEDFunction(const char *name, NEDFunction f, const char *argtypes, const char *returntype);

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
    //@}

    /** @name Member access. */
    //@{
    /**
     * Returns the function pointer.
     */
    NEDFunction functionPointer() const  {return f;}

    /**
     * Return type B/L/D/S/X.
     */
    char returnType() const  {return rettype;}

    /**
     * Returns the number of arguments
     */
    int numArgs() const  {return numargs;}

    /**
     * Argument types as a string of B/L/D/S/X characters.
     */
    const char *argTypes() const  {return argtypes.c_str();}
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


