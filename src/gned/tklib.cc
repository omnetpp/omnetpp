//==========================================================================
//  TKLIB.CC - part of
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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <tk.h>

#ifdef USE_WINMAIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


#include "tklib.h"


char *ptrToStr(void *ptr, char *buffer)
{
    static char staticbuf[20];
    if (buffer==NULL)
           buffer = staticbuf;

    if (ptr==0)
           strcpy(buffer,"ptr0");  // GNU C++'s sprintf() says "nil"
    else
           sprintf(buffer,"ptr%p", ptr );
    return buffer;
}

void *strToPtr(const char *s)
{
    if (s[0]!='p' || s[1]!='t' || s[2]!='r')
       return NULL;

    void *ptr;
    sscanf(s+3,"%p",&ptr);
    return ptr;
}


int exit_omnetpp;


#ifdef USE_WINMAIN
/* setargv --
 *
 *      Parse the Windows command line string into argc/argv.
 *      Copied here from Tk source (win/winMain.c), appending the code
 *      to set argv[0].
 */
void setargv(int *argcPtr, char ***argvPtr)
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
     * Replace argv[0] with full pathname of executable.
     * --Andras
     */
    GetModuleFileName(NULL, buffer, sizeof(buffer));
    argv[0] = buffer;

    /* Replace all backslashes with forward slashes.
     * --Andras
     */
    for (int i=0; i<argc; i++) {
        for (p = argv[i]; *p != '\0'; p++) {
            if (*p == '\\') {
                *p = '/';
            }
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
#endif


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

int runTkApplication(int argc, const char *argv[], Tcl_AppInitProc initApp)
{
    // create interpreter
    Tcl_Interp *interp = Tcl_CreateInterp();

    // pass application name to interpreter
    Tcl_FindExecutable(TCLCONST(argv[0]));

    // Make command-line arguments available in Tcl variables "argc" and "argv"
    char buf[16];
    char *args = Tcl_Merge(argc-1, TCLCONST2(argv+1));
    Tcl_SetVar(interp, "argv", args, TCL_GLOBAL_ONLY);
    Tcl_Free(args);
    sprintf(buf, "%d", argc-1);
    Tcl_SetVar(interp, "argc", buf, TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp, "argv0", TCLCONST(argv[0]), TCL_GLOBAL_ONLY);

    // load packages
    if (initApp(interp)!=TCL_OK)
        return TCL_ERROR;

    // some Tk stuff
    Tk_Window mainWindow = Tk_MainWindow(interp);
    Tk_SetAppName( mainWindow, "omnetpp" );
    Tk_SetClass( mainWindow, "Omnetpp" );

    // Register X error handler and ask for synchr. protocol to help debugging
    Tk_CreateErrorHandler( Tk_Display(mainWindow), -1,-1,-1, XErrorProc, (ClientData)mainWindow );

    // Custom event loop -- the C++ variable exit_omnetpp is used for exiting
    while (!exit_omnetpp)
    {
       Tk_DoOneEvent(TK_ALL_EVENTS);
    }

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

void printTclError(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
#ifdef USE_WINMAIN
    //char messagebuf[1024];
    //strcpy(messagebuf, "Tcl error: ");
    //vsprintf(messagebuf+strlen(messagebuf),fmt,va);
    //MessageBox(0, messagebuf, "Error", MB_OK|MB_ICONERROR);
#else
    fprintf(stderr, "<!> Tcl error: ");
    vfprintf(stderr,fmt,va);
    fprintf(stderr, "\n");
#endif
    va_end(va);
}

