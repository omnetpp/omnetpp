//==========================================================================
//   CSTRUCT.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cStructDescriptor  : meta-info about structures
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSTRUCT_H
#define __CSTRUCT_H

#include "cobject.h"

class sFieldWrapper;


/**
 * Abstract base class for structure description classes, used mainly
 * with message subclassing.
 *
 * The copy constructor, dup() and the assignment operator are redefined
 * to raise an error, since they would be of no use in subclasses.
 *
 */
class cStructDescriptor : public cObject
{
    // Field types.
    enum {
        FT_BASIC,   // int, long, double, bool, char*, char[]
        FT_SPECIAL, // another data type that requires special presentation (e.g IP address)
        FT_STRUCT,  // embedded structure, for which there's another cStructDescriptor
        FT_BASIC_ARRAY,   // array of FT_BASIC
        FT_SPECIAL_ARRAY, // array of FT_SPECIAL
        FT_STRUCT_ARRAY,  // array of FT_STRUCT
        FT_INVALID        // invalid type (signals error condition)
    };
  private:
    void *p;
  protected:
    // utility functions for converting from/to strings
    static void long2string(long l, char *buf, int bufsize);
    static void ulong2string(unsigned long l, char *buf, int bufsize);
    static long string2long(const char *s);
    static unsigned long string2ulong(const char *s);
    static void bool2string(bool b, char *buf, int bufsize);
    static bool string2bool(const char *s);
    static void double2string(double d, char *buf, int bufsize);
    static double string2double(const char *s);
    static void enum2string(long e, const char *enumname, char *buf, int bufsize);
    static long string2enum(const char *s, const char *enumname);
  public:
    cStructDescriptor() {}
    cStructDescriptor(void *p) {p=_p;}
    cStructDescriptor(cStructDescriptor& cs);
    virtual ~cStructDescriptor();

    virtual const char *className() {return "cStructDescriptor";}
    virtual cObject *dup();
    cStructDescriptor& operator=(cStructDescriptor& cs);

    void setStruct(void *_p)  {p=_p;}
    void *getStruct()  {return p;}

    static cStructDescriptor *createDescriptorFor(cObject *obj);
    static cStructDescriptor *createDescriptorFor(const char *classname, void *p);

    virtual int getFieldCount() = 0;
    virtual const char *getFieldName(int field) = 0;
    virtual int getFieldType(int field) = 0;
    virtual const char *getFieldTypeString(int field) = 0;
    virtual int getArraySize(int field) = 0;

    virtual void getFieldAsString(int field, int i, char *buf, int bufsize) = 0;
    virtual void setFieldAsString(int field, int i, char *buf) = 0;

    virtual sFieldWrapper *getFieldWrapper(int field, int i) = 0;
    virtual void setFieldWrapper(int field, int i, sFieldWrapper *w) = 0;

    virtual const char *getFieldStructName(int field) = 0;
    virtual void *getFieldStructPointer(int field, int i) = 0;
};


/**
 * Supporting class for cStructDescriptor. Abstract base class. Subclasses can
 * be used to represent FT_SPECIAL fields for a cStructDescriptor.
 */
class sFieldWrapper
{
  public:
    sFieldWrapper() {}
    virtual ~sFieldWrapper() {}

    virtual const char *fieldType() = 0;
    virtual void getAsString(char *buf, int bufsize) = 0;
    virtual void setAsString(const char *buf) = 0;
    // other getter/setter members will be necessary to make Wrappers useful.
};

#endif

