//==========================================================================
//  CCLASSDESCRIPTOR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cClassDescriptor  : metainfo about structs and classes
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCLASSDESCRIPTOR_H
#define __CCLASSDESCRIPTOR_H

#include <string>
#include "cobject.h"


/**
 * Abstract base class for structure description classes, used mainly
 * with message subclassing.
 *
 * Subclasses of cClassDescriptor encapsulate the kind of reflection
 * information (in the Java sense) which is needed by Tkenv to display
 * fields in a message, struct or object created with the .msg syntax.
 * The cClassDescriptor subclass is generated along with the message class,
 * (struct, object, etc.).
 *
 * When Tkenv encounters a message object, it creates an appropriate
 * cClassDescriptor object and uses that to find out what fields the
 * message object has, what are their values etc. The message object
 * is said to be the `client object' of the cClassDescriptor object.
 *
 * In this class, the copy constructor, dup() and the assignment operator
 * are redefined to raise an error (throw cRuntimeError), since they would
 * be of no use in subclasses.
 *
 * @ingroup Internals
 */
class SIM_API cClassDescriptor : public cObject
{
  public:
    /// Field types.
    enum {
        FD_ISARRAY = 0x01,    ///< field is an array: int a[]; int a[10];
        FD_ISCOMPOUND = 0x02, ///< basic type (T) is struct or class: T a; T *a; T a[10]; T *a[]
        FD_ISPOINTER = 0x04,  ///< field is pointer or pointer array: T *a; T *a[]; T *a[10];
        FD_ISCPOLYMORPHIC = 0x08, ///< if ISCOMPOUND: basic type (T) subclasses from cPolymorphic
        FD_ISCOBJECT = 0x10,  ///< if ISCOMPOUND: basic type (T) subclasses from cObject
        FD_ISEDITABLE = 0x20, ///< whether field supports setFieldAsString()
        FD_NONE = 0x0
    };

  private:
    std::string baseclassname;
    cClassDescriptor *baseclassdesc;
    int inheritancechainlength;

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
    static void oppstring2string(const char *s, char *buf, int bufsize);
    static void oppstring2string(const opp_string& str, char *buf, int bufsize);
    static void oppstring2string(const std::string& str, char *buf, int bufsize);
    static void string2oppstring(const char *s, opp_string& str);
    static void string2oppstring(const char *s, std::string& str);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    cClassDescriptor(const char *classname, const char *_baseclassname=NULL);

    /**
     * Destructor.
     */
    virtual ~cClassDescriptor();
    //@}

    /** @name Getting descriptor for an object or a struct. */
    //@{

    /**
     * Returns the descriptor object for the given class. The returned
     * descriptor object is a singleton, and must not be deleted.
     */
    static cClassDescriptor *getDescriptorFor(const char *classname);

    /**
     * Returns the descriptor object for the given object. This can return
     * descriptor for a base class, if there isn't an exact match.
     * The returned descriptor object is a singleton, and must not be deleted.
     */
    static cClassDescriptor *getDescriptorFor(cPolymorphic *object);
    //@}

    /** @name Querying and setting fields of the client object. */
    //@{

    /**
     * Returns true if this descriptor supports the given object's class.
     * If obj can be cast (dynamic_cast) to the class the descriptor supports,
     * the method should return true.
     */
    virtual bool doesSupport(cPolymorphic *obj) {return false;}

    /**
     * Returns the descriptor for the base class, if available.
     */
    virtual cClassDescriptor *getBaseClassDescriptor();

    /**
     * Returns the number of base classes up to the root -- as far as
     * it's reflected in the descriptors.
     */
    int getInheritanceChainLength();

    /**
     * Returns the value of the given property of the descriptor as a single string.
     * Returns NULL if there's no such property. For structured property values
     * (with multiple keys and/or list(s) inside), the value is returned as a
     * single unparsed string.
     */
    virtual const char *getProperty(const char *propertyname) = 0;

    /**
     * Must be redefined in subclasses to return the number of fields
     * in the client object.
     */
    virtual int getFieldCount(void *object) = 0;

    /**
     * Must be redefined in subclasses to return the name of a field
     * in the client object.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     */
    virtual const char *getFieldName(void *object, int field) = 0;

    /**
     * Must be redefined in subclasses to return the type flags of a field
     * in the client object. Flags is a binary OR of the following:
     * FD_ISARRAY, FD_ISCOMPOUND, FD_ISPOINTER, FD_ISCOBJECT, FD_ISCPOLYMORPHIC.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     */
    virtual unsigned int getFieldTypeFlags(void *object, int field) = 0;

    /** @name Utility functions based on getFieldTypeFlags() */
    //@{
    bool getFieldIsArray(void *object, int field) {return getFieldTypeFlags(object, field) & FD_ISARRAY;}
    bool getFieldIsCompound(void *object, int field) {return getFieldTypeFlags(object, field) & FD_ISCOMPOUND;}
    bool getFieldIsPointer(void *object, int field) {return getFieldTypeFlags(object, field) & FD_ISPOINTER;}
    bool getFieldIsCPolymorphic(void *object, int field) {return getFieldTypeFlags(object, field) & (FD_ISCPOLYMORPHIC|FD_ISCOBJECT);}
    bool getFieldIsCObject(void *object, int field) {return getFieldTypeFlags(object, field) & FD_ISCOBJECT;}
    bool getFieldIsEditable(void *object, int field) {return getFieldTypeFlags(object, field) & FD_ISEDITABLE;}
    //@}

    /**
     * Returns the name of the class on which the given field was declared.
     */
    virtual const char *getFieldDeclaredOn(void *object, int field);

    /**
     * Must be redefined in subclasses to return the type of a field
     * in the client object as a string.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     */
    virtual const char *getFieldTypeString(void *object, int field) = 0;

    /**
     * Returns the value of the given property of the field as a single string.
     * Returns NULL if there's no such property. For structured property values
     * (with multiple keys and/or list(s) inside), the value is returned as a
     * single unparsed string.
     */
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) = 0;

    /**
     * Must be redefined in subclasses to return the array size of a field
     * in the client object. If the field is not an array, it should return 0.
     */
    virtual int getArraySize(void *object, int field) = 0;

    /**
     * Must be redefined in subclasses to return the value of the given field
     * in the client object as a string. Returns true if no error occurred,
     * false otherwise. For compound fields, the message compiler generates
     * code which calls operator<<.
     */
    virtual bool getFieldAsString(void *object, int field, int i, char *buf, int bufsize) = 0;

    /**
     * Must be redefined in subclasses to set the value of a field
     * in the client object from the given value string.
     * Returns true if successful, and false if an error occurred or the
     * field does not support setting.
     */
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) = 0;

    /**
     * Must be redefined in subclasses to return the type name of a compound field
     * in the client object. If it is a pointer, the "*" is removed. The return value
     * may be used then as classname to create a descriptor object for this compound field.
     */
    virtual const char *getFieldStructName(void *object, int field) = 0;

    /**
     * Must be redefined in subclasses to return the pointer of a compound field
     * in the client object.
     */
    virtual void *getFieldStructPointer(void *object, int field, int i) = 0;
    //@}
};

#endif


