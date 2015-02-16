//==========================================================================
//  HTTPSERVER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ENVIR_HTTPSERVER_H
#define __ENVIR_HTTPSERVER_H

#include "envirdefs.h"
#include "cconfiguration.h"
#include "envirext.h"

NAMESPACE_BEGIN

/**
 * Abstract base class for the built-in web server.
 */
class ENVIR_API cHttpServer
{
  public:
    virtual ~cHttpServer() {}
    virtual void start(const char *portSpec) = 0;
    virtual void stop() = 0;
    virtual void addHttpRequestHandler(cHttpRequestHandler *p) = 0;
    virtual void removeHttpRequestHandler(cHttpRequestHandler *p) = 0;
    virtual bool handleOneRequest(bool blocking) = 0; // TODO document: false if queue was empty, true otherwise
};

NAMESPACE_END

#endif
