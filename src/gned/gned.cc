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

#include "arrow.h"
#include "parsened.h"
#include "tklib.h"

int exit_cmd(ClientData, Tcl_Interp *, int, char **);
int arrowcoords_cmd(ClientData, Tcl_Interp *, int, char **);
int parsened_cmd(ClientData, Tcl_Interp *, int, char **);

// command table
OmnetTclCommand tcl_commands[] = {
   // Commands invoked from the menu
   { "opp_exit",        exit_cmd        }, // args: none
   { "opp_arrowcoords", arrowcoords_cmd }, // args: 20 args; see arrow.cc
   { "opp_parsened",    parsened_cmd    }, // args: NED file name
   { NULL, },
};

int main(int argc, char *argv[])
{
    char *gned_dir, *bitmap_dir;
    Tcl_Interp *interp;

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

    // set up Tcl/Tk
    if (initTk( argc, argv, interp )==TCL_ERROR)
    {
        fprintf(stderr,"Cannot start GNED.\n");
        return 1;
    }

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
                         " (e.g. /home/demimoore/omnetpp/src/gned)\n"
#endif
                );
         return 1;
    }
#else
    // Case B: compiled-in TCL code
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
#endif

    // evaluate main script and build user interface
    if (Tcl_Eval(interp,"start_gned")==TCL_ERROR)
    {
         fprintf(stderr, "**** Error: %s\n", interp->result);
         return 1;
    }

    // enter Tk event loop
    runTk( interp );
    return 0;
}

int exit_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
    if (argc!=1) {interp->result="unexpected args";return TCL_ERROR;}
    exit_omnetpp = 1;
    return TCL_OK;
}

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

