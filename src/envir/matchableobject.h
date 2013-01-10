//==========================================================================
//  MATCHABLEOBJECT.H - part of
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

#ifndef __MATCHABLEOBJECT_H
#define __MATCHABLEOBJECT_H

#include "envirdefs.h"
#include "cobject.h"
#include "matchexpression.h"

NAMESPACE_BEGIN

class cClassDescriptor;


/**
 * Wrapper around a cObject to make it matchable with MatchExpression.
 * The default match string is either getFullName() or getFullPath().
 */
class ENVIR_API MatchableObjectAdapter : public MatchExpression::Matchable
{
  public:
    enum DefaultAttribute {FULLNAME, FULLPATH, CLASSNAME};
  protected:
    DefaultAttribute attr;
    cObject *obj;
    mutable cClassDescriptor *desc;
    mutable std::string tmp;
  protected:
    static void splitIndex(char *indexedName, int& index);
    static bool findDescriptorField(cClassDescriptor *desc, cObject *obj, const char *attribute, int& fieldId, int& index);
  public:
    MatchableObjectAdapter(DefaultAttribute attr=FULLPATH, cObject *obj=NULL);
    void setObject(cObject *obj);
    void setDefaultAttribute(DefaultAttribute attr) {this->attr = attr;}
    virtual const char *getAsString() const;
    virtual const char *getAsString(const char *attribute) const;
};

NAMESPACE_END


#endif

