//==========================================================================
//  CREGISTRATIONLIST.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  cRegistrationList class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CREGISTRATIONLIST_H
#define __CREGISTRATIONLIST_H

#include <vector>
#include <map>
#include "simkerneldefs.h"
#include "cnamedobject.h"

NAMESPACE_BEGIN

class cOwnedObject;

/**
 * Stores objects with a qualified name. The name() method of objects
 * should return the unqualified name (without namespace or package name),
 * and the fullName() method the qualified name (with namespace or package).
 */
class SIM_API cRegistrationList : public cNamedObject, noncopyable
{
  private:
    typedef std::map<std::string, cOwnedObject*> StringObjectMap;
    std::vector<cOwnedObject *> vec;  // for fast iteration
    StringObjectMap nameMap;   // for lookup by name()
    StringObjectMap fullnameMap;  // for lookup by fullName()

  public:
    cRegistrationList(const char *name) : cNamedObject(name, false) {}
    virtual ~cRegistrationList();

    /** @name cObject methods */
    //@{
    virtual std::string info() const;
    virtual void forEachChild(cVisitor *v);
    //@}

    /**
     * Adds an object to the container.
     */
    virtual void add(cOwnedObject *obj);

    /**
     * Returns the number of elements.
     */
    virtual int size() const {return vec.size();}

    /**
     * Returns the ith element, or NULL.
     */
    virtual cOwnedObject *get(int i) const;

    /**
     * Returns (one of) the object(s) with the given name (not fullName!).
     * Returns NULL if not found.
     */
    virtual cOwnedObject *get(const char *name) const;

    /**
     * Returns the object with exactly the given qualified name (fullName()).
     * Returns NULL if not found.
     */
    virtual cOwnedObject *lookup(const char *qualifiedName) const;

    /**
     * Sorts the elements by qualified name (fullName()). This affects
     * the order get() will return the elements.
     */
    virtual void sort();

};

/**
 * Singleton class, used for registration lists.
 * Instances are supposed to be global variables.
 *
 * @ingroup Internals
 */
class SIM_API cGlobalRegistrationList
{
  private:
    cRegistrationList *inst;
    const char *tmpname;
  public:
    cGlobalRegistrationList();
    cGlobalRegistrationList(const char *name);
    ~cGlobalRegistrationList();
    cRegistrationList *instance();
    void clear();
};

NAMESPACE_END


#endif

