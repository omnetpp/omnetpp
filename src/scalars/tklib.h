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

#ifndef _TKLIB_H_
#define _TKLIB_H_

#include <tk.h>

#ifdef _NDEBUG
#define CHK(tcl_eval_statement)   tcl_eval_statement
#else
#define CHK(tcl_eval_statement)    \
  do{ if (tcl_eval_statement==TCL_ERROR) \
        fprintf(stderr,"%s#%d:%s\n",__FILE__,__LINE__,interp->result); \
  } while(0)
#endif

char *ptrToStr(void *ptr, char *buffer=NULL);
void *strToPtr(const char *s );

struct OmnetTclCommand {
    char *namestr;
    int (*func)(ClientData, Tcl_Interp *, int, const char **);
};
extern OmnetTclCommand tcl_commands[];

extern int exit_omnetpp;

#ifdef USE_WINMAIN
void setargv(int *argcPtr, char ***argvPtr);
#endif
void printTclError(const char *fmt,...);
int runTkApplication(int argc, const char **argv, Tcl_AppInitProc initApp);
int createTkCommands( Tcl_Interp *interp, OmnetTclCommand *tcl_commands );

#endif


