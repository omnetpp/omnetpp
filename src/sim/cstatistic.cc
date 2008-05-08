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
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <string>

#include "cstatistic.h"
#include "random.h"
#include "distrib.h"
#include "globals.h"
#include "cdetect.h"  //NL
#include "csimplemodule.h"
#include "cexception.h"
#include "cenvir.h"
#include "stringutil.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

using std::ostream;


cStatistic::cStatistic(const cStatistic& r) : cOwnedObject()
{
    setName(r.name());
    td = NULL;
    ra = NULL;
    operator=(r);
}

cStatistic::cStatistic(const char *name) : cOwnedObject(name)
{
    td = NULL;
    ra = NULL;
    genk = 0;
}

cStatistic::~cStatistic()
{
    dropAndDelete(td);
    dropAndDelete(ra);
}

void cStatistic::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::netPack(buffer);

    buffer->pack(genk);

    if (buffer->packFlag(td!=NULL))
        buffer->packObject(td);
    if (buffer->packFlag(ra!=NULL))
        buffer->packObject(ra);
#endif
}

void cStatistic::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cOwnedObject::netUnpack(buffer);

    buffer->unpack(genk);

    if (buffer->checkFlag())
        take(td = (cTransientDetection *) buffer->unpackObject());
    if (buffer->checkFlag())
        take(ra = (cAccuracyDetection *) buffer->unpackObject());
#endif
}

cStatistic& cStatistic::operator=(const cStatistic& res)
{
    if (this==&res) return *this;

    cOwnedObject::operator=(res);
    genk = res.genk;
    dropAndDelete(td);
    dropAndDelete(ra);
    td = res.td;
    if (td)
        take(td = (cTransientDetection *)td->dup());
    ra = res.ra;
    if (ra)
        take(ra = (cAccuracyDetection *)ra->dup());
    return *this;
}

void cStatistic::addTransientDetection(cTransientDetection *obj)  //NL
{
    if (td)
        throw cRuntimeError(this,"addTransientDetection(): object already has a transient detection algorithm");
    td = obj;                       // create pointer to td object
    td->setHostObject(this);        // and create one back
    take(td);
}

void cStatistic::addAccuracyDetection(cAccuracyDetection *obj)  //NL
{
    if (ra)
        throw cRuntimeError(this,"addAccuracyDetection(): object already has an accuracy detection algorithm");
    ra = obj;                       // create pointer to ra object
    ra->setHostObject(this);        // and create one back
    take(ra);
}

void cStatistic::collect2(double, double)
{
    throw cRuntimeError(this, "collect2() not implemented");
}

void cStatistic::recordAs(const char *scalarname, const char *unit)
{
    cSimpleModule *mod = dynamic_cast<cSimpleModule *>(simulation.contextModule());
    if (!mod)
        throw cRuntimeError(this,"record() may only be invoked from within a simple module");
    if (!scalarname)
        scalarname = fullName();

    opp_string_map attributes;
    if (unit)
        attributes["unit"] = unit;
    getAttributesToRecord(attributes);
    ev.recordStatistic(mod, scalarname, this, &attributes);
}

void cStatistic::freadvarsf(FILE *f, const char *fmt, ...)
{
    char line[101];

    // read line and chop CR/LF
    fgets(line,101,f);
    char *end = line+strlen(line)-1;
    while (end>=line && (*end=='\n' || *end=='\r')) *end-- = '\0';

    // match '#=' tags
    const char *fmt_comment = strstr(fmt,"#=");
    const char *line_comment = strstr(line,"#=");
    if (fmt_comment && line_comment && strcmp(fmt_comment,line_comment)!=0)
        throw cRuntimeError(this, "bad file format in loadFromFile(): expected `%s' and got `%s'",fmt,line);

    // actual read
    va_list args;
    va_start(args,fmt);
    opp_vsscanf(line,fmt,args);
}


