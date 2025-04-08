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

#include "errorstore.h"
#include "msgelements.h"
#include "msgtypetable.h"
#include "msganalyzer.h"
#include "msgcodegenerator.h"
#include "msgcompileroptions.h"

namespace omnetpp {
namespace nedxml {

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
    typedef MsgTypeTable::Property Property;
    typedef MsgTypeTable::Properties Properties;
    typedef MsgTypeTable::FieldInfo FieldInfo;
    typedef MsgTypeTable::ClassInfo ClassInfo;
    typedef MsgTypeTable::EnumItem EnumItem;

  protected:
    MsgCompilerOptions opts;
    MsgTypeTable typeTable;
    MsgAnalyzer analyzer;
    MsgCodeGenerator codegen;

    bool used = false;
    ErrorStore *errors;
    StringSet importsSeen;
    StringSet importedFiles;

  private:
    static constexpr const char* ATT_NAME = "name";
    static const char *BUILTIN_DEFINITIONS;

  protected:
    void importBuiltinDefinitions();
    void processBuiltinImport(const char *txt, const char *fname);

    void processImport(ImportElement *importElem, const std::string& currentDir);
    std::string resolveImport(const std::string& importName, const std::string& currentDir);
    void collectTypes(MsgFileElement *fileElement, bool isImport);
    void generateCode(MsgFileElement *fileElement);
    std::string prefixWithNamespace(const std::string& name, const std::string& namespaceName);
    void validateNamespaceName(const std::string& namespaceName, ASTNode *element);

  public:
    /**
     * Constructor.
     */
    MsgCompiler(const MsgCompilerOptions& options, ErrorStore *errors);

    /**
     * Destructor.
     */
    ~MsgCompiler() {}

    /**
     * Generates C++ code from the specified message file. Assumes that the
     * object tree has already passed DTD and syntax validation.
     */
    void generate(MsgFileElement *fileElement, const char *hFile, const char *ccFile, StringSet& outImportedFiles);

    /**
     * Print properties documentation in LaTeX format, for use in the Manual.
     */
    void printPropertiesLatexDocu(std::ostream& out);

    /**
     * Returns the built-in MSG definitions.
     */
    static const char *getBuiltinDefinitions() {return BUILTIN_DEFINITIONS;}
};

}  // namespace nedxml
}  // namespace omnetpp


#endif


