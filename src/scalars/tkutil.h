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

#include <tk.h>

//
// In some installations Tcl headers files have 'char*' without 'const char*'
// in arg lists -- we have to cast away 'const char*' from args in our Tcl calls.
//
#define TCLCONST(x)   const_cast<char*>(x)
#define TCLCONST2(x)  const_cast<char**>(x)

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
// Quoting strings for Tcl_Eval
//
class TclQuotedString
{
  private:
    char buf[80];
    char *quotedstr;
  public:
    TclQuotedString(const char *s);
    TclQuotedString(const char *s, int n);
    ~TclQuotedString();
    const char *get() {return quotedstr;}
    operator const char *() {return quotedstr;}
};

#endif



