//==========================================================================
//  TKLIB.H -
//                graphical network editor for
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,98 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <tk.h>

#ifdef _NDEBUG
#define CHK(tcl_eval_statement)   tcl_eval_statement
#else
#define CHK(tcl_eval_statement)    \
  do{ if (tcl_eval_statement==TCL_ERROR) \
        fprintf(stderr,"%s#%d:%s\n",__FILE__,__LINE__,interp->result); \
  } while(0)
#endif

struct OmnetTclCommand {
    char *namestr;
    int (*func)(ClientData, Tcl_Interp *, int, char **);
};
extern OmnetTclCommand tcl_commands[];

extern int exit_omnetpp;

int initTk(int argc, char **argv, Tcl_Interp *&interp );
int createTkCommands( Tcl_Interp *interp, OmnetTclCommand *tcl_commands );
int runTk( Tcl_Interp *interp );

