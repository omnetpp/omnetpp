//==========================================================================
//  TKLIB.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __TKLIB_H
#define __TKLIB_H

#define WIN32_LEAN_AND_MEAN
#include <tk.h>

struct OmnetTclCommand {
    char *namestr;
    int (*func)(ClientData, Tcl_Interp *, int, const char **);
};
extern OmnetTclCommand tcl_commands[];

extern int exit_omnetpp;

Tcl_Interp *initTk(int argc, char **argv);
int createTkCommands( Tcl_Interp *interp, OmnetTclCommand *tcl_commands );
int runTk( Tcl_Interp *interp );

#endif

