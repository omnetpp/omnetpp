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
// cBag - container class to hold small non-class items (int, double etc.)
//        cBag stores full copy of each item inserted and NOT the pointer

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
    cBag(cBag& bag);
    explicit cBag(const char *name=NULL, int esiz=4,int siz=0,int delt=5);
    virtual ~cBag()  {clear();}

    // redefined functions
    virtual const char *className() const {return "cBag";}
    virtual cObject *dup()  {return new cBag(*this);}
    virtual void info(char *buf);
    virtual const char *inspectorFactoryName() const {return "cBagIFC";}

    virtual int netPack();
    virtual int netUnpack();

    cBag& operator=(cBag& bag);
    void setup(int esiz, int siz, int delt=5); // clear & change params

    // new functions
    void clear();                    // delete whole contents
    int items() const {return lastused+1;} // number of objects contained
    int add(void *obj);              // add a new item
    int addAt(int m, void *obj);     // add a new item at position m
    int find(void *obj) const;             // index of an item (-1 if not found)
    void *get(int m);                // get item by index
    const void *get(int m) const;    // get item by index
    void *operator[](int m)          // act as a vector
          {return get(m);}
    const void *operator[](int m) const // act as a vector
      {return get(m);}
    bool isUsed(int m);              // see if item m exists or not
    void *remove(int m);             // get item; returns true if existed
};

//==========================================================================
// cArray: array of cObjects
//    By default, cArray stores the pointers of the objects.
//    If you want it to make an own copy, set takeOwnership().

class SIM_API cArray : public cObject
{
  private:
    cObject **vect;   // vector of objects
    int size;         // size of vector
    int delta;        // if needed, grows by delta
    int firstfree;    // first free position in vect[]
    int last;         // last used position

  public:
    cArray(cArray& list);
    explicit cArray(const char *name=NULL, int siz=0, int dt=10);
    virtual ~cArray();

    // redefined functions
    virtual const char *className() const {return "cArray";}
    virtual cObject *dup()  {return new cArray(*this);}
    virtual void info(char *buf);
    virtual const char *inspectorFactoryName() const {return "cArrayIFC";}
    virtual void forEach(ForeachFunc f);
    cArray& operator=(cArray& list);

    virtual int netPack();
    virtual int netUnpack();

    // new functions
    int items() const {return last+1;}    // number of objects contained (at most)
    void clear();                   // delete whole contents
    int add(cObject *obj);          // add object at first free position
    int addAt(int m,cObject *obj);  // add object at given position

    int find(cObject *obj)const ;       // index of an item (-1 if not found)
    int find(const char *objname) const;// index of an item (-1 if not found)
    cObject *get(int m);                // get item by index
    cObject *get(const char *objname);  // get item by name
    const cObject *get(int m) const;          // get item by index
    const cObject *get(const char *objname) const;  // get item by name

    cObject *operator[](int m)      // act as a vector
      {return get(m);}
    cObject *operator[](const char *objname)  // indexable with name, too
      {return get(objname);}
    const cObject *operator[](int m) const     // act as a vector
      {return get(m);}
    const cObject *operator[](const char *objname) const  // indexable with name, too
      {return get(objname);}

    bool exist(int m)               // see if slot m used or not
      {return m>=0 && m<=last && vect[m]!=NULL;}
    bool exist(const char *objname) // see if item objname exists or not
      {return find(objname)!=-1;}

    cObject *remove(int m);         // remove item from list
    cObject *remove(const char *objname);
    cObject *remove(cObject *obj);
};

#endif

