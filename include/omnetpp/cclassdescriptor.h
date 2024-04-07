//==========================================================================
//  CCLASSDESCRIPTOR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CCLASSDESCRIPTOR_H
#define __OMNETPP_CCLASSDESCRIPTOR_H

#include <string>
#include "cownedobject.h"
#include "simtime.h"
#include "opp_string.h"
#include "any_ptr.h"

namespace omnetpp {

class cValue;

/**
 * @brief Abstract base class for class descriptors.
 *
 * Subclasses of cClassDescriptor encapsulate reflection information
 * (in the Java sense) which is needed by e.g. Qtenv to display object
 * contents. cClassDescriptor subclasses are normally generated
 * by the message compiler from MSG files.
 *
 * @ingroup Fundamentals
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
        FD_ISEDITABLE = 0x20,     ///< whether field supports setFieldValueAsString()
        FD_ISREPLACEABLE = 0x40,  ///< whether field supports setFieldStructValuePointer()
        FD_ISRESIZABLE = 0x80,    ///< whether field supports setFieldArraySize()
        FD_NONE = 0x0
    };

  private:
    std::string baseClassName;
    cClassDescriptor *baseClassDesc = nullptr;
    int inheritanceChainLength = 1;
    int extendscObject = -1;  // 0:false, 1:true, -1:unset

  protected:
    // utility functions for converting from/to strings
    static std::string long2string(long l);
    static long string2long(const char *s);
    static std::string ulong2string(unsigned long l);
    static unsigned long string2ulong(const char *s);
    static std::string int642string(int64_t l);
    static int64_t string2int64(const char *s);
    static std::string uint642string(uint64_t l);
    static uint64_t string2uint64(const char *s);
    static std::string bool2string(bool b);
    static bool string2bool(const char *s);
    static std::string double2string(double d);
    static double string2double(const char *s);
    static std::string simtime2string(SimTime t) {return t.ustr();}
    static simtime_t string2simtime(const char *s) {return SimTime::parse(s);}
    std::string enum2string(int e, const char *enumName) const;
    intval_t string2enum(const char *s, const char *enumName) const;
    static std::string oppstring2string(const char *s) {return s?s:"";}
    static std::string oppstring2string(const opp_string& s) {return s.c_str();}
    static std::string oppstring2string(const std::string& s)  {return s;}
    static void string2oppstring(const char *s, opp_string& str) {str = s?s:"";}
    static void string2oppstring(const char *s, std::string& str) {str = s?s:"";}
    static const char **mergeLists(const char **list1, const char **list2);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    cClassDescriptor(const char *className, const char *baseClassName=nullptr);

    /**
     * Destructor.
     */
    virtual ~cClassDescriptor() {}
    //@}

    /** @name Getting descriptor for an object or a struct. */
    //@{

    /**
     * Returns the descriptor object for the given class. The returned
     * descriptor object must not be deleted.
     */
    static cClassDescriptor *getDescriptorFor(const char *classname);

    /**
     * Returns the descriptor object for the given object. This can return
     * descriptor for a base class, if there is no exact match.
     * The returned descriptor object must not be deleted.
     */
    static cClassDescriptor *getDescriptorFor(const cObject *object);
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
     * Returns the namespace of this object (getFullName() minus trailing "::" + getName()).
     */
    std::string getNamespace() const;

    /**
     * Returns the number of base classes up to the root, as reflected in the
     * descriptors (see getBaseClassDescriptor()).
     */
    int getInheritanceChainLength() const;

    /**
     * Returns a NUL-terminated string array with the names of properties on this
     * class. The list includes the properties of the ancestor classes as well.
     */
    virtual const char **getPropertyNames() const = 0;

    /**
     * Returns the value of the given property of the descriptor as a single string.
     * Returns nullptr if there is no such property. For structured property values
     * (with multiple keys and/or list(s) inside), the value is returned as a
     * single unparsed string. If a property appears in a base class and its
     * subclass too, the subclass's property will be returned.
     */
    virtual const char *getProperty(const char *propertyname) const = 0;

    /**
     * Returns the number of fields in the described class.
     */
    virtual int getFieldCount() const = 0;

    /**
     * Returns the name of a field in the described class.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     */
    virtual const char *getFieldName(int field) const = 0;

    /**
     * Returns the index of the field with the given name, or -1 if not found.
     * cClassDescriptor provides an default implementation, but it is
     * recommended to replace it in subclasses with a more efficient version.
     */
    virtual int findField(const char *fieldName) const;

    /**
     * Returns the type flags of a field in the described class. Flags is a
     * binary OR of the following: FD_ISARRAY, FD_ISCOMPOUND, FD_ISPOINTER,
     * FD_ISCOWNEDOBJECT, FD_ISCOBJECT.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     */
    virtual unsigned int getFieldTypeFlags(int field) const = 0;

    /** @name Utility functions based on getFieldTypeFlags() */
    //@{
    bool getFieldIsArray(int field) const {return getFieldTypeFlags(field) & FD_ISARRAY;}
    bool getFieldIsCompound(int field) const {return getFieldTypeFlags(field) & FD_ISCOMPOUND;}
    bool getFieldIsPointer(int field) const {return getFieldTypeFlags(field) & FD_ISPOINTER;}
    bool getFieldIsCObject(int field) const {return getFieldTypeFlags(field) & (FD_ISCOBJECT|FD_ISCOWNEDOBJECT);}
    bool getFieldIsCOwnedObject(int field) const {return getFieldTypeFlags(field) & FD_ISCOWNEDOBJECT;}
    bool getFieldIsEditable(int field) const {return getFieldTypeFlags(field) & FD_ISEDITABLE;}
    bool getFieldIsReplaceable(int field) const {return getFieldTypeFlags(field) & FD_ISREPLACEABLE;}
    bool getFieldIsResizable(int field) const {return getFieldTypeFlags(field) & FD_ISRESIZABLE;}
    //@}

    /**
     * Returns the name of the class on which the given field was declared.
     */
    virtual const char *getFieldDeclaredOn(int field) const;

    /**
     * Returns the declared type of a field in the described class as a string.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     *
     * @see getFieldDynamicTypeString()
     */
    virtual const char *getFieldTypeString(int field) const = 0;

    /**
     * Returns a nullptr-terminated string array with the names of the given
     * field's properties.
     */
    virtual const char **getFieldPropertyNames(int field) const = 0;

    /**
     * Returns the value of the given property of the given field in the
     * described class as a single string. Returns nullptr if there is no such
     * property. For structured property values (with multiple keys and/or
     * list(s) inside), the value is returned as a single unparsed string.
     */
    virtual const char *getFieldProperty(int field, const char *propertyname) const = 0;

    /**
     * Returns the array size of a field in the given object. If the field is
     * not an array, it returns 0.
     */
    virtual int getFieldArraySize(any_ptr object, int field) const = 0;

    /**
     * Sets the array size of a field in the given object.
     * If the operation is not successful, an exception is thrown.
     *
     * The field argument must be in the 0..getFieldCount()-1 range.
     * The i argument must be in the 0..getFieldArraySize()-1 range, or
     * 0 if the field is not an array.
     */
    virtual void setFieldArraySize(any_ptr object, int field, int size) const = 0;

    /**
     * Returns the runtime type of the given field as a string if it can be
     * determined. (This method is relevant if the field contains a pointer or
     * or an array of pointers to objects with RTTI.) If a runtime type is
     * not available, nullptr is returned, and the caller should use the
     * declared type instead (getFieldTypeString()).
     *
     * The field argument must be in the 0..getFieldCount()-1 range.
     * The i argument must be in the 0..getFieldArraySize()-1 range, or
     * 0 if the field is not an array.
     */
    virtual const char *getFieldDynamicTypeString(any_ptr object, int field, int i) const {return nullptr;}

    /**
     * Returns the value of the given field in the given object as a string.
     * For compound fields, the message compiler generates code which calls operator<<.
     *
     * The field argument must be in the 0..getFieldCount()-1 range.
     * The i argument must be in the 0..getFieldArraySize()-1 range, or
     * 0 if the field is not an array.
     */
    virtual std::string getFieldValueAsString(any_ptr object, int field, int i) const = 0;

    /**
     * Sets the value of a field in the given object by parsing the given value string.
     * If the operation is not successful, an exception is thrown.
     *
     * The field argument must be in the 0..getFieldCount()-1 range.
     * The i argument must be in the 0..getFieldArraySize()-1 range, or
     * 0 if the field is not an array.
     */
    virtual void setFieldValueAsString(any_ptr object, int field, int i, const char *value) const = 0;

    /**
     * Returns the value of the given field in the given object.
     *
     * The field argument must be in the 0..getFieldCount()-1 range.
     * The i argument must be in the 0..getFieldArraySize()-1 range, or
     * 0 if the field is not an array.
     */
    virtual cValue getFieldValue(any_ptr object, int field, int i) const = 0;

    /**
     * Sets the value of a field in the given object. If the operation is not
     * successful, an exception is thrown.
     *
     * The field argument must be in the 0..getFieldCount()-1 range.
     * The i argument must be in the 0..getFieldArraySize()-1 range, or
     * 0 if the field is not an array.
     */
    virtual void setFieldValue(any_ptr object, int field, int i, const cValue& value) const = 0;

    /**
     * Returns the declared type name of a compound field in the described class.
     * The field is a pointer, the "*" is removed. The return value may be used
     * as class name to obtain a class descriptor for this compound field.
     */
    virtual const char *getFieldStructName(int field) const = 0;

    /**
     * Returns the pointer to the value of a compound field in the given object.
     * The field argument must be in the 0..getFieldCount()-1 range.
     * The i argument must be in the 0..getFieldArraySize()-1 range, or
     * 0 if the field is not an array.
     */
    virtual any_ptr getFieldStructValuePointer(any_ptr object, int field, int i) const = 0;

    /**
     * Sets a pointer field in the given object.
     * If the operation is not successful, an exception is thrown.
     *
     * The field argument must be in the 0..getFieldCount()-1 range.
     * The i argument must be in the 0..getFieldArraySize()-1 range, or
     * 0 if the field is not an array.
     */
    virtual void setFieldStructValuePointer(any_ptr object, int field, int i, any_ptr ptr) const = 0;
    //@}
};

}  // namespace omnetpp


#endif


