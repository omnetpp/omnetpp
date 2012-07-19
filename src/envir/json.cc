//==========================================================================
//  JSON.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "json.h"


void JsonArray::printOn(std::ostream& out) {
    out << "[ ";
    size_t n = size();
    for (int i = 0; i < n; i++) {
        if (i > 0)
            out << ", ";
        (*this)[i]->printOn(out);
    }
    out << " ]";
}

void JsonMap::printOn(std::ostream& out)
{
    out << "{ ";
    for (JsonMap::iterator it = begin(); it != end(); ++it) {
        if (it != begin())
            out << ", ";
        writeStr(out, it->first);
        out << ": ";
        it->second->printOn(out);
    }
    out << " }";
}


