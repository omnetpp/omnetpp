//==========================================================================
//  TKUTIL.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.

*--------------------------------------------------------------*/

#ifndef __TKUTIL_H
#define __TKUTIL_H

#include <tk.h>

//
// In some installations Tcl headers files have 'char*' without 'const char*'
// in arg lists -- we have to cast away 'const char*' from args in our Tcl calls.
//
#define TCLCONST(x)   const_cast<char*>(x)

//
// Print error message on console if Tcl code returns error
//
#ifdef _NDEBUG
#define CHK(tcl_eval_statement)   tcl_eval_statement
#else
#define CHK(tcl_eval_statement)    \
  do{ if (tcl_eval_statement==TCL_ERROR) \
        fprintf(stderr,"%s#%d:%s\n",__FILE__,__LINE__,interp->result); \
  } while(0)
#endif

//
// Turns exceptions into Tcl errors
//
#define TRY(code) \
  try {code;} catch (cException *e) { \
      Tcl_SetResult(interp, TCLCONST(e->message()), TCL_VOLATILE); \
      delete e; \
      return TCL_ERROR; \
  }


//
// Utility functions:
//

class cObject;
class cCollectObjectsVisitor;

typedef char *(*InfoFunc)(cObject *);

char *ptrToStr(void *ptr, char *buffer=NULL);
void *strToPtr(const char *s );
void setObjectListResult(Tcl_Interp *interp, cCollectObjectsVisitor *visitor);

int fillListboxWithChildObjects(cObject *object, Tcl_Interp *interp, const char *listbox, bool deep);
int fillListboxWithChildModules(cModule *parent, Tcl_Interp *interp, const char *listbox, bool simpleonly, bool deep);

void inspectObjectByName(const char *fullpath, const char *classname, int insptype, const char *geometry);

#endif


