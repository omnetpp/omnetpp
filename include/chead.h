//==========================================================================
//   CHEAD.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cHead     : head of a list of cObjs
//    cIterator : walks along a list
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CHEAD_H
#define __CHEAD_H

#include "cobject.h"

//=== classes declared here
class  cIterator;
class  cHead;

//=== classes mentioned
class  cModuleInterface;
class  cModuleType;
class  cLinkType;
class  cFunctionType;
class  cNetworkType;
class  cEnum;

//==========================================================================

/**
 * Head of a cObject list. cObject and its derived classes contain
 * pointers that enable the objects to be a node in a double-linked list.
 * cObject has member functions to link and unlink to and from double-linked
 * lists (See documentation on cObject::setOwner()) The head of such
 * lists is always a cHead object. The lists are a means
 * that provide that each object in the system is part of an object
 * tree and can be accessed through pointers starting from a given
 * point. The existence of such hierarchy is necessary for a user
 * interface where we want each object to be 'visible' to the user.
 * It is also unavoidable when we want the simulation to be restartable
 * (we need to destroy objects created by the running simulation
 * to start a new one). Last, it enables that all objects can be
 * reached through forEach() on which many algorithms rely
 * (e.g. saveresults()).
 *
 * NOTE: the dup() and operator=() functions are NOT implemented.
 * dup() would require that every object in the list be duplicated.
 * Since cHead is mostly an internal class and is NOT intended
 * for use by the programmer as a container class, the dup() operation
 * was considered unnecessary.
 *
 * @ingroup Internals
 * @see cIterator
 */
class SIM_API cHead : public cObject
{
    friend class const_cIterator;
    friend class cIterator;
    friend class cObject;

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    cHead(const char *name=NULL);

    /**
     * Copy constructor.
     */
    cHead(const cHead& h) : cObject(h)  {setName(h.name());operator=(h);}

    /**
     * Destructor. Inherited from cObject, it deletes all owned objects
     * (i.e. those in the list) that were created on the heap.
     */
    virtual ~cHead()  {}

    /**
     * Assignment is not supported by this class: this method throws a cException when called.
     */
    cHead& operator=(const cHead&)  {copyNotSupported();return *this;}
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Dupping is not implemented for this class. This function
     * gives an error (throws cException) when called.
     */
    virtual cObject *dup() const  {return new cHead(*this);}

    /**
     * Calls the function passed for each object
     * in the list.
     */
    virtual void forEach(ForeachFunc f);
    //@}

    /** @name Container methods. */
    //@{

    /**
     * Searches the list for an object with the given name and returns
     * its pointer. If no such object was found, NULL is returned.
     */
    cObject *find(const char *objname) const;

    /**
     * Returns the number of objects in the list.
     */
    int count() const;
    //@}
};

//==========================================================================

/**
 * Walks along a cHead-cObject list.
 */
class SIM_API cIterator
{
  private:
    cObject *p;

  public:
    /**
     * Constructor.
     */
    cIterator(const cObject& h)    {p = &h ? h.firstchildp : NULL;}

    /**
     * Reinitialize the iterator.
     */
    void init(cObject& h)    {p = &h ? h.firstchildp : NULL;}

    /**
     * Returns a pointer to the current object.
     */
    cObject *operator()() const    {return p;}

    /**
     * Returns true if the iterator reached the end of the list.
     */
    bool end() const  {return (bool)(p==NULL);}

    /**
     * Returns the current item, then moves the iterator to the next one.
     */
    cObject *operator++(int)  {if (!p) return NULL; cObject *t=p; p=p->nextp; return t;}
};


/**
 * Walks along a cHead-cObject list.
 */
class SIM_API const_cIterator
{
  private:
    const cObject *p;

  public:
    /**
     * Constructor.
     */
    const_cIterator(const cObject& h)    {p = &h ? h.firstchildp : NULL;}

    /**
     * Reinitialize the iterator.
     */
    void init(const cObject& h)    {p = &h ? h.firstchildp : NULL;}

    /**
     * Returns a pointer to the current object.
     */
    const cObject *operator()() const  {return p;}

    /**
     * Returns true if the iterator reached the end of the list.
     */
    bool end() const  {return (bool)(p==NULL);}

    /**
     * Returns the current item, then moves the iterator to the next one.
     */
    const cObject *operator++(int)  {if (!p) return NULL; const cObject *t=p; p=p->nextp; return t;}
};

//==========================================================================

/**
 * @name Finding global objects by name.
 * @ingroup Functions
 */
//@{

/** Find a cNetworkType. */
inline cNetworkType *findNetwork(const char *s)
  {return (cNetworkType *)networks.find(s);}

/** Find a cModuleType. */
inline cModuleType *findModuleType(const char *s)
  {return (cModuleType *)modtypes.find(s);}

/** Find a cModuleInterface. */
inline cModuleInterface *findModuleInterface(const char *s)
  {return (cModuleInterface *)modinterfaces.find(s);}

/** Find a cLinkType. */
inline cLinkType *findLink(const char *s)
  {return (cLinkType *)linktypes.find(s);}

/** Find a cFunctionType. */
cFunctionType *findFunction(const char *s,int argcount);

/** Find a cEnum. */
inline cEnum *findEnum(const char *s)
  {return (cEnum *)enums.find(s);}
//@}

#endif

