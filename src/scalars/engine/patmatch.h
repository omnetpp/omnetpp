//==========================================================================
//  PATMATCH.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//         pattern matching stuff
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


bool contains_wildcards(const char *pattern);
bool transform_pattern(const char *from, short *topattern);
bool stringmatch(const short *pattern, const char *line);

