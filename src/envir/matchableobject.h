//==========================================================================
//  MATCHABLEOBJECT.H - part of
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

#ifndef __OMNETPP_ENVIR_MATCHABLEOBJECT_H
#define __OMNETPP_ENVIR_MATCHABLEOBJECT_H

#include "common/matchexpression.h"
#include "omnetpp/cobject.h"
#include "envirdefs.h"

namespace omnetpp {

class cClassDescriptor;

namespace envir {

/**
 * Wrapper around a cObject to make it matchable with MatchExpression.
 * The default match string is either getFullName() or getFullPath().
 */
class ENVIR_API MatchableObjectAdapter : public omnetpp::common::MatchExpression::Matchable
{
  public:
    enum DefaultAttribute {FULLNAME, FULLPATH, CLASSNAME};
  protected:
    DefaultAttribute attr;
    cObject *obj;
    mutable cClassDescriptor *desc = nullptr;
    mutable std::string tmp;
  protected:
    static void splitIndex(char *indexedName, int& index);
    static bool findDescriptorField(cClassDescriptor *desc, const char *attribute, int& fieldId, int& index);
  public:
    MatchableObjectAdapter(DefaultAttribute attr=FULLPATH, cObject *obj=nullptr);
    void setObject(cObject *obj);
    void setDefaultAttribute(DefaultAttribute attr) {this->attr = attr;}
    virtual const char *getAsString() const override;
    virtual const char *getAsString(const char *attribute) const override;
};

}  // namespace envir
}  // namespace omnetpp


#endif

