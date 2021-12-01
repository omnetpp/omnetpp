//========================================================================
//  TASK.H - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

//  Author: Andras Varga, based on Stig Kofoed's portable coroutines, see the Manual

#ifndef __OMNETPP_TASK_H
#define __OMNETPP_TASK_H

#include <csetjmp>
#include "omnetpp/platdep/platdefs.h"

namespace omnetpp {

#define JMP_BUF jmp_buf
#define SETJMP  setjmp
#define LONGJMP longjmp

//=== some defines
#define SAFETY_AREA     512
#define MIN_STACKSIZE  1024

//
// All "beef" are used to test stack overflow/stack usage by checking
// whether byte patterns placed in the memory have been overwritten by
// the coroutine function's normal stack usage.
//

#define DEADBEEF       0xdeafbeef
#define EATFRAME_MAX   (sizeof(_Task)+200)

typedef void (*_Task_fn)( void * );

struct _Task
{
  unsigned long guardbeef1;     // contains DEADBEEF; should stay first field
  JMP_BUF jmpb;                 // jump state
  JMP_BUF rst_jmpb;             // jump state restart task
  int used;                     // used or free
  unsigned size;                // size of actually allocated block
  _Task *next;                  // pointer to next control block

  _Task_fn fnp;                  // pointer to task function
  void *arg;                    // argument to task function
  unsigned stack_size;          // requested stack size

  _Task *prevbeef;              // pointer to previous eat() invocation's block
  unsigned long guardbeef2;     // contains DEADBEEF; should stay last field
};

extern _Task main_task;
extern _Task *current_task;
extern JMP_BUF tmp_jmpb;

void task_init( unsigned total_stack, unsigned main_stack );
_Task *task_create( _Task_fn fnp, void *arg, unsigned stack_size );
void task_switchto( _Task *p );
void task_free( _Task *t );
void task_restart( _Task *t );
bool task_testoverflow( _Task *t );
unsigned task_stackusage( _Task *t );

}  // namespace omnetpp


#endif


