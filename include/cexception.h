//==========================================================================
//   CEXCEPTION.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Exception class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CEXCEPTION_H
#define __CEXCEPTION_H

#include <stdarg.h>  // for va_list
#include "defs.h"
#include "util.h"

class cModule;

class SIM_API cException
{
  protected:
    int errorcode;
    opp_string msg;
    opp_string modulefullpath;
    int moduleid;

  public:
    /**
     * Error is identified by an error code, and the message comes from a
     * string table. The error string may expect printf-like arguments
     * (%s, %d) which also have to be passed to the constructor.
     */
    cException(int errcode,...);

    /**
     * To be called like printf(). The error code is set to eCUSTOM.
     */
    cException(const char *msg,...);

    /**
     * Returns the error code.
     */
    int errorCode() {return errorcode;}

    /**
     * Returns the text of the error.
     */
    const char *message() {return msg;}

    /**
     * Returns the full path of the module where the exception
     * occurred.
     */
    const char *moduleFullPath() {return modulefullpath;}

    /**
     * Returns the ID of the module where the exception occurred,
     * or -1 if it was not inside a module. The module may not exist
     * any more when the exception is caught (ie. if the exception occurs
     * during network setup, the network is cleaned up immediately).
     */
    int moduleID() {return moduleid;}

    /**
     * Tell if this exception signalled an error or a normal termination.
     */
    bool isNormalTermination() const;

};

#endif
