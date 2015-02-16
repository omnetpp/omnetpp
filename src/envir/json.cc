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

NAMESPACE_BEGIN

inline void writeStr(std::ostream& out, const char *s)
{
    if (opp_needsquotes(s))
        out << opp_quotestr(s);
    else
        out << "\"" << s << "\"";
}

void JsonString::printOn(std::ostream& out)
{
    writeStr(out, value.c_str());
}

void JsonConstantString::printOn(std::ostream& out)
{
    if (!value)
        out << "\"\"";
    else
        writeStr(out, value);
}

void JsonArray::printOn(std::ostream& out)
{
    out << "[ ";
    size_t n = size();
    for (unsigned int i = 0; i < n; i++)
    {
        if (i != 0)
            out << ", ";
        (*this)[i]->printOn(out);
    }
    out << " ]";
}

void JsonObject::printOn(std::ostream& out)
{
    out << "{ ";
    for (JsonObject::iterator it = begin(); it != end(); ++it)
    {
        if (it != begin())
            out << ", ";
        writeStr(out, it->first);
        out << ": ";
        it->second->printOn(out);
    }
    out << " }";
}

void JsonObject2::printOn(std::ostream& out)
{
    out << "{ ";
    for (JsonObject2::iterator it = begin(); it != end(); ++it)
    {
        if (it != begin())
            out << ", ";
        writeStr(out, it->first.c_str());
        out << ": ";
        it->second->printOn(out);
    }
    out << " }";
}

NAMESPACE_END
