//==========================================================================
//  PATMATCH.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Implementation of
//         pattern matching stuff
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <stdlib.h>
#include <string.h>

#include "patmatch.h"


enum {ANY=-2,    // ?
      ANYCL=-3,  // *
      SET=-4,    // {   (begin set)
      NEGSET=-5, // {^  (begin negated set)
      ENDSET=-6  // }   (end set)
};

bool contains_wildcards(const char *pattern)
{
    return strchr(pattern,'?') || strchr(pattern,'*') ||
           strchr(pattern,'\\') || strchr(pattern,'{');
}

bool transform_pattern(const char *from, short *to)
{
    //DEBUG:printf(" ******** patt=%s",from);
    // Prepares pattern for the match function.
    // Retval: 1=successful, 0=bad pattern (unmatched '{')
    //  handles quoted chars: replaces \x with x (where x can by any char)
    //  replaces *,?,{,{^,} with ANY,ANYCL,SET,NEGSET,ENDSET
    //  expands sets: {0-9} becomes SET,0,1,2,3,4,5,6,7,8,9,ENDSET

    short c;
    while ((c = *from++) != '\0')
    {
        switch( c )
        {
           case '?':  c=ANY; break;
           case '*':  c=ANYCL; break;
           case '\\': c = *from++; break;
           case '{':  // begin set or negated set
                      if ((c = *from++)=='^') {
                          *to++ = NEGSET; c = *from++;
                      } else {
                          *to++ = SET;
                      }
                      // transform set
                      do {
                          if (*from=='-' && from[1]!='}')
                          {
                             while (c<=from[1]) // expand set
                                *to++ = c++;
                             from += 2;
                          }
                          else if (c=='\0')
                             return 0; // error: unmatched '{'
                          else
                             *to++ = c;
                      } while ((c = *from++) != '}');
                      // close set in transformed pattern
                      c = ENDSET; break;
        }
        *to++ = c;
    }
    *to = '\0';
    return 1;
}

static const short *ismatch(const short *pat, short c)
{
    // Matches one character on the beginning of the pattern.
    // Retval: NULL:doesn't match, Other pointer:rest of pattern

    if (c=='\0')    // end of string matches nothing
        return NULL;
    switch(*pat++)
    {
        case ANY:
           return pat;
        case SET:
           while (*pat!=ENDSET)
              if (*pat++ == c)
                  {while (*pat++!=ENDSET); return pat;}
           return NULL;
        case NEGSET:
           while (*pat!=ENDSET)
              if (*pat++ == c)
                  return NULL;
           return pat+1;
        default:
           return *(pat-1)==c ? pat : (short *)NULL;
    }
}

bool stringmatch(const short *pat, const char *line)
{
    short c;
    const short *p;
    const char *lnext;
    // fixed match (no '*') as far as it goes
    while (*pat != ANYCL)
    {
        if ((c = *line++) == '\0')
           return *pat=='\0';
        if ((pat = ismatch(pat,c))==NULL)
           return 0;
    }
    // try to match rest of line to current section of the pattern
    // (until next '*' or end of pattern)
    while (*++pat != '\0')
    {
        lnext = line;
        do {
            line = lnext; lnext++; p = pat;
            while (p!=NULL && *p!=ANYCL)
            {
               if ((c = *line++)=='\0')  // end of string
                   return *p=='\0';      // good if also at end of pattern
               p = ismatch(p,c);
            }
        } while (p==NULL);
        pat = p;  // OK so far, go to next pattern section
    }
    return 1;
}

