//==========================================================================
//  MSGCOMPILER.H - part of
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

#ifndef __OMNETPP_NEDXML_MSGCOMPILER_H
#define __OMNETPP_NEDXML_MSGCOMPILER_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "nedelements.h"
#include "nederror.h"
#include "msgtypetable.h"
#include "msganalyzer.h"
#include "msgcodegenerator.h"

namespace omnetpp {
namespace nedxml {

/**
 * @brief Options for MsgCompiler.Generates C++ code from a MSG file object tree.
 *
 * @ingroup CppGenerator
 */
struct MsgCompilerOptions
{
    std::vector<std::string> importPath;
    std::string exportDef;
    bool generateClasses;
    bool generateDescriptors;
    bool generateSettersInDescriptors;

    MsgCompilerOptions() :
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
class NEDXML_API MsgCompiler
{
  public:
    typedef std::set<std::string> StringSet;
    typedef std::vector<std::string> StringVector;
    typedef MsgTypeTable::TypeDesc TypeDesc;
    typedef MsgTypeTable::ClassType ClassType;
    typedef MsgTypeTable::Properties Properties;
    typedef MsgTypeTable::FieldInfo FieldInfo;
    typedef MsgTypeTable::ClassInfo ClassInfo;
    typedef MsgTypeTable::EnumItem EnumItem;
    typedef MsgTypeTable::EnumInfo EnumInfo;

  protected:
    MsgCompilerOptions opts;

    MsgTypeTable typeTable;
    MsgAnalyzer analyzer;
    MsgCodeGenerator codegen;

    NEDErrorStore *errors;
    std::string currentNamespace;
    StringSet importsSeen;

  protected:
    void processImport(NEDElement *child, const std::string& currentDir);
    std::string resolveImport(const std::string& importName, const std::string& currentDir);
    void process(MsgFileElement *fileElement, bool generateCode);
    void validateNamespaceName(const std::string& namespaceName, NEDElement *element);

  public:
    /**
     * Constructor.
     */
    MsgCompiler(NEDErrorStore *errors, const MsgCompilerOptions& options);

    /**
     * Destructor.
     */
    ~MsgCompiler();

    /**
     * Generates C++ code from the specified message file. Assumes that the
     * object tree has already passed DTD and syntax validation.
     */
    void generate(MsgFileElement *fileElement, const char *hFile, const char *ccFile);
};

}  // namespace nedxml
}  // namespace omnetpp


#endif


