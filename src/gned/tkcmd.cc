//==========================================================================
//  TKCMD.CC -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <strstream>
#include "tklib.h"
#include "arrow.h"
#include "parsened.h"

//----------------------------------------------------------------
// helper
static char *my_itol(long l, char *buf)
{
    sprintf(buf, "%ld", l);
    return buf;
}


//----------------------------------------------------------------
int arrowcoords_cmd(ClientData, Tcl_Interp *, int, const char **);
int parsened_cmd(ClientData, Tcl_Interp *, int, const char **);

// command table
OmnetTclCommand tcl_commands[] = {
   // misc
   { "opp_arrowcoords", arrowcoords_cmd }, // args: 20 args; see arrow.cc
   { "opp_parsened",    parsened_cmd    }, // args: NED file name
   // end of list
   { NULL, },
};

int arrowcoords_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    return arrowcoords(interp,argc,argv);
}

int parsened_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    int nedfilekey = -1;
    const char *nedarray = NULL;
    const char *errorsarray = NULL;
    const char *filename = NULL;
    const char *nedtext = NULL;

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

