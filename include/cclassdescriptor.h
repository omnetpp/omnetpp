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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCLASSDESCRIPTOR_H
#define __CCLASSDESCRIPTOR_H

#include <string>
#include "cownedobject.h"
#include "simtime.h"

NAMESPACE_BEGIN


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
 * @ingroup Internals
 */
class SIM_API cClassDescriptor : public cNoncopyableOwnedObject
{
  public:
    /// Field types.
    enum {
        FD_ISARRAY = 0x01,    ///< field is an array: int a[]; int a[10];
        FD_ISCOMPOUND = 0x02, ///< basic type (T) is struct or class: T a; T *a; T a[10]; T *a[]
        FD_ISPOINTER = 0x04,  ///< field is pointer or pointer array: T *a; T *a[]; T *a[10];
        FD_ISCOBJECT = 0x08,  ///< if ISCOMPOUND: basic type (T) subclasses from cObject
        FD_ISCOWNEDOBJECT = 0x10, ///< if ISCOMPOUND: basic type (T) subclasses from cOwnedObject
        FD_ISEDITABLE = 0x20, ///< whether field supports setFieldAsString()
        FD_NONE = 0x0
    };

  private:
    std::string baseclassname;
    cClassDescriptor *baseclassdesc;
    int inheritancechainlength;
    int extendscobject;

  protected:
    // utility functions for converting from/to strings
    static std::string long2string(long l);
    static long string2long(const char *s);
    static std::string ulong2string(unsigned long l);
    static unsigned long string2ulong(const char *s);
    static std::string int642string(int64 l);
    static int64 string2int64(const char *s);
    static std::string uint642string(uint64 l);
    static uint64 string2uint64(const char *s);
    static std::string bool2string(bool b);
    static bool string2bool(const char *s);
    static std::string double2string(double d);
    static std::string double2string(SimTime t) {return t.str();}
    static double string2double(const char *s);
    static std::string enum2string(long e, const char *enumname);
    static long string2enum(const char *s, const char *enumname);
    static std::string oppstring2string(const char *s) {return s?s:"";}
    static std::string oppstring2string(const opp_string& s) {return s.c_str();}
    static std::string oppstring2string(const std::string& s)  {return s;}
    static void string2oppstring(const char *s, opp_string& str) {str = s?s:"";}
    static void string2oppstring(const char *s, std::string& str) {str = s?s:"";}

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
     * descriptor for a base class, if there is no exact match.
     * The returned descriptor object is a singleton, and must not be deleted.
     */
    static cClassDescriptor *getDescriptorFor(cObject *object);
    //@}

    /** @name Querying and setting fields of the client object. */
    //@{

    /**
     * Returns true if this descriptor supports the given object's class.
     * If obj can be cast (dynamic_cast) to the class the descriptor supports,
     * the method should return true.
     */
    virtual bool doesSupport(cObject *obj) const {return false;}

    /**
     * Returns the descriptor for the base class, if available.
     */
    virtual cClassDescriptor *getBaseClassDescriptor() const;

    /**
     * Returns true if cObject's class descriptor is present on the inheritance chain.
     */
    bool extendsCObject() const;

    /**
     * Returns the number of base classes up to the root -- as far as
     * it is reflected in the descriptors.
     */
    int getInheritanceChainLength() const;

    /**
     * Returns the value of the given property of the descriptor as a single string.
     * Returns NULL if there is no such property. For structured property values
     * (with multiple keys and/or list(s) inside), the value is returned as a
     * single unparsed string.
     */
    virtual const char *getProperty(const char *propertyname) const = 0;

    /**
     * Must be redefined in subclasses to return the number of fields
     * in the client object.
     */
    virtual int getFieldCount(void *object) const = 0;

    /**
     * Must be redefined in subclasses to return the name of a field
     * in the client object.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     */
    virtual const char *getFieldName(void *object, int field) const = 0;

    /**
     * Returns the index of the field with the given name, or -1 if not found.
     * cClassDescriptor provides an default implementation, but it is
     * recommended to replace it in subclasses with a more efficient version.
     */
    virtual int findField(void *object, const char *fieldName) const;

    /**
     * Must be redefined in subclasses to return the type flags of a field
     * in the client object. Flags is a binary OR of the following:
     * FD_ISARRAY, FD_ISCOMPOUND, FD_ISPOINTER, FD_ISCOWNEDOBJECT, FD_ISCOBJECT.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     */
    virtual unsigned int getFieldTypeFlags(void *object, int field) const = 0;

    /** @name Utility functions based on getFieldTypeFlags() */
    //@{
    bool getFieldIsArray(void *object, int field) const {return getFieldTypeFlags(object, field) & FD_ISARRAY;}
    bool getFieldIsCompound(void *object, int field) const {return getFieldTypeFlags(object, field) & FD_ISCOMPOUND;}
    bool getFieldIsPointer(void *object, int field) const {return getFieldTypeFlags(object, field) & FD_ISPOINTER;}
    bool getFieldIsCObject(void *object, int field) const {return getFieldTypeFlags(object, field) & (FD_ISCOBJECT|FD_ISCOWNEDOBJECT);}
    bool getFieldIsCOwnedObject(void *object, int field) const {return getFieldTypeFlags(object, field) & FD_ISCOWNEDOBJECT;}
    bool getFieldIsEditable(void *object, int field) const {return getFieldTypeFlags(object, field) & FD_ISEDITABLE;}
    //@}

    /**
     * Returns the name of the class on which the given field was declared.
     */
    virtual const char *getFieldDeclaredOn(void *object, int field) const;

    /**
     * Must be redefined in subclasses to return the type of a field
     * in the client object as a string.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     */
    virtual const char *getFieldTypeString(void *object, int field) const = 0;

    /**
     * Returns the value of the given property of the field as a single string.
     * Returns NULL if there is no such property. For structured property values
     * (with multiple keys and/or list(s) inside), the value is returned as a
     * single unparsed string.
     */
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const = 0;

    /**
     * Must be redefined in subclasses to return the array size of a field
     * in the client object. If the field is not an array, it should return 0.
     */
    virtual int getArraySize(void *object, int field) const = 0;

    /**
     * Must be redefined in subclasses to return the value of the given field
     * in the client object as a string. For compound fields, the message
     * compiler generates code which calls operator<<.
     */
    virtual std::string getFieldAsString(void *object, int field, int i) const = 0;

    /**
     * DEPRECATED: Use the getFieldAsString(void*, int, int) instead.
     */
    _OPPDEPRECATED virtual bool getFieldAsString(void *object, int field, int i, char *buf, int bufsize) const;

    /**
     * Must be redefined in subclasses to set the value of a field
     * in the client object from the given value string.
     * Returns true if successful, and false if an error occurred or the
     * field does not support setting.
     */
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const = 0;

    /**
     * Must be redefined in subclasses to return the type name of a compound field
     * in the client object. If it is a pointer, the "*" is removed. The return value
     * may be used then as classname to create a descriptor object for this compound field.
     */
    virtual const char *getFieldStructName(void *object, int field) const = 0;

    /**
     * Must be redefined in subclasses to return the pointer of a compound field
     * in the client object.
     */
    virtual void *getFieldStructPointer(void *object, int field, int i) const = 0;
    //@}
};

NAMESPACE_END


#endif


