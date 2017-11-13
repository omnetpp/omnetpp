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

#include "nedelements.h"
#include "nederror.h"
#include "msgtypetable.h"

namespace omnetpp {
namespace nedxml {

struct MsgCompilerOptions;

/**
 * @brief Part of the message compiler. Produces ClassInfo/EnumInfo objects from the NEDElement tree.
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
    NEDErrorStore *errors;
    MsgTypeTable *typeTable;
    const MsgCompilerOptions& opts;

  protected:
    void extractClassInfo(ClassInfo& classInfo);
    void analyzeClassOrStruct(ClassInfo& classInfo, const std::string& namespaceName);
    void analyzeFields(ClassInfo& classInfo, const std::string& namespaceName);
    void analyzeField(ClassInfo& classInfo, FieldInfo *field, const std::string& namespaceName);
    void analyzeBaseClassField(ClassInfo& classInfo, FieldInfo *field);
    std::string prefixWithNamespace(const std::string& name, const std::string& namespaceName);
    Properties extractPropertiesOf(NEDElement *node);
    bool hasProperty(const Properties& p, const char *name)  { return (p.find(name) != p.end()); }
    bool getPropertyAsBool(const Properties& p, const char *name, bool defval);
    std::string getProperty(const Properties& p, const char *name, const std::string& defval = std::string());
    std::string makeFuncall(const std::string& var, const std::string& funcTemplate, bool withIndex=false, const std::string& value="");

  public:
    MsgAnalyzer(const MsgCompilerOptions& opts, MsgTypeTable *typeTable, NEDErrorStore *errors);
    ~MsgAnalyzer();
    ClassInfo makeIncompleteClassInfo(NEDElement *node, const std::string& namespaceName); // accepts StructElement, ClassElement, MessageElement, PacketElement
    void ensureAnalyzed(ClassInfo& classInfo);
    void ensureFieldsAnalyzed(ClassInfo& classInfo);
    EnumInfo extractEnumDecl(EnumDeclElement *node, const std::string& namespaceName);
    EnumInfo extractEnumInfo(EnumElement *node, const std::string& namespaceName);
    ClassInfo extractClassInfoFromEnum(EnumElement *node, const std::string& namespaceName);
};

} // namespace nedxml
}  // namespace omnetpp


#endif


