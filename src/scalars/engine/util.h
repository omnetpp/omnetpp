//=========================================================================
//  UTIL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _ENGINE_UTILS_H_
#define _ENGINE_UTILS_H_


/**
 * Dictionary-compare two strings, the main difference from stricmp()
 * being that integers embedded in the strings are compared in
 * numerical order.  
 */
int strdictcmp(const char *s1, const char *s2);

#endif


