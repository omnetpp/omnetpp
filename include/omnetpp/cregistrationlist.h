//==========================================================================
//  CREGISTRATIONLIST.H - part of
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

#ifndef __OMNETPP_CREGISTRATIONLIST_H
#define __OMNETPP_CREGISTRATIONLIST_H

#include <vector>
#include <map>
#include "simkerneldefs.h"
#include "cnamedobject.h"

namespace omnetpp {

class cOwnedObject;

/**
 * @brief Implements a list or table of objects with qualified names.
 *
 * The getName() method of objects should return the unqualified name
 * (without namespace or package name), and the getFullName() method
 * the qualified name (with namespace or package).
 *
 * @ingroup Internals
 */
class SIM_API cRegistrationList : public cNamedObject, noncopyable
{
  private:
    typedef std::map<std::string, cOwnedObject*> StringObjectMap;
    std::vector<cOwnedObject *> vec;  // for fast iteration
    StringObjectMap nameMap;          // for lookup by getName()
    StringObjectMap fullnameMap;      // for lookup by getFullName()

  public:
    typedef std::vector<cOwnedObject *>::iterator iterator;

    cRegistrationList(const char *name) : cNamedObject(name, false) {}
    virtual ~cRegistrationList();

    /** @name cObject methods */
    //@{
    virtual std::string str() const override;
    virtual void forEachChild(cVisitor *v) override;
    //@}


    /**
     * Returns an iterator referring to the first element.
     */
    iterator begin() {return vec.begin();}

    /**
     * Returns an iterator referring to the past-the-end element.
     */
    iterator end() {return vec.end();}

    /**
     * Adds an object to the container.
     */
    virtual void add(cOwnedObject *obj);

    /**
     * Returns the number of elements.
     */
    virtual int size() const {return vec.size();}

    /**
     * Returns the ith element, or nullptr.
     */
    virtual cOwnedObject *get(int i) const;

    /**
     * Returns (one of) the object(s) with the given name (not fullName!).
     * Returns nullptr if not found.
     */
    virtual cOwnedObject *find(const char *name) const;

    /**
     * Returns the object with the exact given qualified name (getFullName()).
     * Returns nullptr if not found.
     */
    virtual cOwnedObject *lookup(const char *qualifiedName) const;

    /**
     * Returns the object with the given qualified name. If not found, it is
     * also tried in the given context namespace(s). Returns nullptr if not found.
     */
    virtual cOwnedObject *lookup(const char *qualifiedName, const char *contextNamespace, bool fallbackToOmnetpp=false);

    /**
     * Sorts the elements by qualified name (getFullName()). This affects
     * the order get() will return the elements.
     */
    virtual void sort();

};

/**
 * @brief Singleton class, used for registration lists.
 * Instances are supposed to be global variables.
 *
 * @ingroup Internals
 */
class SIM_API cGlobalRegistrationList
{
  private:
    cRegistrationList *inst = nullptr;
    const char *tmpname = nullptr;
  public:
    cGlobalRegistrationList() {}
    cGlobalRegistrationList(const char *name);
    ~cGlobalRegistrationList();
    cRegistrationList *getInstance();
    void clear();

    cRegistrationList::iterator begin() {return getInstance()->begin();}
    cRegistrationList::iterator end() {return getInstance()->end();}
};


}  // namespace omnetpp


#endif

