//========================================================================
//
//  mt19937.h - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
// Header file to export functions from mt19937.c
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MT19937_H
#define __MT19937_H

/* N from mt19937.c */
#define MT19937_N  624

/* should point to an array of size MT19937_N */
extern unsigned long *current_mt;

/* should point to an int */
extern int *current_mti_ptr;

/* initializes mt[N] with a seed */
void mt19937_init_genrand(unsigned long s);

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void mt19937_init_by_array(unsigned long init_key[], int key_length);

/* generates a random number on [0,0xffffffff]-interval */
unsigned long mt19937_genrand_int32(void);

/* generates a random number on [0,0x7fffffff]-interval */
long mt19937_genrand_int31(void);

/* generates a random number on [0,1]-real-interval */
double mt19937_genrand_real1(void);

/* generates a random number on [0,1)-real-interval */
double mt19937_genrand_real2(void);

/* generates a random number on (0,1)-real-interval */
double mt19937_genrand_real3(void);

/* generates a random number on [0,1) with 53-bit resolution*/
double mt19937_genrand_res53(void);

#endif

