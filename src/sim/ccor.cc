//========================================================================
//
//  CCOR.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//   Member functions of
//      cCoroutine : coroutine object
//
//  Author: Andras Varga
//          based on Stig Kofoed's portable coroutines, see the Manual
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef __MSDOS__
#include <dos.h>
#include <float.h>
#endif

#include <string.h>
#include "ccor.h"

//------------------------------------------------------------------------
//--- cCoroutine member functions

#ifdef PORTABLE_COROUTINES

//
// All "beef" are used to test stack overflow/stack usage by checking
// whether byte patterns placed in the memory have been overwritten by
// the coroutine function's normal stack usage.
//

#define DEADBEEF       0xdeafbeef
#define EATFRAME_MAX   (sizeof(_Task)+200)

struct _Task
{
  unsigned long guardbeef1;     // contains DEADBEEF; should stay first field
  JMP_BUF jmpb;                 // jump state
  JMP_BUF rst_jmpb;             // jump state restart task
  int used;                     // used or free
  unsigned size;                // size of actually allocated block
  _Task *next;                  // pointer to next control block

  CoroutineFnp fnp;             // pointer to task function
  void *arg;                    // argument to task function
  unsigned stack_size;          // requested stack size

  _Task *prevbeef;              // pointer to previous eat() invocation's block
  unsigned long guardbeef2;     // contains DEADBEEF; should stay last field
};

static _Task main_task;
static _Task *current_task = NULL;
static JMP_BUF tmp_jmpb;

static void task_init( unsigned total_stack, unsigned main_stack );
static _Task *task_create( CoroutineFnp fnp, void *arg, unsigned stack_size );
static void task_switchto( _Task *p );
static void task_free( _Task *t );
static void task_restart( _Task *t );
static bool task_testoverflow( _Task *t );
static unsigned task_stackusage( _Task *t );

static unsigned dist( _Task *from, _Task *to )
{
    char *c1 = (char *) from,
         *c2 = (char *) to;

    return (unsigned)( c1>c2 ? c1-c2 : c2-c1 );
}

static void eat( _Task *p, unsigned size, _Task *prevbeef )
{
    unsigned d;
    _Task t;

    /* This function does the lion's share of the job. */
    /* Never returns!  p: caller task */

    /* init beef */
    t.guardbeef1 = DEADBEEF;
    t.guardbeef2 = DEADBEEF;
    t.prevbeef = prevbeef;

    /* eat stack space */
    d = dist( p, &t );
    if( d < size )
    {
        eat( p, size, &t );
    }

    /* make t a free block and link into task list */
    t.size = p->size - d;                 // set sizes
    p->size = d;
    t.used = false;
    t.next = p->next;                     // insert into list after p
    p->next = &t;

    /* free block made -- return to caller (task_init() or for(;;) loop below) */
    /* next longjmp to us will be from task_create() */
    if( SETJMP( t.jmpb ) == 0 )           // save state
    {
        memcpy(t.rst_jmpb, t.jmpb, sizeof(jmp_buf)); // save this state
        LONGJMP( p->jmpb, 1 );            // return to caller
    }

    /* setup task --> run task --> free block sequence */
    for( ;; )
    {
        /* we get here from task_create() */
        /*  it has put required stack size into t.stack_size */
        if( t.stack_size + MIN_STACKSIZE <= t.size )    // too big for us?
        {
            if( SETJMP( t.jmpb ) == 0 )    // split block to free unused space
            {
              eat( &t, t.stack_size, NULL ); // make free block
            }
        }
        t.used = true;                     // mark as used

        /* wait for next longjmp to us (that'll be to run task) */
        if( SETJMP( t.jmpb ) == 0 )        // wait
        {
            LONGJMP( tmp_jmpb, 1 );        // return to task_create()
        }

        /* run task */
        (*t.fnp)( t.arg );

        /* task finished -- free this block */
        task_free( &t );

        /* job done -- switch to main task */
        if( SETJMP( t.jmpb ) == 0 )         // save state
        {
            task_switchto( &main_task );
        }
    }
}

static void task_init( unsigned total_stack, unsigned main_stack )
{
    _Task tmp;

    tmp.size = total_stack;               // initialize total stack area
    tmp.next = NULL;
    if( SETJMP( tmp.jmpb ) == 0 )
    {
      eat( &tmp, main_stack, NULL );      // reserve main stack and create
                                          //   first free task block
    }
    main_task = tmp;                      // copy to global variable
    main_task.used = true;
    current_task = &main_task;
}

static _Task *task_create( CoroutineFnp fnp, void *arg, unsigned stack_size )
{
    _Task *p;

    for( p = main_task.next; p != NULL; p = p->next )     // find free block
    {
        if( !p->used && p->size >= stack_size )
        {
            p->fnp = fnp;                     // set task parameters
            p->arg = arg;
            p->stack_size = stack_size;
            if( SETJMP( tmp_jmpb ) == 0 )     // activate control block
            {
                LONGJMP( p->rst_jmpb, 1 );
            }
            return p;
        }
    }
    return NULL;                      // not enough stack
}


static void task_switchto( _Task *p )
{
    if( SETJMP( current_task->jmpb ) == 0  )  // save state
    {
        current_task = p;
        LONGJMP( p->jmpb, 1 );              // run next task
    }
}


static void task_free( _Task *t )
{
    t->used = false;                     // mark as free
    if( t->next != NULL && !t->next->used )
    {
        t->size += t->next->size;        // merge with following block
        t->next = t->next->next;
    }

    _Task *p = main_task.next;           // loop through list
    if( p != t )                         // if not first block
    {
        while( p->next != t )            // locate previous block
        {
            p = p->next;
        }
        if( !p->used )                   // if free
        {
            p->size += t->size;          // then merge
            p->next = t->next;
        }
    }
}

static void task_restart( _Task *p )
{
    if( SETJMP( tmp_jmpb ) == 0 )        // activate control block
    {
       LONGJMP( p->rst_jmpb, 1 );
    }
}

static bool task_testoverflow( _Task *t )
{
    if (!t->used || !t->next)
       return false;
    return t->next->guardbeef1!=DEADBEEF || t->next->guardbeef2!=DEADBEEF;
}

static unsigned task_stackusage( _Task *t )
{
    if (!t->used)
       return 0;
    if (!t->next)                        // if unable to test
       return t->stack_size;             // then return requested size

    _Task *p = t->next;
    if (p->guardbeef1!=DEADBEEF ||       // if overflow
        p->guardbeef2!=DEADBEEF ||
        (p->prevbeef!=NULL && dist(p,p->prevbeef) > EATFRAME_MAX))
       return t->size;                   // then return actual size

    /* walk backwards -- if the beef are still there, that area is untouched */
    while (p->prevbeef!=NULL &&
           p->prevbeef->guardbeef1==DEADBEEF &&
           p->prevbeef->guardbeef2==DEADBEEF &&
           dist(p->prevbeef,p->prevbeef->prevbeef) <= EATFRAME_MAX)
       p = p->prevbeef;

    return dist(t,p);
}

void cCoroutine::init( unsigned total_stack, unsigned main_stack)
{
    task_init( total_stack, main_stack );
}

void cCoroutine::switchTo( cCoroutine *cor )
{
    task_switchto( cor->task );
}

void cCoroutine::switchtoMain()
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

cCoroutine& cCoroutine::operator=(cCoroutine& cor)
{
    setup( cor.task->fnp, cor.task->arg, cor.task->stack_size );
    return *this;
}

bool cCoroutine::setup( CoroutineFnp fnp, void *arg, unsigned stack_size )
{
    task = task_create( fnp, arg, stack_size );
    return task!=NULL;
}

void cCoroutine::free()
{
    task_free(task);
    task = NULL;
}

void cCoroutine::restart()
{
    task_restart( task );
}

bool cCoroutine::stackOverflow()
{
    return task==NULL ? false : task_testoverflow( task );
}

unsigned cCoroutine::stackSize()
{
    return task==NULL ? 0 : task->size;
}

unsigned cCoroutine::stackUsage()
{
    return task==NULL ? 0 : task_stackusage( task );
}

int *cCoroutine::getMainSP()
{
#if defined(__MSDOS__)
    return (int *)MK_FP( main_task.jmpb[0].j_ss, main_task.jmpb[0].j_sp );
#elif defined(_AIX)
    return NULL; //mittomenmi;
#else
    return NULL;  //--> probably segmentation fault
#endif
}


#else /* nonportable coroutines */

#ifdef __MSDOS__
# define EMUWSP     0x104
#endif

static cCoroutine main_task;
static cCoroutine *current_task = NULL;

void cCoroutine::init( unsigned, unsigned)
{
        main_task.started = true;
        current_task = &main_task;
}

void cCoroutine::switchTo( cCoroutine *cor )
{
        if (!cor->started)
        {
             if(SETJMP(current_task->jmpbuf)==0)
             {
                cor->started = true;
                current_task->sp = get_sp_reg();
                current_task = cor;

                static CoroutineFnp _fnp;
                static void *_arg;
                _fnp = cor->fnp;
                _arg = cor->arg;
                set_sp_reg(cor->sp);   // manually switch to another stack
                (*_fnp)( _arg );       // start coroutine function
             }
        }
        else
        {
             if(SETJMP(current_task->jmpbuf)==0)
             {
                current_task->sp = get_sp_reg();
                current_task = cor;
                LONGJMP(cor->jmpbuf,1);
             }
        }
}

void cCoroutine::switchtoMain()
{
        cCoroutine::switchTo( &main_task );
}

cCoroutine::cCoroutine()
{
        stack = NULL;
}

bool cCoroutine::setup( CoroutineFnp _fnp, void *_arg, unsigned _stack_size )
{
        fnp = _fnp;
        arg = _arg;
        stack_size = _stack_size;
#ifdef __MSDOS__
        if (_8087)
        {           // 80x87 present
            stack = stkbeg = (int *)new char[stack_size];
        }
        else
        {           // 80x87 not present, emulator used
                    // it has its working memory at _SS:0000
            stack = (int *)new char[stack_size+EMUWSP+16];
            if (stack==NULL) return false;
            stkbeg = (int *)((char *)stack+EMUWSP+16);
        }
#else
        stack = stkbeg = (int *)new char[stack_size];
#endif
        stklow = (int *)((char *)stkbeg + SAFETY_AREA);

        restart();
        return true;

}

cCoroutine::~cCoroutine()
{
        free();
}

void cCoroutine::free()
{
        if (stack) delete stack;
        stack = NULL;
        stack_size = 0;
}

cCoroutine& cCoroutine::operator=(cCoroutine& cor)
{
        if (stack) delete stack;
        setup( cor.fnp, cor.arg, cor.stack_size );
        return *this;
}

void cCoroutine::restart()
{
        started = false;
        sp = (int *) ((char *)stkbeg+stack_size);

#ifdef __MSDOS__
        if (!_8087)    // fp emulator used, with its working mem at _SS:0000
        {
             sp = (int *)MK_FP( FP_SEG(stack)+1, EMUWSP+stack_size);
             _fpreset();    // init emulator workspace with the original one
             memcpy( MK_FP(FP_SEG(stack)+1,0), MK_FP(_SS,0), EMUWSP);
        }
#endif
}

bool cCoroutine::stackOverflow()
{
    return false;  // test only implemented for portable coroutines
}

unsigned cCoroutine::stackSize()
{
    return stack_size;
}

unsigned cCoroutine::stackUsage()
{
    return stack_size; // stackUsage() only implemented for portable coroutines
}

int *cCoroutine::getMainSP()
{
        return main_task.sp;
}

#endif
