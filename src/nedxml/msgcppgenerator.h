//==========================================================================
//  MSGCPPGENERATOR.H - part of
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

#ifndef __OMNETPP_NEDXML_MSGCPPGENERATOR_H
#define __OMNETPP_NEDXML_MSGCPPGENERATOR_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "nedelements.h"
#include "nederror.h"
#include "msgtypetable.h"

namespace omnetpp {
namespace nedxml {

/**
 * @brief Options for MsgCppGenerator.Generates C++ code from a MSG file object tree.
 *
 * @ingroup CppGenerator
 */
struct MsgCppGeneratorOptions
{
    std::vector<std::string> importPath;
    std::string exportDef;
    bool generateClasses;
    bool generateDescriptors;
    bool generateSettersInDescriptors;

    MsgCppGeneratorOptions() :
        generateClasses(true),
        generateDescriptors(true),
        generateSettersInDescriptors(true)
        {}
};

/**
 * @brief Generates C++ code from a MSG file object tree.
 *
 * Assumes object tree has already passed all validation stages (DTD, syntax, semantic).
 *
 * @ingroup CppGenerator
 */
class NEDXML_API MsgCppGenerator
{
  public:
    typedef std::vector<std::string> StringVector;
    typedef MsgTypeTable::TypeDesc TypeDesc;
    typedef MsgTypeTable::ClassType ClassType;
    typedef MsgTypeTable::Properties Properties;
    typedef MsgTypeTable::FieldInfo FieldInfo;
    typedef MsgTypeTable::ClassInfo ClassInfo;
    typedef MsgTypeTable::EnumItem EnumItem;
    typedef MsgTypeTable::EnumInfo EnumInfo;

  protected:
    static const char *_RESERVED_WORDS[];
    typedef std::set<std::string> StringSet;
    StringSet RESERVED_WORDS;

    std::string hFilename;
    std::string ccFilename;
    std::ostream *hOutp;
    std::ostream *ccOutp;
    NEDErrorStore *errors;
    MsgTypeTable typeTable;
    std::string namespaceName;      // as MSG
    StringVector namespaceNameVector;   // namespacename split by '::'
    StringSet importsSeen;

    // command line options:
    MsgCppGeneratorOptions opts;

  protected:
    void initDescriptors();
    std::string prefixWithNamespace(const std::string& s);
    ClassInfo extractClassInfo(NEDElement *node); // accepts StructElement, ClassElement, MessageElement, PacketElement
    void extractClassDecl(NEDElement *node); // accepts StructElementDecl, ClassElementDecl, MessageElementDecl, PacketElementDecl
    void processImport(NEDElement *child, const std::string& currentDir);
    std::string resolveImport(const std::string& importName, const std::string& currentDir);
    Properties extractPropertiesOf(NEDElement *node);
    void analyzeField(ClassInfo& classInfo, FieldInfo *field);
    void analyze(ClassInfo& classInfo);
    EnumInfo extractEnumInfo(EnumElement *node); // accepts EnumElement
    void generateClass(const ClassInfo& classInfo);
    void generateStruct(const ClassInfo& classInfo);
    void generateDescriptorClass(const ClassInfo& a);
    void generateEnum(const EnumInfo& enumInfo);
    void generateNamespaceBegin(NEDElement *element);
    void generateNamespaceEnd();
    std::string generatePreComment(NEDElement *nedElement);
    void generateTemplates();
    bool hasProperty(const Properties& p, const char *name)  { return (p.find(name) != p.end()); }

    bool getPropertyAsBool(const Properties& p, const char *name, bool defval);
    std::string getProperty(const Properties& p, const char *name, const std::string& defval = std::string());
    void process(MsgFileElement *fileElement, bool generateCode);
    std::string makeFuncall(const std::string& var, const std::string& funcTemplate, bool withIndex=false, const std::string& value="");

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
     * Generates C++ code from the specified message file. Assumes that the
     * object tree has already passed DTD and syntax validation.
     */
    void generate(MsgFileElement *fileElement, const char *hFile, const char *ccFile);
};

} // namespace nedxml
}  // namespace omnetpp


#endif


