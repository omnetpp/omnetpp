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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>  // sprintf
#include <cstdlib>
#include <cstring>
#include <climits> // INT_MIN
#include "common/opp_ctype.h"
#include "common/stringutil.h"
#include "omnetpp/cclassdescriptor.h"
#include "omnetpp/carray.h"
#include "omnetpp/cenum.h"
#include "omnetpp/simutil.h"
#include "omnetpp/cobjectfactory.h"  // createOne()
#include "omnetpp/platdep/platmisc.h"  // PRId64

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {


std::string cClassDescriptor::long2string(long l)
{
    char buf[32];
    sprintf(buf, "%ld", l);
    return buf;
}

long cClassDescriptor::string2long(const char *s)
{
    return strtol(s, nullptr, 10);
}

std::string cClassDescriptor::ulong2string(unsigned long l)
{
    char buf[32];
    sprintf(buf, "%lu", l);
    return buf;
}

unsigned long cClassDescriptor::string2ulong(const char *s)
{
    return strtoul(s, nullptr, 10);
}

std::string cClassDescriptor::int642string(int64_t l)
{
    char buf[32];
    sprintf(buf, "%" PRId64, l);
    return buf;
}

int64_t cClassDescriptor::string2int64(const char *s)
{
    return strtoll(s, nullptr, 10);
}

std::string cClassDescriptor::uint642string(uint64_t l)
{
    char buf[32];
    sprintf(buf, "%" PRId64 "u", l);
    return buf;
}

uint64_t cClassDescriptor::string2uint64(const char *s)
{
    return strtoull(s, nullptr, 10);
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
    return opp_stringf("%.16g", d);
}

double cClassDescriptor::string2double(const char *s)
{
    return atof(s);
}

std::string cClassDescriptor::enum2string(int e, const char *enumName) const
{
    char buf[32];
    sprintf(buf, "%d", e);

    cEnum *enump = cEnum::find(enumName, getNamespace().c_str());
    if (!enump)
        return buf;  // this enum type is not registered

    const char *name = enump->getStringFor(e);
    if (!name)
        return strcat(buf, " (unknown)");  // no string for this numeric value

    return std::string(buf) + " (" + name + ")";
}

int cClassDescriptor::string2enum(const char *s, const char *enumName) const
{
    // return zero if string cannot be parsed
    int value = 0;
    cEnum *enump = cEnum::find(enumName, getNamespace().c_str());

    // skip leading spaces
    while (opp_isspace(*s))
        s++;

    // try to interpret it as numeric value
    if (opp_isdigit(*s)) {
        value = atoi(s);
        if (enump && !enump->getStringFor(value))
            throw cRuntimeError("%s is an invalid value for enum '%s'", s, enumName);
    }
    else {
        // try to recognize string
        if (!enump)
            throw cRuntimeError("Unknown enum '%s'", enumName);

        // try exact match
        const int MISSING = INT_MIN;
        value = enump->lookup(s, MISSING);

        // if not found, try unique case insensitive substring match
        if (value == MISSING) {
            std::map<std::string,int> members = enump->getNameValueMap();
            for (auto & member : members) {
                if (opp_strnistr(member.first.c_str(), s, 0, false) != nullptr) {
                    if (value == MISSING)
                        value = member.second;
                    else
                        throw cRuntimeError("Name '%s' is ambiguous in enum '%s' (substring search)", s, enumName);
                }
            }
            if (value == MISSING)
                throw cRuntimeError("Name '%s' not found in enum '%s' (substring search)", s, enumName);
        }
    }
    return value;
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
    // args may be nullptr
    const char *emptyList[] = { nullptr };
    if (!list1) list1 = emptyList;
    if (!list2) list2 = emptyList;

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

    *dest = nullptr;
    return result;
}

//-----------------------------------------------------------

cClassDescriptor::cClassDescriptor(const char *className, const char *_baseClassName)
    : cNoncopyableOwnedObject(className, false),
      baseClassName(_baseClassName ? _baseClassName : "")
{
}

cClassDescriptor *cClassDescriptor::getBaseClassDescriptor() const
{
    if (!baseClassDesc && !baseClassName.empty()) {
        cClassDescriptor *this_ = const_cast<cClassDescriptor *>(this);
        this_->baseClassDesc = getDescriptorFor(baseClassName.c_str());
        if (baseClassDesc)
            this_->inheritanceChainLength = 1 + baseClassDesc->getInheritanceChainLength();
    }
    return baseClassDesc;
}

std::string cClassDescriptor::getNamespace() const
{
    std::string qname = getFullName();
    int nameLen = strlen(getName());
    if ((int)qname.length() >= nameLen + 2)  // +2 is for "::"
        return qname.substr(0, qname.length() - nameLen - 2);
    else {
        ASSERT(nameLen == (int)qname.length()); // no namespace
        return "";
    }
}

bool cClassDescriptor::extendsCObject() const
{
    if (extendscObject == -1) {
        cClassDescriptor *this_ = const_cast<cClassDescriptor *>(this);
        this_->extendscObject = false;
        const cClassDescriptor *current = this;

        while (current) {
            if (!strcmp("omnetpp::cObject", current->getName())) {
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
    getBaseClassDescriptor();  // force resolution of inheritance
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

cClassDescriptor *cClassDescriptor::getDescriptorFor(const cObject *object)
{
    // find descriptor by class name
    cClassDescriptor *desc = cClassDescriptor::getDescriptorFor(object->getClassName());
    if (desc)
        return desc;

    // bad luck: no descriptor for exactly this class. Try to find one for some base class.
    //XXX we could even cache the result in a {classname->descriptor} hashtable.
    cClassDescriptor *bestDesc = nullptr;
    int bestInheritanceChainLength = -1;
    cRegistrationList *array = classDescriptors.getInstance();
    for (int i = 0; i < array->size(); i++) {
        cClassDescriptor *desc = dynamic_cast<cClassDescriptor *>(array->get(i));
        if (!desc || !desc->doesSupport(const_cast<cObject*>(object)))
            continue;  // skip holes
        int inheritanceChainLength = desc->getInheritanceChainLength();
        if (inheritanceChainLength > bestInheritanceChainLength) {
            bestDesc = desc;
            bestInheritanceChainLength = inheritanceChainLength;
        }
    }
    return bestDesc;
}

int cClassDescriptor::findField(const char *fieldName) const
{
    int n = getFieldCount();
    for (int i = 0; i < n; i++)
        if (strcmp(fieldName, getFieldName(i)) == 0)
            return i;
    return -1;
}

}  // namespace omnetpp

