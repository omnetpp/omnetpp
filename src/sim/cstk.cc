//=========================================================================
//
//  CSTK.CC - part of
//                            OMNeT++
//             Discrete System Simulation in C++
//
//   Static members of
//      Stk   : system stack
//
//   Floating point exception handling
//
//   Mathematical error handling
//
//   Authors: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdlib.h>          // abort
#include <math.h>            // matherr
#include <string.h>
#include <signal.h>          // fp errors
#ifdef __MSDOS__
#include <dos.h>
#include <float.h>
#endif
#include "cstk.h"
#include "util.h"
#include "errmsg.h"

#ifndef __GNUC__        /* I couldn't find matherr() in Linux GNU C */
#  define USE_MATHERR
#endif

//=== static members

#ifdef USE_STK

long Stk::_retval;
int Stk::_stkon;
int Stk::_exit;
JMP_BUF Stk::_jbuf;

#endif

//==========================================================================
//=== matherr - mathematical error handler (linker masks the old one)

#ifdef USE_MATHERR

#if defined(_ULTRIX)   /*Ultrix (__ULTRIX__?)*/
#define MATH_EXCEPTION libm_exception
#elif defined(_MSC_VER) /*MSVC*/
#define MATH_EXCEPTION _exception
#else /*gcc,Borland C++,...*/
#define MATH_EXCEPTION exception
#endif

extern "C" int matherr(struct MATH_EXCEPTION *e);

int matherr(struct MATH_EXCEPTION *e)
{
      char *err;
      switch (e->type) {
        case DOMAIN:    err="Domain error"; break;
        case SING:      err="Singularity"; break;
        case OVERFLOW:  err="Overflow"; break;
        case UNDERFLOW: err="Underflow"; break;
        case TLOSS:     err="Total loss of prec."; break;
        default:        err="Unknown error";
      }
      opp_error("Math error in %s(%g [,%g]): %s",
                      e->name, e->arg1, e->arg2, err);
      e->retval = 0.0;
      return 1;
}
#endif /*USE_MATHERR*/

//==========================================================================
//=== fperr - catches floating point exceptions (SIGFPE signals)

void fperr(int);   // the signal handler

struct SigInit
{
    SigInit() {signal( SIGFPE, fperr);}
};
static SigInit siginit; // installs handler

void fperr(int)
{
#ifdef __MSDOS__
      _fpreset();
#endif
      signal( SIGFPE, fperr);  // Because it was re-set to SIGDFL.
      opp_error(eFPERR);        // Includes transfer(0)
      //exit(eFPERR);
}
