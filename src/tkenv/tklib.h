//==========================================================================
//  TKFUNCS.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
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

//-------

char *ptrToStr( void *ptr, char *buffer=NULL);
void *strToPtr( char *s );

//-------

void setTypes(void *data, ...);

//-------

class cObject;
typedef char *(*InfoFunc)(cObject *);

void collection( cObject *object, Tcl_Interp *interp, char *listbox, InfoFunc infofunc, bool deep);
void modcollection(cModule *parent, Tcl_Interp *interp, char *listbox, InfoFunc infofunc, bool simpleonly, bool deep );

char *infofunc_nameonly( cObject *object);
char *infofunc_infotext( cObject *object);
char *infofunc_fullpath( cObject *object);
char *infofunc_typeandfullpath( cObject *object);
char *infofunc_module( cObject *object);

int inspect_matching(cObject *object, Tcl_Interp *interp, char *pattern, int type, bool countonly);
void inspect_by_name(const char *fullpath, const char *classname, int insptype, const char *geometry);

//-------

struct OmnetTclCommand {
    char *namestr;
    int (*func)(ClientData, Tcl_Interp *, int, char **);
};
extern OmnetTclCommand tcl_commands[];

extern int exit_omnetpp;

Tcl_Interp *initTk(int argc, char **argv);
int createTkCommands( Tcl_Interp *interp, OmnetTclCommand *tcl_commands );
int runTk( Tcl_Interp *interp );

