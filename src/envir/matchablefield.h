//==========================================================================
//  MATCHABLEFIELD.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MATCHABLEFIELD_H
#define __MATCHABLEFIELD_H

#include "envirdefs.h"
#include "cobject.h"
#include "matchexpression.h"

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
    MatchableFieldAdapter(cObject *object = NULL, int fieldIndex = -1);
    void setField(cObject *object, int fieldIndex);
    void setField(cObject *object, const char *fieldName);
    virtual const char *getAsString() const;
    virtual const char *getAsString(const char *attribute) const;
};

NAMESPACE_END


#endif
