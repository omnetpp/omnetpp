//==========================================================================
//  GNED.CC -
//                graphical network editor for
//                            OMNeT++
//
//  contains: startup code
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


#ifdef USE_WINMAIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#include "arrow.h"
#include "parsened.h"
#include "tklib.h"

int exit_cmd(ClientData, Tcl_Interp *, int, char **);
int arrowcoords_cmd(ClientData, Tcl_Interp *, int, char **);
int parsened_cmd(ClientData, Tcl_Interp *, int, char **);

// command table
OmnetTclCommand tcl_commands[] = {
   // Commands invoked from the menu
   { "opp_arrowcoords", arrowcoords_cmd }, // args: 20 args; see arrow.cc
   { "opp_parsened",    parsened_cmd    }, // args: NED file name
   { NULL, },
};


int initGNED(Tcl_Interp *interp)
{
    char *gned_dir, *bitmap_dir;

    // path for the Tcl files
#ifdef OMNETPP_GNED_DIR
    gned_dir = getenv("OMNETPP_GNED_DIR");
    if (!gned_dir)
       gned_dir = OMNETPP_GNED_DIR;
#endif

    // path for icon directory
    bitmap_dir = getenv("OMNETPP_BITMAP_PATH");
    if (!bitmap_dir)
        bitmap_dir = OMNETPP_BITMAP_PATH;

    // add OMNeT++'s commands to Tcl
    createTkCommands( interp, tcl_commands );

    Tcl_SetVar(interp, "OMNETPP_BITMAP_PATH", bitmap_dir, TCL_GLOBAL_ONLY);

    // load sources
#ifdef OMNETPP_GNED_DIR
    // Case A: TCL code in separate .tcl files
    //
    Tcl_SetVar(interp, "OMNETPP_GNED_DIR",  gned_dir, TCL_GLOBAL_ONLY);

    char fname[256];
    sprintf(fname,"%s/gned.tcl", gned_dir);
    if (Tcl_EvalFile(interp,fname)==TCL_ERROR)
    {
         fprintf(stderr, "**** Error: %s\n", interp->result);
         fprintf(stderr, "Is OMNETPP_GNED_DIR set correctly?"
#ifdef _WIN32
                         " (e.g. C:\\omnetpp\\src\\gned)\n"
#else
                         " (e.g. /home/you/omnetpp/src/gned)\n"
#endif
                );
         return TCL_ERROR;
    }
#else
    // Case B: compiled-in TCL code
    //
    // The tclcode.cc file must be generated from the TCL scripts
    // with the tcl2c program (to be compiled from tcl2c.c).
    //
#include "tclcode.cc"
    if (Tcl_Eval(interp,(char *)tcl_code)==TCL_ERROR)
    {
         fprintf(stderr, "**** Error: %s\n", interp->result);
         return TCL_ERROR;
    }
#endif

    // evaluate main script and build user interface
    if (Tcl_Eval(interp,"start_gned")==TCL_ERROR)
    {
         fprintf(stderr, "**** Error: %s\n", interp->result);
         return TCL_ERROR;
    }

    return TCL_OK;
}


#ifdef USE_WINMAIN
/* setargv --
 *
 *	Parse the Windows command line string into argc/argv.  Done here
 *	because we don't trust the builtin argument parser in crt0.
 *	Windows applications are responsible for breaking their command
 *	line into arguments.
 *
 *      Copied here from Tk source (win/winMain.c), appending the code
 *      to set argv[0].
 */
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

    if (initGNED(interp)==TCL_ERROR)
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

int main(int argc, char *argv[])
{
    Tcl_Interp *interp;

    // set up Tcl/Tk
    if (initTk( argc, argv, interp )==TCL_ERROR)
    {
        fprintf(stderr,"Cannot start GNED.\n");
        return 1;
    }

    if (initGNED(interp)==TCL_ERROR)
    {
        return 1;
    }

    // enter Tk event loop
    runTk( interp );
    return 0;
}

#endif //USE_WINMAIN


int arrowcoords_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
    return arrowcoords(interp,argc,argv);
}

int parsened_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
    int nedfilekey = -1;
    char *nedarray = NULL;
    char *errorsarray = NULL;
    char *filename = NULL;
    char *nedtext = NULL;

    for (int i=1; i<argc; i++)
    {
       if (i+1>=argc)
       {
           interp->result="wrong arg count -- missing argument?";
           return TCL_ERROR;
       }

       if (strcmp(argv[i],"-file")==0)
          filename = argv[++i];
       else if (strcmp(argv[i],"-text")==0)
          nedtext = argv[++i];
       else if (strcmp(argv[i],"-nedarray")==0)
          nedarray = argv[++i];
       else if (strcmp(argv[i],"-errorsarray")==0)
          errorsarray = argv[++i];
       else if (strcmp(argv[i],"-nedfilekey")==0)
          nedfilekey = atoi(argv[++i]);
       else
       {
           sprintf(interp->result, "unrecognized arg %s", argv[i]);
           return TCL_ERROR;
       }
    }

    if ((filename && nedtext) || (!filename && !nedtext))
    {
       interp->result="one of -file and -text options must be present";
       return TCL_ERROR;
    }
    if (nedfilekey==-1 || !nedarray || !errorsarray)
    {
       interp->result="-nedfilekey, -nedarray, -errorsarray options must be present";
       return TCL_ERROR;
    }

    if (nedtext)
        return parseNedText(interp, nedtext,  nedfilekey, nedarray, errorsarray);
    else
        return parseNedFile(interp, filename, nedfilekey, nedarray, errorsarray);
}

