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
 * Low-level coroutine class, used by cSimpleModule.
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
     * MISSINGDOC: cCoroutine:static void init(unsigned,unsigned)
     */
    static void init( unsigned total_stack, unsigned main_stack);

    /**
     * MISSINGDOC: cCoroutine:static void switchTo(cCoroutine*)
     */
    static void switchTo( cCoroutine *cor );

    /**
     * MISSINGDOC: cCoroutine:static void switchtoMain()
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
     * Assignment operator. The name member doesn't get copied;
     * see cObject's operator=() for more details.
     */
    cCoroutine& operator=(cCoroutine& cor);

    /**
     * MISSINGDOC: cCoroutine:bool setup(CoroutineFnp,void*,unsigned)
     */
    bool setup( CoroutineFnp fnp, void *arg, unsigned stack_size );

    /**
     * MISSINGDOC: cCoroutine:void free()
     */
    void free();

    /**
     * MISSINGDOC: cCoroutine:void restart()
     */
    void restart();


    /**
     * MISSINGDOC: cCoroutine:bool stackOverflow()
     */
    bool stackOverflow() _CONST;

    /**
     * MISSINGDOC: cCoroutine:unsigned stackSize()
     */
    unsigned stackSize() _CONST;

    /**
     * MISSINGDOC: cCoroutine:unsigned stackUsage()
     */
    unsigned stackUsage() _CONST;


    /**
     * MISSINGDOC: cCoroutine:int stackLeft()
     */
    int stackLeft() _CONST; // obsolete

    /**
     * MISSINGDOC: cCoroutine:bool stackLow()
     */
    bool stackLow() _CONST; // obsolete

    /**
     * MISSINGDOC: cCoroutine:static int*getMainSP()
     */
    static int *getMainSP(); // obsolete
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

