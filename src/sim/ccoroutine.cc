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
#include <new>  //bad::alloc
#include "ccoroutine.h"
#include "cexception.h"

NAMESPACE_BEGIN


#ifdef USE_WIN32_FIBERS

LPVOID cCoroutine::lpMainFiber;

void cCoroutine::init(unsigned total_stack, unsigned main_stack)
{
    lpMainFiber = ConvertThreadToFiber(0);
    if (!lpMainFiber)
    {
        DWORD err = GetLastError();
        if (err == ERROR_ALREADY_FIBER)
            lpMainFiber = GetCurrentFiber();
        if (!lpMainFiber)
            throw cRuntimeError("cCoroutine::init(): cannot convert main thread to fiber");
    }
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

bool cCoroutine::setup(CoroutineFnp fnp, void *arg, unsigned stack_size)
{
    // stack_size sets the *committed* stack size; *reserved* (=available)
    // stack size is 1MB by default; this allows ~2048 coroutines in a
    // 2GB address space
    stackSize = stack_size;

    // XXX: CreateFiberEx() does not seem to work any better than CreateFiber(),
    // it appears to have the same limit for the number of fibers that can be created.
    // lpFiber = CreateFiberEx(stackSize, stackSize, 0, (LPFIBER_START_ROUTINE)fnp, arg);
    lpFiber = CreateFiber(stack_size, (LPFIBER_START_ROUTINE)fnp, arg);
    return lpFiber!=NULL;
}

bool cCoroutine::hasStackOverflow() const
{
    return false;
}

unsigned cCoroutine::getStackSize() const
{
    return stackSize;
}

unsigned cCoroutine::getStackUsage() const
{
    return 0;
}

#endif

#ifdef USE_POSIX_COROUTINES

ucontext_t cCoroutine::mainContext;
ucontext_t *cCoroutine::curContextPtr;
unsigned cCoroutine::totalStackUsage;
unsigned cCoroutine::totalStackLimit;

void cCoroutine::init(unsigned total_stack, unsigned main_stack)
{
    curContextPtr = &mainContext;
    totalStackUsage = 0;
    totalStackLimit = total_stack;
}

void cCoroutine::switchTo(cCoroutine *cor)
{
    ucontext_t *oldContextPtr = curContextPtr;
    curContextPtr = &(cor->context);
    swapcontext(oldContextPtr, curContextPtr);
}

void cCoroutine::switchToMain()
{
    if (curContextPtr == &mainContext)
        return;
    ucontext_t *oldContextPtr = curContextPtr;
    curContextPtr = &mainContext;
    swapcontext(oldContextPtr, curContextPtr);
}

cCoroutine::cCoroutine()
{
    stackSize = 0;
    stackPtr = NULL;
}

cCoroutine::~cCoroutine()
{
    totalStackUsage -= stackSize;
    delete [] stackPtr;
}

bool cCoroutine::setup(CoroutineFnp fnp, void *arg, unsigned stack_size)
{
    if (totalStackLimit != 0 && totalStackUsage + stack_size >= totalStackLimit)
        return false;

    try {
        stackPtr = new char[stack_size];
        stackSize = stack_size;
    } catch (std::bad_alloc& e) {
        return false;
    }

    context.uc_stack.ss_sp = stackPtr;
    context.uc_stack.ss_size = stackSize;
    context.uc_link = &mainContext;
    totalStackUsage += stackSize;
    if (getcontext(&context) != 0)
        return false;
    makecontext(&context, (void (*)(void)) fnp, 1, arg);
    return true;
}

bool cCoroutine::hasStackOverflow() const
{
    return false;
}

unsigned cCoroutine::getStackSize() const
{
    return stackSize;
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

NAMESPACE_END

