//=========================================================================
//  STRINGUTIL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _STRINGUTIL_H_
#define _STRINGUTIL_H_

#include "util.h"

/**
 * Surround the given string with "quotes", also escape with backslash
 * where needed. Returns a new string allocated via new char[].
 *
 * The returned pointer has to be deallocated by the caller.
 */
char *opp_quotestr(const char *txt);

/**
 * Reverse of opp_quotestr(): remove quotes and resolve backslashed escapes.
 *
 * The returned pointer has to be deallocated by the caller.
 */
char *opp_parsequotedstr(const char *txt, const char *&endp);

/**
 * Dictionary-compare two strings, the main difference from stricmp()
 * being that integers embedded in the strings are compared in
 * numerical order.
 */
int strdictcmp(const char *s1, const char *s2);


#endif


