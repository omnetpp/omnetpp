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
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSTRUCT_H
#define __CSTRUCT_H

#include "cobject.h"
#include "cenum.h"
class sFieldWrapper;


/**
 * Abstract base class for structure description classes, used mainly
 * with message subclassing.
 *
 * FIXME: explain client object, fieldwrapper, etc.
 *
 * The copy constructor, dup() and the assignment operator are redefined
 * to raise an error (throw cException), since they would be of no use in subclasses.
 *
 * @ingroup Internals
 * @see sFieldWrapper
 */
class SIM_API cStructDescriptor : public cObject
{
  public:
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

  protected:
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
    static void oppstring2string(const opp_string& str, char *buf, int bufsize);
    static void string2oppstring(const char *s, opp_string& str);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor. The argument is the client object.
     */
    cStructDescriptor(void *_p=NULL) {p=_p;}

    /**
     * Copy constructor.
     */
    cStructDescriptor(const cStructDescriptor& cs)  {setName(cs.name());operator=(cs);}

    /**
     * Destructor.
     */
    virtual ~cStructDescriptor();

    /**
     * Assignment is not supported by this class: this method throws a cException when called.
     */
    cStructDescriptor& operator=(const cStructDescriptor&)  {copyNotSupported();return *this;}
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /* No dup() because this is an abstract class. */
    //@}

    /** @name Getting descriptor for an object or a struct. */
    //@{

    /**
     * Returns true if the class 'classname' has an associated cStructDescriptor object.
     */
    static bool hasDescriptor(const char *classname);

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
    void *getStruct() const  {return p;}
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
     * Returns the enum name associated with the field. This makes only
     * sense with integer-type fields (short, int, long, etc.).
     * Returns NULL if there's no associated enum.
     *
     * @see cEnum
     */
    virtual const char *getFieldEnumName(int field) = 0;

    /**
     * Must be redefined in subclasses to return the array size of a field
     * in the client object. If the field is not an array, it should return 0.
     */
    virtual int getArraySize(int field) = 0;

    /**
     * Must be redefined in subclasses to return the value of a basic field
     * (of type FT_BASIC(_ARRAY)) in the client object as a string.
     * Returns true if no error occurred, false otherwise.
     * FIXME: what if called for non-basic fields?
     */
    virtual bool getFieldAsString(int field, int i, char *buf, int bufsize) = 0;

    /**
     * Must be redefined in subclasses to set the value of a basic field
     * (of type FT_BASIC(_ARRAY)) in the client object as a string.
     * Returns true if no error occurred, false otherwise.
     * FIXME: what if called for non-basic fields?
     */
    virtual bool setFieldAsString(int field, int i, const char *value) = 0;

    /**
     * Must be redefined in subclasses to return a wrapper for an
     * FT_SPECIAL(_ARRAY) field in the client object. There's no corresponding
     * setFieldWrapper() method -- setting the field value should can take
     * place via the wrapper object returned here.                              *
     * Returns NULL if no associated wrapper is defined for this field.
     */
    virtual sFieldWrapper *getFieldWrapper(int field, int i) = 0;

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
class SIM_API sFieldWrapper
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
    virtual void setAsString(const char *value) = 0;
};

#endif

