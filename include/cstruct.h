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
 * FIXME: explain client object, fieldwrapper, etc.
 *
 * The copy constructor, dup() and the assignment operator are redefined
 * to raise an error, since they would be of no use in subclasses.
 *
 * @ingroup Internals
 * @see sFieldWrapper
 */
class cStructDescriptor : public cObject
{
    /// Field types.
    enum {
        FT_BASIC,         ///< int, long, double, bool, char*, char[]
        FT_SPECIAL,       ///< another data type that requires special presentation (e.g IP address)
        FT_STRUCT,        ///< embedded structure, for which there's another cStructDescriptor
        FT_BASIC_ARRAY,   ///< array of FT_BASIC
        FT_SPECIAL_ARRAY, ///< array of FT_SPECIAL
        FT_STRUCT_ARRAY,  ///< array of FT_STRUCT
        FT_INVALID        ///< invalid type (signals error condition)
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
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    cStructDescriptor() {}

    /**
     * Constructor. The argument is the client object.
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
     * Assignment operator. The name member doesn't get copied;
     * see cObject's operator=() for more details.
     */
    cStructDescriptor& operator=(cStructDescriptor& cs);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Returns pointer to a string containing the class name, "cStructDescriptor".
     */
    virtual const char *className() const {return "cStructDescriptor";}

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup();
    //@}

    /** @name Getting descriptor for an object or a struct. */
    //@{

    /**
     * Creates and returns a descriptor object for the object passed as argument.
     * The type of the descriptor object will be determined from the className()
     * of 'obj'. 'obj' will be the client object for the descriptor object.
     */
    static cStructDescriptor *createDescriptorFor(cObject *obj);

    /**
     * Creates and returns a descriptor object for the struct passed as argument.
     * The type of the descriptor object will be determined from the 1st argument,
     * classname. The passed struct will be the client object for the descriptor object.
     */
    static cStructDescriptor *createDescriptorFor(const char *classname, void *p);
    //@}

    /** @name Getting and setting client object. */
    //@{

    /**
     * Sets client object.
     */
    void setStruct(void *_p)  {p=_p;}

    /**
     * Returns client object.
     */
    void *getStruct() _CONST  {return p;}
    //@}

    /** @name Querying and setting fields of the client object. */
    //@{

    /**
     * Must be redefined in subclasses to return the number of fields
     * in the client object.
     */
    virtual int getFieldCount() = 0;

    /**
     * Must be redefined in subclasses to return the name of a field
     * in the client object.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     */
    virtual const char *getFieldName(int field) = 0;

    /**
     * Must be redefined in subclasses to return the type of a field
     * in the client object.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     * FIXME: type constants!
     */
    virtual int getFieldType(int field) = 0;

    /**
     * Must be redefined in subclasses to return the type of a field
     * in the client object as a string.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     */
    virtual const char *getFieldTypeString(int field) = 0;

    /**
     * Must be redefined in subclasses to return the array size of a field
     * in the client object. If the field is not an array, it should return 0.
     */
    virtual int getArraySize(int field) = 0;

    /**
     * Must be redefined in subclasses to return the value of a basic field
     * (of type FT_BASIC(_ARRAY)) in the client object as a string.
     * FIXME: what if called for non-basic fields?
     */
    virtual void getFieldAsString(int field, int i, char *buf, int bufsize) = 0;

    /**
     * Must be redefined in subclasses to set the value of a basic field
     * (of type FT_BASIC(_ARRAY)) in the client object as a string.
     * .
     * FIXME: what if called for non-basic fields?
     */
    virtual void setFieldAsString(int field, int i, char *buf) = 0;

    /**
     * Must be redefined in subclasses to return a wrapper for an
     * FT_SPECIAL(_ARRAY) field in the client object.
     */
    virtual sFieldWrapper *getFieldWrapper(int field, int i) = 0;

    /**
     * Must be redefined in subclasses to set an FT_SPECIAL(_ARRAY) field
     * in the client object via a wrapper.
     */
    // FIXME: remove this method? setting can happen via the wrapper!
    virtual void setFieldWrapper(int field, int i, sFieldWrapper *w) = 0;

    /**
     * Must be redefined in subclasses to return the type name of an
     * FT_STRUCT(_ARRAY) field in the client object.
     * The return value may be used then as classname
     * to create a descriptor object for this structure field.
     */
    virtual const char *getFieldStructName(int field) = 0;

    /**
     * Must be redefined in subclasses to return the pointer of an
     * FT_STRUCT(_ARRAY) field in the client object.
     * The return value may be used then as client object pointer
     * to create a descriptor object for this structure field.
     */
    virtual void *getFieldStructPointer(int field, int i) = 0;
    //@}
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
     * Constructor. In subclasses, the constructor will need to take
     * a pointer to actual object or member it is wrapping, and store
     * it in a private data member (also to be added in subclasses).
     */
    sFieldWrapper() {}

    /**
     * Destructor.
     */
    virtual ~sFieldWrapper() {}

    /**
     * Must be redefined in subclasses to return the type of the field
     * as a string.
     */
    virtual const char *fieldType() = 0;

    /**
     * Must be redefined in subclasses to return the value of the field
     * as a string.
     *
     * Other (more specific) get..() methods will typically need to be
     * added to the class to make it useful.
     */
    virtual void getAsString(char *buf, int bufsize) = 0;

    /**
     * Must be redefined in subclasses to set the value of the field
     * as a string.
     *
     * Other (more specific) set..() methods will typically need to be added
     * to the class to make it useful.
     */
    virtual void setAsString(const char *buf) = 0;
};

#endif

