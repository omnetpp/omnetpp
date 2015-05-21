//==========================================================================
//  MATCHABLEFIELD.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_MATCHABLEFIELD_H
#define __OMNETPP_MATCHABLEFIELD_H

#include "common/matchexpression.h"
#include "omnetpp/cobject.h"
#include "envirdefs.h"

NAMESPACE_BEGIN

class cClassDescriptor;

/**
 * Wrapper around a cObject field to make it matchable with MatchExpression.
 * The default attribute is the field name.
 */
class ENVIR_API MatchableFieldAdapter : public MatchExpression::Matchable
{
  protected:
    cObject *object;
    mutable cClassDescriptor *classDescriptor;
    int fieldIndex;

  public:
    MatchableFieldAdapter(cObject *object = nullptr, int fieldIndex = -1);
    void setField(cObject *object, int fieldIndex);
    void setField(cObject *object, const char *fieldName);
    virtual const char *getAsString() const;
    virtual const char *getAsString(const char *attribute) const;
};

NAMESPACE_END

#endif
