//==========================================================================
//  PLOVE.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <tk.h>

#ifdef USE_WINMAIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


int initPlove(Tcl_Interp *interp)
{
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
         return TCL_ERROR;
    }
    if (Tcl_Eval(interp,"startPlove $argv")==TCL_ERROR)
    {
         fprintf(stderr, "**** Error: %s\n", interp->result);
         return TCL_ERROR;
    }
    return TCL_OK;
}


#ifdef USE_WINMAIN
//
// setargv()
//
//  Parse the Windows command line string into argc/argv.
//  Copied here from Tk source (win/winMain.c), appending the code
//  to set argv[0].
//
static void setargv(int *argcPtr, char ***argvPtr)
{
    char *cmdLine, *p, *arg, *argSpace;
    char **argv;
    int argc, size, inquote, copy, slashes;
    char buffer[MAX_PATH];

    cmdLine = GetCommandLine();

    /*
     * Precompute an overly pessimistic guess at the number of arguments
     * in the command line by counting non-space spans.
     */
    size = 2;
    for (p = cmdLine; *p != '\0'; p++) {
        if (isspace(*p)) {
            size++;
            while (isspace(*p)) {
                p++;
            }
            if (*p == '\0') {
                break;
            }
        }
    }

    /*
     * We must add '--' as the first command-line arg. Therefore we increment
     * size by 1.
     * --Andras
     */
    size++;

    argSpace = (char *) ckalloc((unsigned) (size * sizeof(char *)
            + strlen(cmdLine) + 1));
    argv = (char **) argSpace;
    argSpace += size * sizeof(char *);
    size--;

    /*
     * We must make room for the '--', so shift argv[] by one
     * --Andras
     */
    argv++;

    p = cmdLine;
    for (argc = 0; argc < size; argc++) {
        argv[argc] = arg = argSpace;
        while (isspace(*p)) {
            p++;
        }
        if (*p == '\0') {
            break;
        }

        inquote = 0;
        slashes = 0;
        while (1) {
            copy = 1;
            while (*p == '\\') {
                slashes++;
                p++;
            }
            if (*p == '"') {
                if ((slashes & 1) == 0) {
                    copy = 0;
                    if ((inquote) && (p[1] == '"')) {
                        p++;
                        copy = 1;
                    } else {
                        inquote = !inquote;
                    }
                }
                slashes >>= 1;
            }

            while (slashes) {
                *arg = '\\';
                arg++;
                slashes--;
            }

            if ((*p == '\0') || (!inquote && isspace(*p))) {
                break;
            }
            if (copy != 0) {
                *arg = *p;
                arg++;
            }
            p++;
        }
        *arg = '\0';
        argSpace = arg + 1;
    }
    argv[argc] = NULL;

    /*
     * Replace argv[0] with full pathname of executable, and forward
     * slashes substituted for backslashes.
     * --Andras
     */
    GetModuleFileName(NULL, buffer, sizeof(buffer));
    argv[0] = buffer;
    for (p = buffer; *p != '\0'; p++) {
        if (*p == '\\') {
            *p = '/';
        }
    }

    /*
     * Insert the '--' arg.
     * --Andras
     */
    argv--;
    argc++;
    argv[0] = argv[1];
    argv[1] = "--";

    *argcPtr = argc;
    *argvPtr = argv;

}

static int Tcl_AppInit(Tcl_Interp *interp)
{
    if (Tcl_Init(interp) == TCL_ERROR)
         return TCL_ERROR;
    if (Tk_Init(interp) == TCL_ERROR)
         return TCL_ERROR;
    Tcl_StaticPackage(interp, "Tk", Tk_Init, Tk_SafeInit);

    if (initPlove(interp)==TCL_ERROR)
        return TCL_ERROR;

    return TCL_OK;
}

int APIENTRY
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    Tcl_Interp *interp;
    char **argv;
    int argc;

    setargv(&argc, &argv);

    // enter Tk event loop
    Tk_Main(argc, argv, Tcl_AppInit);
    return 1;
}

#else //not USE_WINMAIN

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

    if (initPlove(interp)==TCL_ERROR)
    {
        return 1;
    }

    // enter Tk event loop
    Tk_MainLoop();
    return 0;
}

#endif //USE_WINMAIN


