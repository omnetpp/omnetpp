//=========================================================================
//  CFUTUREEVENTSET.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cFutureEventSet : future event set, implemented as heap
//
//  Author: Andras Varga, based on the code from Gabor Lencse
//          (the original is taken from G. H. Gonnet's book pp. 273-274)
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include "omnetpp/cfutureeventset.h"

NAMESPACE_BEGIN

std::string cFutureEventSet::info() const
{
    if (isEmpty())
        return std::string("empty");
    std::stringstream out;
    out << "length=" << getLength();
    return out.str();
}

cFutureEventSet& cFutureEventSet::operator=(const cFutureEventSet& other)
{
    if (this == &other)
        return *this;
    cOwnedObject::operator=(other);
    return *this;
}

NAMESPACE_END

