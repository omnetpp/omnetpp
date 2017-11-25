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
#include "exception.h"

#include "omnetpp/simkerneldefs.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

//TODO has problems with circular references (stack overflow)
//TODO incorrect decl order (i.e. derived class before base class, or field before its typedef) causes C++ not to compile (fwd decl would be needed)

static const char *BUILTIN_DEFINITIONS =
        R"ENDMARK(
        class __bool { @actually(bool); @primitive; @cpptype(bool); @fromstring(string2bool($)); @tostring(bool2string($)); @defaultvalue(false); }
        class __float { @actually(float); @primitive; @cpptype(float); @fromstring(string2double($)); @tostring(double2string($)); @defaultvalue(0); }
        class __double { @actually(double); @primitive; @cpptype(double); @fromstring(string2double($)); @tostring(double2string($)); @defaultvalue(0); }
        class __string { @actually(string); @primitive; @cpptype(omnetpp::opp_string); @argtype(const char *); @rettype(const char *); @maybe_c_str(.c_str()); @fromstring(($)); @tostring(oppstring2string($)); }
        class __char { @actually(char); @primitive; @cpptype(char); @fromstring(string2long($)); @tostring(long2string($)); @defaultvalue(0); }
        class __short { @actually(short); @primitive; @cpptype(short); @fromstring(string2long($)); @tostring(long2string($)); @defaultvalue(0); }
        class __int { @actually(int); @primitive; @cpptype(int); @fromstring(string2long($)); @tostring(long2string($)); @defaultvalue(0); }
        class __long { @actually(long); @primitive; @cpptype(long); @fromstring(string2long($)); @tostring(long2string($)); @defaultvalue(0); }
        class __uchar { @actually(unsigned char); @primitive; @cpptype(unsigned char); @fromstring(string2ulong($)); @tostring(ulong2string($)); @defaultvalue(0); }
        class __ushort { @actually(unsigned short); @primitive; @cpptype(unsigned short); @fromstring(string2ulong($)); @tostring(ulong2string($)); @defaultvalue(0); }
        class __uint { @actually(unsigned int); @primitive; @cpptype(unsigned int); @fromstring(string2ulong($)); @tostring(ulong2string($)); @defaultvalue(0); }
        class __ulong { @actually(unsigned long); @primitive; @cpptype(unsigned long); @fromstring(string2ulong($)); @tostring(ulong2string($)); @defaultvalue(0); }
        class int8 { @primitive; @cpptype(int8_t); @fromstring(string2long($)); @tostring(long2string($)); @defaultvalue(0); }
        class int8_t { @primitive; @cpptype(int8_t); @fromstring(string2long($)); @tostring(long2string($)); @defaultvalue(0); }
        class int16 { @primitive; @cpptype(int16_t); @fromstring(string2long($)); @tostring(long2string($)); @defaultvalue(0); }
        class int16_t { @primitive; @cpptype(int16_t); @fromstring(string2long($)); @tostring(long2string($)); @defaultvalue(0); }
        class int32 { @primitive; @cpptype(int32_t); @fromstring(string2long($)); @tostring(long2string($)); @defaultvalue(0); }
        class int32_t { @primitive; @cpptype(int32_t); @fromstring(string2long($)); @tostring(long2string($)); @defaultvalue(0); }
        class uint8 { @primitive; @cpptype(uint8_t); @fromstring(string2ulong($)); @tostring(ulong2string($)); @defaultvalue(0); }
        class uint8_t { @primitive; @cpptype(uint8_t); @fromstring(string2ulong($)); @tostring(ulong2string($)); @defaultvalue(0); }
        class uint16 { @primitive; @cpptype(uint16_t); @fromstring(string2ulong($)); @tostring(ulong2string($)); @defaultvalue(0); }
        class uint16_t { @primitive; @cpptype(uint16_t); @fromstring(string2ulong($)); @tostring(ulong2string($)); @defaultvalue(0); }
        class uint32 { @primitive; @cpptype(uint32_t); @fromstring(string2ulong($)); @tostring(ulong2string($)); @defaultvalue(0); }
        class uint32_t { @primitive; @cpptype(uint32_t); @fromstring(string2ulong($)); @tostring(ulong2string($)); @defaultvalue(0); }
        class int64 { @primitive; @cpptype(int64_t); @fromstring(string2int64($)); @tostring(int642string($)); @defaultvalue(0); }
        class int64_t { @primitive; @cpptype(int64_t); @fromstring(string2int64($)); @tostring(int642string($)); @defaultvalue(0); }
        class uint64 { @primitive; @cpptype(uint64_t); @fromstring(string2uint64($)); @tostring(uint642string($)); @defaultvalue(0); }
        class uint64_t { @primitive; @cpptype(uint64_t); @fromstring(string2uint64($)); @tostring(uint642string($)); @defaultvalue(0); }
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

    importBuiltinDefinitions();

    collectTypes(fileElement);

    codegen.openFiles(hFile, ccFile);
    generateCode(fileElement);
    codegen.closeFiles();

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
    ASTNode *tree = parser.parseMSGText(txt, fname);
    if (errors->containsError()) {
        delete tree;
        return;
    }

    typeTable.storeMsgFile(tree); // keep AST until we're done because ClassInfo/FieldInfo refer to it...

    // extract declarations
    MsgFileElement *fileElement = check_and_cast<MsgFileElement*>(tree);
    collectTypes(fileElement);
}

void MsgCompiler::collectTypes(MsgFileElement *fileElement)
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
                ClassInfo classInfo = analyzer.extractClassInfoFromEnum(check_and_cast<EnumElement *>(child), currentNamespace);
                if (typeTable.isClassDefined(classInfo.msgqname))
                    errors->addError(classInfo.astNode, "attempt to redefine '%s'", classInfo.msgname.c_str());
                typeTable.addClass(classInfo);
                break;
            }

            case MSG_STRUCT:
            case MSG_CLASS:
            case MSG_MESSAGE:
            case MSG_PACKET: {
                ClassInfo classInfo = analyzer.extractClassInfo(child, currentNamespace);
                if (typeTable.isClassDefined(classInfo.msgqname) && !containsKey(classInfo.props, str("overwritePreviousDefinition")))
                    errors->addError(classInfo.astNode, "attempt to redefine '%s'", classInfo.msgname.c_str());
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
    ASTNode *tree = parser.parseMSGFile(fileName.c_str());
    if (errors->containsError()) {
        delete tree;
        return;
    }

    //TODO perform all sorts of validations

    typeTable.storeMsgFile(tree); // keep AST until we're done because ClassInfo/FieldInfo refer to it...

    // extract declarations
    MsgFileElement *fileElement = check_and_cast<MsgFileElement*>(tree);
    collectTypes(fileElement);
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

    // generate forward declarations so that cyclic references compile
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
                if (classInfo.generate_class)
                    codegen.generateTypeAnnouncement(classInfo);
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
                else
                    errors->addError(child, "unrecognized target '%s' for cplusplus block", target.c_str());
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
                if (classInfo.generate_class) {
                    if (isQualified(classInfo.msgclass))
                        errors->addError(classInfo.astNode, "type name may only be qualified when generating descriptor for an existing class: '%s'", classInfo.msgclass.c_str());
                    if (child->getTagCode() == MSG_STRUCT)
                        codegen.generateStruct(classInfo, opts.exportDef);
                    else
                        codegen.generateClass(classInfo, opts.exportDef);
                }
                if (classInfo.generate_descriptor)
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
;
