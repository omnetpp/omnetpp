//==========================================================================
//  MSGCOMPILEROLD.H - part of
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

#ifndef __OMNETPP_NEDXML_MSGCOMPILEROLD_H
#define __OMNETPP_NEDXML_MSGCOMPILEROLD_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "errorstore.h"
#include "msgelements.h"

namespace omnetpp {
namespace nedxml {

/**
 * @brief Options for MsgCompilerOld.
 *
 * @ingroup CppGenerator
 */
struct MsgCompilerOptionsOld
{
    std::string exportDef;
    bool generateClasses;
    bool generateDescriptors;
    bool generateSettersInDescriptors;

    MsgCompilerOptionsOld() :
        generateClasses(true),
        generateDescriptors(true),
        generateSettersInDescriptors(true)
        {}
};

/**
 * @brief Generates C++ code from a MSG file. Covers the MSG language of
 * OMNeT++ 4.x, and 5.x up to 5.2.
 *
 * Assumes object tree has already passed all validation stages (DTD, syntax, semantic).
 *
 * @ingroup CppGenerator
 */
class NEDXML_API MsgCompilerOld
{
  public:
    typedef std::vector<std::string> StringVector;

    enum ClassType {
        UNKNOWN = 0,
        STRUCT,       // struct
        NONCOBJECT,   // class not derived from cObject
        COBJECT,      // class derived from cObject
        COWNEDOBJECT, // class derived from cOwnedObject
        CNAMEDOBJECT  // class derived from cNamedObject
    };

  protected:
    struct TypeDesc
    {
        const char *msgTypeName;
        const char *cppTypeName;
        const char *fromstring;
        const char *tostring;
        const char *emptyValue;
    };

    static TypeDesc _PRIMITIVE_TYPES[];
    typedef std::map<std::string,TypeDesc> TypeDescMap;
    TypeDescMap PRIMITIVE_TYPES;

    static const char *_RESERVED_WORDS[];
    typedef std::set<std::string> WordSet;
    WordSet RESERVED_WORDS;

    std::string hFilename;
    std::string ccFilename;
    std::ostream *hOutp;
    std::ostream *ccOutp;
    ErrorStore *errors;
    std::map<std::string,ClassType> classType;
    std::map<std::string,std::string> enumType;
    std::string namespaceName;      // as MSG
    StringVector namespaceNameVector;   // namespacename split by '::'

    // command line options:
    MsgCompilerOptionsOld opts;

  protected:
    typedef std::map<std::string, std::string> Properties;

    class ClassInfo {
      public:
        class FieldInfo {
          public:
            ASTNode *astNode;

            std::string fname;      // field name in MSG
            std::string ftype;      // field type in MSG
            std::string ftypeqname; // fully qualified C++ name of type
            std::string fval;       // value (or empty)
            bool fisabstract;
            bool fispointer;
            bool fisarray;
            std::string farraysize; // array size in MSG, maybe empty for dynamic arrays
            Properties fprops;      // field properties (name, first value of default key)

            // data needed for code generation
            bool fisprimitivetype;  // whether primitive or compound type
            ClassType classtype;    // cobject/cnamedobject/cownedobject/...
            std::string datatype;   // member C++ datatype
            std::string argtype;    // setter C++ argument type
            std::string rettype;    // getter C++ return type
            std::string var;        // name of data member variable
            std::string argname;    // setter argument name
            std::string varsize;    // data member to store array size
            std::string fsizetype;  // type for storing array size
            std::string getter;     // getter function name
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
            bool fopaque;         // @opaque(true)

          public:
            FieldInfo() : astNode(nullptr), fisabstract(false), fispointer(false), fisarray(false), classtype(UNKNOWN), fnopack(false), feditable(false),fopaque(false) {}
        };
        typedef std::vector<FieldInfo> Fieldlist;

        ASTNode *nedElement;
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

        std::string msgqname;
        std::string msgbaseqname;

        StringVector implements;    //value vector from @implements

      public:
        ClassInfo() : nedElement(nullptr), gap(false), omitgetverb(false), classtype(UNKNOWN),
              generate_class(true), generate_descriptor(true), generate_setters_in_descriptor(true) {}
    };

    class EnumInfo
    {
      public:
        class EnumItem
        {
          public:
            ASTNode *nedElement;
            std::string name;
            std::string value;
            std::string comment;
          public:
            EnumItem() : nedElement(nullptr) {}
        };

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
    std::string prefixWithNamespace(const std::string& s);
    StringVector lookupExistingClassName(const std::string& s);
    StringVector lookupExistingEnumName(const std::string& s);
    bool isClassDeclared(const std::string& classqname) { return classType.find(classqname) != classType.end(); }
    void addClassType(const std::string& classqname, ClassType type, ASTNode *context);
    ClassType getClassType(const std::string& classqname);
    ClassInfo extractClassInfo(ASTNode *node); // accepts StructElement, ClassElement, MessageElement, PacketElement
    void extractClassDecl(ASTNode *node); // accepts StructElementDecl, ClassElementDecl, MessageElementDecl, PacketElementDecl
    Properties extractPropertiesOf(ASTNode *node);
    void prepareFieldForCodeGeneration(ClassInfo& info, ClassInfo::FieldInfo *it);
    void prepareForCodeGeneration(ClassInfo& classInfo);
    EnumInfo extractEnumInfo(EnumElement *node); // accepts EnumElement
    void generateClass(const ClassInfo& classInfo);
    void generateStruct(const ClassInfo& classInfo);
    void generateDescriptorClass(const ClassInfo& a);
    void generateEnum(const EnumInfo& enumInfo);
    void generateNamespaceBegin(ASTNode *element);
    void generateNamespaceEnd();
    std::string generatePreComment(ASTNode *nedElement);
    void generateTemplates();
    bool getPropertyAsBool(const Properties& p, const char *name, bool defval);
    std::string getProperty(const Properties& p, const char *name, const std::string& defval = std::string());
    void generate(MsgFileElement *fileElement);
    std::string makeFuncall(const std::string& var, const std::string& funcTemplate, bool withIndex=false, const std::string& value="");

  public:
    /**
     * Constructor.
     */
    MsgCompilerOld(ErrorStore *errors, const MsgCompilerOptionsOld& options);

    /**
     * Destructor.
     */
    ~MsgCompilerOld();

    /**
     * Generates C++ code from the specified message file. Assumes that the
     * object tree has already passed DTD and syntax validation.
     */
    void generate(MsgFileElement *fileElement, const char *hFile, const char *ccFile);
};

} // namespace nedxml
}  // namespace omnetpp


#endif


