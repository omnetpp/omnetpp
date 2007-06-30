//==========================================================================
//  ENUMSTR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    EnumStringIterator:   loops through numbers in a string like "1,2,5-10"
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ENUMSTR_H
#define __ENUMSTR_H

/**
 * Loops through numbers in a string like "1,2,5-10", expanding ranges.
 * This is used for implementing the -r command-line option and
 * the cmdenv-runs-to-execute= configuration option
 */
//XXX change "-" to "..", or at least accept ".." too? to make it consistent with ${} syntax
class EnumStringIterator
{
     const char *str;  // pointer to the original string (not a copy)
     int current, until;
     int err;
   public:
     EnumStringIterator(const char *s);
     int operator++(int);
     int operator()() {return err ? -1 : current;}
     int error()      {return err;}
};

#endif
