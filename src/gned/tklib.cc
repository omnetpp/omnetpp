//==========================================================================
//  TKLIB.CC -
//                graphical network editor for
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <tk.h>

#include "tklib.h"

//=== the following lines come from a sample tclApp.c
/*
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl.
 */
//extern int matherr();
//int *tclDummyMathPtr = (int *) matherr;

//=== Following source based on:
//  Brent Welch: Practical Programming in Tcl and Tk
//  Chapter 30: C programming and Tk; A Custom Main Program
//

int exit_omnetpp;

//static Tk_ArgvInfo argTable[] =
//  {
//    {"-display", TK_ARGV_STRING,   (char *)NULL, (char *)&display, "Display to use"},
//    {"-debug",   TK_ARGV_CONSTANT, (char *)1,    (char *)&debug,   "Set things up for gdb-style debugging"},
//    {"",         TK_ARGV_END,},
//  };

// Procedure to handle X errors
static int XErrorProc( ClientData, XErrorEvent *errEventPtr)
{
    // Tk_Window w = (Tk_Window)data;
    fprintf(stderr, "X protocol error: ");
    fprintf(stderr, "error=%d request=%d minor=%d\n",
                    errEventPtr->error_code,
                    errEventPtr->request_code,
                    errEventPtr->minor_code );
    return 0;  // claim to have handled the error
}

// initialize Tcl/Tk and return a pointer to the interpreter
int initTk(int argc, char **argv, Tcl_Interp *&interp )
{
    // create interpreter
    interp = Tcl_CreateInterp();

    // pass application name to interpreter
    Tcl_FindExecutable(argv[0]);

    //if (Tk_ParseArgv(interp,(Tk_Window)NULL, &argc,argv,argTable,0)!=TCL_OK)
    //{
    //    fprintf(stderr, "%s\n", interp->result);
    //    return TCL_ERROR;
    //}

    if (Tcl_Init(interp) != TCL_OK)
    {
        fprintf(stderr, "Tcl_Init failed: %s\n", interp->result);
        return TCL_ERROR;
    }

    if (Tk_Init(interp) != TCL_OK)
    {
        fprintf(stderr, "Tk_Init failed: %s\n", interp->result);
        return TCL_ERROR;
    }

    // Make command-line arguments available in Tcl variables "argc" and "argv"
    char buf[16];
    char *args = Tcl_Merge(argc-1, argv+1);
    Tcl_SetVar(interp, "argv", args, TCL_GLOBAL_ONLY);
    Tcl_Free(args);
    sprintf(buf, "%d", argc-1);
    Tcl_SetVar(interp, "argc", buf, TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp, "argv0", argv[0], TCL_GLOBAL_ONLY);


    Tcl_StaticPackage(interp, "Tk", Tk_Init, (Tcl_PackageInitProc *) NULL);

    Tk_Window mainWindow = Tk_MainWindow(interp);

    Tk_SetAppName( mainWindow, "omnetpp" );
    Tk_SetClass( mainWindow, "Omnetpp" );

    // Register X error handler and ask for synchr. protocol to help debugging
    Tk_CreateErrorHandler( Tk_Display(mainWindow), -1,-1,-1, XErrorProc, (ClientData)mainWindow );

    // Grab initial size and background
    Tk_GeometryRequest(mainWindow,200,200);

    return TCL_OK;
}

// create custom commands (implemented in tkcmd.cc) in Tcl
int createTkCommands( Tcl_Interp *interp, OmnetTclCommand *commands)
{
    for(;commands->namestr!=NULL; commands++)
    {
        Tcl_CreateCommand( interp, commands->namestr, 
                           (Tcl_CmdProc *)commands->func,
                           (ClientData)NULL, 
                           (Tcl_CmdDeleteProc *)NULL);
    }
    return TCL_OK;
}

// run the Tk application
int runTk( Tcl_Interp *)
{
    // Custom event loop
    //  the C++ variable exit_omnetpp is used for exiting
    while (!exit_omnetpp)
    {
       Tk_DoOneEvent(TK_ALL_EVENTS);
    }

    return TCL_OK;
}

