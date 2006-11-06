//==========================================================================
//  CSTRUCT.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cClassDescriptor : meta-info about structures
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>   // sprintf
#include <stdlib.h>  // atol
#include <string.h>
#include "cclassdescriptor.h"
#include "carray.h"
#include "cenum.h"
#include "globals.h"
#include "util.h"


void cClassDescriptor::long2string(long l, char *buf, int bufsize)
{
    ASSERT(bufsize>=10);
    sprintf(buf, "%ld", l);
}

void cClassDescriptor::ulong2string(unsigned long l, char *buf, int bufsize)
{
    ASSERT(bufsize>=10);
    sprintf(buf, "%lu", l);
}


long cClassDescriptor::string2long(const char *s)
{
    return atol(s);
}


unsigned long cClassDescriptor::string2ulong(const char *s)
{
    return (unsigned long) atol(s);
}


void cClassDescriptor::bool2string(bool b, char *buf, int bufsize)
{
    ASSERT(bufsize>=7);
    strcpy(buf, b ? "true" : "false");
}


bool cClassDescriptor::string2bool(const char *s)
{
    return s[0]=='t' || s[0]=='T' || s[0]=='y' || s[0]=='Y' || s[0]=='1';
}


void cClassDescriptor::double2string(double d, char *buf, int bufsize)
{
    ASSERT(bufsize>=20);
    sprintf(buf, "%f", d);
}


double cClassDescriptor::string2double(const char *s)
{
    return atof(s);
}


void cClassDescriptor::enum2string(long e, const char *enumname, char *buf, int bufsize)
{
    ASSERT(bufsize>=30); // FIXME: very crude check

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


long cClassDescriptor::string2enum(const char *s, const char *enumname)
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

    // TBD should strip possible spaces, parens etc.
    return enump->lookup(s,0);
}

void cClassDescriptor::oppstring2string(const opp_string& str, char *buf, int buflen)
{
    strncpy(buf, str.c_str(), buflen);
    buf[buflen-1] = '\0';
}

void cClassDescriptor::oppstring2string(const std::string& str, char *buf, int buflen)
{
    strncpy(buf, str.c_str(), buflen);
    buf[buflen-1] = '\0';
}

void cClassDescriptor::string2oppstring(const char *s, opp_string& str)
{
    str = s;
}

void cClassDescriptor::oppstring2string(const char *s, char *buf, int buflen)
{
    strncpy(buf, s, buflen);
    buf[buflen-1] = '\0';
}

void cClassDescriptor::string2oppstring(const char *s, std::string& str)
{
    str = s;
}


//-----------------------------------------------------------

cClassDescriptor::cClassDescriptor(const char *classname, const char *_baseclassname) :
cObject(classname)
{
    baseclassname = _baseclassname ? _baseclassname : "";
    baseclassdesc = NULL;
    inheritancechainlength = 1;
}

cClassDescriptor::~cClassDescriptor()
{
}

cClassDescriptor *cClassDescriptor::getBaseClassDescriptor()
{
    if (!baseclassdesc && !baseclassname.empty())
    {
        baseclassdesc = getDescriptorFor(baseclassname.c_str());
        if (baseclassdesc)
            inheritancechainlength = 1 + baseclassdesc->getInheritanceChainLength();
    }
    return baseclassdesc;
}

int cClassDescriptor::getInheritanceChainLength()
{
    getBaseClassDescriptor(); // force resolution of inheritance
    return inheritancechainlength;
}

const char *cClassDescriptor::getFieldDeclaredOn(void *object, int field)
{
    cClassDescriptor *base = getBaseClassDescriptor();
    if (base && field < base->getFieldCount(object))
        return base->getFieldDeclaredOn(object, field);
    return name();
}

cClassDescriptor *cClassDescriptor::getDescriptorFor(const char *classname)
{
    return dynamic_cast<cClassDescriptor *>(classDescriptors.instance()->get(classname));
}

cClassDescriptor *cClassDescriptor::getDescriptorFor(cPolymorphic *object)
{
    // find descriptor by class name
    cClassDescriptor *desc = cClassDescriptor::getDescriptorFor(object->className());
    if (desc)
        return desc;

    // bad luck: no descriptor for exactly this class. Try to find one for some base class.
    //XXX we could even cache the result in a {classname->descriptor} hashtable.
    cClassDescriptor *bestDesc = NULL;
    int bestInheritanceChainLength = -1;
    cArray *array = classDescriptors.instance();
    for (int i=0; i<array->items(); i++)
    {
        cClassDescriptor *desc = dynamic_cast<cClassDescriptor *>(array->get(i));
        if (!desc || !desc->doesSupport(object))
            continue; // skip holes
        int inheritanceChainLength = desc->getInheritanceChainLength();
        if (inheritanceChainLength > bestInheritanceChainLength)
        {
            bestDesc = desc;
            bestInheritanceChainLength = inheritanceChainLength;
        }
    }
    return bestDesc;
}

