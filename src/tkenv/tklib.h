//==========================================================================
//  TKLIB.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

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


char *ptrToStr(void *ptr, char *buffer=NULL);
void *strToPtr(const char *s );

class cObject;
typedef char *(*InfoFunc)(cObject *);

int fillListboxWithChildObjects(cObject *object, Tcl_Interp *interp, const char *listbox, InfoFunc infofunc, bool deep);
int fillListboxWithChildModules(cModule *parent, Tcl_Interp *interp, const char *listbox, InfoFunc infofunc, bool simpleonly, bool deep);

char *infofunc_infotext(cObject *object);
char *infofunc_module(cObject *object);

int inspectMatchingObjects(cObject *object, Tcl_Interp *interp, char *pattern, int type, bool countonly);
void inspectObjectByName(const char *fullpath, const char *classname, int insptype, const char *geometry);


//-------

struct OmnetTclCommand {
    char *namestr;
    int (*func)(ClientData, Tcl_Interp *, int, const char **);
};
extern OmnetTclCommand tcl_commands[];

extern int exit_omnetpp;

Tcl_Interp *initTk(int argc, char **argv);
int createTkCommands( Tcl_Interp *interp, OmnetTclCommand *tcl_commands );
int runTk( Tcl_Interp *interp );

