//==========================================================================
//  TKUTIL.H - part of
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

#ifndef __TKUTIL_H
#define __TKUTIL_H

#define WIN32_LEAN_AND_MEAN
#include <tk.h>

#include "tkdefs.h"
#include "cobject.h"

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

#define E_TRY   try {

#define E_CATCH \
  } catch (cException *e) { \
      Tcl_SetResult(interp, TCLCONST(e->message()), TCL_VOLATILE); \
      delete e; \
      return TCL_ERROR; \
  }


//
// Quoting strings for Tcl_Eval
//
class TclQuotedString
{
  private:
    char buf[80];  // used only if string is short, to spare alloc/dealloc
    char *quotedstr;
  public:
    TclQuotedString();
    TclQuotedString(const char *s);
    TclQuotedString(const char *s, int n);

    void set(const char *s);
    void set(const char *s, int n);

    ~TclQuotedString();
    const char *get() {return quotedstr;}
    operator const char *() {return quotedstr;}
};

//
// Utility functions:
//
class cCollectObjectsVisitor;

typedef char *(*InfoFunc)(cObject *);

char *voidPtrToStr(void *ptr, char *buffer=NULL);
void *strToVoidPtr(const char *s);

inline char *ptrToStr(cPolymorphic *ptr, char *buffer=NULL) {return voidPtrToStr((void *)ptr, buffer);}
inline cPolymorphic *strToPtr(const char *s) {return (cPolymorphic *)strToVoidPtr(s);}

void setObjectListResult(Tcl_Interp *interp, cCollectObjectsVisitor *visitor);

void insertIntoInspectorListbox(Tcl_Interp *interp, const char *listbox, cObject *obj, bool fullpath);

void feedCollectionIntoInspectorListbox(cCollectObjectsVisitor *visitor, Tcl_Interp *interp, const char *listbox, bool fullpath);

int fillListboxWithChildObjects(cObject *object, Tcl_Interp *interp, const char *listbox, bool deep);

void inspectObjectByName(const char *fullpath, const char *classname, int insptype, const char *geometry);

#endif



