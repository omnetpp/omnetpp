//==========================================================================
//  MSGCODEGENERATOR.H - part of
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

#ifndef __OMNETPP_NEDXML_MSGCODEGENERATOR_H
#define __OMNETPP_NEDXML_MSGCODEGENERATOR_H

#include <iostream>
#include <fstream>
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

/**
 * @brief Code generator part of the message compiler.
 *
 * @ingroup CppGenerator
 */
class NEDXML_API MsgCodeGenerator
{
  public:
    typedef std::vector<std::string> StringVector;
    typedef MsgTypeTable::Properties Properties;
    typedef MsgTypeTable::FieldInfo FieldInfo;
    typedef MsgTypeTable::ClassInfo ClassInfo;
    typedef MsgTypeTable::EnumItem EnumItem;
    typedef MsgTypeTable::EnumInfo EnumInfo;

  protected:
    std::string hFilename;
    std::string ccFilename;
    std::ofstream hStream;
    std::ofstream ccStream;
    std::string headerGuard;

  protected:
    std::string prefixWithNamespace(const std::string& name, const std::string& namespaceName);
    std::string makeFuncall(const std::string& var, const std::string& funcTemplate, bool withIndex=false, const std::string& value="");

    void generateClassDecl(const ClassInfo& classInfo, const std::string& exportDef, const std::string& extraCode);
    void generateClassImpl(const ClassInfo& classInfo);
    void generateStructDecl(const ClassInfo& classInfo, const std::string& exportDef, const std::string& extraCode);
    void generateStructImpl(const ClassInfo& classInfo);

  public:
    void openFiles(const char *hFile, const char *ccFile);
    void closeFiles();
    void deleteFiles();
    void generateProlog(const std::string& msgFileName, const std::string& firstNamespace, const std::string& exportDef);
    void generateEpilog();
    void generateClass(const ClassInfo& classInfo, const std::string& exportDef, const std::string& extraCode="");
    void generateStruct(const ClassInfo& classInfo, const std::string& exportDef, const std::string& extraCode="");
    void generateDescriptorClass(const ClassInfo& a);
    void generateEnum(const EnumInfo& enumInfo);
    void generateImport(const std::string& importName);
    void generateNamespaceBegin(const std::string& namespaceName, bool intoCcFile=true);
    void generateNamespaceEnd(const std::string& namespaceName, bool intoCcFile=true);
    void generateTypeAnnouncement(const ClassInfo& classInfo);
    std::string generatePreComment(ASTNode *nedElement);
    void generateCplusplusBlock(const std::string& target, const std::string& body);
    void generateTemplates();
};

} // namespace nedxml
} // namespace omnetpp


#endif


