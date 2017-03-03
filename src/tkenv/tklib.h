//==========================================================================
//  TKLIB.H - part of
//
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

#ifndef __OMNETPP_TKENV_TKLIB_H
#define __OMNETPP_TKENV_TKLIB_H

#include "tkdefs.h"

namespace omnetpp {
namespace tkenv {

struct OmnetTclCommand {
    const char *namestr;
    int (*func)(ClientData, Tcl_Interp *, int, const char **);
};
extern OmnetTclCommand tcl_commands[];

extern int exitOmnetpp;

Tcl_Interp *initTk(int argc, char **argv);
int createTkCommands(Tcl_Interp *interp, OmnetTclCommand *tcl_commands);
int runTk(Tcl_Interp *interp);

} // namespace tkenv
}  // namespace omnetpp


#endif

