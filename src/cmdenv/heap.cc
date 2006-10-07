//==========================================================================
//  HEAP.CC
//
//  A safety pool heap manager with some debugging features.
//  Debugging features can be selected via #defines, see below.
//
//  This file is DEPRECATED, see below
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

//
// **********  DEPRECATED  DEPRECATED  DEPRECATED  DEPRECATED  **********
//
//  USING THESE ROUTINES FOR HEAP DEBUGGING IS DEPRECATED AND UNSUPPORTED.
//  Use tools like Valgrind or Purify instead.
//
// **********  DEPRECATED  DEPRECATED  DEPRECATED  DEPRECATED  **********
//


//
// Compile this file only if explicitly enabled
//
#ifndef USE_CMDENV_HEAPDEBUG

bool cmdenvMemoryIsLow()  {return false;}

#else

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>   // malloc

#if defined(__MSDOS__) || defined(__CONSOLE__)
#include <conio.h>
#include <alloc.h>
#endif

#include <new>


//=================================================================
//
// #defines for memory allocation debugging, turn them on as needed.
//
// They may help if you have problems with heap allocations.
//  o  #defining HEAPCHECK, COUNTBLOCKS and ALLOCTABLE is recommended
//     while you are developing.
//  o  For running a tested simulation, you can turn them off.
//
//=================================================================

//#define HEAPCHECK      /*check heap on new/delete*/
//#define COUNTBLOCKS    /*count blocks on heap & dislay if none left*/
//#define ALLOCTABLE     /*remember pointers & display heap LASTN times*/
//#define DISPLAYALL     /*report every new/delete*/
//#define DISPSTRAYS     /*report deleting of ptrs not in the table*/
//#define BKPT           /*break at new/delete #bkpt*/

#define LASTN          0  /* list heap contents when less than LASTN */
                          /* blocks are allocated */
#define TABLESIZE   8192  /* your program should not have more than */
                          /* this many blocks allocated at the same time */

#ifdef BKPT
unsigned breakat = 0; //!!!! PUT HERE THE NUMBER OF BLOCK IN QUESTION !!!!

void breakpoint()
{
     ; //!!!!!!!!!!!!!!! PUT YOUR BREAKPOINT HERE !!!!!!!!!!!!!!!!!!

    /* PURPOSE: use this function to find out
     *   o  which block was the one that was never deleted, or
     *   o  when is block #n deleted!
     * Set the 'breakat' variable or the BREAKAT #define
     * for this function to be called at a given allocation/deallocation
     * and put a breakpoint here!
     */
}
#endif


//==========================================================================
#define DEFAULT_POOLSIZE (256*1024)

//
// Safery pool memory manager
//
class MemManager
{
   private:
      //  States:
      //   safetypool      lowmem
      //     NULL           false     -> before construction or after destruction
      //   NOT NULL         false     -> ok, size of safety pool = maxpoolsize
      //     NULL           true      -> low memory, no safety pool
      //   NOT NULL         true      -> never occurs
      void *safetypool;
      int maxpoolsize;
      bool lowmem;
   public:
      MemManager(int mps = DEFAULT_POOLSIZE);
      ~MemManager();
      void makeRoom();
      void restorePool();
      bool lowMemory() {return lowmem;}
};

MemManager::MemManager(int mps)
{
    maxpoolsize = mps;
    safetypool = malloc(maxpoolsize);
    lowmem = (safetypool==NULL);
}

MemManager::~MemManager()
{
    free(safetypool);
    safetypool = NULL;
    maxpoolsize = 0;
    lowmem = false;
}

void MemManager::makeRoom()
{
    lowmem = true;
    free(safetypool);
    safetypool = NULL;
}

void MemManager::restorePool()
{
    if (lowmem)
    {
        safetypool = malloc(maxpoolsize);
        if(safetypool)
            lowmem = false;
    }
}


// static instance of MemManager and a function accessible from outside
static MemManager memManager;

bool cmdenvMemoryIsLow()
{
    return memManager.lowMemory();
}

//==========================================================================
//=== The allocation control functions: NEW,DELETE
// Defines to help debugging:
//  HEAPCHECK, COUNTBLOCKS, ALLOCTABLE, DISPLAYALL, BKPT (switches)
//  TABLESIZE, LASTN(=1,2,...) (parameters)

#if defined(__BORLANDC__)
#if (defined(__WIN32__) && !defined(__DPMI32__)) || /*Win32*/ \
   (defined(__MSDOS__) && !defined(_Windows))     /*real-mode DOS*/
#define HEAPCHECK_OK
#endif
#endif

#if defined(BKPT) || defined(DISPSTRAYS)
#define ALLOCTABLE
#endif

#if defined(DISPLAYALL) && !defined(ALLOCTABLE)
#define COUNTBLOCKS
#endif

#if defined(COUNTBLOCKS)
static int blocksonheap = 0;
#endif

#if defined(ALLOCTABLE)
struct Allocation { unsigned long id; void *ptr; };
static Allocation *alloctable;
static int lastused = -1;  // index into alloctable[]; will not decrease!
static int table_off = 0; // if table ever got full: not reliable
static int blocksintable = 0; // #blocks in table
static unsigned long last_id = 0;   // id of last allocation
#endif

#if defined(BKPT)
void brk(char *s)
{
        printf(" [HEAP.CC-debug:%s#%u: BREAKPOINT HIT - Press ENTER.]",s,breakat);
        while (getchar()!='\n');
        breakpoint();
}
#endif

#if defined(HEAPCHECK) || defined(ALLOCTABLE) || defined(COUNTBLOCKS)
int askyesno()
{
        char c;
        while (c=getchar(), toupper(c)!='Y' && toupper(c)!='N');
        while (getchar()!='\n');
        return toupper(c)=='Y';
}
#endif

#if defined(HEAPCHECK)
void checkheap(char *s)
{
#ifdef HEAPCHECK_OK
        if (heapcheck()<0) {
            printf("\n[HEAP.CC-debug:%s:HEAP CORRUPTED,continue? (y/n)]",s);
            if(!askyesno()) abort();
        }
#endif
}

int checkheapnode(char *s,void *p)
{
#ifdef HEAPCHECK_OK
        //if (p != MK_FP( FP_SEG(p), 0x0004))
        //    printf(" [HEAP.CC-debug:%s:PTR WITH BAD OFFSET]",s);
        int a = heapchecknode(p);
        if(a==_BADNODE)
            printf(" [HEAP.CC-debug:%s:NOT A VALID NODE]",s);
        else if(a==_FREEENTRY)
            printf(" [HEAP.CC-debug:%s:ALREADY FREED]",s);
        return a==_USEDENTRY;
#else
        return true;
#endif
}
#endif

#if defined(ALLOCTABLE)
void insertintotable(void *p)
{
        if (table_off) return;

        if (alloctable==NULL)
            alloctable = (Allocation *)malloc(TABLESIZE*sizeof(Allocation));

        int i;
        for (i=lastused; i>=0; i--)   // find a hole in used area
          if (alloctable[i].ptr==NULL)
             break;
        if (i==-1 && lastused<TABLESIZE-1) // expand used area
             i = lastused = lastused+1;
        if (i!=-1)
        {
            blocksintable++;
            alloctable[i].id = ++last_id;
            alloctable[i].ptr = p;
        }
        else // table really full
        {
            table_off = 1;
            printf(" [HEAP.CC-debug:NEW:ALLOCTABLE FULL,checks turned off!]");
        }
}

int delfromtable(void *p)
{
        if (table_off) return -1;

        if (alloctable==NULL)
            alloctable = (Allocation *)malloc(TABLESIZE*sizeof(Allocation));

        int i;
        for(i=0; i<=lastused; i++)
           if (alloctable[i].ptr==p) break;
        if(i>lastused) {
#ifdef DISPSTRAYS
             printf(" [HEAP.CC-debug:DELETE:BLOCK NOT IN ALLOCTABLE!]");
#endif
             return -1;
        } else {
             blocksintable--;
             alloctable[i].ptr = NULL;
             return alloctable[i].id;
        }
}
void dispheap()
{
        if(blocksintable!=0) {
             printf("\n[HEAP.CC-debug:%u blocks left - display heap? (y/n)]", blocksintable);
             if(!askyesno()) return;
             printf("\n------%u blocks still on heap------\n",blocksintable);
             for(int i=0; i<=lastused; i++)
               if(alloctable[i].ptr)
                  printf(" #%lu %p [%.20s]\n",alloctable[i].id,alloctable[i].ptr,alloctable[i].ptr);
             printf("----------end heap contents-----------\n");
        }
}
#endif

void *operator new(size_t m)
{
#ifdef HEAPCHECK
      checkheap("NEW");
#endif
      if (m==0) m=1;
      void *p;
      if ((p=malloc(m)) != NULL)   // allocation successful
      {
#ifdef COUNTBLOCKS
         blocksonheap++;
#endif
#ifdef ALLOCTABLE
         insertintotable(p);
#endif
#ifdef DISPLAYALL
#ifdef ALLOCTABLE
         if (table_off)
            printf(" [HEAP.CC-debug:NEW(%lu),alloctable full]", (long)m );
         else
            printf(" [HEAP.CC-debug:NEW#%lu (%lu),%u blocks%s]", last_id,
                    (long)m, blocksintable, memManager.lowMemory() ? " LOWMEM!":"" );
#else
         printf(" [HEAP.CC-debug:NEW(%lu),%u blocks%s]",
                    (long)m, blocksonheap, memManager.lowMemory() ? " LOWMEM!":"" );
#endif
#endif
#ifdef BKPT
         if (last_id==breakat)
            brk("NEW");
#endif
         return p;
      }
      else if (!memManager.lowMemory())   // still we have the safety pool
      {
         // try allocation again after freeing pool
         memManager.makeRoom();
         return (void *)new char[m];  // recursive call
      }
      else   // really no more memory, throw exception
      {
#ifdef COUNTBLOCKS
#ifdef ALLOCTABLE
         if (table_off)
            printf("\n[HEAP.CC-debug:NEW(%lu) FAILED,alloctable full,will throw bad::alloc!]",
                   (long)m );
         else
            printf("\n[HEAP.CC-debug: NEW#%lu (%lu) FAILED,%u blocks%s,will throw bad::alloc!]",
                   last_id, (long)m, blocksintable, memManager.lowMemory() ? " LOWMEM!":"" );
         dispheap();
#else
         printf("\n[HEAP.CC-debug:NEW (%lu) FAILED,%u blocks%s,throwing bad::alloc!]",
                 (long)m, blocksonheap, memManager.lowMemory() ? " LOWMEM!":"" );
#endif
#endif
         printf("\n[NEW (%lu) FAILED,throwing bad::alloc!]\n", (long)m );
         throw std::bad_alloc();
      }
}

void operator delete(void *p)
{
#ifdef HEAPCHECK
    checkheap("DELETE");
#endif
    if (p==NULL) return;
#ifdef HEAPCHECK
    if(!checkheapnode("DELETE",p)) return;
#endif
#ifdef COUNTBLOCKS
    bool heapempty=false;
    if (blocksonheap==0)
           printf(" [HEAP.CC-debug:DELETE:DELETE AFTER HEAP EMPTY ???]");
    else {
           blocksonheap--;
           if (blocksonheap==0)  heapempty=true;
    }
#endif
#ifdef ALLOCTABLE
    unsigned id = delfromtable(p);
#endif
#ifdef DISPLAYALL
    char *s = (char *)p;
    s[11]=0;       // ok, BC++: smallest block is 16-4 = 12 bytes long
#ifdef ALLOCTABLE
    if (id!=-1)
       printf(" [HEAP.CC-debug:DELETE#%lu ('%s'),%u blocks]",id,s,blocksintable);
#else
       printf(" [HEAP.CC-debug:DELETE ('%s'),%u blocks]",s,blocksonheap);
#endif
#endif
#ifdef ALLOCTABLE
    if (blocksintable<=LASTN) dispheap();
#endif
#ifdef COUNTBLOCKS
    if(heapempty) printf(" [HEAP.CC-debug:DELETE:ALL BLOCKS FREED OK] ");
#endif
    free(p);
    if (memManager.lowMemory())
           memManager.restorePool();
#ifdef BKPT
    if(id==breakat) brk("DELETE");
#endif
}

#endif //USE_CMDENV_HEAPDEBUG


