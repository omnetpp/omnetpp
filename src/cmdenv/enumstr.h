//==========================================================================
//   ENUMSTR.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    EnumStringIterator:   loops through numbers in a string like "1,2,5-10"
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ENUMSTR_H
#define __ENUMSTR_H

//==========================================================================
// EnumStringIterator: loops through numbers in a string like "1,2,5-10"
//    used to implement [Cmdenv]/runs-to-execute= ini file option

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
