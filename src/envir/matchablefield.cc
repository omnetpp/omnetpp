//==========================================================================
//  MATCHABLEFIELD.CC - part of
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

#include "common/commonutil.h"
#include "omnetpp/cclassdescriptor.h"
#include "matchablefield.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

MatchableFieldAdapter::MatchableFieldAdapter(cObject *object, int fieldIndex)
{
    setField(object, fieldIndex);
}

void MatchableFieldAdapter::setField(cObject *object, int fieldIndex)
{
    Assert(object);
    this->object = object;
    this->fieldIndex = fieldIndex;
    this->classDescriptor = object->getDescriptor();
}

void MatchableFieldAdapter::setField(cObject *object, const char *fieldName)
{
    Assert(object);
    this->object = object;
    this->fieldIndex = -1;
    this->classDescriptor = object->getDescriptor();
    this->fieldIndex = classDescriptor->findField(fieldName);
    Assert(fieldIndex != -1);
}

const char *MatchableFieldAdapter::getAsString() const
{
    Assert(object && classDescriptor);
    return classDescriptor->getFieldName(fieldIndex);
}

const char *MatchableFieldAdapter::getAsString(const char *attribute) const
{
    Assert(object && classDescriptor);

    if (!strcmp("name", attribute))
        return classDescriptor->getFieldName(fieldIndex);
    else if (!strcmp("type", attribute))
        return classDescriptor->getFieldTypeString(fieldIndex);
    else if (!strcmp("declaredOn", attribute))
        return classDescriptor->getFieldDeclaredOn(fieldIndex);
    else
        return nullptr;
}

}  // namespace envir
}  // namespace omnetpp

