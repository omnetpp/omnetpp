//==========================================================================
//   CCOROUTINE.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Declarations:
//    class cCoroutine: coroutines
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCOROUTINE_H
#define __CCOROUTINE_H

// select coroutine library
#ifdef _WIN32
#define USE_WIN32_FIBERS
#else
#define USE_PORTABLE_COROUTINES
#endif

#include "defs.h"


#ifdef USE_WIN32_FIBERS
// Fiber API is not accessible in MSVC6.0 without a hack (next 3 lines):
#if _MSC_VER>=1200
#define _WIN32_WINNT 0x0400
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// their min() and max() macros interfere with us
#undef min
#undef max
#endif

#ifdef USE_PORTABLE_COROUTINES
struct _Task;
#endif


/**
 * Prototype for functions that can be used with cCoroutine objects as
 * coroutine bodies.
 * @ingroup EnumsTypes
 */
typedef void (*CoroutineFnp)( void * );

//--------------------------------------------------------------------------

/**
 * Low-level coroutine library. Coroutines are used by cSimpleModule.
 *
 * cCoroutine has platform-dependent implementation:
 *
 * On Windows, it uses the Win32 Fiber API.

 * On other platforms, the implementation a portable coroutine library,
 * first described by Stig Kofoed ("Portable coroutines", see the Manual
 * for a better reference). It creates all coroutine stacks within the main
 * stack, and uses setjmp()/longjmp() for context switching. This
 * implies that the maximum stack space allowed by the operating system
 * for the \opp process must be sufficiently high (several,
 * maybe several hundred megabytes), otherwise a segmentation fault
 * will occur.
 *
 * @ingroup Internals
 */
class SIM_API cCoroutine
{
  protected:
#ifdef USE_WIN32_FIBERS
    LPVOID lpFiber;
    static LPVOID lpMainFiber;
    unsigned stacksize;
#endif
#ifdef USE_PORTABLE_COROUTINES
    _Task *task;
#endif

  public:
    /** Coroutine control */
    //@{

    /**
     * Initializes the coroutine library. This function has to be called
     * exactly once in a program, possibly at the top of main().
     */
    static void init(unsigned total_stack, unsigned main_stack);

    /**
     * Switch to another coroutine. The execution of the current coroutine
     * is suspended and the other coroutine is resumed from the point it
     * last left off.
     */
    static void switchTo(cCoroutine *cor);

    /**
     * Switch to the main coroutine (the one main() runs in).
     */
    static void switchToMain();
    //@}

    /** Constructor, destructor */
    //@{

    /**
     * Sets up a coroutine. The arguments are the function that should be
     * run in the coroutine, a pointer that is passed to the coroutine
     * function, and the stack size.
     */
    bool setup(CoroutineFnp fnp, void *arg, unsigned stack_size);

    /**
     * Constructor.
     */
    cCoroutine();

    /**
     * Destructor.
     */
    virtual ~cCoroutine();
    //@}

    /** Coroutine statistics */
    //@{

    /**
     * Returns true if there was a stack overflow during execution of the
     * coroutine.
     *
     * Windows/Fiber API: Not implemented: always returns false.
     *
     * Portable coroutines: it checks the intactness of a predefined byte pattern
     * (0xdeadbeef) at the stack boundary, and report stack overflow
     * if it was overwritten. The mechanism usually works fine, but occasionally
     * it can be fooled by large uninitialized local variables
     * (e.g. char buffer[256]): if the byte pattern happens to fall in the
     * middle of such a local variable, it may be preserved intact and
     * stack violation is not detected.
     */
    virtual bool stackOverflow() const;

    /**
     * Returns the stack size of the coroutine.
     */
    virtual unsigned stackSize() const;

    /**
     * Returns the amount of stack actually used by the coroutine.
     *
     * Windows/Fiber API: Not implemented, always returns 0.
     *
     * Portable coroutines: It works by checking the intactness of
     * predefined byte patterns (0xdeadbeef) placed in the stack.
     */
    virtual unsigned stackUsage() const;
    //@}
};

#endif


