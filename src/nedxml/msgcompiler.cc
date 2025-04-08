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

const char *MsgCompiler::BUILTIN_DEFINITIONS =
        R"ENDMARK(
        @property[property](type=any; usage=file; desc="Property for declaring properties.");
        @property[customize](type=bool; usage=class; desc="If true: Customize the class via inheritance. Generates base class <name>_Base.");
        @property[abstract](type=bool; usage=field,class; desc="If true: For a class, it indicates that it is an abstract class in the C++ sense (one which cannot be instantiated). For a field, it requests that the accessor methods for the field be made pure virtual and no data member be generated; it also makes the class that contains the field abstract (unless the class has @customize whereas it is assumed that the custom code implements the pure virtual member functions).");
        @property[str](type=string; usage=class; desc="Expression to be returned from the generated str() method.");
        @property[primitive](type=bool; usage=field,class; desc="Shortcut for @opaque @byValue @editable @subclassable(false) @supportsPtr(false).");
        @property[opaque](type=bool; usage=field,class; desc="Affects descriptor class only. If true: Treat the field as atomic (non-compound) type, i.e. having no descriptor class. When specified on a class, it determines the default for fields of that type.");
        @property[byValue](type=bool; usage=field,class; desc="If true: Causes the value to be passed by value (instead of by reference) in setters/getters. When specified on a class, it determines the default for fields of that type.");
        @property[supportsPtr](type=bool; usage=field,class; desc="Specifies whether this type supports creating a pointer (or pointer array) from it.");
        @property[subclassable](type=bool; usage=class; desc="Specifies whether this type can be subclassed (e.g. C++ primitive types and final classes cannot).");
        @property[polymorphic](type=bool; usage=class; desc="Specifies whether this type is polymorphic, i.e. has any virtual member function.");
        @property[class](type=bool; usage=enum; desc="Specifies whether this enum is to be declared as 'enum class'.");
        @property[baseType](type=string; usage=enum; desc="Specifies the underlying data type for the enum.");
        @property[defaultValue](type=string; usage=class; desc="Default value for fields of this type.");
        @property[cppType](type=string; usage=field,class; desc="C++ datatype. Provides a common default for @datamemberType, @argType and @returnType. When specified on a class, it determines the default for fields of that type.");
        @property[datamemberType](type=string; usage=field,class; desc="Data member C++ base data type. This type is decorated with '*' if the field is a pointer. When specified on a class, it determines the default for fields of that type.");
        @property[argType](type=string; usage=field,class; desc="Field setter argument C++ base type. This type may be decorated with 'const' and '*'/'&' to produce the actual argument type. When specified on a class, it determines the default for fields of that type.");
        @property[returnType](type=string; usage=field,class; desc="Field getter C++ base return type. This type may be decorated with 'const' and '*'/'&' to produce the actual return type. When specified on a class, it determines the default for fields of that type.");
        @property[fromString](type=string; usage=field,class; desc="Affects descriptor class only. Method name, code fragment or funcall template to convert string to field value. When specified on a class, it determines the default for fields of that type.");
        @property[toString](type=string; usage=field,class; desc="Affects descriptor class only. Method name, code fragment or funcall template to convert field value to string. When specified on a class, it determines the default for fields of that type.");
        @property[fromValue](type=string; usage=field,class; desc="Affects descriptor class only. Method name, code fragment or funcall template to convert cValue to field value. When specified on a class, it determines the default for fields of that type.");
        @property[toValue](type=string; usage=field,class; desc="Affects descriptor class only. Method name, code fragment or funcall template to convert field value to cValue. When specified on a class, it determines the default for fields of that type.");
        @property[getterConversion](type=string; usage=field,class; desc="Method name, code fragment or funcall template to convert field data type to the return type in getters. When specified on a class, it determines the default for fields of that type.");
        @property[clone](type=string; usage=field,class; desc="For owned pointer fields: Code to duplicate (one array element of) the field value. When specified on a class, it determines the default for fields of that type.");
        @property[existingClass](type=bool; usage=class; desc="If true: This is a type is already defined in C++, i.e. it does not need to be generated.");
        @property[descriptor](type=string; usage=class; desc="A 'true'/'false' value specifies whether to generate descriptor class; special value 'readonly' requests generating a read-only descriptor (but specifying @editable/@replaceable/@resizable on individual fields overrides that).");
        @property[castFunction](type=bool; usage=class; desc="If false: Do not specialize the fromAnyPtr<T>(any_ptr) function for this class. Useful for preventing compile errors if the function already exists, e.g. in hand-written form, or generated for another type (think aliased typedefs).");
        @property[omitGetVerb](type=bool; usage=class; desc="If true: Drop the 'get' verb from the names of getter methods.");
        @property[fieldNameSuffix](type=string; usage=class; desc="Suffix to append to the names of data members.");
        @property[beforeChange](type=string; usage=class; desc="Method to be called before mutator code (in setters, non-const getters, operator=, etc.).");
        @property[implements](type=stringlist; usage=class; desc="Names of additional base classes.");
        @property[nopack](type=bool; usage=field; desc="If true: Ignore this field in parsimPack/parsimUnpack methods.");
        @property[editable](type=bool; usage=field,class; desc="Affects descriptor class only. If true: Value of the field (or value of fields that are instances of this type) can be set via the class descriptor's setFieldValueFromString() and setFieldValue() methods.");
        @property[replaceable](type=bool; usage=field; desc="Affects descriptor class only. If true: Field is a pointer whose value can be set via the class descriptor's setFieldStructValuePointer() and setFieldValue() methods.");
        @property[resizable](type=bool; usage=field; desc="Affects descriptor class only. If true: Field is a variable-size array whose size can be set via the class descriptor's setFieldArraySize() method.");
        @property[readonly](type=bool; usage=field; desc="Affects descriptor class only. Equivalent to @editable(false) @replaceable(false) @resizable(false).");
        @property[overrideGetter](type=bool; usage=field; desc="If true: Add the 'override' keyword to the declaration of the getter method.");
        @property[overrideSetter](type=bool; usage=field; desc="If true: Add the 'override' keyword to the declaration of the setter method.");
        @property[enum](type=string; usage=field; desc="For integer fields: Values are from the given enum.");
        @property[sizeType](type=string; usage=field; desc="C++ type to use for array sizes and indices.");
        @property[setter](type=string; usage=field; desc="Name of the setter method. When generating a descriptor for an existing class (see @existingClass), a code fragment or funcall template for the equivalent functionality is also accepted.");
        @property[getter](type=string; usage=field; desc="Name of the (const) getter method. When generating a descriptor for an existing class (see @existingClass), a code fragment or funcall template for the equivalent functionality is also accepted.");
        @property[getterForUpdate](type=string; usage=field; desc="Name of the non-const getter method. When generating a descriptor for an existing class (see @existingClass), a code fragment or funcall template for the equivalent functionality is also accepted.");
        @property[sizeSetter](type=string; usage=field; desc="Name of the method that sets size of dynamic array. When generating a descriptor for an existing class (see @existingClass), a code fragment or funcall template for the equivalent functionality is also accepted.");
        @property[sizeGetter](type=string; usage=field; desc="Name of the method that returns the array size. When generating a descriptor for an existing class (see @existingClass), a code fragment or funcall template for the equivalent functionality is also accepted.");
        @property[inserter](type=string; usage=field; desc="Name of the inserter method. (This method inserts an element into a dynamic array.) When generating a descriptor for an existing class (see @existingClass), a code fragment or funcall template for the equivalent functionality is also accepted.");
        @property[appender](type=string; usage=field; desc="Name of the appender method. (This method appends an element to a dynamic array.) When generating a descriptor for an existing class (see @existingClass), a code fragment or funcall template for the equivalent functionality is also accepted.");
        @property[eraser](type=string; usage=field; desc="Name of the eraser method. (This method erases an element from a dynamic array. Indices above the specified one are shifted down.) When generating a descriptor for an existing class (see @existingClass), a code fragment or funcall template for the equivalent functionality is also accepted.");
        @property[remover](type=string; usage=field; desc="Name of the remover method. (This method is generated for owned pointer fields. It releases the ownership of the stored object, sets the field to nullptr, then returns the object.) When generating a descriptor for an existing class (see @existingClass), a code fragment or funcall template for the equivalent functionality is also accepted.");
        @property[allowReplace](type=bool; usage=field; desc="Specifies whether the setter method of an owned pointer field is allowed to delete the previously set object.");
        @property[actually](type=string; usage=class; internal=true; desc="Internal use.");
        @property[overwritePreviousDefinition](type=bool; usage=class; internal=true; desc="Internal use.");
        @property[owned](type=bool; usage=field; desc="For pointers and pointer arrays: Whether allocated memory is owned by the object (needs to be duplicated in dup(), and deleted in destructor). If field type is also a cOwnedObject, take()/drop() calls are also generated.");
        @property[custom](type=bool; usage=field; desc="If true: Do not generate any data or code for the field, only add it to the descriptor. Indicates that the field's implementation will be added to the class via targeted cplusplus blocks.");
        @property[customImpl](type=bool; usage=field; desc="If true: Do not generate implementations for the field's accessor methods, to allow custom implementations to be supplied by the user via cplusplus blocks or in separate .cc files.");

        class __bool { @actually(bool); @primitive; @fromString(string2bool($)); @toString(bool2string($)); @defaultValue(false); @toValue($); @fromValue($.boolValue()); }
        class __float { @actually(float); @primitive; @fromString(string2double($)); @toString(double2string($)); @defaultValue(0); @toValue((double)($)); @fromValue(static_cast<float>($.doubleValue())); }
        class __double { @actually(double); @primitive; @fromString(string2double($)); @toString(double2string($)); @defaultValue(0); @toValue($); @fromValue($.doubleValue()); }
        class __string { @actually(string); @primitive; @cppType(omnetpp::opp_string); @argType(const char *); @returnType(const char *); @getterConversion(.c_str()); @fromString(($)); @toString(oppstring2string($)); @toValue($); @fromValue($.stringValue()); }
        class __char { @actually(char); @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); @toValue($); @fromValue(omnetpp::checked_int_cast<char>($.intValue())); }
        class __short { @actually(short); @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); @toValue($); @fromValue(omnetpp::checked_int_cast<short>($.intValue())); }
        class __int { @actually(int); @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); @toValue($); @fromValue(omnetpp::checked_int_cast<int>($.intValue())); }
        class __long { @actually(long); @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); @toValue((omnetpp::intval_t)($)); @fromValue(omnetpp::checked_int_cast<long>($.intValue())); }
        class __uchar { @actually(unsigned char); @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); @toValue((omnetpp::intval_t)($)); @fromValue(omnetpp::checked_int_cast<unsigned char>($.intValue())); }
        class __ushort { @actually(unsigned short); @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); @toValue((omnetpp::intval_t)($)); @fromValue(omnetpp::checked_int_cast<unsigned short>($.intValue())); }
        class __uint { @actually(unsigned int); @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); @toValue((omnetpp::intval_t)($)); @fromValue(omnetpp::checked_int_cast<unsigned int>($.intValue())); }
        class __ulong { @actually(unsigned long); @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); @toValue(omnetpp::checked_int_cast<omnetpp::intval_t>($)); @fromValue(omnetpp::checked_int_cast<unsigned long>($.intValue())); }
        class int8_t { @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); @toValue($); @fromValue(omnetpp::checked_int_cast<int8_t>($.intValue())); }
        class int16_t { @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); @toValue($); @fromValue(omnetpp::checked_int_cast<int16_t>($.intValue())); }
        class int32_t { @primitive; @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); @toValue($); @fromValue(omnetpp::checked_int_cast<int32_t>($.intValue())); }
        class int64_t { @primitive; @fromString(string2int64($)); @toString(int642string($)); @defaultValue(0); @toValue($); @fromValue(omnetpp::checked_int_cast<int64_t>($.intValue())); }
        class uint8_t { @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); @toValue((omnetpp::intval_t)($)); @fromValue(omnetpp::checked_int_cast<uint8_t>($.intValue())); }
        class uint16_t { @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); @toValue((omnetpp::intval_t)($)); @fromValue(omnetpp::checked_int_cast<uint16_t>($.intValue())); }
        class uint32_t { @primitive; @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); @toValue((omnetpp::intval_t)($)); @fromValue(omnetpp::checked_int_cast<uint32_t>($.intValue())); }
        class uint64_t { @primitive; @fromString(string2uint64($)); @toString(uint642string($)); @defaultValue(0); @toValue((omnetpp::intval_t)($)); @fromValue(omnetpp::checked_int_cast<uint64_t>($.intValue())); }
        class int8 { @primitive; @cppType(int8_t); @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); @toValue($); @fromValue(omnetpp::checked_int_cast<int8_t>($.intValue())); }
        class int16 { @primitive; @cppType(int16_t); @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); @toValue($); @fromValue(omnetpp::checked_int_cast<int16_t>($.intValue())); }
        class int32 { @primitive; @cppType(int32_t); @fromString(string2long($)); @toString(long2string($)); @defaultValue(0); @toValue($); @fromValue(omnetpp::checked_int_cast<int32_t>($.intValue())); }
        class int64 { @primitive; @cppType(int64_t); @fromString(string2int64($)); @toString(int642string($)); @defaultValue(0); @toValue((omnetpp::intval_t)($)); @fromValue(omnetpp::checked_int_cast<int64_t>($.intValue())); }
        class uint8 { @primitive; @cppType(uint8_t); @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); @toValue((omnetpp::intval_t)($)); @fromValue(omnetpp::checked_int_cast<uint8_t>($.intValue())); }
        class uint16 { @primitive; @cppType(uint16_t); @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); @toValue((omnetpp::intval_t)($)); @fromValue(omnetpp::checked_int_cast<uint16_t>($.intValue())); }
        class uint32 { @primitive; @cppType(uint32_t); @fromString(string2ulong($)); @toString(ulong2string($)); @defaultValue(0); @toValue((omnetpp::intval_t)($)); @fromValue(omnetpp::checked_int_cast<uint32_t>($.intValue())); }
        class uint64 { @primitive; @cppType(uint64_t); @fromString(string2uint64($)); @toString(uint642string($)); @defaultValue(0); @toValue(omnetpp::checked_int_cast<omnetpp::intval_t>($)); @fromValue(omnetpp::checked_int_cast<uint64_t>($.intValue())); }
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

MsgCompiler::MsgCompiler(const MsgCompilerOptions& opts, ErrorStore *errors) :
    opts(opts), analyzer(opts, &typeTable, errors), codegen(errors), errors(errors)
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

            case MSG_PROPERTY: {
                PropertyElement *propertyElem = check_and_cast<PropertyElement *>(child);
                Property property = analyzer.extractProperty(propertyElem);
                typeTable.addGlobalProperty(property);
                analyzer.validateFileProperty(property); // do not swap with previous
                break;
            }

            case MSG_STRUCT_DECL:
            case MSG_CLASS_DECL:
            case MSG_MESSAGE_DECL:
            case MSG_PACKET_DECL:
                errors->addError(child, "Type declarations are not needed with imports, try invoking the message compiler in legacy (4.x) mode using the --msg4 option");
                break;

            case MSG_ENUM_DECL:
            case MSG_ENUM: {
                ClassInfo enumInfo = analyzer.extractEnumInfo(child, currentNamespace);
                if (enumInfo.isDeclaration) {
                    if (!typeTable.isClassDefined(enumInfo.qname))
                        typeTable.addClass(enumInfo);
                }
                else {
                    if (isQualified(enumInfo.name))
                        errors->addError(enumInfo.astNode, "type name may not be qualified: '%s'", enumInfo.name.c_str()); //TODO into some validator class
                    if (typeTable.isClassDefined(enumInfo.qname))
                        errors->addError(enumInfo.astNode, "attempt to redefine '%s'", enumInfo.qname.c_str());
                    typeTable.addClass(enumInfo);
                }
                break;
            }

            case MSG_STRUCT:
            case MSG_CLASS:
            case MSG_MESSAGE:
            case MSG_PACKET: {
                ClassInfo classInfo = analyzer.extractClassInfo(child, currentNamespace, isImported);
                if (typeTable.isClassDefined(classInfo.qname) && !classInfo.props.contains(MsgAnalyzer::PROP_OVERWRITEPREVIOUSDEFINITION))
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
    std::map<std::string, std::map<std::string, std::string>> classExtraCode; //per-class, per-method

    // generate forward declarations so that cyclic references compile; also collect cplusplus blocks
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
                if (classInfo.generateTypeDefinition)
                    codegen.generateTypeAnnouncement(classInfo);
                break;
            }
            case MSG_CPLUSPLUS: {
                CplusplusElement *cppElem = check_and_cast<CplusplusElement *>(child);
                analyzer.analyzeCplusplusBlockTarget(cppElem, currentNamespace);
                break;
            }
        }
    }
    if (!currentNamespace.empty()) {
        codegen.generateNamespaceEnd(currentNamespace, false);
        currentNamespace = "";
    }

    // generate classes, C++ blocks, etc.
    std::vector<ClassInfo*> types;
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
                if (target == "" || target == "h")
                    codegen.generateHeaderCplusplusBlock(body);
                else if (target == "cc")
                    codegen.generateImplCplusplusBlock(body);
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
                const ClassInfo& classInfo = typeTable.getClassInfo(qname);
                codegen.generateEnum(classInfo);
                break;
            }

            case MSG_STRUCT:
            case MSG_CLASS:
            case MSG_MESSAGE:
            case MSG_PACKET: {
                std::string qname = prefixWithNamespace(child->getAttribute(ATT_NAME), currentNamespace);
                ClassInfo& classInfo = typeTable.getClassInfo(qname);
                types.push_back(&classInfo);
                analyzer.ensureAnalyzed(classInfo);
                analyzer.ensureFieldsAnalyzed(classInfo);
                if (errors->containsError())
                    break; // prevent asserts/errors from firing during code generation
                if (classInfo.generateTypeDefinition) {
                    if (isQualified(classInfo.className))
                        errors->addError(classInfo.astNode, "type name may only be qualified when generating descriptor for an existing class: '%s'", classInfo.className.c_str());
                    if (child->getTagCode() == MSG_STRUCT)
                        codegen.generateStruct(classInfo, opts.exportDef);
                    else
                        codegen.generateClass(classInfo, opts.exportDef);
                }
                if (classInfo.generateDescriptor)
                    codegen.generateDescriptorClass(classInfo);
                break;
            }
        }
    }

    if (!currentNamespace.empty())
        codegen.generateNamespaceEnd(currentNamespace);

    // generate cast functions from/to any_ptr
    codegen.generateNamespaceBegin("omnetpp");
    for (ClassInfo *classInfo: types) {
        if (classInfo->generateCastFunction) {
            int numBases = (classInfo->baseClass.empty() ? 0 : 1) + classInfo->implementsQNames.size();
            if (numBases != 1)  // for numBases >= 2, we must generate one to avoid ambiguity among toAnytPtr() functions for its base classes
                codegen.generateToAnyPtr(*classInfo);
            codegen.generateFromAnyPtr(*classInfo, opts.exportDef);
        }
    }
    codegen.generateNamespaceEnd("omnetpp");

    codegen.generateEpilog();
}

void MsgCompiler::validateNamespaceName(const std::string& namespaceName, ASTNode *element)
{
    // note: namespaceName may be empty (for returning to the global namespace)
    for (auto token : opp_split(namespaceName, "::"))
        if (contains(MsgAnalyzer::RESERVED_WORDS, token))
            errors->addError(element, "namespace name '%s' is a reserved word", token.c_str());
}

void MsgCompiler::printPropertiesLatexDocu(std::ostream& out)
{
    if (used)
        throw opp_runtime_error("MsgCompiler is a one-shot object, make a new instance to compile another file");
    used = true;

    importBuiltinDefinitions();

    std::map<std::string,const Property*> sortedPropertyDecls;
    auto& properties = typeTable.getGlobalProperties().getAll();
    for (const Property& p : properties)
        if (p.getName() == "property" && opp_join(p.getValue("internal"),"") != "true")
            sortedPropertyDecls[p.getIndex()] = &p;

    out << "\\begin{description}\n";
    for (auto pair : sortedPropertyDecls) {
        const Property *p = pair.second;
        out << "\\item[@" << p->getIndex() << "] ";
        out << "\\textit{(type: " << opp_join(p->getValue("type"), "") << ", ";
        out << "use: " << opp_join(p->getValue("usage"), ", ") << ")} \\\\\n";
        out << opp_indentlines(opp_breaklines(opp_latexquote(opp_join(p->getValue("desc"), "")), 76), "  ") << "\n\n";
    }
    out << "\\end{description}\n";
}

}  // namespace nedxml
}  // namespace omnetpp
