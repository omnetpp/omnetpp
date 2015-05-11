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
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>   // sprintf
#include <cstdlib>  // atol
#include <cstring>
#include "common/opp_ctype.h"
#include "omnetpp/cclassdescriptor.h"
#include "omnetpp/carray.h"
#include "omnetpp/cenum.h"
#include "omnetpp/simutil.h"
#include "omnetpp/cobjectfactory.h"  // createOne()
#include "omnetpp/platdep/platmisc.h" // INT64_PRINTF_FORMAT

NAMESPACE_BEGIN


#define LL  INT64_PRINTF_FORMAT  /* abbreviation */

std::string cClassDescriptor::long2string(long l)
{
    char buf[32];
    sprintf(buf, "%ld", l);
    return buf;
}

long cClassDescriptor::string2long(const char *s)
{
    return strtol(s, NULL, 10);
}

std::string cClassDescriptor::ulong2string(unsigned long l)
{
    char buf[32];
    sprintf(buf, "%lu", l);
    return buf;
}

unsigned long cClassDescriptor::string2ulong(const char *s)
{
    return strtoul(s, NULL, 10);
}

std::string cClassDescriptor::int642string(int64_t l)
{
    char buf[32];
    sprintf(buf, "%" LL "d", l);
    return buf;
}

int64_t cClassDescriptor::string2int64(const char *s)
{
    return strtoll(s, NULL, 10);
}

std::string cClassDescriptor::uint642string(uint64_t l)
{
    char buf[32];
    sprintf(buf, "%" LL "u", l);
    return buf;
}

uint64_t cClassDescriptor::string2uint64(const char *s)
{
    return strtoull(s, NULL, 10);
}

std::string cClassDescriptor::bool2string(bool b)
{
    return b ? "true" : "false";
}

bool cClassDescriptor::string2bool(const char *s)
{
    return s[0]=='t' || s[0]=='T' || s[0]=='y' || s[0]=='Y' || s[0]=='1';
}


std::string cClassDescriptor::double2string(double d)
{
    char buf[32];
    sprintf(buf, "%f", d);
    return buf;
}


double cClassDescriptor::string2double(const char *s)
{
    return atof(s);
}

std::string cClassDescriptor::enum2string(int e, const char *enumname)
{
    char buf[32];
    sprintf(buf, "%d", e);

    cEnum *enump = cEnum::find(enumname);
    if (!enump)
        return buf; // this enum type is not registered

    const char *name = enump->getStringFor(e);
    if (!name)
        return strcat(buf, " (unknown)");  // no string for this numeric value

    return std::string(buf) + " (" + name + ")";
}

int cClassDescriptor::string2enum(const char *s, const char *enumname)
{
    // return zero if string cannot be parsed
    int val = 0;
    cEnum *enump = cEnum::find(enumname);

    // skip spaces
    while (opp_isspace(*s))
        s++;
    // try to interpret it as numeric value
    if (opp_isdigit(*s))
    {
        val = atoi(s);
        if (!enump->getStringFor(val))
            throw cRuntimeError("Value '%s' invalid for enum '%s'", s, enumname);
    }
    else
    {
        // try to recognize string
        if (!enump)
            throw cRuntimeError("Unknown enum '%s'", enumname);

        // TBD should strip possible spaces, parens etc.
        val = enump->resolve(s);
    }
    return val;
}

// helper for the next one
static bool listContains(const char **list, const char *s)
{
    for (const char **p = list; *p; p++)
        if (!strcmp(*p, s))
            return true;
    return false;
}

const char **cClassDescriptor::mergeLists(const char **list1, const char **list2)
{
    // args may be NULL
    const char *emptylist[] = { NULL };
    if (!list1) list1 = emptylist;
    if (!list2) list2 = emptylist;

    // count all elements
    int n = 0;
    for (const char **p = list1; *p; p++, n++);
    for (const char **p = list2; *p; p++, n++);

    // preallocate the result array
    const char **result = new const char *[n+1];

    // copy items
    const char **dest = result;
    for (const char **p = list1; *p; p++)
        *dest++ = *p;
    for (const char **p = list2; *p; p++)
        if (!listContains(list1, *p))
            *dest++ = *p;
    *dest = NULL;
    return result;
}


//-----------------------------------------------------------

cClassDescriptor::cClassDescriptor(const char *classname, const char *_baseclassname) :
cNoncopyableOwnedObject(classname, false)
{
    baseClassName = _baseclassname ? _baseclassname : "";
    baseClassDesc = NULL;
    inheritanceChainLength = 1;
    extendscObject = -1;
}

cClassDescriptor::~cClassDescriptor()
{
}

cClassDescriptor *cClassDescriptor::getBaseClassDescriptor() const
{
    if (!baseClassDesc && !baseClassName.empty())
    {
        cClassDescriptor *this_ = const_cast<cClassDescriptor*>(this);
        this_->baseClassDesc = getDescriptorFor(baseClassName.c_str());
        if (baseClassDesc)
            this_->inheritanceChainLength = 1 + baseClassDesc->getInheritanceChainLength();
    }
    return baseClassDesc;
}

bool cClassDescriptor::extendsCObject() const
{
    if (extendscObject == -1) {
        cClassDescriptor *this_ = const_cast<cClassDescriptor*>(this);
        this_->extendscObject = false;
        const cClassDescriptor *current = this;

        while (current) {
            if (!strcmp(OPP_PREFIX "cObject", current->getName())) {
                this_->extendscObject = true;
                break;
            }
            else {
                current = current->getBaseClassDescriptor();
            }
        }
    }
    return extendscObject;
}

int cClassDescriptor::getInheritanceChainLength() const
{
    getBaseClassDescriptor(); // force resolution of inheritance
    return inheritanceChainLength;
}

const char *cClassDescriptor::getFieldDeclaredOn(int field) const
{
    cClassDescriptor *base = getBaseClassDescriptor();
    if (base && field < base->getFieldCount())
        return base->getFieldDeclaredOn(field);
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

// for backward compatibility with 4.0
bool cClassDescriptor::getFieldAsString(void *object, int field, int i, char *buf, int bufsize) const
{
    std::string result = getFieldValueAsString(object, field, i);
    strncpy(buf, result.c_str(), bufsize);
    buf[bufsize-1] = 0;
    return true;
}

int cClassDescriptor::findField(const char *fieldName) const
{
    int n = getFieldCount();
    for (int i=0; i<n; i++)
        if (strcmp(fieldName, getFieldName(i))==0)
            return i;
    return -1;
}

NAMESPACE_END

