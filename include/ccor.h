//==========================================================================
//   CCOR.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Declarations:
//    class cCoroutine: coroutines
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCOR_H
#define __CCOR_H

#include "cobject.h"  // bool (for old C++)

// The following define selects which coroutine-set to use.
// The nonportable version (module stacks allocated on heap)
// is selected for 16-bit MSDOS (otherwise the total of coroutine
// stacks would be limited to 64K.)
#if !defined(__BORLANDC__) || defined(__FLAT__) || defined(__DPMI16__)
#  define PORTABLE_COROUTINES
#endif

// Getting around the unwanted behaviour of longjmp() under AIX
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

// coroutine function pointer
typedef void (*CoroutineFnp)( void * );

#ifdef PORTABLE_COROUTINES

struct _Task;

#else /* nonportable coroutines */

extern "C"
{
  void set_sp_reg(int *sp);   // overwrite SS:SP (or equiv.on a diff.platform)
  int *get_sp_reg();          // return SS:SP
};

#endif

//--------------------------------------------------------------------------

/**
 * Low-level coroutine class, used by cSimpleModule. The present
 * implementation uses Stig Kofoed's "Portable coroutines" (see Manual
 * for reference). It creates all coroutine stacks within the main
 * stack, and uses setjmp()/longjmp() for context switching. This
 * implies that the maximum stack space allowed by the operating system
 * for the OMNeT++ process must be sufficiently high (several,
 * maybe several hundred megabytes), otherwise a segmentation fault
 * will occur.
 *
 * Future versions may use platform-specific support for coroutines,
 * such as the Fiber library in the Win32 API.
 *
 * @ingroup Internals
 */
class SIM_API cCoroutine
{
  private:
#ifdef PORTABLE_COROUTINES
    _Task *task;
    bool started;           // true after 1st stack switch
#else  /* nonportable coroutines */
    CoroutineFnp fnp;       // pointer to task function
    void *arg;              // argument to task function
    int stack_size;         // size of stack
    int *stack;             // ptr to stack allocated on heap
    int *stkbeg;            // beginning of stack
    int *stklow;            // stkbeg + safety area
    int *sp;                // stack ptr (not used in task switches)
    JMP_BUF jmpbuf;         // task state (incl. stack ptr)
    bool started;           // true after 1st stack switch
#endif

  public:
    /**
     * Initializes the coroutine library. This function has to be called
     * exactly once in a program, possibly at the top of main().
     */
    static void init( unsigned total_stack, unsigned main_stack);

    /**
     * Switch to another coroutine. The execution of the current coroutine
     * is suspended and the other coroutine is resumed from the point it
     * last left off.
     */
    static void switchTo( cCoroutine *cor );

    /**
     * Switch to the main coroutine (the one main() runs in).
     */
    static void switchtoMain();

    /**
     * Constructor.
     */
    cCoroutine();

    /**
     * Destructor.
     */
    ~cCoroutine();

    /**
     * Assignment is not implemented for coroutines.
     */
    cCoroutine& operator=(_CONST cCoroutine& cor);

    /**
     * Sets up a coroutine. The arguments are the function that should be
     * run in the coroutine, a pointer that is passed to the coroutine
     * function, and the stack size.
     */
    bool setup( CoroutineFnp fnp, void *arg, unsigned stack_size );

    /**
     * Delete coroutine and release its stack space.
     */
    void free();

    /**
     * Restart the coroutine.
     */
    void restart();

    /**
     * Returns true if there was a stack overflow during execution of the
     * coroutine. It checks the intactness of a predefined byte pattern
     * (0xdeadbeef) at the stack boundary, and report stack overflow
     * if it was overwritten. The mechanism usually works fine, but occasionally
     * it can be fooled by large uninitialized local variables
     * (e.g. char buffer[256]): if the byte pattern happens to fall in the
     * middle of such a local variable, it may be preserved intact and
     * stack violation is not detected.
     */
    bool stackOverflow() _CONST;

    /**
     * Returns the stack size of the coroutine. This is the same number
     * as the one passed to setup().
     */
    unsigned stackSize() _CONST;

    /**
     * Returns the amount of stack actually used by the coroutine.
     * It works by checking the intactness of predefined byte patterns
     * (0xdeadbeef) placed in the stack.
     */
    unsigned stackUsage() _CONST;

    /**
     * Obsolete.
     */
    int stackLeft() _CONST;

    /**
     * Obsolete.
     */
    bool stackLow() _CONST;

    /**
     * Obsolete.
     */
    static int *getMainSP();
};

#ifdef PORTABLE_COROUTINES
inline int cCoroutine::stackLeft() _CONST {return 0;}
inline bool cCoroutine::stackLow() _CONST {return false;}
//inline int *cCoroutine::getSP() _CONST    {return NULL;}
#else /* nonportable coroutines */
inline int cCoroutine::stackLeft() _CONST {return (char *)sp - (char *)stkbeg;}
inline bool cCoroutine::stackLow() _CONST {return sp<stklow;}
//inline int *cCoroutine::getSP() _CONST    {return sp;}
#endif

#endif

