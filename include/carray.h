//==========================================================================
//   CARRAY.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cBag      : expandable array to store small no-class items
//    cArray    : flexible array to store cObject objects
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CARRAY_H
#define __CARRAY_H

#include "cobject.h"


/**
 * Container class to hold small non-class items (int, double, etc).
 * cBag stores full copy of each item inserted and NOT the pointer.
 *
 * cBag is a container class which is designed to hold non-cObject
 * items without constructors/destructors (ints, doubles,
 * small structs etc.). cBag stores full, bit-by-bit copies
 * of the items inserted (and not the pointers). cBag works
 * as an array, but if it gets full, it grows automatically by a
 * specified delta.
 *
 * @ingroup Containers
 */
class SIM_API cBag : public cObject
{
  private:
    char *vect;
    int elemsize;
    int size;
    int delta;
    int lastused;
    int firstfree;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cBag(const cBag& bag);

    /**
     * Constructor. Parameters are: name, the size of elements in bytes,
     * the initial size of the container and the delta (by which the size
     * will grow if it gets full).
     */
    explicit cBag(const char *name=NULL, int esiz=4,int siz=0,int delt=5);

    /**
     * Destructor. The items in the object will be deleted.
     */
    virtual ~cBag()  {clear();}

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     * Duplication and assignment work all right with cBag.
     */
    cBag& operator=(const cBag& bag);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const  {return new cBag(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netUnpack();
    //@}

    /** @name Container functions. */
    //@{

    /**
     * Clears the current contents and changes element size, initial
     * array size and delta parameters.
     */
    void setup(int esiz, int siz, int delt=5);

    /**
     * Clears the whole contents of the cBag.
     */
    void clear();

    /**
     * Returns the index of last used position+1. This is the same as the
     * number of contained objects if there are no 'holes' in the array.
     * (Removals may create holes.)
     */
    int items() const {return lastused+1;}

    /**
     * Inserts a new item into the array. A copy will be made of the
     * item pointed to by data. The return value is the item's
     * index in the array.
     */
    int add(void *obj);

    /**
     * Inserts a new item into the array at the given position.
     */
    int addAt(int m, void *obj);

    /**
     * Returns the index of the first item in the array that equals the
     * item pointed to by obj. The comparison is done byte-by-byte.
     * If no such item was found, -1 is returned.
     */
    int find(void *obj) const;

    /**
     * Returns a pointer to the mth item in the array or NULL
     * if the mth position is not used.
     */
    void *get(int m);

    /**
     * Returns a pointer to the mth item in the array or NULL
     * if the mth position is not used.
     */
    const void *get(int m) const;

    /**
     * The same as get(int m). With the indexing operator, cBag
     * can be used as a vector.
     */
    void *operator[](int m)
          {return get(m);}

    /**
     * The same as get(int m). With the indexing operator, cBag
     * can be used as a vector.
     */
    const void *operator[](int m) const
      {return get(m);}

    /**
     * Returns true if the mth position exists and
     * an item was inserted there.
     */
    bool isUsed(int m) const;

    /**
     * FIXME: Remove item at position m and return its pointer. (???)
     */
    void *remove(int m);
    //@}
};

//==========================================================================

/**
 * Container object that holds objects derived from cObject.
 * cArray stores the pointers of the objects inserted instead of making copies.
 * cArray works as an array, but if it gets full, it grows automatically by
 * a specified delta. Ownership of contained objects (responsibility of deletion)
 * can be specified per-object basis (see cObject::takeOwnership()).
 * Default is that cArray takes the ownership of each object inserted
 * (that is, takeOwnership() is set to true).
 *
 * @ingroup Containers
 */
class SIM_API cArray : public cObject
{
  private:
    cObject **vect;   // vector of objects
    int size;         // size of vector
    int delta;        // if needed, grows by delta
    int firstfree;    // first free position in vect[]
    int last;         // last used position

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor. Contained objects that are owned by cArray
     * (that is, whose owner() is the cArray) will
     * be duplicated so that the new cArray will have its own
     * copy of them.
     */
    cArray(const cArray& list);

    /**
     * Constructor. The initial size of the container and the delta
     * (by which the size will grow if it gets full) can be specified.
     */
    explicit cArray(const char *name=NULL, int siz=0, int dt=10);

    /**
     * Destructor. The contained objects that were owned by the container
     * will be deleted.
     */
    virtual ~cArray();

    /**
     * Assignment operator. The name member doesn't get copied;
     * see cObject's operator=() for more details.
     * Duplication and assignment work all right with cArray.
     * Contained objects that are owned by cArray will be duplicated
     * so that the new cArray will have its own copy of them.
     */
    cArray& operator=(const cArray& list);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Duplication and assignment work all right with cArray.
     * Contained objects that are owned by cArray will be duplicated
     * so that the new cArray will have its own copy of them.
     */
    virtual cObject *dup() const  {return new cArray(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Calls the given function for each object contained.
     */
    virtual void forEach(ForeachFunc f);

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netUnpack();
    //@}

    /** @name Container functions. */
    //@{

    /**
     * Returns the index of last used position+1. This is the same as the
     * number of contained objects if there are no 'holes' in the array.
     * (Removals may create holes.)
     */
    int items() const {return last+1;}

    /**
     * As a result, the container will be empty. Contained objects that
     * were owned by the container will be deleted.
     */
    void clear();

    /**
     * Inserts the object into the array. Only the pointer of the object
     * will be stored. The return value is the object's index in the
     * array.
     */
    int add(cObject *obj);

    /**
     * Inserts the object into the array at the given position. If
     * the position is occupied, the function throws cException.
     * The return value is the object's index in the array.
     */
    int addAt(int m,cObject *obj);

    /**
     * Inserts the object into the array. If the array already contains
     * an object with the same name, it will be replaced (hashtable-like
     * behavior.) The replaced object, if it was owned by the container,
     * is deleted using discard().
     * The return value is the object's index in the array.
     */
    int set(cObject *obj);

    /**
     * Searches the array for the pointer of the object passed and returns
     * the index of the first match. If the object was not found, -1 is
     * returned.
     */
    int find(cObject *obj) const;

    /**
     * Returns the index of the first item in the array that has the
     * name pointed to by s (cObject::isName() is used.)
     * If no such item was found, -1 is returned.
     */
    int find(const char *objname) const;

    /**
     * Returns reference to the mth object in the array. Returns NULL
     * if the mth position is not used.
     */
    cObject *get(int m);

    /**
     * Returns reference to the first object in the array with name s.
     * Returns NULL if no object with the given name was found.
     */
    cObject *get(const char *objname);

    /**
     * Returns reference to the mth object in the array. Returns NULL
     * if the mth position is not used.
     */
    const cObject *get(int m) const;

    /**
     * Returns reference to the first object in the array with name s.
     * Returns NULL if no object with the given name was found.
     */
    const cObject *get(const char *objname) const;

    /**
     * The same as get(int). With the indexing operator,
     * cArray can be used as a vector.
     */
    cObject *operator[](int m)
      {return get(m);}

    /**
     * The same as get(const char *). With the indexing operator,
     * cArray can be used as a vector.
     */
    cObject *operator[](const char *objname)
      {return get(objname);}

    /**
     * The same as get(int). With the indexing operator,
     * cArray can be used as a vector.
     */
    const cObject *operator[](int m) const
      {return get(m);}

    /**
     * The same as get(const char *). With the indexing operator,
     * cArray can be used as a vector.
     */
    const cObject *operator[](const char *objname) const
      {return get(objname);}

    /**
     * Returns true if position m is used in the array, otherwise false.
     */
    bool exist(int m) const
      {return m>=0 && m<=last && vect[m]!=NULL;}

    /**
     * Returns true if the array contains an object with the given name,
     * otherwise false.
     */
    bool exist(const char *objname) const
      {return find(objname)!=-1;}

    /**
     * Removes the object given with its index/name/pointer from the
     * container. (If the object was owned by the container, drop()
     * is called.)
     */
    cObject *remove(int m);

    /**
     * Removes the object given with its index/name/pointer from the
     * container. (If the object was owned by the container, drop()
     * is called.)
     */
    cObject *remove(const char *objname);

    /**
     * Removes the object given with its index/name/pointer from the
     * container, and returns the same pointer. If the object was not
     * found, NULL is returned. (If the object was owned by the container, drop()
     * is called.)
     */
    cObject *remove(cObject *obj);
    //@}
};

#endif

