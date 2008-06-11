//========================================================================
//  CCOROUTINE.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//      cCoroutine : coroutine object
//
//  Author: Andras Varga
//          based on Stig Kofoed's portable coroutines, see the Manual
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include "ccoroutine.h"

USING_NAMESPACE


#ifdef USE_WIN32_FIBERS

LPVOID cCoroutine::lpMainFiber;

void cCoroutine::init(unsigned total_stack, unsigned main_stack)
{
    lpMainFiber = ConvertThreadToFiber(0);
}

void cCoroutine::switchTo(cCoroutine *cor)
{
    SwitchToFiber(cor->lpFiber);
}

void cCoroutine::switchToMain()
{
    SwitchToFiber(lpMainFiber);
}

cCoroutine::cCoroutine()
{
    lpFiber = 0;
}

cCoroutine::~cCoroutine()
{
    if (lpFiber!=0)
        DeleteFiber(lpFiber);
}

bool cCoroutine::setup(CoroutineFnp fnp, void *arg, unsigned stack_size )
{
    // stack_size sets the *committed* stack size; *reserved* (=available)
    // stack size is 1MB by default; this allows ~2048 coroutines in a
    // 2GB address space
    stacksize = stack_size;
    //XXX: CreateFiberEx() does not seem to work any better than CreateFiber(),
    //it appears to have the same limit for the number of fibers that can be created.
    //lpFiber = CreateFiberEx(stacksize, stacksize, 0, (LPFIBER_START_ROUTINE)fnp, arg);
    lpFiber = CreateFiber(stack_size, (LPFIBER_START_ROUTINE)fnp, arg);
    return lpFiber!=NULL;
}

bool cCoroutine::hasStackOverflow() const
{
    return false;
}

unsigned cCoroutine::getStackSize() const
{
    return stacksize;
}

unsigned cCoroutine::getStackUsage() const
{
    return 0;
}

#endif

#ifdef USE_PORTABLE_COROUTINES

#include "task.h"  // Stig Kofoed's "Portable Multitasking" coroutine library

void cCoroutine::init( unsigned total_stack, unsigned main_stack)
{
    task_init( total_stack, main_stack );
}

void cCoroutine::switchTo(cCoroutine *cor)
{
    task_switchto( ((cCoroutine *)cor)->task );
}

void cCoroutine::switchToMain()
{
    task_switchto( &main_task );
}

cCoroutine::cCoroutine()
{
    task = NULL;
}

cCoroutine::~cCoroutine()
{
    if (task) task_free( task );
}

bool cCoroutine::setup(CoroutineFnp fnp, void *arg, unsigned stack_size)
{
    task = task_create( fnp, arg, stack_size );
    return task!=NULL;
}

bool cCoroutine::hasStackOverflow() const
{
    return task==NULL ? false : task_testoverflow( task );
}

unsigned cCoroutine::getStackSize() const
{
    return task==NULL ? 0 : task->size;
}

unsigned cCoroutine::getStackUsage() const
{
    return task==NULL ? 0 : task_stackusage( task );
}

#endif

