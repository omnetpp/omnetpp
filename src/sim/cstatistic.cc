//=========================================================================
//  CSTATISTIC.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//        Based on the MISS simulator's result collection
//
//   Member functions of
//     cStatistic: base class of different statistic collecting classes
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>

#include "common/stringutil.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/distrib.h"
#include "omnetpp/globals.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

using namespace omnetpp::common;

namespace omnetpp {

using std::ostream;

cStatistic::cStatistic(const cStatistic& r) : cRandom(r)
{
    copy(r);
}

cStatistic::cStatistic(const char *name) : cRandom(name)
{
}

void cStatistic::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cRandom::parsimPack(buffer);
#endif
}

void cStatistic::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cRandom::parsimUnpack(buffer);
#endif
}

void cStatistic::copy(const cStatistic& res)
{
}

cStatistic& cStatistic::operator=(const cStatistic& res)
{
    cOwnedObject::operator=(res);
    copy(res);
    return *this;
}

void cStatistic::collectWeighted(double, double)
{
    throw cRuntimeError(this, "collectWeighted() not implemented");
}

void cStatistic::recordAs(const char *scalarname, const char *unit)
{
    cSimpleModule *mod = dynamic_cast<cSimpleModule *>(getSimulation()->getContextModule());
    if (!mod)
        throw cRuntimeError(this, "record() may only be invoked from within a simple module");
    if (!scalarname)
        scalarname = getFullName();

    opp_string_map attributes;
    if (unit)
        attributes["unit"] = unit;
    getAttributesToRecord(attributes);
    getEnvir()->recordStatistic(mod, scalarname, this, &attributes);
}

void cStatistic::freadvarsf(FILE *f, const char *fmt, ...)
{
    char line[101];

    // read a non-empty, non-comment line
    do {
        fgets(line, 101, f);
        // chop CR/LF
        char *lastp = line + strlen(line) - 1;
        while (lastp >= line && (*lastp == '\n' || *lastp == '\r'))
            *lastp-- = '\0';
    } while (line[0] == '\0' || (line[0] == '#' && line[1] != '='));

    // match '#=' tags
    const char *fmtComment = strstr(fmt, "#=");
    const char *lineComment = strstr(line, "#=");
    if (opp_strcmp(fmtComment, lineComment) != 0)
        throw cRuntimeError(this, "Bad file format in loadFromFile(): Expected '%s' and got '%s'", fmt, line);

    // actual read
    va_list va;
    va_start(va, fmt);
    opp_vsscanf(line, fmt, va);
    va_end(va);
}

}  // namespace omnetpp

