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


inline void writeStr(std::ostream& out, const std::string& s)
{
    if (opp_needsquotes(s.c_str()))
        out << opp_quotestr(s.c_str());
    else
        out << "\"" << s << "\"";
}

void JsonString::printOn(std::ostream& out)
{
    writeStr(out, value);
}

void JsonConstantString::printOn(std::ostream& out)
{
    if (!value)
        out << "\"\"";
    else if (opp_needsquotes(value))
        out << opp_quotestr(value);
    else
        out << "\"" << value << "\"";
}

void JsonArray::printOn(std::ostream& out)
{
    out << "[ ";
    size_t n = size();
    for (int i = 0; i < n; i++)
    {
        if (i != 0)
            out << ", ";
        (*this)[i]->printOn(out);
    }
    out << " ]";
}

void JsonMap::printOn(std::ostream& out)
{
    out << "{ ";
    for (JsonMap::iterator it = begin(); it != end(); ++it)
    {
        if (it != begin())
            out << ", ";
        const char *key = it->first;
        if (opp_needsquotes(key))
            out << opp_quotestr(key);
        else
            out << "\"" << key << "\"";
        out << ": ";
        it->second->printOn(out);
    }
    out << " }";
}

void JsonMap2::printOn(std::ostream& out)
{
    out << "{ ";
    for (JsonMap2::iterator it = begin(); it != end(); ++it)
    {
        if (it != begin())
            out << ", ";
        const char *key = it->first.c_str();
        if (opp_needsquotes(key))
            out << opp_quotestr(key);
        else
            out << "\"" << key << "\"";
        out << ": ";
        it->second->printOn(out);
    }
    out << " }";
}

