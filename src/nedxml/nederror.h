//==========================================================================
// nederror.h  -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   error handling functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDERROR_H
#define __NEDERROR_H

#include "nedelement.h"

/**
 * Adds a compiler error message. Currently the message is printed to the standard
 * error; this is going to be refined.
 */
void NEDError(NEDElement *context, const char *message, ...);

/**
 * Returns true if NEDError() has been called since the last clearErrors().
 */
bool errorsOccurred();

/**
 * Resets the flag used by errorsOccurred().
 */
void clearErrors();

#define INTERNAL_ERROR0(context,msg) NEDInternalError(__FILE__,__LINE__,context,msg)
#define INTERNAL_ERROR1(context,msg,arg1) NEDInternalError(__FILE__,__LINE__,context,msg,arg1)
#define INTERNAL_ERROR2(context,msg,arg1,arg2)   NEDInternalError(__FILE__,__LINE__,context,msg,arg1,arg2)
#define INTERNAL_ERROR3(context,msg,arg1,arg2,arg3) NEDInternalError(__FILE__,__LINE__,context,msg,arg1,arg2,arg3)

/**
 * Called when an internal error occurs. It prints an appropriate message, then calls abort().
 * This method is typically used via the INTERNAL_ERROR0()...INTERNAL_ERROR4() macros that
 * add the __FILE__, __LINE__ args implicitly.
 */
void NEDInternalError(const char *file, int line, NEDElement *context, const char *message, ...);


/**
 * Low-level routines throw an exception instead of calling NEDError().
 */
class NEDException
{
  protected:
    std::string errormsg;

  public:
    /**
     * The error message can be generated in a printf-like manner.
     */
    NEDException(const char *msg,...);

    /**
     * Returns the text of the error.
     */
    const char *errorMessage() {return errormsg.c_str();}
};


#endif

