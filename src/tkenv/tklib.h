//==========================================================================
//  TKLIB.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __TKLIB_H
#define __TKLIB_H

#define WIN32_LEAN_AND_MEAN
#include "platmisc.h"   // must precede <tk.h> otherwise Visual Studio 2013 fails to compile
#include <tk.h>
#include "platdefs.h"

NAMESPACE_BEGIN

struct OmnetTclCommand {
    const char *namestr;
    int (*func)(ClientData, Tcl_Interp *, int, const char **);
};
extern OmnetTclCommand tcl_commands[];

extern int exitOmnetpp;

Tcl_Interp *initTk(int argc, char **argv);
int createTkCommands(Tcl_Interp *interp, OmnetTclCommand *tcl_commands);
int runTk(Tcl_Interp *interp);

NAMESPACE_END


#endif

