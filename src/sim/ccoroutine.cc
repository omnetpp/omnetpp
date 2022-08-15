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
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/globals.h"
#include "omnetpp/regmacros.h"

#ifdef USE_PORTABLE_COROUTINES
#include "task.h"  // Stig Kofoed's "Portable Multitasking" coroutine library
#endif

namespace omnetpp {

#ifdef USE_PORTABLE_COROUTINES  /* coroutine stacks reside in main stack area */
# define TOTAL_STACK_SIZE    (2*1024*1024)
# define MAIN_STACK_SIZE     (128*1024)
#else /* nonportable coroutines, stacks are allocated on heap */
# define TOTAL_STACK_SIZE    0  // dummy value
# define MAIN_STACK_SIZE     0  // dummy value
#endif


Register_GlobalConfigOptionU(CFGID_TOTAL_STACK, "total-stack", "B", nullptr, "Specifies the maximum memory for `activity()` simple module stacks. You need to increase this value if you get a \"Cannot allocate coroutine stack\" error.");


void cCoroutine::configure(cConfiguration *cfg)
{
    size_t totalStack = (size_t)cfg->getAsDouble(CFGID_TOTAL_STACK, TOTAL_STACK_SIZE);
    if (TOTAL_STACK_SIZE != 0 && totalStack <= MAIN_STACK_SIZE+4096) {
        //TODO:
        //if (opt->verbose)
        //    out << "Total stack size " << totalStack << " increased to " << MAIN_STACK_SIZE << endl;
        totalStack = MAIN_STACK_SIZE+4096;
    }
    init(totalStack, MAIN_STACK_SIZE);
}


#ifdef USE_WIN32_FIBERS

OPP_THREAD_LOCAL LPVOID cCoroutine::lpMainFiber;

void cCoroutine::init(unsigned /*totalStack*/, unsigned /*mainStack*/)
{
    if (lpMainFiber != 0)
        return; // already initialized
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
    // stackSize sets the *committed* stack size; *reserved* stack size (available
    // address range) is 1 MiB by default. In a 32-bit process, this limits the
    // number of fibres to less than 2048.
    stackSize = stkSize;
    lpFiber = CreateFiber(stackSize, (LPFIBER_START_ROUTINE)fnp, arg);
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

OPP_THREAD_LOCAL bool cCoroutine::initialized;
OPP_THREAD_LOCAL unsigned cCoroutine::totalStackUsage;
OPP_THREAD_LOCAL unsigned cCoroutine::totalStackLimit;
OPP_THREAD_LOCAL ucontext_t cCoroutine::mainContext;
OPP_THREAD_LOCAL ucontext_t *cCoroutine::curContextPtr;

void cCoroutine::init(unsigned totalStackReq, unsigned /*mainStack*/)
{
    if (initialized) {
        if (totalStackReq != 0 && totalStackUsage > totalStackReq)
            throw cRuntimeError("cCoroutine::init(): Already using more stack space for coroutines than the newly requested limit (usage=%u, new limit=%u)", totalStackUsage, totalStackReq);
        totalStackLimit = totalStackReq;
        return;
    }
    curContextPtr = &mainContext;
    totalStackUsage = 0;
    totalStackLimit = totalStackReq;
    initialized = true;
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

OPP_THREAD_LOCAL unsigned cCoroutine::totalStack;
OPP_THREAD_LOCAL unsigned cCoroutine::mainStack;

void cCoroutine::init(unsigned totalStackReq, unsigned mainStackReq)
{
    if (totalStack != 0)
        return; // already initialized
    if (totalStack != totalStackReq || mainStack != mainStackReq)
        throw cRuntimeError("cCoroutine::init(): Cannot change total stack size or main stack size after first the initialization (original size=%u/%u, new size=%u/%u)", totalStack, mainStack, totalStackReq, mainStackReq);
    totalStack = totalStackReq;
    mainStack = mainStackReq;
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

