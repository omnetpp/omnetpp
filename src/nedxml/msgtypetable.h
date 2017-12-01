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

    typedef std::map<std::string, std::string> Properties;

    class FieldInfo {
      public:
        ASTNode *astNode;     // pointer to field element in AST

        std::string fname;      // field name in MSG
        std::string ftype;      // field type in MSG, without 'const' and '*' modifiers
        std::string ftypeqname; // fully qualified C++ name of type //TODO should not be needed
        std::string fval;       // value (or empty). for arrays, this is the value for one array element (we have no syntax for initializing with a list of values)
        bool fisabstract;       // "abstract" keyword specified for field
        bool fisconst;          // "const" keyword specified for field
        bool fispointer;        // field is a pointer or pointer array ("*" syntax)
        bool fisownedpointer;   // from @owned; if true, allocated memory is owned by the object (needs to be duplicated in dup(), and deleted in destructor).
                                // if field type is also cOwnedObject, take()/drop() calls should be generated
        bool fisarray;          // field is an array ("[]" or "[size]" syntax)
        bool byvalue;           // @byValue(true); whether value should be passed by value (instead of by reference) in setters/getters
        std::string farraysize; // if field is an array: array size (string inside the square brackets)
        Properties fprops;      // field properties (name, first value of default key)

        // data needed for code generation
        bool isClass;           // field type is a class (true) or struct (false)
        bool iscObject;         // field type is derived from cObject
        bool iscNamedObject;    // field type is derived from cNamedObject
        bool iscOwnedObject;    // field type is derived from cOwnedObject
        std::string datatype;   // member C++ datatype
        std::string argtype;    // setter C++ argument type
        std::string rettype;    // getter C++ return type
        std::string mutablerettype; // mutableGetter C++ return type
        std::string var;        // name of data member variable
        std::string argname;    // setter argument name
        std::string varsize;    // data member to store array size | value of farraysize
        std::string fsizetype;  // type for storing array size
        std::string getter;     // getter function name:  "T getter() const;" "const T& getter() const"  default value is getFoo
        std::string mGetter;    // mutable getter function name:  "T& mGetter();" default value is the value of getter, @mutableGetter
        bool hasMutableGetter;  // whether a mutableGetter method needs to be generated
        std::string remover;    // remover function name (for owned pointers)
        std::string setter;     // Setter function name
        std::string alloc;      // setArraySize() function name
        std::string getsize;    // array size getter function name
        std::string tostring;   // function to convert data to string, defined in property @toString
                                // if tostring begins with a dot, then it is taken as member function call, parentheses needed in property; otherwise toString is understood as name of a normal function, do not use parentheses
                                // std::string <function>(<datatype>);           // @toString(function)
                                // std::string <function>(const <datatype>&);    // @toString(function)
                                // const char * <function>(<datatype>);          // @toString(function)
                                // const char * <function>(const <datatype>&);   // @toString(function)
                                // std::string <datatype>::<function>(...);      // @toString(.function(...))
                                // const char * <datatype>::<function>(...);     // @toString(.function(...))
        std::string fromstring; // function to convert string to data member, defined in property @fromString
                                // <datatype> <function>(const char *);          // @fromString(function)
        std::string getterconversion;  // currently only with strings: ".c_str()"
        std::string enumname;   // from @enum
        std::string enumqname;  // fully qualified type name of enum
        bool fnopack;           // @nopack(true)
        bool feditable;         // @editable(true): field value is editable in the UI via the descriptor's setFieldValueFromString() method
        bool editNotDisabled;   // true when field doesn't have property "@editable(false)"
        bool fopaque;           // @opaque(true), means that field type is treated as atomic (has no fields), i.e. has no descriptor
        bool overrideGetter;    // @overrideGetter|@override, used when field getter function overrides a function in base class
        bool overrideSetter;    // @overrideSetter|@override, used when field setter function overrides a function in base class

      public:
        FieldInfo() : astNode(nullptr), fisabstract(false), fispointer(false), fisarray(false), fnopack(false), feditable(false),fopaque(false) {}
    };

    class ClassInfo {
      public:
        typedef std::vector<FieldInfo> Fieldlist;

        ASTNode *astNode = nullptr;
        std::string keyword;        // struct/class/packet from MSG
        std::string msgname;        // class name from MSG
        std::string msgqname;
        Properties props;           // class properties

        bool classInfoComplete = false;  // whether following fields are filled in
        bool classBeingAnalyzed = false;
        bool fieldsComplete = false;  // whether fieldlist / baseclassFieldlist are filled in
        bool fieldsBeingAnalyzed = false;

        std::string msgbaseqname;
        std::string msgbase;        // base class name from MSG
        bool gap = false;                   // true if @customize
        bool omitgetverb = false;
        bool isClass; // or struct
        bool iscObject;
        bool iscNamedObject;
        bool iscOwnedObject;
        bool subclassable;
        bool supportsPtr;
        std::string namespacename;
        std::string msgclass;
        std::string realmsgclass;
        std::string msgbaseclass;
        std::string msgdescclass;
        std::string fieldnamesuffix; // e.g. "_var"
        Fieldlist fieldlist;        // list of fields
        Fieldlist baseclassFieldlist;   //modified baseclass fields, e.g. baseclass.basefield = value

        bool generate_class = true;
        bool generate_descriptor = true;
        bool generate_setters_in_descriptor = true;


        StringVector implements;    //value vector from @implements

        std::string defaultvalue;       // value (or empty)
        bool isopaque = false;         // @opaque(true)        // TODO: @opaque should rather be the attribute of the field's type, not the field itself
        bool byvalue = false;           // @byValue, default value is false        // TODO: @byValue should rather be the attribute of the field's type, not the field itself
        std::string datatypebase;   // member C++ datatype
        std::string argtypebase;    // setter C++ argument type
        std::string returntypebase;    // getter C++ return type
        std::string tostring;   // function to convert data to string, defined in property @toString
        std::string fromstring; // function to convert string to data member, defined in property @fromString
        std::string getterconversion;       // uses ".c_str()"
        std::string beforeChange;  // method to be called before mutator methods
//??        bool feditable;         // @editable(true)
    };

    class EnumItem
    {
      public:
        ASTNode *astNode;
        std::string name;
        std::string value;
        std::string comment;
      public:
        EnumItem() : astNode(nullptr) {}
    };

    class EnumInfo
    {
      public:
        ASTNode *astNode;
        std::string enumName;
        std::string enumQName;
        typedef std::vector<EnumItem> FieldList;
        bool isDeclaration = false; // i.e. not a definition
        FieldList fieldlist;
      public:
        EnumInfo() : astNode(nullptr) {}
    };

  private:
    std::map<std::string,ClassInfo> definedClasses;
    std::map<std::string,EnumInfo> definedEnums;
    std::vector<ASTNode*> importedMsgFiles;

  protected:
    void initDescriptors();
    std::string prefixWithNamespace(const std::string& namespaceName, const std::string& name) {  //TODO not needed
        return !namespaceName.empty() ? namespaceName + "::" + name : name;
    }

  public:
    MsgTypeTable() {initDescriptors();}
    ~MsgTypeTable();
    StringVector lookupExistingEnumName(const std::string& name, const std::string& contextNamespace);
    bool isClassDefined(const std::string& classqname) { return definedClasses.find(classqname) != definedClasses.end(); }
    bool isEnumDefined(const std::string& enumqname) { return definedEnums.find(enumqname) != definedEnums.end(); }
    ClassInfo& getClassInfo(const std::string& classqname);
    const EnumInfo& getEnumInfo(const std::string& qname);
    void storeMsgFile(ASTNode *tree) {importedMsgFiles.push_back(tree);}
    void addClass(const ClassInfo& classInfo) {definedClasses[classInfo.msgqname] = classInfo;} // TODO assert not already there
    void addEnum(const EnumInfo& enumInfo) {definedEnums[enumInfo.enumQName] = enumInfo;}  //TODO assert not already there
};

} // namespace nedxml
}  // namespace omnetpp


#endif


