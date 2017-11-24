//==========================================================================
//  MSGANALYZER.H - part of
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

#ifndef __OMNETPP_NEDXML_MSGANALYZER_H
#define __OMNETPP_NEDXML_MSGANALYZER_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "errorstore.h"
#include "msgelements.h"
#include "msgtypetable.h"

namespace omnetpp {
namespace nedxml {

struct MsgCompilerOptions;

/**
 * @brief Part of the message compiler. Produces ClassInfo/EnumInfo objects from the ASTNode tree.
 * Assumes object tree has already passed all validation stages (DTD, syntax, semantic).
 *
 * @ingroup CppGenerator
 */
class NEDXML_API MsgAnalyzer
{
  public:
    typedef std::set<std::string> StringSet;
    typedef std::vector<std::string> StringVector;
    typedef MsgTypeTable::Properties Properties;
    typedef MsgTypeTable::FieldInfo FieldInfo;
    typedef MsgTypeTable::ClassInfo ClassInfo;
    typedef MsgTypeTable::EnumItem EnumItem;
    typedef MsgTypeTable::EnumInfo EnumInfo;

  public:
    static StringSet RESERVED_WORDS;

  protected:
    ErrorStore *errors;
    MsgTypeTable *typeTable;
    const MsgCompilerOptions& opts;

  protected:
    void extractClassInfo(ClassInfo& classInfo);
    void analyzeClassOrStruct(ClassInfo& classInfo, const std::string& namespaceName);
    void analyzeFields(ClassInfo& classInfo, const std::string& namespaceName);
    void analyzeField(ClassInfo& classInfo, FieldInfo *field, const std::string& namespaceName);
    void analyzeInheritedField(ClassInfo& classInfo, FieldInfo *field);
    std::string prefixWithNamespace(const std::string& name, const std::string& namespaceName);
    Properties extractProperties(ASTNode *node);
    bool hasSuperclass(ClassInfo& classInfo, const std::string& superclassQName);
    FieldInfo *findSuperclassField(ClassInfo& classInfo, const std::string& fieldName);
    bool hasProperty(const Properties& p, const char *name)  { return (p.find(name) != p.end()); }
    bool getPropertyAsBool(const Properties& p, const char *name, bool defval);
    std::string getProperty(const Properties& p, const char *name, const std::string& defval = std::string());
    std::string makeFuncall(const std::string& var, const std::string& funcTemplate, bool withIndex=false, const std::string& value="");
    std::string lookupExistingClassName(const std::string& name, const std::string& contextNamespace,  ClassInfo *contextClass=nullptr);

  public:
    MsgAnalyzer(const MsgCompilerOptions& opts, MsgTypeTable *typeTable, ErrorStore *errors);
    ~MsgAnalyzer();
    ClassInfo makeIncompleteClassInfo(ASTNode *node, const std::string& namespaceName); // accepts StructElement, ClassElement, MessageElement, PacketElement
    void ensureAnalyzed(ClassInfo& classInfo);
    void ensureFieldsAnalyzed(ClassInfo& classInfo);
    EnumInfo extractEnumDecl(EnumDeclElement *node, const std::string& namespaceName);
    EnumInfo extractEnumInfo(EnumElement *node, const std::string& namespaceName);
    ClassInfo extractClassInfoFromEnum(EnumElement *node, const std::string& namespaceName);

  private:
    static constexpr const char* ATT_NAME = "name";
    static constexpr const char* ATT_EXTENDS_NAME = "extends-name";

    static constexpr const char* PROP_ACTUALLY = "actually";
    static constexpr const char* PROP_PRIMITIVE = "primitive";
    static constexpr const char* PROP_OPAQUE = "opaque";
    static constexpr const char* PROP_BYVALUE = "byvalue";
    static constexpr const char* PROP_DEFAULTVALUE = "defaultvalue";
    static constexpr const char* PROP_CPPTYPE = "cpptype";
    static constexpr const char* PROP_ARGTYPE = "argtype";
    static constexpr const char* PROP_RETTYPE = "rettype";
    static constexpr const char* PROP_TOSTRING = "tostring";
    static constexpr const char* PROP_FROMSTRING = "fromstring";
    static constexpr const char* PROP_MAYBECSTR = "maybe_c_str";
    static constexpr const char* PROP_EXISTINGCLASS = "existingClass";
    static constexpr const char* PROP_DESCRIPTOR = "descriptor";
    static constexpr const char* PROP_OMITGETVERB = "omitGetVerb";
    static constexpr const char* PROP_FIELDNAMESUFFIX = "fieldNameSuffix";
    static constexpr const char* PROP_BEFORECHANGE = "beforeChange";
    static constexpr const char* PROP_IMPLEMENTS = "implements";
    static constexpr const char* PROP_NOPACK = "nopack";
    static constexpr const char* PROP_OWNED = "owned";
    static constexpr const char* PROP_EDITABLE = "editable";
    static constexpr const char* PROP_OVERRIDEGETTER = "overridegetter";
    static constexpr const char* PROP_OVERRIDESETTER = "overridesetter";
    static constexpr const char* PROP_ENUM = "enum";
    static constexpr const char* PROP_SIZETYPE = "sizetype";
    static constexpr const char* PROP_SETTER = "setter";
    static constexpr const char* PROP_GETTER = "getter";
    static constexpr const char* PROP_MUTABLEGETTER = "mgetter";
    static constexpr const char* PROP_SIZESETTER = "sizeSetter";
    static constexpr const char* PROP_SIZEGETTER = "sizeGetter";
    static constexpr const char* PROP_CUSTOMIZE = "customize";
};

} // namespace nedxml
}  // namespace omnetpp


#endif


