//==========================================================================
//  ENUMSTR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_ENUMSTR_H
#define __OMNETPP_COMMON_ENUMSTR_H

#include "commondefs.h"

namespace omnetpp {
namespace common {

/**
 * Loops through numbers in a string like "1,2,5..10", expanding ranges.
 * Both ".." and "-" are accepted for ranges.
 *
 * This is used for implementing the -r command-line option and
 * the cmdenv-runs-to-execute= configuration option.
 */
class COMMON_API EnumStringIterator
{
     const char *str = nullptr;  // pointer to the original string (not a copy)
     int current = -1, until = -1;
     bool err = false;
   public:
     EnumStringIterator(const char *s);
     int operator++(int);
     int operator()() {return err ? -1 : current;}
     bool hasError() const {return err;}
};

}  // namespace common
}  // namespace omnetpp


#endif
