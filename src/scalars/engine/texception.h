//=========================================================================
//  TEXCEPTION.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _TEXCEPTION_H_
#define _TEXCEPTION_H_

#include <string>

/**
 * Exceptions of this type are thrown on errors during processing.
 */
class TException
{
  protected:
    std::string errormsg;

  public:
    /**
     * The error message can be generated in a printf-like manner.
     */
    TException(const char *msg,...);

    /**
     * Returns the text of the error.
     */
    const char *message() {return errormsg.c_str();}
};


#endif


