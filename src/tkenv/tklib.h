//==========================================================================
//  TKLIB.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __TKLIB_H
#define __TKLIB_H

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

