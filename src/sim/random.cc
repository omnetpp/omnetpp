//=========================================================================
//
//  RANDOM.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Utility functions
//
//   Random number functions
//     randseed :
//     intrand  : random number in range 0..(2^31-1)
//     other random functions
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <time.h>
#include "random.h"
#include "macros.h"  // Define_Function()
#include "cexception.h"


// random number seeds:
static long seeds[NUM_RANDOM_GENERATORS];

// a hack to initialize all random number seeds to 1:
class init_generators {
   public: init_generators();
};
init_generators::init_generators() {
  for (int i=0; i<NUM_RANDOM_GENERATORS; i++)  seeds[i] = 1;
}
static init_generators InitGenerators;

void opp_randomize()
{
       randseed( time(NULL) );
}

void genk_opp_randomize(int gen_nr)
{
       genk_randseed( gen_nr, time(NULL) );
}

long randseed()
{
       return seeds[0];
}

long genk_randseed(int gen_nr)
{
       if (gen_nr<0 || gen_nr>=NUM_RANDOM_GENERATORS)
          throw new cException("Invalid random number generator %d",gen_nr);
       return seeds[gen_nr];
}

long randseed(long seed)
{
       if (seed<=0 || seed>INTRAND_MAX)
          throw new cException("Invalid random number seed %ld",seed);

       long res = seeds[0];
       seeds[0] = seed;
       return res;
}

long genk_randseed(int gen_nr, long seed)
{
       if (gen_nr<0 || gen_nr>=NUM_RANDOM_GENERATORS)
          throw new cException("Invalid random number generator %d",gen_nr);
       if (seed<=0 || seed>INTRAND_MAX)
          throw new cException("Invalid random number seed %ld",seed);

       long res = seeds[gen_nr];
       seeds[gen_nr] = seed;
       return res;
}

/*------- long int pseudorandom number generator -------------
*    Range:          1 ... 2**31-2  (INTRAND_MAX=2**31-2)
*    Period length:  2**31-2
*    Global variable used:   long int theSeed : seed
*    Method:  x=(x * 7**5) mod (2**31-1)
*    To check:  if  x[0]=1  then  x[10000]=1,043,618,065
*    Required hardware:  exactly 32-bit integer aritmetics
*    Source:  Raj Jain: The Art of Computer Systems Performance Analysis
*                (John Wiley & Sons, 1991)   Pages 441-444, 455
*---------------------------------------------------------*/
long opp_nextrand(long& seed)
{
     const long int a=16807, q=127773, r=2836;
     seed=a*(seed%q) - r*(seed/q);
     if (seed<=0) seed+=INTRAND_MAX+1;
     return seed;
}

long intrand()
{
     return opp_nextrand(seeds[0]);
}

long genk_intrand(int gen_nr)
{
     if (gen_nr<0 || gen_nr>=NUM_RANDOM_GENERATORS)
        throw new cException("Invalid random number generator %d",gen_nr);

     return opp_nextrand(seeds[gen_nr]);
}

int testrand()
{
     long seed= 1;
     for(int i=0; i<10000; i++) opp_nextrand(seed);
     return (seed==1043618065L);
}

long intrand(long r)
{
     if (r>0)
         return intrand()%r;   // good if r<<MAX_LONG
     else if (r==0)
         throw new cException("intrand(r) called with r=0 (cannot generate 0<=x<0 integer)");
     else
         throw new cException("intrand(r) called with negative r argument");
     return 0L;
}

long genk_intrand(int gen_nr, long r)
{
     if (r>0)
         return genk_intrand(gen_nr)%r;   // good if r<<MAX_LONG
     else if (r==0)
         throw new cException("genk_intrand(g,r) called with r=0 (cannot generate 0<=x<0 integer)");
     else
         throw new cException("genk_intrand(g,r) called with negative r argument");
     return 0L;
}



