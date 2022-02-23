//==========================================================================
//  MSGCODEGENERATOR.CC - part of
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

#include <algorithm>
#include <cctype>

#include "common/stringutil.h"
#include "common/stlutil.h"
#include "omnetpp/platdep/platmisc.h"  // unlink()
#include "msgcodegenerator.h"
#include "msggenerator.h"
#include "exception.h"
#include "nedutil.h"

#include "omnetpp/simkerneldefs.h"


//TODO bug: dup() and parsimPack() generated into non-cObject classes ("class X {}")

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

using std::ostream;

#define H     hStream
#define CC    ccStream

#define PROGRAM    "opp_msgtool"

inline std::string str(const char *s)
{
    return s;
}

inline std::string TS(const std::string& s)
{
    return s.empty() ? s : s + " ";
}

static char charToNameFilter(char ch)
{
    return (isalnum(ch)) ? ch : '_';
}

inline std::ostream& operator<<(std::ostream& o, const std::pair<std::string, int>& p)
{
    return o << '(' << p.first << ':' << p.second << ')';
}

void MsgCodeGenerator::openFiles(const char *hFile, const char *ccFile)
{
    hFilename = hFile;
    ccFilename = ccFile;

    hStream.open(hFile);
    if (hStream.fail())
        throw opp_runtime_error("Cannot open '%s' for write", hFile);

    ccStream.open(ccFile);
    if (ccStream.fail())
        throw opp_runtime_error("Cannot open '%s' for write", ccFile);
}

void MsgCodeGenerator::closeFiles()
{
    hStream.close();
    ccStream.close();
    if (!hStream)
        throw opp_runtime_error("Could not write '%s'", hFilename.c_str());
    if (!ccStream)
        throw opp_runtime_error("Could not write '%s'", ccFilename.c_str());
}

void MsgCodeGenerator::deleteFiles()
{
    unlink(hFilename.c_str());
    unlink(ccFilename.c_str());
}

static const char *PARSIMPACK_BOILERPLATE =
    "namespace omnetpp {\n"
    "\n"
    "// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.\n"
    "// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument\n"
    "\n"
    "// Packing/unpacking an std::vector\n"
    "template<typename T, typename A>\n"
    "void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)\n"
    "{\n"
    "    int n = v.size();\n"
    "    doParsimPacking(buffer, n);\n"
    "    for (int i = 0; i < n; i++)\n"
    "        doParsimPacking(buffer, v[i]);\n"
    "}\n"
    "\n"
    "template<typename T, typename A>\n"
    "void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)\n"
    "{\n"
    "    int n;\n"
    "    doParsimUnpacking(buffer, n);\n"
    "    v.resize(n);\n"
    "    for (int i = 0; i < n; i++)\n"
    "        doParsimUnpacking(buffer, v[i]);\n"
    "}\n"
    "\n"
    "// Packing/unpacking an std::list\n"
    "template<typename T, typename A>\n"
    "void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)\n"
    "{\n"
    "    doParsimPacking(buffer, (int)l.size());\n"
    "    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)\n"
    "        doParsimPacking(buffer, (T&)*it);\n"
    "}\n"
    "\n"
    "template<typename T, typename A>\n"
    "void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)\n"
    "{\n"
    "    int n;\n"
    "    doParsimUnpacking(buffer, n);\n"
    "    for (int i = 0; i < n; i++) {\n"
    "        l.push_back(T());\n"
    "        doParsimUnpacking(buffer, l.back());\n"
    "    }\n"
    "}\n"
    "\n"
    "// Packing/unpacking an std::set\n"
    "template<typename T, typename Tr, typename A>\n"
    "void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)\n"
    "{\n"
    "    doParsimPacking(buffer, (int)s.size());\n"
    "    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)\n"
    "        doParsimPacking(buffer, *it);\n"
    "}\n"
    "\n"
    "template<typename T, typename Tr, typename A>\n"
    "void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)\n"
    "{\n"
    "    int n;\n"
    "    doParsimUnpacking(buffer, n);\n"
    "    for (int i = 0; i < n; i++) {\n"
    "        T x;\n"
    "        doParsimUnpacking(buffer, x);\n"
    "        s.insert(x);\n"
    "    }\n"
    "}\n"
    "\n"
    "// Packing/unpacking an std::map\n"
    "template<typename K, typename V, typename Tr, typename A>\n"
    "void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)\n"
    "{\n"
    "    doParsimPacking(buffer, (int)m.size());\n"
    "    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {\n"
    "        doParsimPacking(buffer, it->first);\n"
    "        doParsimPacking(buffer, it->second);\n"
    "    }\n"
    "}\n"
    "\n"
    "template<typename K, typename V, typename Tr, typename A>\n"
    "void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)\n"
    "{\n"
    "    int n;\n"
    "    doParsimUnpacking(buffer, n);\n"
    "    for (int i = 0; i < n; i++) {\n"
    "        K k; V v;\n"
    "        doParsimUnpacking(buffer, k);\n"
    "        doParsimUnpacking(buffer, v);\n"
    "        m[k] = v;\n"
    "    }\n"
    "}\n"
    "\n"
    "// Default pack/unpack function for arrays\n"
    "template<typename T>\n"
    "void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)\n"
    "{\n"
    "    for (int i = 0; i < n; i++)\n"
    "        doParsimPacking(b, t[i]);\n"
    "}\n"
    "\n"
    "template<typename T>\n"
    "void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)\n"
    "{\n"
    "    for (int i = 0; i < n; i++)\n"
    "        doParsimUnpacking(b, t[i]);\n"
    "}\n"
    "\n"
    "// Default rule to prevent compiler from choosing base class' doParsimPacking() function\n"
    "template<typename T>\n"
    "void doParsimPacking(omnetpp::cCommBuffer *, const T& t)\n"
    "{\n"
    "    throw omnetpp::cRuntimeError(\"Parsim error: No doParsimPacking() function for type %s\", omnetpp::opp_typename(typeid(t)));\n"
    "}\n"
    "\n"
    "template<typename T>\n"
    "void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)\n"
    "{\n"
    "    throw omnetpp::cRuntimeError(\"Parsim error: No doParsimUnpacking() function for type %s\", omnetpp::opp_typename(typeid(t)));\n"
    "}\n"
    "\n"
    "}  // namespace omnetpp\n"
    "\n";

void MsgCodeGenerator::generateProlog(const std::string& msgFileName, const std::string& firstNamespace, const std::string& exportDef)
{
    // make header guard using the file name
    std::string hfilenamewithoutdir = hFilename;
    size_t pos = hfilenamewithoutdir.find_last_of('/');
    if (pos != hfilenamewithoutdir.npos)
        hfilenamewithoutdir = hfilenamewithoutdir.substr(pos+1);

    headerGuard = hfilenamewithoutdir;

    // add first namespace to headerguard
    if (!firstNamespace.empty())
        headerGuard = firstNamespace + "_" + headerGuard;

    // replace non-alphanumeric characters by '_'
    std::transform(headerGuard.begin(), headerGuard.end(), headerGuard.begin(), charToNameFilter);
    // capitalize
    std::transform(headerGuard.begin(), headerGuard.end(), headerGuard.begin(), ::toupper);
    headerGuard = str("__") + headerGuard;

    H << "//\n// Generated file, do not edit! Created by " PROGRAM " " << (OMNETPP_VERSION / 0x100) << "." << (OMNETPP_VERSION % 0x100)
                  << " from " << msgFileName << ".\n//\n\n";
    H << "#ifndef " << headerGuard << "\n";
    H << "#define " << headerGuard << "\n\n";
    H << "#if defined(__clang__)\n";
    H << "#  pragma clang diagnostic ignored \"-Wreserved-id-macro\"\n";
    H << "#endif\n";
    H << "#include <omnetpp.h>\n";
    H << "\n";
    H << "// " PROGRAM " version check\n";
    H << "#define MSGC_VERSION 0x" << std::hex << std::setfill('0') << std::setw(4) << OMNETPP_VERSION << "\n";
    H << "#if (MSGC_VERSION!=OMNETPP_VERSION)\n";
    H << "#    error Version mismatch! Probably this file was generated by an earlier version of " PROGRAM ": 'make clean' should help.\n";
    H << "#endif\n";
    H << "\n";

    if (exportDef.length() > 4 && opp_stringendswith(exportDef.c_str(), "_API")) {
        // # generate boilerplate code for dll export
        std::string exportbase = exportDef.substr(0, exportDef.length()-4);
        H << "// dll export symbol\n";
        H << "#ifndef " << exportDef << "\n";
        H << "#  if defined(" << exportbase << "_EXPORT)\n";
        H << "#    define " << exportDef << "  OPP_DLLEXPORT\n";
        H << "#  elif defined(" << exportbase << "_IMPORT)\n";
        H << "#    define " << exportDef << "  OPP_DLLIMPORT\n";
        H << "#  else\n";
        H << "#    define " << exportDef << "\n";
        H << "#  endif\n";
        H << "#endif\n";
        H << "\n";
    }

    CC << "//\n// Generated file, do not edit! Created by " PROGRAM " " << (OMNETPP_VERSION / 0x100) << "." << (OMNETPP_VERSION % 0x100) << " from " << msgFileName << ".\n//\n\n";
    CC << "// Disable warnings about unused variables, empty switch stmts, etc:\n";
    CC << "#ifdef _MSC_VER\n";
    CC << "#  pragma warning(disable:4101)\n";
    CC << "#  pragma warning(disable:4065)\n";
    CC << "#endif\n\n";

    CC << "#if defined(__clang__)\n";
    CC << "#  pragma clang diagnostic ignored \"-Wshadow\"\n";
    CC << "#  pragma clang diagnostic ignored \"-Wconversion\"\n";
    CC << "#  pragma clang diagnostic ignored \"-Wunused-parameter\"\n";
    CC << "#  pragma clang diagnostic ignored \"-Wc++98-compat\"\n";
    CC << "#  pragma clang diagnostic ignored \"-Wunreachable-code-break\"\n";
    CC << "#  pragma clang diagnostic ignored \"-Wold-style-cast\"\n";
    CC << "#elif defined(__GNUC__)\n";
    CC << "#  pragma GCC diagnostic ignored \"-Wshadow\"\n";
    CC << "#  pragma GCC diagnostic ignored \"-Wconversion\"\n";
    CC << "#  pragma GCC diagnostic ignored \"-Wunused-parameter\"\n";
    CC << "#  pragma GCC diagnostic ignored \"-Wold-style-cast\"\n";
    CC << "#  pragma GCC diagnostic ignored \"-Wsuggest-attribute=noreturn\"\n";
    CC << "#  pragma GCC diagnostic ignored \"-Wfloat-conversion\"\n";
    CC << "#endif\n\n";

    CC << "#include <iostream>\n";
    CC << "#include <sstream>\n";
    CC << "#include <memory>\n";
    CC << "#include <type_traits>\n";
    CC << "#include \"" << hfilenamewithoutdir << "\"\n\n";

    CC << PARSIMPACK_BOILERPLATE;

}

void MsgCodeGenerator::generateEpilog()
{
    H << "#endif // ifndef " << headerGuard << "\n\n";
}

std::string MsgCodeGenerator::generatePreComment(ASTNode *nedElement)
{
    // reproduce original source
    std::ostringstream s;
    MsgGenerator().generate(s, nedElement, "");
    std::string str = s.str();

    // print it inside a comment
    std::ostringstream o;
    o << " * <pre>\n";
    o << opp_indentlines(opp_trim(opp_replacesubstring(opp_replacesubstring(str, "*/", "  ", true), "@", "\\@", true)), " * ");
    o << "\n";
    o << " * </pre>\n";
    return o.str();
}

void MsgCodeGenerator::generateClass(const ClassInfo& classInfo, const std::string& exportDef)
{
    generateClassDecl(classInfo, exportDef);
    generateClassImpl(classInfo);
}

void MsgCodeGenerator::generateClassDecl(const ClassInfo& classInfo, const std::string& exportDef)
{
    H << "/**\n";
    H << " * Class generated from <tt>" << classInfo.astNode->getSourceLocation().str() << "</tt> by " PROGRAM ".\n";
    H << generatePreComment(classInfo.astNode);

    if (classInfo.customize) {
        H << " *\n";
        H << " * " << classInfo.className << " is only useful if it gets subclassed, and " << classInfo.realClass << " is derived from it.\n";
        H << " * The minimum code to be written for " << classInfo.realClass << " is the following:\n";
        H << " *\n";
        H << " * <pre>\n";
        H << " * class " << TS(exportDef) << classInfo.realClass << " : public " << classInfo.className << "\n";
        H << " * {\n";
        H << " *   private:\n";
        H << " *     void copy(const " << classInfo.realClass << "& other) { ... }\n\n";
        H << " *   public:\n";
        if (classInfo.iscNamedObject) {
            if (classInfo.keyword == "message" || classInfo.keyword == "packet")
                H << " *     " << classInfo.realClass << "(const char *name=nullptr, short kind=0) : " << classInfo.className << "(name,kind) {}\n";
            else
                H << " *     " << classInfo.realClass << "(const char *name=nullptr) : " << classInfo.className << "(name) {}\n";
        }
        else {
            H << " *     " << classInfo.realClass << "() : " << classInfo.className << "() {}\n";
        }
        H << " *     " << classInfo.realClass << "(const " << classInfo.realClass << "& other) : " << classInfo.className << "(other) {copy(other);}\n";
        H << " *     " << classInfo.realClass << "& operator=(const " << classInfo.realClass << "& other) {if (this==&other) return *this; " << classInfo.className << "::operator=(other); copy(other); return *this;}\n";
        if (classInfo.iscObject)
            H << " *     virtual " << classInfo.realClass << " *dup() const override {return new " << classInfo.realClass << "(*this);}\n";
        H << " *     // ADD CODE HERE to redefine and implement pure virtual functions from " << classInfo.className << "\n";
        H << " * };\n";
        H << " * </pre>\n";
        if (classInfo.iscObject) {
            H << " *\n";
            H << " * The following should go into a .cc (.cpp) file:\n";
            H << " *\n";
            H << " * <pre>\n";
            H << " * Register_Class(" << classInfo.realClass << ")\n";
            H << " * </pre>\n";
        }
    }
    H << " */\n";

    H << "class " << TS(exportDef) << classInfo.className;
    const char *baseclassSepar = " : ";
    if (!classInfo.baseClass.empty()) {
        H << baseclassSepar << "public ::" << classInfo.baseClass;  // make namespace explicit and absolute to disambiguate the way PROGRAM understood it
        baseclassSepar = ", ";
    }

    for (const auto& impl : classInfo.implementsQNames) {
        H << baseclassSepar << "public " << impl;
        baseclassSepar = ", ";
    }

    H << "\n{\n";
    H << "  protected:\n";
    for (const FieldInfo& field : classInfo.fieldList) {
        if (field.isAbstract || field.isCustom)
            continue;
        if (field.isFixedArray) {
            H << "    " << field.dataType << " " << field.var << "[" << field.arraySize << "]" << (field.value == "0" ? " = {0}" : "") << ";\n"; // note: C++ has no syntax for filling a full array with a (nonzero) value in an expression
        }
        else if (field.isDynamicArray) {
            H << "    " << field.dataType << " *" << field.var << " = nullptr;\n";
            H << "    " << field.sizeType << " " << field.sizeVar << " = 0;\n";
        }
        else {
            H << "    " << field.dataType << " " << field.var << (field.value.empty() ? "" : str(" = ") + field.value) << ";\n";
        }
    }
    H << "\n";
    H << "  private:\n";
    H << "    void copy(const " << classInfo.className << "& other);\n\n";
    H << "  protected:\n";
    H << "    bool operator==(const " << classInfo.className << "&) = delete;\n";
    if (classInfo.customize) {
        H << "    // make constructors protected to avoid instantiation\n";
    }
    else {
        H << "\n";
        H << "  public:\n";
    }
    if (classInfo.iscNamedObject) {
        if (classInfo.keyword == "message" || classInfo.keyword == "packet") {
            H << "    " << classInfo.className << "(const char *name=nullptr, short kind=0);\n";
        }
        else {
            H << "    " << classInfo.className << "(const char *name=nullptr);\n";
        }
    }
    else {
        H << "    " << classInfo.className << "();\n";
    }
    H << "    " << classInfo.className << "(const " << classInfo.className << "& other);\n";
    if (classInfo.customize) {
        H << "    // make assignment operator protected to force the user override it\n";
        H << "    " << classInfo.className << "& operator=(const " << classInfo.className << "& other);\n";
        H << "\n";
        H << "  public:\n";
    }
    H << "    virtual ~" << classInfo.className << "();\n";
    if (!classInfo.customize) {
        H << "    " << classInfo.className << "& operator=(const " << classInfo.className << "& other);\n";
    }
    if (classInfo.iscObject) {
        H << "    virtual " << classInfo.className << " *dup() const override ";
        if (classInfo.customize)
            H << "{throw omnetpp::cRuntimeError(\"You forgot to manually add a dup() function to class " << classInfo.realClass << "\");}\n";
        else if (classInfo.isAbstract)
            H << "{throw omnetpp::cRuntimeError(\"Abstract class '%s' does not support dup()\", getClassName());}\n";
        else
            H << "{return new " << classInfo.className << "(*this);}\n";
    }
    std::string maybe_override = classInfo.iscObject ? " override" : "";
    std::string maybe_handleChange = classInfo.beforeChange.empty() ? "" : (classInfo.beforeChange + ";");
    if (!classInfo.str.empty())
        H << "    virtual std::string str() const" << maybe_override << ";\n";
    H << "    virtual void parsimPack(omnetpp::cCommBuffer *b) const" << maybe_override << ";\n";
    H << "    virtual void parsimUnpack(omnetpp::cCommBuffer *b)" << maybe_override << ";\n";
    for (const auto& field : classInfo.fieldList) {
        if (field.isCustom)
            continue;
        std::string pure;
        if (field.isAbstract)
            pure = " = 0";
        std::string overrideGetter(field.overrideGetter ? " override" : "");
        std::string overrideSetter(field.overrideSetter ? " override" : "");
        std::string getterIndexVar("");
        std::string getterIndexArg("");
        std::string setterIndexArg("");
        H << "\n";

        // size setter, size getter
        if (field.isArray) {
            getterIndexVar = "k";
            getterIndexArg = field.sizeType + " " + getterIndexVar;
            setterIndexArg = getterIndexArg + ", ";
            if (field.isDynamicArray)
                H << "    virtual void " << field.sizeSetter << "(" << field.sizeType << " size)" << pure << ";\n";
            H << "    virtual " << field.sizeType << " " << field.sizeGetter << "() const" << pure << ";\n";
        }

        // getter, setter, remover
        H << "    virtual " << field.returnType << " " << field.getter << "(" << getterIndexArg << ") const" << overrideGetter << pure << ";\n";
        if (field.hasGetterForUpdate) {
            H << "    virtual " << field.mutableReturnType << " " << field.getterForUpdate << "(" << getterIndexArg << ")" << overrideGetter;
            H << " { " << maybe_handleChange << "return const_cast<" << field.mutableReturnType << ">(const_cast<" << classInfo.className << "*>(this)->" << field.getter << "(" << getterIndexVar << "));}\n";
        }
        if (field.isPointer || !field.isConst)
            H << "    virtual void " << field.setter << "(" << setterIndexArg << field.argType << " " << field.argName << ")" << overrideSetter << pure << ";\n";
        if (field.isOwnedPointer) {
            H << "    virtual " << field.mutableReturnType << " " << field.remover << "(" << getterIndexArg << ")" << overrideSetter /*TODO*/ << pure << ";\n";
            H << "    [[deprecated]] " << field.mutableReturnType << " " << field.dropper << "(" << getterIndexArg << ")" << overrideSetter << " {return " << field.remover << "(" << getterIndexVar << ");}\n";
        }
        if (field.isDynamicArray) {
            H << "    virtual void " << field.inserter << "(" << setterIndexArg << field.argType << " " << field.argName << ")" << overrideSetter /*TODO*/ << pure << ";\n";
            H << "    [[deprecated]] void " << field.inserter << "(" << field.argType << " " << field.argName << ")" << overrideSetter /*TODO*/ << " {" << field.appender << "(" << field.argName << ");}\n";
            H << "    virtual void " << field.appender << "(" << field.argType << " " << field.argName << ")" << overrideSetter /*TODO*/ << pure << ";\n";
            H << "    virtual void " << field.eraser << "(" << getterIndexArg << ")" << overrideSetter /*TODO*/ << pure << ";\n";
        }
    }
    if (!classInfo.classExtraCode.empty())
        H << "\n" << classInfo.classExtraCode;
    H << "};\n\n";

    if (!classInfo.customize && classInfo.iscObject) {
        H << "inline void doParsimPacking(omnetpp::cCommBuffer *b, const " << classInfo.realClass << "& obj) {obj.parsimPack(b);}\n";
        H << "inline void doParsimUnpacking(omnetpp::cCommBuffer *b, " << classInfo.realClass << "& obj) {obj.parsimUnpack(b);}\n\n";
    }
}

inline std::string var(const MsgTypeTable::FieldInfo& field)
{
    return str("this->") + field.var;
}

inline std::string varElem(const MsgTypeTable::FieldInfo& field)
{
    return str("this->") + field.var + (field.isArray ? "[i]" : "");
}

inline std::string forEachIndex(const MsgTypeTable::FieldInfo& field)
{
    return str("    for (") + field.sizeType + " i = 0; i < " + field.sizeVar + "; i++)";
}

void MsgCodeGenerator::generateClassImpl(const ClassInfo& classInfo)
{
    std::string maybe_handleChange_line = classInfo.beforeChange.empty() ? "" : (str("    ") + classInfo.beforeChange + ";\n");

    if (!classInfo.customize && classInfo.iscObject)
        CC << "Register_Class(" << classInfo.className << ")\n\n";

    // constructor:
    bool isMessage = classInfo.keyword == "message" || classInfo.keyword == "packet";
    std::string ctorArgs = isMessage ? "(const char *name, short kind)" : classInfo.iscNamedObject ? "(const char *name)" : "()";
    std::string baseArgs = isMessage ? "(name, kind)" : classInfo.iscNamedObject ? "(name)" : "()";
    CC << classInfo.className << "::" << classInfo.className << ctorArgs;
    if (classInfo.baseClass != "")
        CC << " : ::" << classInfo.baseClass << baseArgs;
    CC << "\n";
    CC << "{\n";
    for (const auto& baseclassField : classInfo.baseclassFieldlist)
        CC << "    this->" << baseclassField.setter << "(" << baseclassField.value << ");\n";
    if (!classInfo.baseclassFieldlist.empty() && !classInfo.fieldList.empty())
        CC << "\n";
    for (const auto& field : classInfo.fieldList) {
        if (field.isAbstract || field.isCustom)
            continue;

        if (field.isFixedArray && !field.value.empty() && field.value != "0")
            CC << "    std::fill(" << var(field) << ", " << var(field) << " + " << field.sizeVar << ", " << field.value << ");\n";

        std::ostringstream takeElem;
        if (field.iscOwnedObject) {
            if (!field.isPointer)
                takeElem << "    take(&" << varElem(field) << ");\n";
            else if (field.isOwnedPointer)
                takeElem << "    if (" << varElem(field) << " != nullptr) take(" << varElem(field) << ");\n";
        }

        if (field.isFixedArray && !takeElem.str().empty())
            CC << forEachIndex(field) << "\n" << "    " << takeElem.str();
        if (!field.isArray)
            CC << takeElem.str();
    }
    generateMethodCplusplusBlock(classInfo, classInfo.className);
    CC << "}\n\n";

    // copy constructor:
    CC << "" << classInfo.className << "::" << classInfo.className << "(const " << classInfo.className << "& other)";
    if (!classInfo.baseClass.empty())
        CC << " : ::" << classInfo.baseClass << "(other)";
    CC << "\n{\n";
    for (const auto& field : classInfo.fieldList) {
        if (field.isAbstract || field.isCustom)
            continue;
        if (field.isFixedArray && field.isOwnedPointer)
            CC << "    std::fill(" << var(field) << ", " << var(field) << " + " << field.sizeVar << ", nullptr);\n";
        if (!field.isArray && !field.isPointer && field.iscOwnedObject)
            CC << "    take(&" << varElem(field) << ");\n";
        if (field.isFixedArray && !field.isPointer && field.iscOwnedObject) {
            CC << forEachIndex(field) << "\n";
            CC << "        take(&" << varElem(field) << ");\n";
        }
    }
    generateMethodCplusplusBlock(classInfo, classInfo.className + "&");
    CC << "    copy(other);\n";
    CC << "}\n\n";

    // destructor:
    CC << "" << classInfo.className << "::~" << classInfo.className << "()\n";
    CC << "{\n";
    for (const auto& field : classInfo.fieldList) {
        if (field.isAbstract || field.isCustom)
            continue;
        std::ostringstream releaseElem;
        if (field.isOwnedPointer && field.iscOwnedObject)
            releaseElem << "    dropAndDelete(" << varElem(field) << ");\n";
        else if (field.isOwnedPointer && !field.iscOwnedObject)
            releaseElem << "    delete " << varElem(field) << ";\n";
        else if (!field.isPointer && field.iscOwnedObject)
            releaseElem << "    drop(&" << varElem(field) << ");\n";
        if (field.isArray) {
            if (!releaseElem.str().empty())
                CC << forEachIndex(field) << "\n" << opp_indentlines(releaseElem.str(), "    ");
            if (field.isDynamicArray)
                CC << "    delete [] " << var(field) << ";\n";
        }
        else {
            CC << releaseElem.str();
        }
        generateMethodCplusplusBlock(classInfo, std::string("~") + classInfo.className);
    }
    CC << "}\n\n";

    // operator = :
    CC << "" << classInfo.className << "& " << classInfo.className << "::operator=(const " << classInfo.className << "& other)\n";
    CC << "{\n";
    CC << "    if (this == &other) return *this;\n";
    if (classInfo.baseClass != "")
        CC << "    ::" << classInfo.baseClass << "::operator=(other);\n";
    generateMethodCplusplusBlock(classInfo, "operator=");
    CC << "    copy(other);\n";
    CC << "    return *this;\n";
    CC << "}\n\n";

    // copy function:
    CC << "void " << classInfo.className << "::copy(const " << classInfo.className << "& other)\n";
    CC << "{\n";
    for (const auto& field : classInfo.fieldList) {
        if (field.isAbstract || field.isCustom)
            continue;
        if (!field.isPointer && field.isConst)
            continue;

        // delete old content (code similar to destructor code)
        std::ostringstream releaseElem;
        if (field.isOwnedPointer && field.iscOwnedObject)
            releaseElem << "    dropAndDelete(" << varElem(field) << ");\n";
        else if (field.isOwnedPointer && !field.iscOwnedObject)
            releaseElem << "    delete " << varElem(field) << ";\n";
        else if (field.isDynamicArray && !field.isPointer && field.iscOwnedObject)
            releaseElem << "    drop(&" << varElem(field) << ");\n";

        if (!field.isArray)
            CC << releaseElem.str();
        if (field.isArray && !releaseElem.str().empty())
            CC << forEachIndex(field) << "\n" << opp_indentlines(releaseElem.str(), "    ");
        if (field.isDynamicArray)
            CC << "    delete [] " << var(field) << ";\n";

        // allocate new dynamic array
        if (field.isDynamicArray) {
            CC << "    " << var(field) << " = (other." << field.sizeVar << "==0) ? nullptr : new " << field.dataType << "[other." << field.sizeVar << "];\n";
            CC << "    " << field.sizeVar << " = other." << field.sizeVar << ";\n";
        }

        // copy new content
        std::string thisVarElem = varElem(field);
        std::string otherVarElem = str("other.") + field.var + (field.isArray ? "[i]" : "");
        std::ostringstream copyElem;
        if (field.isPointer) {
            copyElem << "    " << thisVarElem << " = " << otherVarElem << ";\n";
            if (field.isOwnedPointer) {
                copyElem << "    if (" << thisVarElem << " != nullptr) {\n";
                copyElem << "        " << thisVarElem << " = " << makeFuncall(thisVarElem, true, field.clone) << ";\n";
                if (field.iscOwnedObject)
                    copyElem << "        take(" << thisVarElem << ");\n";
                if (field.iscNamedObject)
                    copyElem << "        " << thisVarElem << "->setName(" << otherVarElem << "->getName());\n";
                copyElem << "    }\n";
            }
        }
        else if (!field.isConst) {
            copyElem << "    " << thisVarElem << " = " << otherVarElem << ";\n";
            if (field.iscNamedObject)
                copyElem << "    " << thisVarElem << ".setName(" << otherVarElem << ".getName());\n";
            if (field.isDynamicArray && !field.isPointer && field.iscOwnedObject) {
                copyElem << "    take(&" << thisVarElem << ");\n";
            }
        }

        if (field.isArray) {
            CC << forEachIndex(field) << " {\n";
            CC << opp_indentlines(copyElem.str(), "    ");
            CC << "    }\n";
        }
        else {
            CC << copyElem.str();
        }
    }
    generateMethodCplusplusBlock(classInfo, "copy");
    CC << "}\n\n";

    // str() function:
    if (!classInfo.str.empty()) {
        CC << "std::string " << classInfo.className << "::str() const\n";
        CC << "{\n";
        generateMethodCplusplusBlock(classInfo, "str");
        CC << "    return " << classInfo.str << ";\n";
        CC << "}\n\n";
    }

    //
    // Note: This class may not be derived from cObject, and then this parsimPack()/
    // parsimUnpack() is NOT that of cObject. However it's still needed because a
    // "friend" doParsimPacking() function would not be able to access protected
    // members otherwise.
    //
    CC << "void " << classInfo.className << "::parsimPack(omnetpp::cCommBuffer *b) const\n";
    CC << "{\n";
    if (classInfo.baseClass != "") {
        if (classInfo.iscObject) {
            if (classInfo.baseClass != "omnetpp::cObject")
                CC << "    ::" << classInfo.baseClass << "::parsimPack(b);\n";
        }
        else {
            CC << "    doParsimPacking(b,(::" << classInfo.baseClass << "&)*this);\n";  // this would do for cOwnedObject too, but the other is nicer
        }
    }
    for (const auto& field : classInfo.fieldList) {
        if (field.nopack)
            continue; // @nopack specified
        if (field.isAbstract || field.isCustom) {
            CC << "    // field " << field.name << " is abstract or custom -- please do packing in customized class\n";
        }
        else {
            if (field.isArray) {
                if (field.isDynamicArray)
                    CC << "    b->pack(" << field.sizeVar << ");\n";
                CC << "    doParsimArrayPacking(b," << var(field) << "," << field.sizeVar << ");\n";
            }
            else {
                CC << "    doParsimPacking(b," << var(field) << ");\n";
            }
        }
    }
    generateMethodCplusplusBlock(classInfo, "parsimPack");
    CC << "}\n\n";

    CC << "void " << classInfo.className << "::parsimUnpack(omnetpp::cCommBuffer *b)\n";
    CC << "{\n";
    if (classInfo.baseClass != "") {
        if (classInfo.iscObject) {
            if (classInfo.baseClass != "omnetpp::cObject")
                CC << "    ::" << classInfo.baseClass << "::parsimUnpack(b);\n";
        }
        else {
            CC << "    doParsimUnpacking(b,(::" << classInfo.baseClass << "&)*this);\n";  // this would do for cOwnedObject too, but the other is nicer
        }
    }
    for (const auto& field : classInfo.fieldList) {
        if (field.nopack)
            continue; // @nopack specified
        if (field.isAbstract || field.isCustom) {
            CC << "    // field " << field.name << " is abstract or custom -- please do unpacking in customized class\n";
        }
        else {
            if (field.isArray) {
                if (field.isFixedArray) {
                    CC << "    doParsimArrayUnpacking(b," << var(field) << "," << field.arraySize << ");\n";
                }
                else {
                    CC << "    delete [] " << var(field) << ";\n";
                    CC << "    b->unpack(" << field.sizeVar << ");\n";
                    CC << "    if (" << field.sizeVar << " == 0) {\n";
                    CC << "        " << var(field) << " = nullptr;\n";
                    CC << "    } else {\n";
                    CC << "        " << var(field) << " = new " << field.dataType << "[" << field.sizeVar << "];\n";
                    CC << "        doParsimArrayUnpacking(b," << var(field) << "," << field.sizeVar << ");\n";
                    CC << "    }\n";
                }
            }
            else {
                CC << "    doParsimUnpacking(b," << var(field) << ");\n";
            }
        }
    }
    generateMethodCplusplusBlock(classInfo, "parsimUnpack");
    CC << "}\n\n";

    for (const auto& field : classInfo.fieldList) {
        if (field.isAbstract || field.isCustom || field.isCustomImpl)
            continue;

        std::string idx = (field.isArray) ? "[k]" : "";
        std::string idxarg = (field.isArray) ? (field.sizeType + " k") : std::string("");
        std::string idxarg2 = (field.isArray) ? (idxarg + ", ") : std::string("");
        std::string indexedVar = str("this->") + field.var + idx;

        // getters:
        if (field.isArray) {
            CC << "" << field.sizeType << " " << classInfo.className << "::" << field.sizeGetter << "() const\n";
            CC << "{\n";
            generateMethodCplusplusBlock(classInfo, field.sizeGetter);
            CC << "    return " << field.sizeVar << ";\n";
            CC << "}\n\n";
        }

        CC << field.returnType << " " << classInfo.className << "::" << field.getter << "(" << idxarg << ")" << " const\n";
        CC << "{\n";
        if (field.isArray)
            CC << "    if (k >= " << field.sizeVar << ") throw omnetpp::cRuntimeError(\"Array of size %lu indexed by %lu\", (unsigned long)" << field.sizeVar << ", (unsigned long)k);\n";
        generateMethodCplusplusBlock(classInfo, field.getter);
        CC << "    return " << makeFuncall(indexedVar, field.isPointer, field.getterConversion) + ";\n";
        CC << "}\n\n";

        // resizer:
        if (field.isDynamicArray) {
            CC << "void " << classInfo.className << "::" << field.sizeSetter << "(" << field.sizeType << " newSize)\n";
            CC << "{\n";
            CC << maybe_handleChange_line;
            CC << "    " << field.dataType << " *" << field.var << "2 = (newSize==0) ? nullptr : new " << field.dataType << "[newSize];\n";
            CC << "    " << field.sizeType << " minSize = " << field.sizeVar << " < newSize ? " << field.sizeVar << " : newSize;\n";
            CC << "    for (" << field.sizeType << " i = 0; i < minSize; i++)\n";
            CC << "        " << field.var << "2[i] = " << var(field) << "[i];\n";
            if (!field.value.empty()) {
                CC << "    for (" << field.sizeType << " i = minSize; i < newSize; i++)\n";
                CC << "        " << field.var << "2[i] = " << field.value << ";\n";
            }
            if (!field.isPointer && field.iscOwnedObject)
                CC << forEachIndex(field) << "\n" << "        drop(&" << varElem(field) << ");\n";
            if (field.isPointer && field.isOwnedPointer) {
                CC << "    for (" << field.sizeType << " i = newSize; i < " << field.sizeVar << "; i++)\n";
                if (field.iscOwnedObject)
                    CC << "        dropAndDelete(" << field.var << "[i]);\n";
                else
                    CC << "        delete " << field.var << "[i];\n";
            }
            CC << "    delete [] " << var(field) << ";\n";
            CC << "    " << var(field) << " = " << field.var << "2;\n";
            CC << "    " << field.sizeVar << " = newSize;\n";
            if (!field.isPointer && field.iscOwnedObject)
                CC << forEachIndex(field) << "\n" << "        take(&" << varElem(field) << ");\n";
            generateMethodCplusplusBlock(classInfo, field.sizeGetter);
            CC << "}\n\n";
        }

        // setter:
        if (field.isPointer || !field.isConst) {
            CC << "void " << classInfo.className << "::" << field.setter << "(" << idxarg2 << field.argType << " " << field.argName << ")\n";
            CC << "{\n";
            if (field.isArray)
                CC << "    if (k >= " << field.sizeVar << ") throw omnetpp::cRuntimeError(\"Array of size %lu indexed by %lu\", (unsigned long)" << field.sizeVar << ", (unsigned long)k);\n";
            CC << maybe_handleChange_line;
            generateMethodCplusplusBlock(classInfo, field.setter);
            if (field.isOwnedPointer) {
                if (!field.allowReplace)
                    CC << "    if (" << indexedVar << " != nullptr) throw omnetpp::cRuntimeError(\"" << field.setter << "(): a value is already set, remove it first with " << field.remover << "()\");\n";
                else {
                    CC << "    if (" << field.argName << " == " << indexedVar << ") return;\n";
                    generateOwnershipOp(field, indexedVar, "delete");
                }
            }
            CC << "    " << indexedVar << " = " << field.argName << ";\n";
            if (field.isOwnedPointer)
                generateOwnershipOp(field, indexedVar, "take");
            CC << "}\n\n";
        }

        // remover:
        if (field.isOwnedPointer) {
            CC << field.mutableReturnType << " " << classInfo.className << "::" << field.remover << "(" << idxarg << ")\n";
            CC << "{\n";
            if (field.isArray)
                CC << "    if (k >= " << field.sizeVar << ") throw omnetpp::cRuntimeError(\"Array of size %lu indexed by %lu\", (unsigned long)" << field.sizeVar << ", (unsigned long)k);\n";
            CC << maybe_handleChange_line;
            generateMethodCplusplusBlock(classInfo, field.remover);
            CC << "    " << field.mutableReturnType << " retval = ";
            if (field.isConst)
                CC << "const_cast<" << field.mutableReturnType << ">(" << indexedVar << ");\n";
            else
                CC << indexedVar << ";\n";
            generateOwnershipOp(field, "retval", "drop");
            CC << "    " << indexedVar << " = nullptr;\n";
            CC << "    return retval;\n";
            CC << "}\n\n";
        }

        // inserter
        if (field.isDynamicArray) {
            CC << "void " << classInfo.className << "::" << field.inserter << "(" << idxarg2 << field.argType << " " << field.argName << ")\n";
            CC << "{\n";
            CC << "    if (k > " << field.sizeVar << ") throw omnetpp::cRuntimeError(\"Array of size %lu indexed by %lu\", (unsigned long)" << field.sizeVar << ", (unsigned long)k);\n";
            CC << maybe_handleChange_line;
            generateMethodCplusplusBlock(classInfo, field.inserter);
            CC << "    " << field.sizeType << " newSize = " << field.sizeVar << " + 1;\n";
            CC << "    " << field.dataType << " *" << field.var << "2 = new " << field.dataType << "[newSize];\n";
            CC << "    " << field.sizeType << " i;\n";
            CC << "    for (i = 0; i < k; i++)\n";
            CC << "        " << field.var << "2[i] = " << var(field) << "[i];\n";
            CC << "    " << field.var << "2[k] = " << field.argName << ";\n";
            if (field.isOwnedPointer)
                generateOwnershipOp(field, field.var + "2[k]", "take");
            CC << "    for (i = k + 1; i < newSize; i++)\n";
            CC << "        " << field.var << "2[i] = " << var(field) << "[i-1];\n";
            if (!field.isPointer && field.iscOwnedObject)
                CC << forEachIndex(field) << "\n" << "        drop(&" << varElem(field) << ");\n";
            CC << "    delete [] " << var(field) << ";\n";
            CC << "    " << var(field) << " = " << field.var << "2;\n";
            CC << "    " << field.sizeVar << " = newSize;\n";
            if (!field.isPointer && field.iscOwnedObject)
                CC << forEachIndex(field) << "\n" << "        take(&" << varElem(field) << ");\n";
            CC << "}\n\n";

            CC << "void " << classInfo.className << "::" << field.appender << "(" << field.argType << " " << field.argName << ")\n";
            CC << "{\n";
            CC << "    " << field.inserter << "(" << field.sizeVar << ", " << field.argName << ");\n";
            CC << "}\n\n";
        }

        // eraser
        if (field.isDynamicArray) {
            CC << "void " << classInfo.className << "::" << field.eraser << "(" << idxarg << ")\n";
            CC << "{\n";
            CC << "    if (k >= " << field.sizeVar << ") throw omnetpp::cRuntimeError(\"Array of size %lu indexed by %lu\", (unsigned long)" << field.sizeVar << ", (unsigned long)k);\n";
            CC << maybe_handleChange_line;
            generateMethodCplusplusBlock(classInfo, field.eraser);
            CC << "    " << field.sizeType << " newSize = " << field.sizeVar << " - 1;\n";
            CC << "    " << field.dataType << " *" << field.var << "2 = (newSize == 0) ? nullptr : new " << field.dataType << "[newSize];\n";
            CC << "    " << field.sizeType << " i;\n";
            CC << "    for (i = 0; i < k; i++)\n";
            CC << "        " << field.var << "2[i] = " << var(field) << "[i];\n";
            CC << "    for (i = k; i < newSize; i++)\n";
            CC << "        " << field.var << "2[i] = " << var(field) << "[i+1];\n";
            if (!field.isPointer && field.iscOwnedObject)
                CC << forEachIndex(field) << "\n" << "        drop(&" << varElem(field) << ");\n";
            if (field.isOwnedPointer)
                generateOwnershipOp(field, var(field) + "[k]", "delete");
            CC << "    delete [] " << var(field) << ";\n";
            CC << "    " << var(field) << " = " << field.var << "2;\n";
            CC << "    " << field.sizeVar << " = newSize;\n";
            if (!field.isPointer && field.iscOwnedObject)
                CC << forEachIndex(field) << "\n" << "        take(&" << varElem(field) << ");\n";
            CC << "}\n\n";
        }
    }

    reportUnusedMethodCplusplusBlocks(classInfo);
}

void MsgCodeGenerator::generateStruct(const ClassInfo& classInfo, const std::string& exportDef)
{
    generateStructDecl(classInfo, exportDef);
    generateStructImpl(classInfo);
}

void MsgCodeGenerator::generateStructDecl(const ClassInfo& classInfo, const std::string& exportDef)
{
    H << "/**\n";
    H << " * Struct generated from " << classInfo.astNode->getSourceLocation().str() << " by " PROGRAM ".\n";
    H << " */\n";
    H << "struct " << TS(exportDef) << classInfo.className;
    const char *baseclassSepar = " : ";
    if (!classInfo.baseClass.empty()) {
        H << baseclassSepar << "::" << classInfo.baseClass;  // make namespace explicit and absolute to disambiguate the way PROGRAM understood it
        baseclassSepar = ", ";
    }

    for (const auto& impl : classInfo.implementsQNames) {
        H << baseclassSepar << impl;
        baseclassSepar = ", ";
    }

    H << "\n{\n";
    H << "    " << classInfo.className << "();\n";
    for (const auto& field : classInfo.fieldList) {
        if (field.isCustom)
            continue;
        H << "    " << field.dataType << " " << field.var;
        if (field.isArray)
            H << "[" << field.arraySize << "]";
        else if (!field.value.empty())
            H << " = " << field.value;
        H << ";\n";
    }
    H << classInfo.classExtraCode;
    H << "};\n\n";

    H << "// helpers for local use\n";
    H << "void " << TS(exportDef) << "__doPacking(omnetpp::cCommBuffer *b, const " << classInfo.className << "& a);\n";
    H << "void " << TS(exportDef) << "__doUnpacking(omnetpp::cCommBuffer *b, " << classInfo.className << "& a);\n\n";

    H << "inline void doParsimPacking(omnetpp::cCommBuffer *b, const " << classInfo.realClass << "& obj) { " << "__doPacking(b, obj); }\n";
    H << "inline void doParsimUnpacking(omnetpp::cCommBuffer *b, " << classInfo.realClass << "& obj) { " << "__doUnpacking(b, obj); }\n\n";
}

void MsgCodeGenerator::generateStructImpl(const ClassInfo& classInfo)
{
    // Constructor:
    CC << "" << classInfo.className << "::" << classInfo.className << "()\n";
    CC << "{\n";
    // override baseclass fields initial value
    for (const auto& field : classInfo.baseclassFieldlist) {
        if (!field.isCustom)
            CC << "    " << var(field) << " = " << field.value << ";\n";
    }
    if (!classInfo.baseclassFieldlist.empty() && !classInfo.fieldList.empty())
        CC << "\n";

    for (const auto& field : classInfo.fieldList) {
        if (field.isCustom)
            continue;
        Assert(!field.isAbstract); // ensured in the analyzer
        Assert(!field.iscOwnedObject); // ensured in the analyzer
        if (field.isArray) {
            Assert(field.isFixedArray); // ensured in the analyzer
            if (!field.value.empty()) {
                CC << "    for (" << field.sizeType << " i = 0; i < " << field.arraySize << "; i++)\n";
                CC << "        " << var(field) << "[i] = " << field.value << ";\n";
            }
        }
    }
    generateMethodCplusplusBlock(classInfo, classInfo.className);
    CC << "}\n\n";

    // doPacking/doUnpacking go to the global namespace
    CC << "void __doPacking(omnetpp::cCommBuffer *b, const " << classInfo.className << "& a)\n";
    CC << "{\n";
    if (!classInfo.baseClass.empty())
        CC << "    doParsimPacking(b,(::" << classInfo.baseClass << "&)a);\n";
    for (const auto& field : classInfo.fieldList) {
        if (field.isCustom)
            continue;
        if (field.isArray)
            CC << "    doParsimArrayPacking(b,a." << field.var << "," << field.arraySize << ");\n";
        else
            CC << "    doParsimPacking(b,a." << field.var << ");\n";
    }
    generateMethodCplusplusBlock(classInfo, "__doPacking");
    CC << "}\n\n";

    CC << "void __doUnpacking(omnetpp::cCommBuffer *b, " << classInfo.className << "& a)\n";
    CC << "{\n";
    if (!classInfo.baseClass.empty())
        CC << "    doParsimUnpacking(b,(::" << classInfo.baseClass << "&)a);\n";
    for (const auto& field : classInfo.fieldList) {
        if (field.isCustom)
            continue;
        if (field.isArray)
            CC << "    doParsimArrayUnpacking(b,a." << field.var << "," << field.arraySize << ");\n";
        else
            CC << "    doParsimUnpacking(b,a." << field.var << ");\n";
    }
    generateMethodCplusplusBlock(classInfo, "__doUnpacking");
    CC << "}\n\n";

    reportUnusedMethodCplusplusBlocks(classInfo);
}

void MsgCodeGenerator::generateToAnyPtr(const ClassInfo& classInfo)
{
    // only for root classes! and for classes with multiple inheritance, to prevent compiler errors due to ambiguity
    if ((classInfo.extendsName.empty() && classInfo.implementsQNames.empty()) || !classInfo.implementsQNames.empty()) {
        std::string classQName = classInfo.classQName;
        H << "inline any_ptr toAnyPtr(const " << classQName << " *p) {";
        if (!classInfo.isPolymorphic) {
            H << "return any_ptr(p);";
        }
        else {
            // must try casting to cObject as it may be an object which subclasses both this type and cObject (think cIListener!)
            H << "if (auto obj = as_cObject(p)) return any_ptr(obj); else return any_ptr(p);";
        }
        H << "}\n";
    }
}

void MsgCodeGenerator::generateFromAnyPtr(const ClassInfo& classInfo, const std::string& exportDef)
{
    // in theory, we should try and look it up by EACH polymorphic base class
    // that exists in the world; however, since we obviously can't do that,
    // we just try via cObject which is sufficient in practice.
    std::string classQName = classInfo.classQName;
    if (classInfo.iscObject) {
        H << "template<> inline " << classQName << " *fromAnyPtr(any_ptr ptr) ";
        H << "{ return check_and_cast<" << classQName << "*>(ptr.get<cObject>()); }\n";
    }
    else if (classInfo.rootClasses.size() < 2) {
        H << "template<> inline " << classQName << " *fromAnyPtr(any_ptr ptr) ";
        // there should always be at least one rootClass
        std::string rootClass = classInfo.rootClasses.at(0);
        if (rootClass == classQName)
            H << "{ return ptr.get<" << rootClass << ">(); }\n";
        else
            H << "{ return static_cast<" << classQName << "*>(ptr.get<" << rootClass << ">()); }\n";
    }
    else { // multiple roots; needs to try via each
        H << "template<> " << classQName << " *fromAnyPtr(any_ptr ptr);\n";
        CC << "template<> " << classQName << " *fromAnyPtr(any_ptr ptr) {\n";
        for (std::string rootClass : classInfo.rootClasses)
            CC << "    if (ptr.contains<" << rootClass << ">()) return static_cast<" << classQName << "*>(ptr.get<" << rootClass << ">());\n";
        CC << "    throw cRuntimeError(\"Unable to obtain " << classQName << "* pointer from any_ptr(%s)\", ptr.pointerTypeName());\n";
        CC << "}\n";
    }
}

inline std::string removePointer(const std::string& type)
{
    Assert(opp_stringendswith(type.c_str(), "*"));
    return opp_trim(type.substr(0, type.size()-1));
}

void MsgCodeGenerator::generateDescriptorClass(const ClassInfo& classInfo)
{
    CC << "class " << classInfo.descriptorClass << " : public omnetpp::cClassDescriptor\n";
    CC << "{\n";
    CC << "  private:\n";
    CC << "    mutable const char **propertyNames;\n";

    CC << "    enum FieldConstants {\n";
    for (auto& field : classInfo.fieldList)
        CC << "        " << field.symbolicConstant << ",\n";
    CC << "    };\n";

    CC << "  public:\n";
    CC << "    " << classInfo.descriptorClass << "();\n";
    CC << "    virtual ~" << classInfo.descriptorClass << "();\n";
    CC << "\n";
    CC << "    virtual bool doesSupport(omnetpp::cObject *obj) const override;\n";
    CC << "    virtual const char **getPropertyNames() const override;\n";
    CC << "    virtual const char *getProperty(const char *propertyName) const override;\n";
    CC << "    virtual int getFieldCount() const override;\n";
    CC << "    virtual const char *getFieldName(int field) const override;\n";
    CC << "    virtual int findField(const char *fieldName) const override;\n";
    CC << "    virtual unsigned int getFieldTypeFlags(int field) const override;\n";
    CC << "    virtual const char *getFieldTypeString(int field) const override;\n";
    CC << "    virtual const char **getFieldPropertyNames(int field) const override;\n";
    CC << "    virtual const char *getFieldProperty(int field, const char *propertyName) const override;\n";
    CC << "    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;\n";
    CC << "    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;\n";
    CC << "\n";
    CC << "    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;\n";
    CC << "    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;\n";
    CC << "    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;\n";
    CC << "    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;\n";
    CC << "    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;\n";
    CC << "\n";
    CC << "    virtual const char *getFieldStructName(int field) const override;\n";
    CC << "    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;\n";
    CC << "    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;\n";
    CC << "};\n\n";

    // register class
    CC << "Register_ClassDescriptor(" << classInfo.descriptorClass << ")\n\n";

    // ctor, dtor
    size_t numFields = classInfo.fieldList.size();
    std::string qualifiedrealmsgclass = prefixWithNamespace(classInfo.realClass, classInfo.namespaceName);
    CC << "" << classInfo.descriptorClass << "::" << classInfo.descriptorClass << "() : omnetpp::cClassDescriptor(";
    if (classInfo.customize)
        CC << "\"" << qualifiedrealmsgclass << "\"";
    else
        CC << "omnetpp::opp_typename(typeid(" << qualifiedrealmsgclass << "))";
    CC << ", \"" << classInfo.baseClass << "\")\n";
    CC << "{\n";
    CC << "    propertyNames = nullptr;\n";
    CC << "}\n";
    CC << "\n";

    CC << "" << classInfo.descriptorClass << "::~" << classInfo.descriptorClass << "()\n";
    CC << "{\n";
    CC << "    delete[] propertyNames;\n";
    CC << "}\n";
    CC << "\n";

    // doesSupport()
    CC << "bool " << classInfo.descriptorClass << "::doesSupport(omnetpp::cObject *obj) const\n";
    CC << "{\n";
    CC << "    return dynamic_cast<" << classInfo.className << " *>(obj)!=nullptr;\n";
    CC << "}\n";
    CC << "\n";

    // getPropertyNames()
    CC << "const char **" << classInfo.descriptorClass << "::getPropertyNames() const\n";
    CC << "{\n";
    CC << "    if (!propertyNames) {\n";
    CC << "        static const char *names[] = { ";
    for (const auto& prop : classInfo.props.getAll()) {
        CC << opp_quotestr(prop.getIndexedName()) << ", ";
    }
    CC << " nullptr };\n";
    CC << "        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();\n";
    CC << "        const char **baseNames = base ? base->getPropertyNames() : nullptr;\n";
    CC << "        propertyNames = mergeLists(baseNames, names);\n";
    CC << "    }\n";
    CC << "    return propertyNames;\n";
    CC << "}\n";
    CC << "\n";

    // getProperty()
    CC << "const char *" << classInfo.descriptorClass << "::getProperty(const char *propertyName) const\n";
    CC << "{\n";
    for (const auto& prop : classInfo.props.getAll()) {
        CC << "    if (!strcmp(propertyName, " << opp_quotestr(prop.getIndexedName()) << ")) return " << opp_quotestr(prop.getValueAsString()) << ";\n";
    }
    CC << "    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();\n";
    CC << "    return base ? base->getProperty(propertyName) : nullptr;\n";
    CC << "}\n";
    CC << "\n";

    // getFieldCount()
    CC << "int " << classInfo.descriptorClass << "::getFieldCount() const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();\n";
    CC << "    return base ? " << numFields << "+base->getFieldCount() : " << numFields << ";\n";
    CC << "}\n";
    CC << "\n";

    // getFieldTypeFlags()
    CC << "unsigned int " << classInfo.descriptorClass << "::getFieldTypeFlags(int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();\n";
    CC << "    if (base) {\n";
    CC << "        if (field < base->getFieldCount())\n";
    CC << "            return base->getFieldTypeFlags(field);\n";
    CC << "        field -= base->getFieldCount();\n";
    CC << "    }\n";
    if (numFields == 0) {
        CC << "    return 0;\n";
    }
    else {
        CC << "    static unsigned int fieldTypeFlags[] = {\n";
        for (auto& field : classInfo.fieldList) {
            StringVector flags;
            if (field.isArray)
                flags.push_back("FD_ISARRAY");
            if (!field.isOpaque)
                flags.push_back("FD_ISCOMPOUND");
            if (field.isPointer)
                flags.push_back("FD_ISPOINTER");
            if (field.iscObject)
                flags.push_back("FD_ISCOBJECT");
            if (field.iscOwnedObject)
                flags.push_back("FD_ISCOWNEDOBJECT");
            if (field.isEditable)
                flags.push_back("FD_ISEDITABLE");
            if (field.isReplaceable)
                flags.push_back("FD_ISREPLACEABLE");
            if (field.isResizable)
                flags.push_back("FD_ISRESIZABLE");
            std::string flagss;
            if (flags.empty())
                flagss = "0";
            else {
                flagss = flags[0];
                for (size_t i = 1; i < flags.size(); i++)
                    flagss = flagss + " | " + flags[i];
            }

            CC << "        " << flagss << ",    // " << field.symbolicConstant << "\n";
        }
        CC << "    };\n";
        CC << "    return (field >= 0 && field < " << numFields << ") ? fieldTypeFlags[field] : 0;\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldName()
    CC << "const char *" << classInfo.descriptorClass << "::getFieldName(int field) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("return base->getFieldName(field);");
    if (numFields == 0) {
        CC << "    return nullptr;\n";
    }
    else {
        CC << "    static const char *fieldNames[] = {\n";
        for (const auto& field : classInfo.fieldList) {
            CC << "        \"" << field.name << "\",\n";
        }
        CC << "    };\n";
        CC << "    return (field >= 0 && field < " << numFields << ") ? fieldNames[field] : nullptr;\n";
    }
    CC << "}\n";
    CC << "\n";

    // findField()
    CC << "int " << classInfo.descriptorClass << "::findField(const char *fieldName) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();\n";
    if (numFields > 0) {
        CC << "    int baseIndex = base ? base->getFieldCount() : 0;\n";
        for (size_t i = 0; i < classInfo.fieldList.size(); ++i) {
            const FieldInfo& field = classInfo.fieldList[i];
            CC << "    if (strcmp(fieldName, \"" << field.name << "\") == 0) return baseIndex + " << i << ";\n";
        }
    }
    CC << "    return base ? base->findField(fieldName) : -1;\n";
    CC << "}\n";
    CC << "\n";

    // getFieldTypeString()
    CC << "const char *" << classInfo.descriptorClass << "::getFieldTypeString(int field) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("return base->getFieldTypeString(field);");
    if (numFields == 0) {
        CC << "    return nullptr;\n";
    }
    else {
        CC << "    static const char *fieldTypeStrings[] = {\n";
        for (const auto& field : classInfo.fieldList) {
            CC << "        \"" << field.typeQName << "\",    // " << field.symbolicConstant << "\n";
        }
        CC << "    };\n";
        CC << "    return (field >= 0 && field < " << numFields << ") ? fieldTypeStrings[field] : nullptr;\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldPropertyNames()
    CC << "const char **" << classInfo.descriptorClass << "::getFieldPropertyNames(int field) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("return base->getFieldPropertyNames(field);");
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < numFields; ++i) {
        const FieldInfo& field = classInfo.fieldList[i];
        const Properties& props = field.props;
        if (!props.empty()) {
            CC << "        case " << field.symbolicConstant << ": {\n";
            CC << "            static const char *names[] = { ";
            for (const auto& prop : props.getAll())
                CC << opp_quotestr(prop.getIndexedName()) << ", ";
            CC << " nullptr };\n";
            CC << "            return names;\n";
            CC << "        }\n";
        }
    }
    CC << "        default: return nullptr;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldProperty()
    CC << "const char *" << classInfo.descriptorClass << "::getFieldProperty(int field, const char *propertyName) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("return base->getFieldProperty(field, propertyName);");
    CC << "    switch (field) {\n";

    for (size_t i = 0; i < numFields; ++i) {
        const FieldInfo& field = classInfo.fieldList[i];
        const Properties& props = field.props;
        if (!props.empty()) {
            CC << "        case " << field.symbolicConstant << ":\n";
            for (const auto& prop : props.getAll()) {
                CC << "            if (!strcmp(propertyName, " << opp_quotestr(prop.getIndexedName()) << ")) return " << opp_quotestr(prop.getValueAsString()) << ";\n";
            }
            CC << "            return nullptr;\n";
        }
    }

    CC << "        default: return nullptr;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldArraySize()
    CC << "int " << classInfo.descriptorClass << "::getFieldArraySize(omnetpp::any_ptr object, int field) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("return base->getFieldArraySize(object, field);");
    CC << "    " << classInfo.className << " *pp = omnetpp::fromAnyPtr<" << classInfo.className << ">(object); (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < numFields; i++) {
        const FieldInfo& field = classInfo.fieldList[i];
        if (field.isArray) {
            CC << "        case " << field.symbolicConstant << ": ";
            if (field.isFixedArray) {
                CC << "return " << field.arraySize << ";\n";
            }
            else if (!classInfo.isClass) {
                CC << "return pp->" << field.sizeVar << ";\n";
            }
            else {
                CC << "return " << makeFuncall("pp", true, field.sizeGetter) << ";\n";
            }
        }
    }
    CC << "        default: return 0;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // setFieldArraySize()
    CC << "void " << classInfo.descriptorClass << "::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("base->setFieldArraySize(object, field, size);");
    CC << "    " << classInfo.className << " *pp = omnetpp::fromAnyPtr<" << classInfo.className << ">(object); (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < numFields; i++) {
        const FieldInfo& field = classInfo.fieldList[i];
        if (field.isArray && field.isResizable) {
            Assert(classInfo.isClass); // we don't support variable-length arrays in structs
            CC << "        case " << field.symbolicConstant << ": ";
            CC << makeFuncall("pp", true, field.sizeSetter, false, "size") << "; break;\n";
        }
    }
    CC << "        default: throw omnetpp::cRuntimeError(\"Cannot set array size of field %d of class '" << classInfo.className << "'\", field);\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldDynamicTypeString()
    CC << "const char *" << classInfo.descriptorClass << "::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("return base->getFieldDynamicTypeString(object,field,i);");
    CC << "    " << classInfo.className << " *pp = omnetpp::fromAnyPtr<" << classInfo.className << ">(object); (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < numFields; i++) {
        const FieldInfo& field = classInfo.fieldList[i];
        if (field.isPointer) {
            CC << "        case " << field.symbolicConstant << ": ";
            CC << "{ " << field.returnType << " value = " << makeFuncall("pp", true, field.getter, field.isArray) << "; ";
            if (field.isConst)
                CC << "return omnetpp::opp_typename(typeid(*const_cast<" << field.mutableReturnType << ">(value))); }\n";
            else
                CC << "return omnetpp::opp_typename(typeid(*value)); }\n";
        }
    }
    CC << "        default: return nullptr;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldValueAsString()
    CC << "std::string " << classInfo.descriptorClass << "::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("return base->getFieldValueAsString(object,field,i);");
    CC << "    " << classInfo.className << " *pp = omnetpp::fromAnyPtr<" << classInfo.className << ">(object); (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < numFields; i++) {
        const FieldInfo& field = classInfo.fieldList[i];
        CC << "        case " << field.symbolicConstant << ": ";
        if (!classInfo.isClass && field.isArray) {
            Assert(field.isFixedArray); // struct may not contain dynamic arrays; checked by analyzer
            CC << "if (i >= " << field.arraySize << ") return \"\";\n                ";
        }
        std::string value = classInfo.isClass ?
                makeFuncall("pp", true, field.getter, field.isArray) :
                (str("pp->") + field.var + (field.isArray ? "[i]" : ""));
        if (!field.isPointer) {
            if (!field.toString.empty())
                CC << "return " << makeFuncall(value, false, field.toString) << ";\n";
            else if (field.hasStrMethod)
                CC << "return " << value << ".str();\n";
            else
                CC << "return \"\";\n";
        }
        else {
            if (!field.toString.empty())
                CC << "{ auto obj = " << value << "; return obj == nullptr ? \"\" : " << makeFuncall("obj", true, field.toString) << "; }\n";
            else if (field.hasStrMethod)
                CC << "{ auto obj = " << value << "; return obj == nullptr ? \"\" : obj->str(); }\n";
            else
                CC << "return \"\";\n";
        }


    }
    CC << "        default: return \"\";\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // setFieldValueAsString()
    CC << "void " << classInfo.descriptorClass << "::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("base->setFieldValueAsString(object, field, i, value);");
    CC << "    " << classInfo.className << " *pp = omnetpp::fromAnyPtr<" << classInfo.className << ">(object); (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < numFields; i++) {
        const FieldInfo& field = classInfo.fieldList[i];
        if (field.isEditable) {
            if (field.fromString.empty())
                throw opp_runtime_error("Field '%s' is editable, but @fromString is unspecified", field.name.c_str()); // ensured by MsgAnalyzer
            CC << "        case " << field.symbolicConstant << ": ";
            if (!classInfo.isClass && field.isArray) {
                Assert(field.isFixedArray); // struct may not contain dynamic arrays; checked by analyzer
                CC << "if (i < 0 || i >= " << field.arraySize << ") throw omnetpp::cRuntimeError(\"Array index %d out of bounds for field %d of class '" << classInfo.className << "'\", i, field);\n";
                CC << "                ";
            }
            std::string fromstringCall = makeFuncall("value", field.isPointer, field.fromString);
            if (!classInfo.isClass)
                CC << "pp->" << field.var << (field.isArray ? "[i]" : "") << " = " << fromstringCall << "; break;\n";
            else
                CC << makeFuncall("pp", true, field.setter, field.isArray, fromstringCall) << "; break;\n";
        }
    }
    CC << "        default: throw omnetpp::cRuntimeError(\"Cannot set field %d of class '" << classInfo.className << "'\", field);\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldValue()
    CC << "omnetpp::cValue " << classInfo.descriptorClass << "::getFieldValue(omnetpp::any_ptr object, int field, int i) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("return base->getFieldValue(object,field,i);");
    CC << "    " << classInfo.className << " *pp = omnetpp::fromAnyPtr<" << classInfo.className << ">(object); (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < numFields; i++) {
        const FieldInfo& field = classInfo.fieldList[i];
        CC << "        case " << field.symbolicConstant << ": ";
        if (!classInfo.isClass && field.isArray) {
            Assert(field.isFixedArray); // struct may not contain dynamic arrays; checked by analyzer
            CC << "if (i >= " << field.arraySize << ") return omnetpp::cValue();\n                ";
        }

        if (!field.toValue.empty()) {
            // via @toValue
            std::string value = classInfo.isClass ?
                    makeFuncall("pp", true, field.getter, field.isArray) :
                    (str("pp->") + field.var + (field.isArray ? "[i]" : ""));
            CC << "return " << makeFuncall(value, field.isPointer, field.toValue) << ";\n";
        }
        else if (!field.byValue) {
            // return pointer
            std::string value;
            if (!classInfo.isClass)
                value = str("pp->") + field.var + (field.isArray ? "[i]" : "");
            else
                value = makeFuncall("pp", true, field.getter, field.isArray);
            std::string maybeAddressOf = field.isPointer ? "" : "&";
            CC << "return omnetpp::toAnyPtr(" << maybeAddressOf << value << "); break;\n";
        }
        else {
            errors->addWarning(field.astNode, "Cannot generate code in the descriptor class to return field '%s' as cValue, because no @toValue() is specified on the field or its type '%s' (and also cannot return a pointer due to @byValue)", field.name.c_str(), field.typeQName.c_str());
            CC << "throw omnetpp::cRuntimeError(\"Cannot return field '" << classInfo.qname << "::" << field.name << "' (type '" << field.type << "') as cValue, please provide @toValue in the msg file\");\n";
        }
    }
    CC << "        default: throw omnetpp::cRuntimeError(\"Cannot return field %d of class '" << classInfo.className << "' as cValue -- field index out of range?\", field);\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // setFieldValue()
    CC << "void " << classInfo.descriptorClass << "::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("base->setFieldValue(object, field, i, value);");
    CC << "    " << classInfo.className << " *pp = omnetpp::fromAnyPtr<" << classInfo.className << ">(object); (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < numFields; i++) {
        const FieldInfo& field = classInfo.fieldList[i];
        if (field.isEditable || field.isReplaceable) {
            CC << "        case " << field.symbolicConstant << ": ";
            if (!classInfo.isClass && field.isArray) {
                Assert(field.isFixedArray); // struct may not contain dynamic arrays; checked by analyzer
                CC << "if (i < 0 || i >= " << field.arraySize << ") throw omnetpp::cRuntimeError(\"Array index %d out of bounds for field %d of class '" << classInfo.className << "'\", i, field);\n";
                CC << "                ";
            }
            if (field.isEditable) {
                if (!field.fromValue.empty()) {
                    // @fromValue
                    //TODO for enums, set fromValue beforehand instead of hardwiring it here!
                    std::string valueToSet = !field.enumName.empty() ? "(" + field.enumQName + ")value.intValue()" : makeFuncall("value", field.isPointer, field.fromValue);
                    if (!classInfo.isClass)
                        CC << "pp->" << field.var << (field.isArray ? "[i]" : "") << " = " << valueToSet << "; break;\n";
                    else
                        CC << makeFuncall("pp", true, field.setter, field.isArray, valueToSet) << "; break;\n";
                }
                else {
                    errors->addWarning(field.astNode, "Field '%s' is marked @editable but cannot generate code in the descriptor class to set it from cValue, as @fromValue() is not specified on the field or its type '%s'", field.name.c_str(), field.typeQName.c_str());
                    CC << "throw omnetpp::cRuntimeError(\"Cannot set field '" << classInfo.qname << "::" << field.name << "' (type '" << field.type << "') from cValue, please provide @fromValue in the msg file\");\n";
                }
            }
            else if (field.isReplaceable) {
                // via pointer
                std::string maybeDereference = field.isPointer ? "" : "*";
                std::string valuePtr = "omnetpp::fromAnyPtr<" + removePointer(field.argType) + ">(value.pointerValue())";
                if (!classInfo.isClass)
                    CC <<  "pp->" << field.var << (field.isArray ? "[i]" : "") << " = " << maybeDereference << valuePtr << ";";
                else
                    CC << makeFuncall("pp", true, field.setter, field.isArray, maybeDereference + valuePtr) << ";";
                CC << " break;\n";
            }
            else {
                Assert(false); // we are within an "if (field.isEditable || field.isReplaceable)" block
            }
        }
    }
    CC << "        default: throw omnetpp::cRuntimeError(\"Cannot set field %d of class '" << classInfo.className << "'\", field);\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldStructName()
    CC << "const char *" << classInfo.descriptorClass << "::getFieldStructName(int field) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("return base->getFieldStructName(field);");
    if (numFields == 0) {
        CC << "    return nullptr;\n";
    }
    else {
        CC << "    switch (field) {\n";
        for (size_t i = 0; i < numFields; i++) {
            const FieldInfo& field = classInfo.fieldList[i];
            if (!field.isOpaque && !field.byValue) {
                CC << "        case " << field.symbolicConstant << ": return omnetpp::opp_typename(typeid(" << field.baseDataType << "));\n";
            }
        }
        CC << "        default: return nullptr;\n";
        CC << "    };\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldStructValuePointer()
    CC << "omnetpp::any_ptr " << classInfo.descriptorClass << "::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("return base->getFieldStructValuePointer(object, field, i);");
    CC << "    " << classInfo.className << " *pp = omnetpp::fromAnyPtr<" << classInfo.className << ">(object); (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < numFields; i++) {
        const FieldInfo& field = classInfo.fieldList[i];
        if (!field.byValue) {
            std::string value;
            if (!classInfo.isClass)
                value = str("pp->") + field.var + (field.isArray ? "[i]" : "");
            else
                value = makeFuncall("pp", true, field.getter, field.isArray);
            std::string maybeAddressOf = field.isPointer ? "" : "&";
            CC << "        case " << field.symbolicConstant << ": return omnetpp::toAnyPtr(" << maybeAddressOf << value << "); break;\n";
        }
    }
    CC << "        default: return omnetpp::any_ptr(nullptr);\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // setFieldStructValuePointer()
    CC << "void " << classInfo.descriptorClass << "::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const\n";
    CC << "{\n";
    generateDelegationForBaseClassFields("base->setFieldStructValuePointer(object, field, i, ptr);");
    CC << "    " << classInfo.className << " *pp = omnetpp::fromAnyPtr<" << classInfo.className << ">(object); (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < numFields; i++) {
        const FieldInfo& field = classInfo.fieldList[i];
        if (field.isReplaceable) {
            CC << "        case " << field.symbolicConstant << ": ";
            std::string maybeDereference = field.isPointer ? "" : "*";
            std::string castPtr = "omnetpp::fromAnyPtr<" + removePointer(field.argType) + ">(ptr)";
            if (!classInfo.isClass)
                CC <<  str("pp->") << field.var << (field.isArray ? "[i]" : "") << " = " << maybeDereference << castPtr << ";";
            else
                CC << makeFuncall("pp", true, field.setter, field.isArray, maybeDereference + castPtr) << ";";
            CC << " break;\n";
        }
    }
    CC << "        default: throw omnetpp::cRuntimeError(\"Cannot set field %d of class '" << classInfo.className << "'\", field);\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

}

void MsgCodeGenerator::generateDelegationForBaseClassFields(const std::string& code)
{
    bool containsReturn = code.find("return") != std::string::npos;
    auto semicolonPos = code.find(";");
    Assert(semicolonPos != std::string::npos);
    bool multiStatement = semicolonPos < code.size()-1;  // contains a semicolon in the middle
    bool needsBraces = !containsReturn || multiStatement;

    CC << "    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();\n";
    CC << "    if (base) {\n";
    CC << "        if (field < base->getFieldCount())" << (needsBraces ? "{" : "") << "\n";
    CC << "            " << code << "\n";
    if (!containsReturn)
        CC << "            return;\n";
    if (needsBraces)
        CC << "        }\n";
    CC << "        field -= base->getFieldCount();\n";
    CC << "    }\n";
}

void MsgCodeGenerator::generateEnum(const EnumInfo& enumInfo)
{
    H << "/**\n";
    H << " * Enum generated from <tt>" << enumInfo.astNode->getSourceLocation().str() << "</tt> by " PROGRAM ".\n";
    H << generatePreComment(enumInfo.astNode);
    H << " */\n";

    H << "enum " << enumInfo.enumName <<" {\n";
    for (EnumInfo::FieldList::const_iterator it = enumInfo.fieldList.begin(); it != enumInfo.fieldList.end(); ) {
        H << "    " << it->name << " = " << it->value;
        if (++it != enumInfo.fieldList.end())
            H << ",";
        H << "\n";
    }
    H << "};\n\n";

    H << "inline void doParsimPacking(omnetpp::cCommBuffer *b, const " << enumInfo.enumName << "& e) { b->pack(static_cast<int>(e)); }\n";
    H << "inline void doParsimUnpacking(omnetpp::cCommBuffer *b, " << enumInfo.enumName << "& e) { int n; b->unpack(n); e = static_cast<" << enumInfo.enumName << ">(n); }\n\n";

    CC << "Register_Enum(" << enumInfo.enumQName << ", (";
    bool first = true;
    for (const auto& enumItem : enumInfo.fieldList)
        CC << (first ? (first=false,"") : ", ") << enumInfo.enumQName << "::" << enumItem.name;
    CC << "));\n\n";
}

std::string MsgCodeGenerator::prefixWithNamespace(const std::string& name, const std::string& namespaceName)
{
    return !namespaceName.empty() ? namespaceName + "::" + name : name;  // prefer name from local namespace
}

inline bool containsChar(const std::string& s, char c) { return s.find(c) != std::string::npos; }

std::string MsgCodeGenerator::makeFuncall(const std::string& var, bool isPointer, const std::string& funcTemplate, bool withIndex, const std::string& value)
{
    if (funcTemplate.empty()) {
        return var;
    }
    else if (funcTemplate[0] == '.' || opp_stringbeginswith(funcTemplate.c_str(), "->")) {
        // ".foo()" becomes "var.foo()", "->foo()" becomes "var->foo()"
        int off = (funcTemplate[0] == '.') ? 1 : 2;
        return var + (isPointer ? "->" : ".") + funcTemplate.substr(off);
    }
    else if (containsChar(funcTemplate, '$')) {
        // "tostring($)" becomes "tostring(var)", "getchild($,i)" becomes "getchild(var,i)"
        return opp_replacesubstring(funcTemplate, "$", var, true);
    }
    else if (!containsChar(funcTemplate, '(')) {
        // "foo" is a shorthand for "var->foo()" or "var->foo(i)", depending on flag
        return var + (isPointer ? "->" : ".") + funcTemplate + "(" + (withIndex ? "i" : "") + ((withIndex && value!="") ? "," : "") + value + ")";
    }
    else {
        return funcTemplate;
    }
}

void MsgCodeGenerator::generateOwnershipOp(const FieldInfo& field, const std::string& var, const std::string& op)
{
    Assert(field.isOwnedPointer);
    std::string code;
    if (op == "delete") {
        if (!field.iscObject)
            code = "delete " + var + ";";
        else if (field.iscOwnedObject)
            code = "if (" + var + " != nullptr) dropAndDelete(" + var +");";
        else // plain cObject*
            code = "if (" + var + " != nullptr && " + var + "->isOwnedObject()) dropAndDelete((cOwnedObject*)" + var +"); else delete " + var + ";";
    }
    else {
        Assert(op == "take" || op == "drop");
        if (!field.iscObject)
            code = "";
        else if (field.iscOwnedObject)
            code = "if (" + var + " != nullptr) " + op + "(" + var + ");";
        else // plain cObject*
            code = "if (" + var + " != nullptr && " + var +"->isOwnedObject()) " + op + "((cOwnedObject*)" + var +");";
    }
    if (!code.empty())
        CC << "    " + code + "\n";
}

void MsgCodeGenerator::generateImport(const std::string& importName)
{
    // assuming C++ include path is identical to msg import path, we can directly derive the include from importName
    std::string header = opp_replacesubstring(importName, ".", "/", true) + "_m.h";
    H << "#include \"" << header << "\" // import " << importName << "\n\n";
}

void MsgCodeGenerator::generateNamespaceBegin(const std::string& namespaceName, bool intoCcFile)
{
    H << std::endl;
    auto tokens = opp_split(namespaceName, "::");
    for (auto token : tokens) {
        if (token.empty())
            continue; // leading "::"
        H << "namespace " << token << " {\n";
        if (intoCcFile)
            CC << "namespace " << token << " {\n";
    }
    H << std::endl;
    if (intoCcFile)
        CC << std::endl;
}

void MsgCodeGenerator::generateNamespaceEnd(const std::string& namespaceName, bool intoCcFile)
{
    auto tokens = opp_split(namespaceName, "::");
    std::reverse(tokens.begin(), tokens.end());
    H << std::endl;
    for (auto token : tokens) {
        if (token.empty())
            continue; // leading "::"
        H << "}  // namespace " << token << std::endl;
        if (intoCcFile)
            CC << "}  // namespace " << token << std::endl;
    }
    H << std::endl;
    if (intoCcFile)
        CC << std::endl;
}

void MsgCodeGenerator::generateTypeAnnouncement(const ClassInfo& classInfo)
{
    H << (classInfo.isClass ? "class " : "struct ") << classInfo.name << ";\n";
}

void MsgCodeGenerator::generateMethodCplusplusBlock(const ClassInfo& classInfo, const std::string& method)
{
    auto it = classInfo.methodCplusplusBlocks.find(method);
    if (it != classInfo.methodCplusplusBlocks.end()) {
        generateCplusplusBlock(CC, it->second->getBody());
        classInfo.usedMethodCplusplusBlocks.insert(method);
    }
}

void MsgCodeGenerator::reportUnusedMethodCplusplusBlocks(const ClassInfo& classInfo)
{
    for (auto pair : classInfo.methodCplusplusBlocks)
        if (!contains(classInfo.usedMethodCplusplusBlocks, pair.first))
            errors->addError(pair.second, "invalid target for cplusplus block: no '%s' method generated in the specified type", pair.first.c_str());
}

void MsgCodeGenerator::generateHeaderCplusplusBlock(const std::string& body)
{
    generateCplusplusBlock(H, body);
    H << "\n";
}

void MsgCodeGenerator::generateImplCplusplusBlock(const std::string& body)
{
    generateCplusplusBlock(CC, body);
    CC << "\n";
}

void MsgCodeGenerator::generateCplusplusBlock(std::ofstream& out, const std::string& body)
{
    std::string trimmedBody = body;
    size_t pos0 = trimmedBody.find_first_not_of("\r\n");
    if (pos0 != trimmedBody.npos)
        trimmedBody = trimmedBody.substr(pos0);
    size_t pos = trimmedBody.find_last_not_of("\r\n");
    if (pos != trimmedBody.npos)
        trimmedBody = trimmedBody.substr(0, pos+1);

    out << "// cplusplus {{\n";
    out << trimmedBody << "\n";
    out << "// }}\n";
}



}  // namespace nedxml
}  // namespace omnetpp
