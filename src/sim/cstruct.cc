//==========================================================================
//   CSTRUCT.CC  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cStructDescriptor : meta-info about structures
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>   // sprintf
#include <stdlib.h>  // atol
#include <string.h>
#include <assert.h>
#include "cstruct.h"
#include "cenum.h"
#include "util.h"
#include "errmsg.h"
#include "ctypes.h"  // createOne()
// #include "cenum.h" --TBD


void cStructDescriptor::long2string(long l, char *buf, int bufsize)
{
    assert(bufsize>=10);
    sprintf("%ld", buf, l);
}

void cStructDescriptor::ulong2string(unsigned long l, char *buf, int bufsize)
{
    assert(bufsize>=10);
    sprintf("%lu", buf, l);
}


long cStructDescriptor::string2long(const char *s)
{
    return atol(s);
}


unsigned long cStructDescriptor::string2ulong(const char *s)
{
    return (unsigned long) atol(s);
}


void cStructDescriptor::bool2string(bool b, char *buf, int bufsize)
{
    assert(bufsize>=7);
    strcpy(buf, b ? "true" : "false");
}


bool cStructDescriptor::string2bool(const char *s)
{
    return s[0]=='t' || s[0]=='T' || s[0]=='y' || s[0]=='Y' || s[0]=='1';
}


void cStructDescriptor::double2string(double d, char *buf, int bufsize)
{
    assert(bufsize>=20);
    sprintf("%f", buf, d);
}


double cStructDescriptor::string2double(const char *s)
{
    return atof(s);
}


void cStructDescriptor::enum2string(long e, const char *enumname, char *buf, int bufsize)
{
    assert(bufsize>=30); // FIXME: very crude check

    sprintf(buf,"%ld",e);
    cEnum *enump = findEnum(enumname);
    if (!enump) {
        // this enum type is not registered
        return;
    }
    const char *s = enump->stringFor(e);
    if (!s) {
        // no string for this numeric value
        sprintf(buf+strlen(buf), " (unknown)");
        return;
    }
    sprintf(buf+strlen(buf), " (%s)",s);
}


long cStructDescriptor::string2enum(const char *s, const char *enumname)
{
    // return zero if string cannot be parsed

    // try to interpret as numeric value
    if (s[0]>=0 && s[0]<=9) {
        return atol(s);
    }

    // try to recognize string
    cEnum *enump = findEnum(enumname);
    if (!enump) {
        // this enum type is not registered
        return 0;
    }

    // FIXME: should strip possible spaces, parens etc.
    return enump->lookup(s,0);
}

//-----------------------------------------------------------

cStructDescriptor::cStructDescriptor(const cStructDescriptor& cs)
{
    opp_error("Cannot copy itself"); //FIXME better msg
}


cStructDescriptor::~cStructDescriptor()
{
}

cObject *cStructDescriptor::dup() const
{
    opp_error(eCANTDUP);
    return NULL;
}

cStructDescriptor& cStructDescriptor::operator=(const cStructDescriptor& cs)
{
    opp_error("Assignment not supported");  //FIXME better msg
    return *this;
}

cStructDescriptor *cStructDescriptor::createDescriptorFor(cObject *obj)
{
    return createDescriptorFor(obj->className(), (void *)obj);
}

cStructDescriptor *cStructDescriptor::createDescriptorFor(const char *classname, void *p)
{
    // produce name of struct descriptor class
    char sdclass[80];
    strcpy(sdclass,classname);
    strcat(sdclass,"Descriptor");

    // create and initialize a structure descriptor object
    cStructDescriptor *sd = (cStructDescriptor *) createOne(sdclass);
    sd->setStruct(p);
    return sd;
}


