//========================================================================
//
//  TASK.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//          based on Stig Kofoed's portable coroutines, see the Manual
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include "task.h"


_Task main_task;
_Task *current_task = NULL;
JMP_BUF tmp_jmpb;

unsigned dist( _Task *from, _Task *to )
{
    char *c1 = (char *) from,
         *c2 = (char *) to;

    return (unsigned)( c1>c2 ? c1-c2 : c2-c1 );
}

void eat( _Task *p, unsigned size, _Task *prevbeef )
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

void task_init( unsigned total_stack, unsigned main_stack )
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

_Task *task_create( _Task_fn fnp, void *arg, unsigned stack_size )
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


void task_switchto( _Task *p )
{
    if( SETJMP( current_task->jmpb ) == 0  )  // save state
    {
        current_task = p;
        LONGJMP( p->jmpb, 1 );              // run next task
    }
}


void task_free( _Task *t )
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

void task_restart( _Task *p )
{
    if( SETJMP( tmp_jmpb ) == 0 )        // activate control block
    {
       LONGJMP( p->rst_jmpb, 1 );
    }
}

bool task_testoverflow( _Task *t )
{
    if (!t->used || !t->next)
       return false;
    return t->next->guardbeef1!=DEADBEEF || t->next->guardbeef2!=DEADBEEF;
}

unsigned task_stackusage( _Task *t )
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


