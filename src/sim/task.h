//========================================================================
//
//  TASK.H - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//          based on Stig Kofoed's portable coroutines, see the Manual
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __TASK_H
#define __TASK_H

#if defined(_AIX)
   typedef struct {char regblk[240];} my_jmp_buf[1];
   extern "C" {
     int my_setjmp(my_jmp_buf b);
     void my_longjmp(my_jmp_buf b, int retv);
   };
   #define JMP_BUF my_jmp_buf
   #define SETJMP  my_setjmp
   #define LONGJMP my_longjmp
#else
   #include <setjmp.h>
   #define JMP_BUF jmp_buf
   #define SETJMP  setjmp
   #define LONGJMP longjmp
#endif

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

#endif


