/***************************************************/
/*            OMNeT++ NEDC (JAR) source            */
/*                                                 */
/*  File: jar_mem.c                                */
/*                                                 */
/*  Contents:                                      */
/*    debugging memory allocations                 */
/*                                                 */
/*  By: Andras Varga 1996                          */
/*                                                 */
/***************************************************/

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>

#ifdef __BORLANDC__
#include <alloc.h>
#endif

#include "jar_func.h"

//#define HEAPCHECK

void *jar_malloc (unsigned size)
{
#if defined(HEAPCHECK) && defined(__BORLANDC__)
   if (heapcheck()<=0)
      {fprintf(stderr,"heapcheck() failed in jar_malloc()!\n");abort();}
#endif
   return malloc(size);
}

void jar_free (void *ptr)
{
#if defined(HEAPCHECK) && defined(__BORLANDC__)
   if (heapcheck()<=0)
      {fprintf(stderr,"heapcheck() failed in jar_free()!\n");abort();}
   if (ptr==NULL) return;
   if (heapchecknode(ptr)==_USEDENTRY)
      free(ptr);
   else
      {fprintf(stderr,"heapchecknode() failed in jar_free()!\n");ptr=NULL;}
#else
   if (ptr!=NULL) free(ptr);
#endif
}
