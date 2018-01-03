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

#include "nedxmldefs.h"

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

    class Property
    {
      protected:
        ASTNode *astNode = nullptr;       // pointer to property element in AST
        std::string name;
        std::string index;
        std::map<std::string, StringVector> propertyValue;
      public:
        Property() {}
        Property(const std::string& name, const std::string& index, ASTNode *astNode) : astNode(astNode), name(name), index(index) {}
        Property(const Property& other) : astNode(other.astNode), name(other.name), index(other.index), propertyValue(other.propertyValue) {}
        const std::string& getName() const {return name;}
        const std::string& getIndex() const {return index;}
        std::string getIndexedName() const;
        ASTNode *getASTNode() const {return astNode;}
        StringVector getValue(const std::string& key="") const;
        std::string getValueAsString() const;
        void addValue(const std::string& key, const std::string& value);
    };

    class Properties
    {
      protected:
        std::vector<Property> properties;
      public:
        void add(const Property& p) {properties.push_back(p);}
        bool empty() const {return properties.empty();}
        const std::vector<Property>& getAll() const {return properties;}
        const Property *get(const std::string& name, const std::string& index="") const;
        bool contains(const std::string& name, const std::string& index="") const {return get(name, index) != nullptr;}
    };

    class FieldInfo {
      public:
        ASTNode *astNode;       // pointer to field element in AST

        std::string name;       // field name in MSG
        std::string type;       // field type in MSG, without 'const' and '*' modifiers
        std::string typeQName;  // fully qualified C++ name of type
        std::string value;      // value (or empty). for arrays, this is the value for one array element (we have no syntax for initializing with a list of values)
        std::string symbolicConstant; // symbolic name for local field index in the generated class descriptor
        bool isAbstract;        // "abstract" keyword specified for field
        bool isConst;           // "const" keyword specified for field
        bool byValue;           // @byValue(true); whether value should be passed by value (instead of by reference) in setters/getters
        bool isPointer;         // field is a pointer or pointer array ("*" syntax)
        bool isOwnedPointer;    // from @owned; if true, field is a pointer or pointer array, and allocated memory is owned by the object (needs to be duplicated in dup(), and deleted in destructor).
                                // if field type is also cOwnedObject, take()/drop() calls should be generated
        bool isArray;           // field is an array ("[]" or "[size]" syntax)
        bool isDynamicArray;    // if field is a dynamic array
        bool isFixedArray;      // if field is a fixed-size array
        std::string arraySize;  // if field is an array: array size (string inside the square brackets)
        Properties props;       // field properties (name, first value of default key)

        // data needed for code generation
        bool isClass;           // field type is a class (true) or struct (false)
        bool iscObject;         // field type is derived from cObject
        bool iscNamedObject;    // field type is derived from cNamedObject
        bool iscOwnedObject;    // field type is derived from cOwnedObject
        std::string dataType;   // member C++ datatype
        std::string argType;    // setter C++ argument type
        std::string returnType; // getter C++ return type
        std::string mutableReturnType; // mutableGetter C++ return type
        std::string var;        // name of data member variable
        std::string argName;    // setter argument name
        std::string sizeVar;    // data member to store size of dynamic array
        std::string sizeType;   // type of array sizes and array indices
        std::string getter;     // getter function name:  "T getter() const;" "const T& getter() const"  default value is getFoo
        std::string getterForUpdate; // mutable getter function name:  "T& getterForUpdate();" default value is getFooForUpdate
        bool hasGetterForUpdate; // whether a getterForUpdate method needs to be generated
        bool allowReplace;      // @allowReplace; whether setter of an owned pointer field is allowed to delete an already-set object
        std::string dropper;    // dropper function name (for owned pointers: drop pointer from class and return to caller)
        std::string clone;      // @clone; code to duplicate (one array element of) the field (for owned pointers)
        std::string setter;     // @setter; setter function name
        std::string inserter;   // @inserter; inserter function name (insert element to dynamic array)
        std::string eraser;     // @eraser; eraser function name (erase element from dynamic array)
        std::string sizeSetter; // setArraySize() function name
        std::string sizeGetter; // array size getter function name
        std::string toString;   // function to convert data to string, defined in property @toString
                                // if tostring begins with a dot, then it is taken as member function call, parentheses needed in property; otherwise toString is understood as name of a normal function, do not use parentheses
                                // std::string <function>(<datatype>);           // @toString(function)
                                // std::string <function>(const <datatype>&);    // @toString(function)
                                // const char * <function>(<datatype>);          // @toString(function)
                                // const char * <function>(const <datatype>&);   // @toString(function)
                                // std::string <datatype>::<function>(...);      // @toString(.function(...))
                                // const char * <datatype>::<function>(...);     // @toString(.function(...))
        std::string fromString; // function to convert string to data member, defined in property @fromString
                                // <datatype> <function>(const char *);          // @fromString(function)
        std::string getterConversion;  // currently only with strings: ".c_str()"
        std::string enumName;   // from @enum
        std::string enumQName;  // fully qualified type name of enum
        bool nopack;            // @nopack(true)
        bool isEditable;        // @editable(true): field value is editable in the UI via the descriptor's setFieldValueFromString() method
        bool isOpaque;          // @opaque(true), means that field type is treated as atomic (has no fields), i.e. has no descriptor
        bool overrideGetter;    // @overrideGetter|@override, used when field getter function overrides a function in base class
        bool overrideSetter;    // @overrideSetter|@override, used when field setter function overrides a function in base class
    };

    class ClassInfo {
      public:
        typedef std::vector<FieldInfo> Fieldlist;

        ASTNode *astNode = nullptr;
        std::string keyword;        // struct/class/packet from MSG
        std::string name;           // class name from MSG
        std::string qname;          // qualified name from MSG (namespace :: name)
        Properties props;           // class properties

        bool classInfoComplete = false;  // whether following fields are filled in
        bool classBeingAnalyzed = false;
        bool fieldsComplete = false;   // whether fieldList and baseclassFieldlist are filled in
        bool fieldsBeingAnalyzed = false;

        std::string extendsQName;      // fully qualified name of base type
        std::string extendsName;       // base type's name from MSG
        bool customize;                // from @customize
        bool omitGetVerb;              // from @omitGetVerb
        bool isClass;                  // true=class, false=struct
        bool iscObject;                // whether type is subclassed from cObject
        bool iscNamedObject;           // whether type is subclassed from cNamedObject
        bool iscOwnedObject;           // whether type is subclassed from cOwnedObject
        bool isEnum = false;
        bool isImported;               // whether this type imported from an another msg file
        bool subclassable;             // whether this type can be subclasses (e.g. "int" or final classes cannot)
        bool supportsPtr;              // whether type supports creating a pointer (or pointer array) from it
        std::string namespaceName;
        std::string className;         // name of actual C++ class or struct generated (<name>_Base when @customize is present)
        std::string realClass;         // name of intended C++ class, usually the same as <name>
        std::string baseClass;         // C++ base class
        std::string descriptorClass;   // name of C++ descriptor class to be generated
        std::string fieldNameSuffix;   // member variable suffix, e.g. "_var" or "_m"

        Fieldlist fieldList;           // list of fields
        Fieldlist baseclassFieldlist;  // assignments to fields declared in super classes

        bool generateClass = true;
        bool generateDescriptor = true;
        bool generateSettersInDescriptor = true;

        StringVector implements;       // values from @implements
        std::string beforeChange;      // @beforeChange; method to be called before mutator methods
        std::string str;               // @str; expression to be returned from str() method

        // The following members describe how the class should behave when instantiated as field
        std::string defaultValue;      // default value (or empty)
        bool isOpaque;                 // from @opaque
        bool byValue;                  // from @byValue, default value is false
        bool isEditable;               // from @editable
        std::string dataTypeBase;      // member C++ datatype
        std::string argTypeBase;       // setter C++ argument type
        std::string returnTypeBase;    // getter C++ return type
        std::string toString;          // function to convert data to string, defined in property @toString
        std::string fromString;        // @fromString; function to convert string to data member, defined in property @fromString
        std::string clone;             // @clone; code to clone a dynamically allocated object of this type (for owned pointer fields)
        std::string getterConversion;  // @getterConversion; conversion from storage type to return type in getters
    };

    class EnumItem
    {
      public:
        ASTNode *astNode;
        std::string name;
        std::string value;
        std::string comment;
    };

    class EnumInfo
    {
      public:
        ASTNode *astNode;
        std::string enumName;
        std::string enumQName;
        typedef std::vector<EnumItem> FieldList;
        bool isDeclaration = false;   // i.e. not a definition
        FieldList fieldList;
    };

  private:
    Properties globalProperties;
    std::map<std::string,ClassInfo> definedClasses;
    std::map<std::string,EnumInfo> definedEnums;
    std::vector<ASTNode*> importedMsgFiles;

  protected:
    void initDescriptors();
    std::string prefixWithNamespace(const std::string& namespaceName, const std::string& name) {
        return !namespaceName.empty() ? namespaceName + "::" + name : name;
    }

  public:
    MsgTypeTable() {initDescriptors();}
    ~MsgTypeTable();
    StringVector lookupExistingEnumName(const std::string& name, const std::string& contextNamespace);
    bool isClassDefined(const std::string& classqname) { return definedClasses.find(classqname) != definedClasses.end(); }
    bool isEnumDefined(const std::string& enumqname) { return definedEnums.find(enumqname) != definedEnums.end(); }
    ClassInfo *findClassInfo(const std::string& classqname);
    ClassInfo& getClassInfo(const std::string& classqname);
    const EnumInfo& getEnumInfo(const std::string& qname);
    const Properties& getGlobalProperties() const {return globalProperties;}
    void storeMsgFile(ASTNode *tree) {importedMsgFiles.push_back(tree);}
    void addGlobalProperty(const Property& p) {globalProperties.add(p);}
    void addClass(const ClassInfo& classInfo) {definedClasses[classInfo.qname] = classInfo;} // TODO assert not already there
    void addEnum(const EnumInfo& enumInfo) {definedEnums[enumInfo.enumQName] = enumInfo;}  //TODO assert not already there
};

} // namespace nedxml
}  // namespace omnetpp


#endif


