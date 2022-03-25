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
    cConfiguration *cfg = nullptr;
  public:
    void setConfiguration(cConfiguration *cfg) {this->cfg = cfg;}
    std::string makeDebuggerCommand();
    DebuggerAttachmentPermission debuggerAttachmentPermitted();
    void attachDebugger();
    DebuggerPresence detectDebugger();
};

}  // namespace envir
}  // namespace omnetpp

#endif

