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
 * @ingroup Internals
 */
class cStructDescriptor : public cObject
{
    /// Field types.
    enum {
        FT_BASIC,   /// int, long, double, bool, char*, char[]
        FT_SPECIAL, /// another data type that requires special presentation (e.g IP address)
        FT_STRUCT,  /// embedded structure, for which there's another cStructDescriptor
        FT_BASIC_ARRAY,   /// array of FT_BASIC
        FT_SPECIAL_ARRAY, /// array of FT_SPECIAL
        FT_STRUCT_ARRAY,  /// array of FT_STRUCT
        FT_INVALID        /// invalid type (signals error condition)
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
    /**
     * Constructor.
     */
    cStructDescriptor() {}

    /**
     * Constructor.
     */
    cStructDescriptor(void *_p) {p=_p;}

    /**
     * Copy constructor.
     */
    cStructDescriptor(cStructDescriptor& cs);

    /**
     * Destructor.
     */
    virtual ~cStructDescriptor();

    /**
     * Returns pointer to a string containing the class name, "cStructDescriptor".
     */
    virtual const char *className() const {return "cStructDescriptor";}

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup();

    /**
     * Assignment operator. The name member doesn't get copied;
     * see cObject's operator=() for more details.
     */
    cStructDescriptor& operator=(cStructDescriptor& cs);

    /**
     * Factory method.
     */
    static cStructDescriptor *createDescriptorFor(cObject *obj);

    /**
     * Factory method.
     */
    static cStructDescriptor *createDescriptorFor(const char *classname, void *p);

    /**
     * MISSINGDOC: xxx
     */
    void setStruct(void *_p)  {p=_p;}

    /**
     * MISSINGDOC: xxx
     */
    void *getStruct()  {return p;}

    /**
     * MISSINGDOC: xxx
     */
    virtual int getFieldCount() = 0;

    /**
     * MISSINGDOC: xxx
     */
    virtual const char *getFieldName(int field) = 0;

    /**
     * MISSINGDOC: xxx
     */
    virtual int getFieldType(int field) = 0;

    /**
     * MISSINGDOC: xxx
     */
    virtual const char *getFieldTypeString(int field) = 0;

    /**
     * MISSINGDOC: xxx
     */
    virtual int getArraySize(int field) = 0;

    /**
     * MISSINGDOC: xxx
     */
    virtual void getFieldAsString(int field, int i, char *buf, int bufsize) = 0;

    /**
     * MISSINGDOC: xxx
     */
    virtual void setFieldAsString(int field, int i, char *buf) = 0;

    /**
     * MISSINGDOC: xxx
     */
    virtual sFieldWrapper *getFieldWrapper(int field, int i) = 0;

    /**
     * MISSINGDOC: xxx
     */
    virtual void setFieldWrapper(int field, int i, sFieldWrapper *w) = 0;

    /**
     * MISSINGDOC: xxx
     */
    virtual const char *getFieldStructName(int field) = 0;

    /**
     * MISSINGDOC: xxx
     */
    virtual void *getFieldStructPointer(int field, int i) = 0;
};


/**
 * Supporting class for cStructDescriptor. Abstract base class. Subclasses can
 * be used to represent FT_SPECIAL fields for a cStructDescriptor.
 *
 * Additional getter/setter members will be necessary to make sFieldWrappers
 * useful.
 */
class sFieldWrapper
{
  public:
    /**
     * MISSINGDOC: xxx
     */
    sFieldWrapper() {}

    /**
     * MISSINGDOC: xxx
     */
    virtual ~sFieldWrapper() {}

    /**
     * MISSINGDOC: xxx
     */
    virtual const char *fieldType() = 0;

    /**
     * MISSINGDOC: xxx
     */
    virtual void getAsString(char *buf, int bufsize) = 0;

    /**
     * MISSINGDOC: xxx
     */
    virtual void setAsString(const char *buf) = 0;
};

#endif

