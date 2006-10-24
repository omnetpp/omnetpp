//==========================================================================
//  CSTRUCT.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cStructDescriptor  : meta-info about structures
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSTRUCT_H
#define __CSTRUCT_H

#include <string>
#include "cobject.h"

class sFieldWrapper;


/**
 * Abstract base class for structure description classes, used mainly
 * with message subclassing.
 *
 * Subclasses of cStructDescriptor encapsulate the kind of reflection
 * information (in the Java sense) which is needed by Tkenv to display
 * fields in a message, struct or object created with the .msg syntax.
 * The cStructDescriptor subclass is generated along with the message class,
 * (struct, object, etc.).
 *
 * When Tkenv encounters a message object, it creates an appropriate
 * cStructDescriptor object and uses that to find out what fields the
 * message object has, what are their values etc. The message object
 * is said to be the `client object' of the cStructDescriptor object.
 *
 * In this class, the copy constructor, dup() and the assignment operator
 * are redefined to raise an error (throw cRuntimeError), since they would
 * be of no use in subclasses.
 *
 * @ingroup Internals
 * @see sFieldWrapper
 */
class SIM_API cStructDescriptor : public cNoncopyableObject
{
  public:
    /// Field types.
    enum {
        FT_BASIC,         ///< int, long, double, bool, char*, char[]
        FT_STRUCT,        ///< embedded structure, for which there's another cStructDescriptor
        FT_BASIC_ARRAY,   ///< array of FT_BASIC
        FT_STRUCT_ARRAY,  ///< array of FT_STRUCT
        FT_INVALID        ///< invalid type (signals error condition)
    };

  private:
    std::string baseclassname;
    cStructDescriptor *baseclassdesc;
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
    cStructDescriptor(const char *classname, const char *_baseclassname=NULL);

    /**
     * Destructor.
     */
    virtual ~cStructDescriptor();
    //@}

    /** @name Getting descriptor for an object or a struct. */
    //@{

    /**
     * Returns the descriptor object for the given class. The returned
     * descriptor object is a singleton, and must not be deleted.
     */
    static cStructDescriptor *getDescriptorFor(const char *classname);

    /**
     * Returns the descriptor object for the given object. This can return
     * descriptor for a base class, if there isn't an exact match.
     * The returned descriptor object is a singleton, and must not be deleted.
     */
    static cStructDescriptor *getDescriptorFor(cPolymorphic *object);
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
    virtual cStructDescriptor *getBaseClassDescriptor();

    /**
     * Returns the number of base classes up to the root -- as far as
     * it's reflected in the descriptors.
     */
    int getInheritanceChainLength();

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
     * Must be redefined in subclasses to return the type of a field
     * in the client object.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     * The returned field type is one of the FT_BASIC, FT_STRUCT,
     * FT_BASIC_ARRAY, FT_STRUCT_ARRAY constants, or FT_INVALID if there's
     * no such field.
     */
    //FIXME replace by 2 booleans: isArray, isStruct
    virtual int getFieldType(void *object, int field) = 0;

    /**
     * Must be redefined in subclasses to return the type of a field
     * in the client object as a string.
     * The argument must be in the 0..getFieldCount()-1 range, inclusive.
     */
    virtual const char *getFieldTypeString(void *object, int field) = 0;

    /**
     * Returns the enum name associated with the field. This makes only
     * sense with integer-type fields (short, int, long, etc.).
     * Returns NULL if there's no associated enum.
     *
     * @see cEnum
     */
    virtual const char *getFieldEnumName(void *object, int field) = 0;

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
     * Must be redefined in subclasses to return the value of a basic field
     * (of type FT_BASIC(_ARRAY)) in the client object as a string.
     * It is an error if this method is invoked for non-basic fields.
     * Returns true if no error occurred, false otherwise.
     */
    virtual bool getFieldAsString(void *object, int field, int i, char *buf, int bufsize) = 0;

    /**
     * Must be redefined in subclasses to set the value of a basic field
     * (of type FT_BASIC(_ARRAY)) in the client object as a string.
     * It is an error if this method is invoked for non-basic fields.
     * Returns true if no error occurred, false otherwise.
     */
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) = 0;

    /**
     * Must be redefined in subclasses to return the type name of an
     * FT_STRUCT(_ARRAY) field in the client object.
     * The return value may be used then as classname
     * to create a descriptor object for this structure field.
     */
    virtual const char *getFieldStructName(void *object, int field) = 0;

    /**
     * Must be redefined in subclasses to return the pointer of an
     * FT_STRUCT(_ARRAY) field in the client object.
     * The return value may be used then as client object pointer
     * to create a descriptor object for this structure field.
     */
    virtual void *getFieldStructPointer(void *object, int field, int i) = 0;
    //@}
};

#endif


