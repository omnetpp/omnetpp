//==========================================================================
//  MATCHABLEFIELD.H - part of
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

#ifndef __OMNETPP_ENVIR_MATCHABLEFIELD_H
#define __OMNETPP_ENVIR_MATCHABLEFIELD_H

#include "common/matchexpression.h"
#include "omnetpp/cobject.h"
#include "envirdefs.h"

namespace omnetpp {

class cClassDescriptor;

namespace envir {

/**
 * Wrapper around a cObject field to make it matchable with MatchExpression.
 * The default attribute is the field name.
 */
class ENVIR_API MatchableFieldAdapter : public omnetpp::common::MatchExpression::Matchable
{
  protected:
    cObject *object;
    mutable cClassDescriptor *classDescriptor;
    int fieldIndex;

  public:
    MatchableFieldAdapter(cObject *object = nullptr, int fieldIndex = -1);
    void setField(cObject *object, int fieldIndex);
    void setField(cObject *object, const char *fieldName);
    virtual const char *getAsString() const override;
    virtual const char *getAsString(const char *attribute) const override;
};

}  // namespace envir
}  // namespace omnetpp

#endif
