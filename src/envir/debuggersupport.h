//==========================================================================
//  DEBUGGERSUPPORT.H - part of
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

#ifndef __OMNETPP_ENVIR_DEBUGGERSUPPORT_H
#define __OMNETPP_ENVIR_DEBUGGERSUPPORT_H

#include "omnetpp/cconfiguration.h"
#include "envirdefs.h"

namespace omnetpp {
namespace envir {

enum class DebuggerAttachmentPermission {
    PERMITTED,
    DENIED,
    CANT_DETECT
};

enum class DebuggerPresence {
    NOT_PRESENT,
    PRESENT,
    CANT_DETECT
};

/**
 * Functionality related to a attaching a debugger to the simulation process.
 */
class DebuggerSupport
{
  protected:
    std::string debuggerCommand;
    int attachWaitTimeSecs = 5;

  public:
    DebuggerSupport() {}
    virtual ~DebuggerSupport() {}
    virtual void configure(cConfiguration *cfg);
    virtual std::string substitutePid(const std::string& debuggerCommand);
    virtual std::string getDebuggerCommand() const {return debuggerCommand;}
    virtual void setDebuggerCommand(const std::string& cmd) {debuggerCommand = substitutePid(cmd);}
    virtual int getAttachWaitTime() const {return attachWaitTimeSecs;}
    virtual void setAttachWaitTime(int secs) {attachWaitTimeSecs = secs;}
    virtual DebuggerAttachmentPermission debuggerAttachmentPermitted();
    virtual void attachDebugger();
    virtual DebuggerPresence detectDebugger();
};

}  // namespace envir
}  // namespace omnetpp

#endif

