//==========================================================================
//  MSGCOMPILER.CC - part of
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

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "common/stringutil.h"
#include "common/stlutil.h"
#include "common/fileutil.h"
#include "msgcompiler.h"
#include "msgparser.h"
#include "msgdtdvalidator.h"
#include "exception.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

static const char *BUILTIN_DEFINITIONS =
        R"ENDMARK(
        class __bool { @actually(bool); @primitive; @fromString(string2bool($)); @toString(bool2string($)); @defaultValue(false); }
        class __float { @actually(float); @primitive; @fromString(string2double($)); @toString(double2string($)); @defaultValue(0); }
        class __double { @actually(double); @primitive; @fromString(string2double($)); @toString(double2string($)); @defaultValue(0); }
        class __string { @actually(string); @primitive; @cppType(omnetpp::opp_string); @argType(const char *); @returnType(const char *); @getterConversion(.c_str()); @fromString(($)); @toString(oppstring2string($)); }
        class __char { @actually(char); @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); }
        class __short { @actually(short); @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); }
        class __int { @actually(int); @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); }
        class __long { @actually(long); @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); }
        class __uchar { @actually(unsigned char); @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); }
        class __ushort { @actually(unsigned short); @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); }
        class __uint { @actually(unsigned int); @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); }
        class __ulong { @actually(unsigned long); @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); }
        class int8_t { @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); }
        class int16_t { @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); }
        class int32_t { @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); }
        class int64_t { @primitive; @fromString(string2int64($)); @toString(int642string($)); @defaultValue(0); }
        class uint8_t { @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); }
        class uint16_t { @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); }
        class uint32_t { @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); }
        class uint64_t { @primitive; @fromString(string2uint64($)); @toString(uint642string($)); @defaultValue(0); }
        class int8 { @primitive; @cppType(int8_t); @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); }
        class int16 { @primitive; @cppType(int16_t); @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); }
        class int32 { @primitive; @cppType(int32_t); @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); }
        class int64 { @primitive; @cppType(int64_t); @fromString(string2int64($)); @toString(int642string($)); @defaultValue(0); }
        class uint8 { @primitive; @cppType(uint8_t); @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); }
        class uint16 { @primitive; @cppType(uint16_t); @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); }
        class uint32 { @primitive; @cppType(uint32_t); @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); }
        class uint64 { @primitive; @cppType(uint64_t); @fromString(string2uint64($)); @toString(uint642string($)); @defaultValue(0); }
        )ENDMARK";

extern const char *SIM_STD_DEFINITIONS;  // contents of sim/sim_std.msg, stringified into sim_std_msg.cc

inline std::string str(const char *s)
{
    return s ? s : "";
}

static bool isQualified(const std::string& qname)
{
    return qname.find("::") != qname.npos;
}

template<class P, class T>
P check_and_cast(T *p)
{
    Assert(p);
    P ret = dynamic_cast<P>(p);
    Assert(ret);
    return ret;
}

MsgCompiler::MsgCompiler(const MsgCompilerOptions& opts, ErrorStore *errors) : opts(opts), analyzer(opts, &typeTable, errors), errors(errors)
{
}

MsgCompiler::~MsgCompiler()
{
}

void MsgCompiler::generate(MsgFileElement *fileElement, const char *hFile, const char *ccFile, StringSet& outImportedFiles)
{
    if (used)
        throw opp_runtime_error("MsgCompiler is a one-shot object, make a new instance to compile another file");
    used = true;

    try {
        importBuiltinDefinitions();

        collectTypes(fileElement, false);

        codegen.openFiles(hFile, ccFile);
        generateCode(fileElement);
        codegen.closeFiles();
    }
    catch (std::exception& e) {
        errors->addError(fileElement, "Fatal: %s", e.what());
    }

    if (errors->containsError())
        codegen.deleteFiles();

    outImportedFiles = importedFiles;
}


void MsgCompiler::importBuiltinDefinitions()
{
    processBuiltinImport(BUILTIN_DEFINITIONS, "builtin-definitions");
    processBuiltinImport(SIM_STD_DEFINITIONS, "sim_std.msg");
}

void MsgCompiler::processBuiltinImport(const char *txt, const char *fname)
{
    MsgParser parser(errors);
    parser.setMsgNewSyntaxFlag(true);
    ASTNode *tree = parser.parseMsgText(txt, fname);
    if (errors->containsError()) {
        delete tree;
        return;
    }

    MsgDtdValidator dtdvalidator(errors);
    dtdvalidator.validate(tree);
    if (errors->containsError()) {
        delete tree;
        return;
    }

    typeTable.storeMsgFile(tree); // keep AST until we're done because ClassInfo/FieldInfo refer to it...

    // extract declarations
    MsgFileElement *fileElement = check_and_cast<MsgFileElement*>(tree);
    collectTypes(fileElement, true);
}

void MsgCompiler::collectTypes(MsgFileElement *fileElement, bool isImported)
{
    std::string currentDir = directoryOf(fileElement->getFilename());
    std::string currentNamespace = "";
    for (ASTNode *child = fileElement->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
            case MSG_NAMESPACE: {
                NamespaceElement *nsElem = check_and_cast<NamespaceElement *>(child);
                currentNamespace = nsElem->getName();
                validateNamespaceName(currentNamespace, child); //TODO into syntax validator class!!!
                break;
            }

            case MSG_CPLUSPLUS:
                break;

            case MSG_IMPORT: {
                ImportElement *importElem = check_and_cast<ImportElement *>(child);
                std::string importName = importElem->getImportSpec();
                if (!currentNamespace.empty())
                    errors->addError(child, "misplaced import '%s': imports are not allowed within a namespace", importName.c_str()); //TODO into syntax validator class!!!
                if (!common::contains(importsSeen, importName)) {
                    importsSeen.insert(importName);
                    processImport(importElem, currentDir);
                }
                 break;
             }

            case MSG_STRUCT_DECL:
            case MSG_CLASS_DECL:
            case MSG_MESSAGE_DECL:
            case MSG_PACKET_DECL:
                errors->addWarning(child, "type declarations are not needed with imports, and will be ignored"); //TODO into syntax validator class!!!
                break;

            case MSG_ENUM_DECL: { // for enums already defined and registered in C++
                EnumDeclElement *enumDeclElem = check_and_cast<EnumDeclElement *>(child);
                EnumInfo enumInfo = analyzer.extractEnumDecl(enumDeclElem, currentNamespace);
                if (!typeTable.isEnumDefined(enumInfo.enumQName))
                    typeTable.addEnum(enumInfo);
                break;
            }

            case MSG_ENUM: {
                EnumElement *enumElem = check_and_cast<EnumElement *>(child);
                EnumInfo enumInfo = analyzer.extractEnumInfo(enumElem, currentNamespace);
                if (isQualified(enumInfo.enumName))
                    errors->addError(enumInfo.astNode, "type name may not be qualified: '%s'", enumInfo.enumName.c_str()); //TODO into some validator class
                if (typeTable.isEnumDefined(enumInfo.enumQName))
                    errors->addError(enumInfo.astNode, "attempt to redefine '%s'", enumInfo.enumName.c_str());
                typeTable.addEnum(enumInfo);
                ClassInfo classInfo = analyzer.extractClassInfoFromEnum(check_and_cast<EnumElement *>(child), currentNamespace, isImported);
                if (typeTable.isClassDefined(classInfo.qname))
                    errors->addError(classInfo.astNode, "attempt to redefine '%s'", classInfo.name.c_str());
                typeTable.addClass(classInfo);
                break;
            }

            case MSG_STRUCT:
            case MSG_CLASS:
            case MSG_MESSAGE:
            case MSG_PACKET: {
                ClassInfo classInfo = analyzer.extractClassInfo(child, currentNamespace, isImported);
                if (typeTable.isClassDefined(classInfo.qname) && !containsKey(classInfo.props, str("overwritePreviousDefinition")))
                    errors->addError(classInfo.astNode, "attempt to redefine '%s'", classInfo.name.c_str());
                typeTable.addClass(classInfo);
                break;
            }
        }
    }
}

void MsgCompiler::processImport(ImportElement *importElem, const std::string& currentDir)
{
    std::string importName = importElem->getImportSpec();
    std::string fileName = resolveImport(importName, currentDir);
    if (fileName == "") {
        errors->addError(importElem, "cannot resolve import '%s'", importName.c_str());
        return;
    }

    importedFiles.insert(fileName);

    MsgParser parser(errors);
    parser.setMsgNewSyntaxFlag(true);
    ASTNode *tree = parser.parseMsgFile(fileName.c_str());
    if (errors->containsError()) {
        delete tree;
        return;
    }

    typeTable.storeMsgFile(tree); // keep AST until we're done because ClassInfo/FieldInfo refer to it...

    // extract declarations
    MsgFileElement *fileElement = check_and_cast<MsgFileElement*>(tree);
    collectTypes(fileElement, true);
}

std::string MsgCompiler::resolveImport(const std::string& importName, const std::string& currentDir)
{
    std::string msgFile = opp_replacesubstring(importName, ".", "/", true) + ".msg";
    std::string candidate = concatDirAndFile(currentDir.c_str(), msgFile.c_str());
    if (fileExists(candidate.c_str()))
        return candidate;
    for (auto dir : opts.importPath) {
        std::string candidate = concatDirAndFile(dir.c_str(), msgFile.c_str());
        if (fileExists(candidate.c_str()))
            return candidate;
    }
    return "";
}


std::string MsgCompiler::prefixWithNamespace(const std::string& name, const std::string& namespaceName)
{
    return !namespaceName.empty() ? namespaceName + "::" + name : name;  // prefer name from local namespace
}

void MsgCompiler::generateCode(MsgFileElement *fileElement)
{
    std::string currentNamespace = "";
    NamespaceElement *firstNSElem = fileElement->getFirstNamespaceChild();
    std::string firstNSName = firstNSElem ? firstNSElem->getName() : "";
    codegen.generateProlog(fileElement->getFilename(), firstNSName, opts.exportDef);
    std::map<std::string, std::string> classExtraCode;

    // generate forward declarations so that cyclic references compile; also collect classExtraCode blocks
    for (ASTNode *child = fileElement->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
            case MSG_NAMESPACE: {
                // open namespace(s)
                if (!currentNamespace.empty())
                    codegen.generateNamespaceEnd(currentNamespace, false);
                NamespaceElement *nsElem = check_and_cast<NamespaceElement *>(child);
                currentNamespace = nsElem->getName();
                codegen.generateNamespaceBegin(currentNamespace, false);
                break;
            }
            case MSG_STRUCT:
            case MSG_CLASS:
            case MSG_MESSAGE:
            case MSG_PACKET: {
                std::string qname = prefixWithNamespace(child->getAttribute(ATT_NAME), currentNamespace);
                ClassInfo& classInfo = typeTable.getClassInfo(qname);
                analyzer.ensureAnalyzed(classInfo);
                if (classInfo.generateClass)
                    codegen.generateTypeAnnouncement(classInfo);
                break;
            }
            case MSG_CPLUSPLUS: {
                CplusplusElement *cppElem = check_and_cast<CplusplusElement *>(child);
                std::string body = cppElem->getBody();
                std::string target = cppElem->getTarget();
                if (target != "" && target != "h" && target != "cc") { // target must be a type name
                    std::string qname = prefixWithNamespace(target, currentNamespace);
                    ClassInfo *classInfo = typeTable.findClassInfo(qname);
                    if (classInfo == nullptr)
                        errors->addError(cppElem, "invalid target for cplusplus block: no such type '%s'", qname.c_str());
                    else if (classInfo->isImported)
                        errors->addError(cppElem, "invalid target for cplusplus block: type '%s' is imported", qname.c_str());
                    else if (classInfo->isEnum)
                        errors->addError(cppElem, "invalid target for cplusplus block: type '%s' is enum", qname.c_str());
                    else if (!classInfo->generateClass)
                        errors->addError(cppElem, "invalid target for cplusplus block: class '%s' is not generated", qname.c_str());
                    else if (classExtraCode.find(qname) != classExtraCode.end())
                        errors->addError(cppElem, "invalid target for cplusplus block: duplicate block for type '%s'", qname.c_str());
                    else
                        classExtraCode[qname] = body;
                }
                break;
            }
        }
    }
    if (!currentNamespace.empty()) {
        codegen.generateNamespaceEnd(currentNamespace, false);
        currentNamespace = "";
    }

    // generate classes, C++ blocks, etc.
    for (ASTNode *child = fileElement->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
            case MSG_NAMESPACE: {
                // open namespace(s)
                if (!currentNamespace.empty())
                    codegen.generateNamespaceEnd(currentNamespace);
                NamespaceElement *nsElem = check_and_cast<NamespaceElement *>(child);
                currentNamespace = nsElem->getName();
                codegen.generateNamespaceBegin(currentNamespace);
                break;
            }

            case MSG_CPLUSPLUS: {
                // print C++ block
                CplusplusElement *cppElem = check_and_cast<CplusplusElement *>(child);
                std::string body = cppElem->getBody();
                std::string target = cppElem->getTarget();
                if (target == "" || target == "h" || target == "cc")
                    codegen.generateCplusplusBlock(target, body);
                break;
            }

            case MSG_IMPORT: {
                ImportElement *importElem = check_and_cast<ImportElement *>(child);
                std::string importName = importElem->getImportSpec();
                codegen.generateImport(importName);
                break;
            }

            case MSG_ENUM: {
                EnumElement *enumElem = check_and_cast<EnumElement *>(child);
                std::string qname = prefixWithNamespace(enumElem->getName(), currentNamespace);
                const EnumInfo& enumInfo = typeTable.getEnumInfo(qname);
                codegen.generateEnum(enumInfo);
                break;
            }

            case MSG_STRUCT:
            case MSG_CLASS:
            case MSG_MESSAGE:
            case MSG_PACKET: {
                std::string qname = prefixWithNamespace(child->getAttribute(ATT_NAME), currentNamespace);
                ClassInfo& classInfo = typeTable.getClassInfo(qname);
                analyzer.ensureAnalyzed(classInfo);
                analyzer.ensureFieldsAnalyzed(classInfo);
                if (classInfo.generateClass) {
                    if (isQualified(classInfo.className))
                        errors->addError(classInfo.astNode, "type name may only be qualified when generating descriptor for an existing class: '%s'", classInfo.className.c_str());
                    std::string extraCode = containsKey(classExtraCode, classInfo.qname) ? classExtraCode[classInfo.qname] : "";
                    if (child->getTagCode() == MSG_STRUCT)
                        codegen.generateStruct(classInfo, opts.exportDef, extraCode);
                    else
                        codegen.generateClass(classInfo, opts.exportDef, extraCode);
                }
                if (classInfo.generateDescriptor)
                    codegen.generateDescriptorClass(classInfo);
                break;
            }
        }
    }

    if (!currentNamespace.empty())
        codegen.generateNamespaceEnd(currentNamespace);
    codegen.generateEpilog();
}

void MsgCompiler::validateNamespaceName(const std::string& namespaceName, ASTNode *element)
{
    if (namespaceName.empty())
        errors->addError(element, "namespace name is empty");
    for (auto token : opp_split(namespaceName, "::"))
        if (contains(MsgAnalyzer::RESERVED_WORDS, token))
            errors->addError(element, "namespace name '%s' is a reserved word", token.c_str());
}

}  // namespace nedxml
}  // namespace omnetpp
