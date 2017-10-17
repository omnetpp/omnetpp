//==========================================================================
//  MSGTYPETABLE.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_NEDXML_MSGTYPETABLE_H
#define __OMNETPP_NEDXML_MSGTYPETABLE_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "nedelements.h"
#include "nederror.h"

namespace omnetpp {
namespace nedxml {

/**
 * Holds types from msg files.
 *
 * @ingroup CppGenerator
 */
class NEDXML_API MsgTypeTable
{
  public:
    typedef std::vector<std::string> StringVector;

    enum class ClassType {
        UNKNOWN = 0,
        STRUCT,       // struct
        NONCOBJECT,   // class not derived from cObject
        COBJECT,      // class derived from cObject
        COWNEDOBJECT, // class derived from cOwnedObject
        CNAMEDOBJECT  // class derived from cNamedObject
    };

    struct TypeDesc
    {
        const char *nedTypeName;
        const char *cppTypeName;
        const char *fromstring;
        const char *tostring;
        const char *emptyValue;
    };

    static TypeDesc _PRIMITIVE_TYPES[];
    typedef std::map<std::string,TypeDesc> TypeDescMap;
    TypeDescMap PRIMITIVE_TYPES;

    std::map<std::string,ClassType> classType;
    std::map<std::string,std::string> enumType;

    typedef std::map<std::string, std::string> Properties;  //FIXME kell egy reszletesebb modell...

    class FieldInfo {
      public:
        NEDElement *nedElement;     // pointer to NED element

        std::string fname;      // field name in MSG
        std::string ftype;      // field type in MSG
        std::string ftypeqname; // fully qualified C++ name of type //TODO should not be needed
        std::string fval;       // value (or empty)
        bool fisabstract;
        bool fispointer;
        bool fisownedpointer;   // true, when should use dup()/delete/take()/drop()/dropAndDelete(); read from @owned, default value is true for cOwnedObject, otherwise false
                                // "T *removeFoo()" also generated for owned pointer members
        bool fisarray;
        bool byvalue;           // @byvalue, default value is false        // TODO: @byvalue should rather be the attribute of the field's type, not the field itself
        std::string farraysize; // array size in MSG, maybe empty for dynamic arrays
        Properties fprops;      // field properties (name, first value of default key)

        // data needed for code generation
        bool fisprimitivetype;  // whether primitive or compound type (TODO merge into ClassType?)
        ClassType classtype;    // cobject/cnamedobject/cownedobject/...
        std::string datatype;   // member C++ datatype
        std::string argtype;    // setter C++ argument type
        std::string rettype;    // getter C++ return type
        std::string var;        // name of data member variable
        std::string argname;    // setter argument name
        std::string varsize;    // data member to store array size | value of farraysize
        std::string fsizetype;  // type for storing array size
        std::string getter;     // getter function name:  "T getter() const;" "const T& getter() const"  default value is getFoo
        std::string mGetter;    // const getter function name:  "T& mGetter();" default value is the value of getter, @mgetter
        std::string remover;    // remover function name (for owned pointers)
        std::string setter;     // Setter function name
        std::string alloc;      // setArraySize() function name
        std::string getsize;    // array size getter function name
        std::string tostring;   // function to convert data to string, defined in property @tostring, default values for primitive types found in MsgCppGenerator::TypeMap MsgCppGenerator::PRIMITIVE_TYPES
                                // if tostring begins a dot, then uses as member function, parentheses needed in property; otherwise tostring implemented as name of a normal function, do not use parentheses
                                // std::string <function>(<datatype>);           // @tostring(function)
                                // std::string <function>(const <datatype>&);    // @tostring(function)
                                // const char * <function>(<datatype>);          // @tostring(function)
                                // const char * <function>(const <datatype>&);   // @tostring(function)
                                // std::string <datatype>::<function>(...);      // @tostring(.function(...))
                                // const char * <datatype>::<function>(...);     // @tostring(.function(...))
        std::string fromstring; // function to convert string to data member, defined in property @fromstring, default values for primitive types found in MsgCppGenerator::TypeMap MsgCppGenerator::PRIMITIVE_TYPES
                                // <datatype> <function>(const char *);          // @fromstring(function)
        std::string maybe_c_str;       // uses ".c_str()"
        std::string enumname;
        std::string enumqname;
        bool fnopack;           // @nopack(true)
        bool feditable;         // @editable(true)
        bool editNotDisabled;   // true when field doesn't have property "@editable(false)"
        bool fopaque;         // @opaque(true)        // TODO: @opaque should rather be the attribute of the field's type, not the field itself
        bool overrideGetter;   // @overridegetter|@override, uses when field getter function overrides a function in base class
        bool overrideSetter;   // @overridesetter|@override, uses when field setter function overrides a function in base class

      public:
        FieldInfo() : nedElement(nullptr), fisabstract(false), fispointer(false), fisarray(false), classtype(ClassType::UNKNOWN), fnopack(false), feditable(false),fopaque(false) {}
    };

    class ClassInfo {
      public:
        typedef std::vector<FieldInfo> Fieldlist;

        NEDElement *nedElement;
        std::string keyword;        // struct/class/packet from MSG
        std::string msgname;        // class name from MSG
        std::string msgbase;        // base class name from MSG
        Properties props;           // class properties

        bool gap;                   // true if @customize
        bool omitgetverb;
        ClassType classtype;
        std::string msgclass;
        std::string realmsgclass;
        std::string msgbaseclass;
        std::string msgdescclass;
        std::string fieldnamesuffix; // e.g. "_var"
        Fieldlist fieldlist;        // list of fields
        Fieldlist baseclassFieldlist;   //modified baseclass fields, e.g. baseclass.basefield = value

        bool generate_class;
        bool generate_descriptor;
        bool generate_setters_in_descriptor;

        //TODO kellenek ezek?
        std::string msgqname;
        std::string msgbaseqname;

        StringVector implements;    //value vector from @implements

      public:
        ClassInfo() : nedElement(nullptr), gap(false), omitgetverb(false), classtype(ClassType::UNKNOWN),
              generate_class(true), generate_descriptor(true), generate_setters_in_descriptor(true) {}
    };

    class EnumItem
    {
      public:
        NEDElement *nedElement;
        std::string name;
        std::string value;
        std::string comment;
      public:
        EnumItem() : nedElement(nullptr) {}
    };

    class EnumInfo
    {
      public:
        EnumElement *nedElement;
        std::string enumName;
        std::string enumQName;
        typedef std::vector<EnumItem> FieldList;
        FieldList fieldlist;
      public:
        EnumInfo() : nedElement(nullptr) {}
    };

  protected:
    void initDescriptors();
    std::string prefixWithNamespace(const std::string& namespaceName, const std::string& name) {
        return !namespaceName.empty() ? namespaceName + "::" + name : name;
    }

  public:
    MsgTypeTable() {initDescriptors();}
    StringVector lookupExistingClassName(const std::string& name, const std::string& contextNamespace);
    StringVector lookupExistingEnumName(const std::string& name, const std::string& contextNamespace);
    bool isClassDeclared(const std::string& classqname) { return classType.find(classqname) != classType.end(); }
    void addClassType(const std::string& classqname, ClassType type, NEDElement *context);
    ClassType getClassType(const std::string& classqname);
};

} // namespace nedxml
}  // namespace omnetpp


#endif


