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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CARRAY_H
#define __CARRAY_H

#include "cobject.h"

//=== classes declared here
class  cArray;
class  cBag;

//==========================================================================

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

    /**
     * Copy constructor.
     */
    cBag(cBag& bag);

    /**
     * Constructor.
     */
    explicit cBag(const char *name=NULL, int esiz=4,int siz=0,int delt=5);

    /**
     * Destructor. The items in the object will be deleted.
     */
    virtual ~cBag()  {clear();}

    // redefined functions

    /**
     * Returns a pointer to the class name string, "cBag".
     */
    virtual const char *className() const {return "cBag";}

    /**
     * Duplication and assignment work all right with cBag.
     */
    virtual cObject *dup()  {return new cBag(*this);}

    /**
     * Redefined.
     */
    virtual void info(char *buf);

    /**
     * Redefined.
     */
    virtual const char *inspectorFactoryName() const {return "cBagIFC";}


    /**
     * MISSINGDOC: cBag:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cBag:int netUnpack()
     */
    virtual int netUnpack();


    /**
     * Duplication and assignment work all right with cBag.
     */
    cBag& operator=(cBag& bag);

    /**
     * Clears the current contents and changes element size, initial
     * array size and delta parameters.
     */
    void setup(int esiz, int siz, int delt=5); // clear & change params

    // new functions

    /**
     * Clears the whole contents of the cBag.
     */
    void clear();                    // delete whole contents

    /**
     * Returns the index of last used position+1.
     */
    int items() const {return lastused+1;} // number of objects contained

    /**
     * Inserts a new item into the array. A copy will be made of the
     * item pointed to by data. The return value is the item's
     * index in the array.
     */
    int add(void *obj);              // add a new item

    /**
     * Inserts a new item into the array at the given position.
     */
    int addAt(int m, void *obj);     // add a new item at position m

    /**
     * Returns the index of the first item in the array that equals the
     * item pointed to by obj. The comparison is done byte-by-byte.
     * If no such item was found, -1 is returned.
     */
    int find(void *obj) const;             // index of an item (-1 if not found)

    /**
     * Returns a pointer to the mth item in the array or NULL
     * if the mth position is not used.
     */
    void *get(int m);                // get item by index

    /**
     * MISSINGDOC: cBag:void*get(int)
     */
    const void *get(int m) const;    // get item by index

    /**
     * The same as get(int m). With the indexing operator, cBag
     * can be used as a vector.
     */
    void *operator[](int m)          // act as a vector
          {return get(m);}

    /**
     * MISSINGDOC: cBag:void*operator[](int)
     */
    const void *operator[](int m) const // act as a vector
      {return get(m);}

    /**
     * Returns true if the mth position exists and
     * an item was inserted there.
     */
    bool isUsed(int m);              // see if item m exists or not

    /**
     * MISSINGDOC: cBag:void*remove(int)
     */
    void *remove(int m);             // get item; returns true if existed
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

    /**
     * Copy constructor. Contained objects that are owned by cArray
     * (that is, whose owner() is the cArray) will
     * be duplicated so that the new cArray will have its own
     * copy of them.
     */
    cArray(cArray& list);

    /**
     * Constructor.
     */
    explicit cArray(const char *name=NULL, int siz=0, int dt=10);

    /**
     * Destructor. The contained objects that were owned by the container
     * will be deleted.
     */
    virtual ~cArray();

    // redefined functions

    /**
     * Returns pointer to the class name string,"cArray".
     */
    virtual const char *className() const {return "cArray";}

    /**
     * Duplication and assignment work all right with cArray.
     * Contained objects that are owned by cArray will be duplicated
     * so that the new cArray will have its own copy of them.
     */
    virtual cObject *dup()  {return new cArray(*this);}

    /**
     * Redefined.
     */
    virtual void info(char *buf);

    /**
     * Redefined.
     */
    virtual const char *inspectorFactoryName() const {return "cArrayIFC";}

    /**
     * Calls the given function for each object contained.
     */
    virtual void forEach(ForeachFunc f);

    /**
     * Duplication and assignment work all right with cArray.
     * Contained objects that are owned by cArray will be duplicated
     * so that the new cArray will have its own copy of them.
     */
    cArray& operator=(cArray& list);


    /**
     * MISSINGDOC: cArray:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cArray:int netUnpack()
     */
    virtual int netUnpack();

    // new functions

    /**
     * Returns the index of last used position+1.
     */
    int items() const {return last+1;}    // number of objects contained (at most)

    /**
     * As a result, the container will be empty. Contained objects that
     * were owned by the container will be deleted.
     */
    void clear();                   // delete whole contents

    /**
     * Inserts a new object into the array. Only the pointer of the object
     * will be stored. The return value is the object's index in the
     * array.
     */
    int add(cObject *obj);          // add object at first free position

    /**
     * Inserts a new object into the array, at the given position. If
     * the position is occupied, the function generates an error message.
     */
    int addAt(int m,cObject *obj);  // add object at given position


    /**
     * Searches the array for the pointer of the object passed and returns
     * the index of the first match. If the object wasn't found, -1 is
     * returned.
     */
    int find(cObject *obj)const ;       // index of an item (-1 if not found)

    /**
     * Returns the index of the first item in the array that has the
     * name pointed to by s (cObject::isName() is used.)
     * If no such item was found, -1 is returned.
     */
    int find(const char *objname) const;// index of an item (-1 if not found)

    /**
     * Returns reference to the mth object in the array or null
     * pointer if the mth position is not used.
     */
    cObject *get(int m);                // get item by index

    /**
     * Returns reference to the first object in the array with name s
     * or null reference (*NOOBJ) if no object with the given
     * name was found.
     */
    cObject *get(const char *objname);  // get item by name

    /**
     * MISSINGDOC: cArray:cObject*get(int)
     */
    const cObject *get(int m) const;          // get item by index

    /**
     * MISSINGDOC: cArray:cObject*get(char*)
     */
    const cObject *get(const char *objname) const;  // get item by name


    /**
     * The same as get(int)/get(const char *). With
     * the indexing operator, cArray can be used as a vector.
     */
    cObject *operator[](int m)      // act as a vector
      {return get(m);}

    /**
     * The same as get(int)/get(const char *). With
     * the indexing operator, cArray can be used as a vector.
     */
    cObject *operator[](const char *objname)  // indexable with name, too
      {return get(objname);}

    /**
     * MISSINGDOC: cArray:cObject*operator[](int)
     */
    const cObject *operator[](int m) const     // act as a vector
      {return get(m);}

    /**
     * MISSINGDOC: cArray:cObject*operator[](char*)
     */
    const cObject *operator[](const char *objname) const  // indexable with name, too
      {return get(objname);}


    /**
     * Returns true if the value returned by get(int)/get(const
     * char *) would not be null reference (*NOOBJ).
     */
    bool exist(int m)               // see if slot m used or not
      {return m>=0 && m<=last && vect[m]!=NULL;}

    /**
     * Returns true if the value returned by get(int)/get(const
     * char *) would not be null reference (*NOOBJ).
     */
    bool exist(const char *objname) // see if item objname exists or not
      {return find(objname)!=-1;}


    /**
     * Removes the object given with its index/name/pointer from the
     * container. (If the object was owned by the container, drop()
     * is called.)
     */
    cObject *remove(int m);         // remove item from list

    /**
     * Removes the object given with its index/name/pointer from the
     * container. (If the object was owned by the container, drop()
     * is called.)
     */
    cObject *remove(const char *objname);

    /**
     * Removes the object given with its index/name/pointer from the
     * container. (If the object was owned by the container, drop()
     * is called.)
     */
    cObject *remove(cObject *obj);
};

#endif
