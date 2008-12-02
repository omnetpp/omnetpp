//==========================================================================
//  CCLASSDESCRIPTOR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cClassDescriptor : meta-info about structures
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>   // sprintf
#include <stdlib.h>  // atol
#include <string.h>
#include "cclassdescriptor.h"
#include "carray.h"
#include "cenum.h"
#include "simutil.h"
#include "cclassfactory.h"  // createOne()

USING_NAMESPACE


#define LL  INT64_PRINTF_FORMAT  /* abbreviation */

void cClassDescriptor::long2string(long l, char *buf, int bufsize)
{
    ASSERT(bufsize>=32);
    sprintf(buf, "%ld", l);
}

long cClassDescriptor::string2long(const char *s)
{
    return strtol(s, NULL, 10);
}

void cClassDescriptor::ulong2string(unsigned long l, char *buf, int bufsize)
{
    ASSERT(bufsize>=32);
    sprintf(buf, "%lu", l);
}

unsigned long cClassDescriptor::string2ulong(const char *s)
{
    return strtoul(s, NULL, 10);
}

void cClassDescriptor::int642string(int64 l, char *buf, int bufsize)
{
    ASSERT(bufsize>=32);
    sprintf(buf, "%"LL"d", l);
}

int64 cClassDescriptor::string2int64(const char *s)
{
    return strtoll(s, NULL, 10);
}

void cClassDescriptor::uint642string(uint64 l, char *buf, int bufsize)
{
    ASSERT(bufsize>=32);
    sprintf(buf, "%"LL"u", l);
}

uint64 cClassDescriptor::string2uint64(const char *s)
{
    return strtoull(s, NULL, 10);
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
    cEnum *enump = cEnum::find(enumname);
    if (!enump) {
        // this enum type is not registered
        return;
    }
    const char *s = enump->getStringFor(e);
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

    // try to interpret it as numeric value
    if (opp_isdigit(*s))
        return atol(s);

    // try to recognize string
    cEnum *enump = cEnum::find(enumname);
    if (!enump)
        return 0; // this enum type is not registered

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
    str = s ? s : "";
}

void cClassDescriptor::oppstring2string(const char *s, char *buf, int buflen)
{
    strncpy(buf, s ? s : "", buflen);
    buf[buflen-1] = '\0';
}

void cClassDescriptor::string2oppstring(const char *s, std::string& str)
{
    str = s ? s : "";
}


//-----------------------------------------------------------

cClassDescriptor::cClassDescriptor(const char *classname, const char *_baseclassname) :
cNoncopyableOwnedObject(classname, false)
{
    baseclassname = _baseclassname ? _baseclassname : "";
    baseclassdesc = NULL;
    inheritancechainlength = 1;
    extendscobject = -1;
}

cClassDescriptor::~cClassDescriptor()
{
}

cClassDescriptor *cClassDescriptor::getBaseClassDescriptor() const
{
    if (!baseclassdesc && !baseclassname.empty())
    {
        cClassDescriptor *this_ = const_cast<cClassDescriptor*>(this);
        this_->baseclassdesc = getDescriptorFor(baseclassname.c_str());
        if (baseclassdesc)
            this_->inheritancechainlength = 1 + baseclassdesc->getInheritanceChainLength();
    }
    return baseclassdesc;
}

bool cClassDescriptor::extendsCObject() const
{
    if (extendscobject == -1) {
        cClassDescriptor *this_ = const_cast<cClassDescriptor*>(this);
        this_->extendscobject = false;
        const cClassDescriptor *current = this;

        while (current) {
            if (!strcmp("cObject", current->getName())) {
                this_->extendscobject = true;
                break;
            }
            else {
                current = current->getBaseClassDescriptor();
            }
        }
    }
    return extendscobject;
}

int cClassDescriptor::getInheritanceChainLength() const
{
    getBaseClassDescriptor(); // force resolution of inheritance
    return inheritancechainlength;
}

const char *cClassDescriptor::getFieldDeclaredOn(void *object, int field) const
{
    cClassDescriptor *base = getBaseClassDescriptor();
    if (base && field < base->getFieldCount(object))
        return base->getFieldDeclaredOn(object, field);
    return getName();
}

cClassDescriptor *cClassDescriptor::getDescriptorFor(const char *classname)
{
    return dynamic_cast<cClassDescriptor *>(classDescriptors.getInstance()->lookup(classname));
}

cClassDescriptor *cClassDescriptor::getDescriptorFor(cObject *object)
{
    // find descriptor by class name
    cClassDescriptor *desc = cClassDescriptor::getDescriptorFor(object->getClassName());
    if (desc)
        return desc;

    // bad luck: no descriptor for exactly this class. Try to find one for some base class.
    //XXX we could even cache the result in a {classname->descriptor} hashtable.
    cClassDescriptor *bestDesc = NULL;
    int bestInheritanceChainLength = -1;
    cRegistrationList *array = classDescriptors.getInstance();
    for (int i=0; i<array->size(); i++)
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

