//========================================================================
//  CCOROUTINE.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//      cCoroutine : coroutine object
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

//  Author: Andras Varga, based on Stig Kofoed's portable coroutines, see the Manual

#include <cstring>
#include <cstdio>
#include <new>  // bad::alloc
#include "omnetpp/ccoroutine.h"
#include "omnetpp/cexception.h"

#ifdef USE_PORTABLE_COROUTINES
#include "task.h"  // Stig Kofoed's "Portable Multitasking" coroutine library
#endif

namespace omnetpp {

#ifdef USE_WIN32_FIBERS

LPVOID cCoroutine::lpMainFiber;

void cCoroutine::init(unsigned totalStack, unsigned mainStack)
{
    lpMainFiber = ConvertThreadToFiber(0);
    if (!lpMainFiber) {
        DWORD err = GetLastError();
        if (err == ERROR_ALREADY_FIBER)
            lpMainFiber = GetCurrentFiber();
        if (!lpMainFiber)
            throw cRuntimeError("cCoroutine::init(): Cannot convert main thread to fiber");
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
}

cCoroutine::~cCoroutine()
{
    if (lpFiber != 0)
        DeleteFiber(lpFiber);
}

bool cCoroutine::setup(CoroutineFnp fnp, void *arg, unsigned stkSize)
{
    // stack_size sets the *committed* stack size; *reserved* (=available)
    // stack size is 1MB by default; this allows ~2048 coroutines in a
    // 2GB address space
    stackSize = stkSize;

    // XXX: CreateFiberEx() does not seem to work any better than CreateFiber(),
    // it appears to have the same limit for the number of fibers that can be created.
    // lpFiber = CreateFiberEx(stackSize, stackSize, 0, (LPFIBER_START_ROUTINE)fnp, arg);
    lpFiber = CreateFiber(stkSize, (LPFIBER_START_ROUTINE)fnp, arg);
    return lpFiber != nullptr;
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

void cCoroutine::init(unsigned totalStack, unsigned mainStack)
{
    curContextPtr = &mainContext;
    totalStackUsage = 0;
    totalStackLimit = totalStack;
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
}

cCoroutine::~cCoroutine()
{
    totalStackUsage -= stackSize;
    delete[] stackPtr;
}

bool cCoroutine::setup(CoroutineFnp fnp, void *arg, unsigned stkSize)
{
    if (totalStackLimit != 0 && totalStackUsage + stkSize >= totalStackLimit)
        return false;

    try {
        stackPtr = new char[stkSize];
        stackSize = stkSize;
    }
    catch (std::bad_alloc& e) {
        return false;
    }
    context.uc_stack.ss_sp = stackPtr;
    context.uc_stack.ss_size = stackSize;
    context.uc_link = &mainContext;
    totalStackUsage += stackSize;
    if (getcontext(&context) != 0)
        return false;
    makecontext(&context, (void (*)(void))fnp, 1, arg);
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

void cCoroutine::init(unsigned totalStack, unsigned mainStack)
{
    task_init(totalStack, mainStack);
}

void cCoroutine::switchTo(cCoroutine *cor)
{
    task_switchto(((cCoroutine *)cor)->task);
}

void cCoroutine::switchToMain()
{
    task_switchto(&main_task);
}

cCoroutine::cCoroutine()
{
    task = nullptr;
}

cCoroutine::~cCoroutine()
{
    if (task)
        task_free(task);
}

bool cCoroutine::setup(CoroutineFnp fnp, void *arg, unsigned stackSize)
{
    task = task_create(fnp, arg, stackSize);
    return task != nullptr;
}

bool cCoroutine::hasStackOverflow() const
{
    return task == nullptr ? false : task_testoverflow(task);
}

unsigned cCoroutine::getStackSize() const
{
    return task == nullptr ? 0 : task->size;
}

unsigned cCoroutine::getStackUsage() const
{
    return task == nullptr ? 0 : task_stackusage(task);
}

#endif

}  // namespace omnetpp

