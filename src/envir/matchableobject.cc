//==========================================================================
//  MATCHABLEOBJECT.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cclassdescriptor.h"
#include "matchableobject.h"

namespace omnetpp {
namespace envir {

MatchableObjectAdapter::MatchableObjectAdapter(DefaultAttribute attr, cObject *obj) :
    attr(attr), obj(obj)
{
}

void MatchableObjectAdapter::setObject(cObject *obj)
{
    this->obj = obj;
    desc = nullptr;
}

const char *MatchableObjectAdapter::getAsString() const
{
    switch (attr) {
        case FULLPATH:  tmp = obj->getFullPath(); return tmp.c_str();
        case FULLNAME:  return obj->getFullName();
        case CLASSNAME: return obj->getClassName();
        default: throw opp_runtime_error("Unknown setting for default attribute");
    }
}

void MatchableObjectAdapter::splitIndex(char *indexedName, int& index)
{
    index = 0;
    char *startbracket = strchr(indexedName, '[');
    if (startbracket) {
        char *lastcharp = indexedName + strlen(indexedName) - 1;
        if (*lastcharp != ']')
            throw opp_runtime_error("Unmatched '['");
        *startbracket = '\0';
        char *end;
        index = strtol(startbracket+1, &end, 10);
        if (end != lastcharp)
            throw opp_runtime_error("Brackets [] must contain numeric index");
    }
}

bool MatchableObjectAdapter::findDescriptorField(cClassDescriptor *desc, const char *attribute, int& fieldId, int& index)
{
    // attribute may be in the form "fieldName[index]"; split the two
    char *fieldNameBuf = new char[strlen(attribute)+1];
    strcpy(fieldNameBuf, attribute);
    splitIndex(fieldNameBuf, index);

    // find field by name
    fieldId = desc->findField(fieldNameBuf);
    delete[] fieldNameBuf;
    return fieldId != -1;
}

const char *MatchableObjectAdapter::getAsString(const char *attribute) const
{
    if (!desc) {
        desc = obj->getDescriptor();
        if (!desc)
            return nullptr;
    }

/*FIXME TBD
    // start tokenizing the path
    cStringTokenizer tokenizer(attribute, ".");
    const char *token;
    void *currentObj = obj;
    cClassDescriptor *currentDesc = desc;
    int currentFieldId = id
    while ((token = tokenizer.nextToken())!=nullptr)
    {
        bool found = findDescriptorField(d, token, fid, index);
        if (!found) return nullptr;
    }
*/

    int fieldId;
    int index;
    bool found = findDescriptorField(desc, attribute, fieldId, index);
    if (!found)
        return nullptr;

    tmp = desc->getFieldValueAsString(toAnyPtr(obj), fieldId, index);
    return tmp.c_str();
}

}  // namespace envir
}  // namespace omnetpp

