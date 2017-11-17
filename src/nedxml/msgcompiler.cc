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
#include "nedparser.h"
#include "nedexception.h"
#include "nedutil.h"

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
    NEDParser parser(errors);
    NEDElement *tree = parser.parseMSGText(txt, fname);
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
    for (NEDElement *child = fileElement->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
            case NED_NAMESPACE:
                currentNamespace = str(child->getAttribute("name"));
                validateNamespaceName(currentNamespace, child); //TODO into syntax validator class!!!
                break;

            case NED_CPLUSPLUS:
                break;

            case NED_IMPORT: {
                std::string importName = child->getAttribute("import-spec");
                if (!currentNamespace.empty())
                    errors->addError(child, "misplaced import '%s': imports are not allowed within a namespace", importName.c_str()); //TODO into syntax validator class!!!
                if (!common::contains(importsSeen, importName)) {
                    importsSeen.insert(importName);
                    processImport(child, currentDir);
                }
                 break;
             }

            case NED_STRUCT_DECL:
            case NED_CLASS_DECL:
            case NED_MESSAGE_DECL:
            case NED_PACKET_DECL:
                errors->addWarning(child, "type declarations are not needed with imports, and will be ignored"); //TODO into syntax validator class!!!
                break;

            case NED_ENUM_DECL: { // for enums already defined and registered in C++
                EnumInfo enumInfo = analyzer.extractEnumDecl(check_and_cast<EnumDeclElement *>(child), currentNamespace);
                if (!typeTable.isEnumDefined(enumInfo.enumQName))
                    typeTable.addEnum(enumInfo);
                break;
            }

            case NED_ENUM: {
                EnumInfo enumInfo = analyzer.extractEnumInfo(check_and_cast<EnumElement *>(child), currentNamespace);
                if (isQualified(enumInfo.enumName))
                    errors->addError(enumInfo.nedElement, "type name may not be qualified: '%s'", enumInfo.enumName.c_str()); //TODO into some validator class
                if (typeTable.isEnumDefined(enumInfo.enumQName))
                    errors->addError(enumInfo.nedElement, "attempt to redefine '%s'", enumInfo.enumName.c_str());
                typeTable.addEnum(enumInfo);
                ClassInfo classInfo = analyzer.extractClassInfoFromEnum(check_and_cast<EnumElement *>(child), currentNamespace);
                if (typeTable.isClassDefined(classInfo.msgqname))
                    errors->addError(classInfo.nedElement, "attempt to redefine '%s'", classInfo.msgname.c_str());
                typeTable.addClass(classInfo);
                break;
            }

            case NED_STRUCT:
            case NED_CLASS:
            case NED_MESSAGE:
            case NED_PACKET: {
                ClassInfo classInfo = analyzer.makeIncompleteClassInfo(child, currentNamespace);
                if (typeTable.isClassDefined(classInfo.msgqname) && !containsKey(classInfo.props, str("overwritePreviousDefinition")))
                    errors->addError(classInfo.nedElement, "attempt to redefine '%s'", classInfo.msgname.c_str());
                typeTable.addClass(classInfo);
                break;
            }
        }
    }
}

void MsgCompiler::processImport(NEDElement *child, const std::string& currentDir)
{
    std::string importName = child->getAttribute("import-spec");
    std::string fileName = resolveImport(importName, currentDir);
    if (fileName == "") {
        errors->addError(child, "cannot resolve import '%s'", importName.c_str());
        return;
    }

    importedFiles.insert(fileName);

    NEDParser parser(errors);
    NEDElement *tree = parser.parseMSGFile(fileName.c_str());
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
    NamespaceElement *firstNS = fileElement->getFirstNamespaceChild();
    std::string firstNSName = firstNS ? str(firstNS->getAttribute("name")) : "";
    codegen.generateProlog(fileElement->getFilename(), firstNSName, opts.exportDef);

    for (NEDElement *child = fileElement->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
            case NED_NAMESPACE:
                // open namespace(s)
                if (!currentNamespace.empty())
                    codegen.generateNamespaceEnd(currentNamespace);
                currentNamespace = str(child->getAttribute("name"));
                codegen.generateNamespaceBegin(currentNamespace);
                break;

            case NED_CPLUSPLUS: {
                // print C++ block
                std::string body = str(child->getAttribute("body"));
                std::string target = str(child->getAttribute("target"));
                if (target == "" || target == "h" || target == "cc")
                    codegen.generateCplusplusBlock(target, body);
                else
                    errors->addError(child, "unrecognized target '%s' for cplusplus block", target.c_str());
                break;
            }

            case NED_IMPORT: {
                std::string importName = child->getAttribute("import-spec");
                codegen.generateImport(importName);
                break;
            }

            case NED_ENUM: {
                std::string qname = prefixWithNamespace(str(child->getAttribute("name")), currentNamespace);
                const EnumInfo& enumInfo = typeTable.getEnumInfo(qname);
                codegen.generateEnum(enumInfo);
                break;
            }

            case NED_STRUCT:
            case NED_CLASS:
            case NED_MESSAGE:
            case NED_PACKET: {
                std::string qname = prefixWithNamespace(str(child->getAttribute("name")), currentNamespace);
                ClassInfo& classInfo = typeTable.getClassInfo(qname);
                analyzer.ensureAnalyzed(classInfo);
                analyzer.ensureFieldsAnalyzed(classInfo);
                if (classInfo.generate_class) {
                    if (isQualified(classInfo.msgclass))
                        errors->addError(classInfo.nedElement, "type name may only be qualified when generating descriptor for an existing class: '%s'", classInfo.msgclass.c_str());
                    if (child->getTagCode() == NED_STRUCT)
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

void MsgCompiler::validateNamespaceName(const std::string& namespaceName, NEDElement *element)
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
