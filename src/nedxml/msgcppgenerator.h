//==========================================================================
//  MSGCPPGENERATOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MSGCPPGENERATOR_H
#define __MSGCPPGENERATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "nedelements.h"
#include "nederror.h"

NAMESPACE_BEGIN

struct MsgCppGeneratorOptions
{
    std::string exportdef;
    bool generate_classes;
    bool generate_descriptors;
    bool generate_setters_in_descriptors;

    MsgCppGeneratorOptions() :
        generate_classes(true),
        generate_descriptors(true),
        generate_setters_in_descriptors(true)
        {}
};

/**
 * Generates C++ code from a MSG file object tree.
 * Assumes object tree has already passed all validation stages (DTD, syntax, semantic).
 *
 * @ingroup NEDGenerator
 */
class NEDXML_API MsgCppGenerator
{
  public:
    typedef std::vector<std::string> StringVector;
    enum ClassType {UNKNOWN = 0, STRUCT, COBJECT, COWNEDOBJECT, CNAMEDOBJECT, FOREIGN};
  protected:
    struct TypeDesc
    {
        const char *fromstring;
        const char *tostring;
        TypeDesc()
            : fromstring(0), tostring(0) {};
        TypeDesc(const char *fromstring, const char *tostring)
            : fromstring(fromstring), tostring(tostring) {};
    };
    typedef std::map<std::string,TypeDesc> TypeMap;
    static const TypeMap PRIMITIVE_TYPES;

    std::string hFilename;
    std::string ccFilename;
    std::ostream *hOutp;
    std::ostream *ccOutp;
    NEDErrorStore *errors;
    std::map<std::string,ClassType> classtype;
    std::map<std::string,std::string> enumtype;
    std::string namespacename;      // as MSG
    StringVector namespacenamevector;   // namespacename split by '::'

    // command line options:
    MsgCppGeneratorOptions opts;

  protected:
    typedef std::map<std::string, std::string> Properties;  //FIXME kell egy reszletesebb modell...

    class ClassInfo {
      public:
        class FieldInfo {
          public:
            NEDElement *nedElement;     // pointer to NED element

            std::string fname;      // field name in MSG
            std::string ftype;      // field type in MSG
            std::string ftypeqname; // fully qualified C++ name of type //TODO should not be needed
            std::string fval;       // value (or empty)
            bool fisabstract;
            bool fispointer;
            bool fisarray;
            std::string farraysize; // array size in MSG, maybe empty for dynamic arrays
            Properties fprops;      // field properties (name, first value of default key)

            // data needed for code generation
            std::string fkind;
            ClassType classtype;  // cobject/cnamedobject/cownedobject/...
            std::string datatype;   // member C++ datatype
            std::string argtype;    // setter C++ argument type
            std::string rettype;    // getter C++ return type
            std::string var;    // name of data member variable
            std::string argname;    // setter argument name
            std::string varsize;    // data member to store array size
            std::string fsizetype;  // type for storing array size
            std::string getter;     // getter function name
            std::string setter;     // Setter function name
            std::string alloc;      // setArraySize() function name
            std::string getsize;    // array size getter function name
            std::string tostring;   // function to convert datamember to string
            std::string fromstring;   // function to convert string to datamember
            std::string maybe_c_str;       // uses ".c_str()"
            std::string enumname;
            std::string enumqname;
            bool fnopack;           // @nopack(true)
            bool feditable;         // @editable(true)
            bool fopaque;         // @opaque(true)        // TODO: @opaque should rather be the attribute of the field's type, not the field itself

          public:
            FieldInfo() : nedElement(NULL), fisabstract(false), fispointer(false), fisarray(false), classtype(UNKNOWN), fnopack(false), feditable(false),fopaque(false) {}
        };
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
        ClassInfo() : nedElement(NULL), gap(false), omitgetverb(false), classtype(UNKNOWN),
              generate_class(true), generate_descriptor(true), generate_setters_in_descriptor(true) {}
    };

    class EnumInfo
    {
      public:
        class EnumItem
        {
          public:
            NEDElement *nedElement;
            std::string name;
            std::string value;
            std::string comment;
          public:
            EnumItem() : nedElement(NULL) {}
        };

        EnumElement *nedElement;
        std::string enumName;
        std::string enumQName;
        typedef std::vector<EnumItem> FieldList;
        FieldList fieldlist;
      public:
        EnumInfo() : nedElement(NULL) {}
    };
  protected:
    std::string prefixWithNamespace(const std::string& s);
    StringVector lookupExistingClassName(const std::string& s);
    StringVector lookupExistingEnumName(const std::string& s);
    bool isClassDeclared(const std::string& s) { return classtype.find(s) != classtype.end(); }
    ClassType getClassType(const std::string& s);
    ClassInfo extractClassInfo(NEDElement *node); // accepts StructElement, ClassElement, MessageElement, PacketElement
    void extractClassDecl(NEDElement *node); // accepts StructElementDecl, ClassElementDecl, MessageElementDecl, PacketElementDecl
    Properties extractPropertiesOf(NEDElement *node);
    void prepareFieldForCodeGeneration(ClassInfo& info, ClassInfo::FieldInfo *it);
    void prepareForCodeGeneration(ClassInfo& classInfo);
    EnumInfo extractEnumInfo(EnumElement *node); // accepts EnumElement
    void generateClass(const ClassInfo& classInfo);
    void generateStruct(const ClassInfo& classInfo);
    void generateDescriptorClass(const ClassInfo& a);
    void generateEnum(const EnumInfo& enumInfo);
    void generateNamespaceBegin(NEDElement *element);
    void generateNamespaceEnd();
    std::string generatePreComment(NEDElement *nedElement);
    void generateTemplates();
    bool getPropertyAsBool(const Properties& p, const char *name, bool defval);
    std::string getProperty(const Properties& p, const char *name, const std::string& defval = std::string());

    /**
     * Generates C++ code from the specified message file. Assumes object tree has already
     * passed DTD and syntax validation.
     */
    void generate(MsgFileElement *fileElement);

  public:
    /**
     * Constructor.
     */
    MsgCppGenerator(NEDErrorStore *errors, const MsgCppGeneratorOptions& options);

    /**
     * Destructor.
     */
    ~MsgCppGenerator();

    /**
     * TODO
     */
    void generate(MsgFileElement *fileElement, const char *hFile, const char *ccFile);
};

NAMESPACE_END


#endif


