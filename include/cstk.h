//==========================================================================
//   CSTK.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Declaration:
//    Stk : system stack for functions that use a lot of stack space
//
//  Macros:
//    iSTK(), vSTK() : interface to Stk
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSTK_H
#define __CSTK_H

#include "ccoroutine.h"

#if defined(__BORLANDC__) && __BCPLUSPLUS__<=0x310 && defined(__MSDOS__)
//#  define USE_STK         /* enables stack switching with iSTK,vSTK */
#endif

/*------
 Windowing implementations of cEnvir generally require a lot of stack space.
 When a module function calls member functions of cEnvir, either for screen
 I/O or indirectly, through objectDeleted() or messageSent(), the stack
 space allocated for that simple module may not be enough for the call.
 In order to be able to keep the simple module stack sizes low, a two macros
 are provided. When these macros are placed at the entry points of the
 stack-intensive cEnvir member functions, the macros will switch to the main
 stack. This way the call will use the main stack, which is big enough and
 not the simple module's stack, which can be kept small.
    *** The mechanism is implemented only for MS-DOS/Borland C++ 3.1!
    *** Other platforms should use BIG module stacks!
------*/

#if !defined( USE_STK )

#define iSTK(x) long x;
#define vSTK(x) long x;

#else // if defined( USE_STK )


//
// internal class: enables stack switching with the iSTK and vSTK macros
//
// ** OBSOLETE **
//
struct Stk
{
   static long _retval;
   static int _stkon;
   static int _exit;
   static JMP_BUF _jbuf;
   int _switched;

   Stk()  {_switched=0;}
   ~Stk() {if(_switched && _stkon) {_stkon=0;LONGJMP(_jbuf,1);}}

   long operator=(long l) {return _retval=l;}
};

extern "C" void use_stack(int *sp);

//--- macro for use in functions returning char/int/long
#define iSTK(x) Stk x; \
                {if(!Stk::_stkon && simulation.runningModule()!=NULL) \
                  if(SETJMP(Stk::_jbuf)==0) { \
                     x._switched=1; \
                     Stk::_stkon=1; \
                     use_stack(cCoroutine::getMainSP()-32); \
                  } else \
                    return Stk::_retval;}

//--- macro for use in void functions
#define vSTK(x) Stk x; \
                {if(!Stk::_stkon && simulation.runningModule()!=NULL) \
                  if(SETJMP(Stk::_jbuf)==0) { \
                     x._switched=1; \
                     Stk::_stkon=1; \
                     use_stack(cCoroutine::getMainSP()-32); \
                  } else \
                    return;}

#endif  // defined( USE_STK )

#endif

