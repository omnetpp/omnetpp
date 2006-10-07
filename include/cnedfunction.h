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
#include "defs.h"
#include "globals.h"
#include "cobject.h"
#include "cdynamicexpression.h"


/**
 * A function that can be used with cDynamicExpression.
 *
 * @see cNEDFunction, Define_NED_Function().
 * @ingroup EnumsTypes
 */
typedef cDynamicExpression::StkValue (*NEDFunction)(cComponent *context,
                                                    cDynamicExpression::StkValue args[],
                                                    int numargs);


/**
 * Registration class for extending NED with new functions.
 *
 * Objects of this class are usually created via the Define_NED_Function() macro.
 *
 * @ingroup Internals
 */
class SIM_API cNEDFunction : public cNoncopyableObject
{
  private:
    NEDFunction f;             // function ptr
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

    /** @name Member access. */
    //@{
    /**
     * Returns the function pointer.
     */
    NEDFunction functionPointer()  {return f;}

    /**
     * Return type B/L/D/S/X.
     */
    char returnType() {return rettype;}

    /**
     * Returns the number of arguments
     */
    int numArgs() {return numargs;}

    /**
     * Argument types as a string of B/L/D/S/X characters.
     */
    const char *argTypes() {return argtypes.c_str();}
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

#endif


