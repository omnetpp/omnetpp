//==========================================================================
//  PLOVE.CC -
//                output vector plotter tool for
//                            OMNeT++
//
//  This is an executable wrapper around the otherwise 100% pure Tcl/Tk
//  plove code.
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <tk.h>


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


int main(int argc, char *argv[])
{
    Tcl_Interp *interp;

    // set up Tcl/Tk
    if (initTk( argc, argv, interp )==TCL_ERROR)
    {
        fprintf(stderr,"Cannot start Plove.\n");
        return 1;
    }

    // Compiled-in TCL code
    //
    // The tclcode.cc file must be generated from the TCL scripts
    // with the tcl2c program (to be compiled from tcl2c.c).
    // The Unix makefile automatically does so; on Win95/NT, you have
    // to do it by hand: bcc tcl2c.c; tcl2c tclcode.cc *.tcl
    //
#include "tclcode.cc"
    if (Tcl_Eval(interp,(char *)tcl_code)==TCL_ERROR)
    {
         fprintf(stderr, "**** Error: %s\n", interp->result);
         return 1;
    }
    Tk_MainLoop();
    return 0;
}


