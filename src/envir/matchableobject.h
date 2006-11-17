//==========================================================================
//  MATCHABLEOBJECT.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cClassDescriptor  : metainfo about structs and classes
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MATCHABLEOBJECT_H
#define __MATCHABLEOBJECT_H

#include "cobject.h"
#include "matchexpression.h"

class cClassDescriptor;


/**
 * Wrapper around a cObject to make it matchable with MatchExpression.
 * The default attribute is either fullName() or fullPath().
 */
class MatchableObject : public MatchExpression::Matchable
{
  public:
    enum DefaultAttribute {FULLNAME, FULLPATH, CLASSNAME};
  protected:
    DefaultAttribute attr;
    cObject *obj;
    mutable cClassDescriptor *desc;
    mutable std::string tmp;
  protected:
    static void splitIndex(char *fieldname, int& index);
    static bool findDescriptorField(cClassDescriptor *desc, cObject *obj, char *fieldname, int& fieldId, int& index);
  public:
    MatchableObject(cObject *obj, DefaultAttribute attr);
    void setObject(cObject *obj) {this->obj = obj; desc = NULL;}
    virtual const char *getDefaultAttribute() const;
    virtual const char *getAttribute(const char *name) const;
};

#endif

