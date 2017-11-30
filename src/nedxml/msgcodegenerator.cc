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

#define PROGRAM    "nedtool"

#define SL(x)    (x)

inline std::string str(const char *s) {
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
    ccStream.open(ccFile);
}

void MsgCodeGenerator::closeFiles()
{
    hStream.close();
    ccStream.close();
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

const char *DESCRIPTOR_BOILERPLATE =
        "namespace {\n"
        "template <class T> inline\n"
        "typename std::enable_if<std::is_polymorphic<T>::value && std::is_base_of<omnetpp::cObject,T>::value, void *>::type\n"
        "toVoidPtr(T* t)\n"
        "{\n"
        "    return (void *)(static_cast<const omnetpp::cObject *>(t));\n"
        "}\n"
        "\n"
        "template <class T> inline\n"
        "typename std::enable_if<std::is_polymorphic<T>::value && !std::is_base_of<omnetpp::cObject,T>::value, void *>::type\n"
        "toVoidPtr(T* t)\n"
        "{\n"
        "    return (void *)dynamic_cast<const void *>(t);\n"
        "}\n"
        "\n"
        "template <class T> inline\n"
        "typename std::enable_if<!std::is_polymorphic<T>::value, void *>::type\n"
        "toVoidPtr(T* t)\n"
        "{\n"
        "    return (void *)static_cast<const void *>(t);\n"
        "}\n"
        "\n"
        "}\n"
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
    H << "#if defined(__clang__)\n";
    H << "#  pragma clang diagnostic ignored \"-Wreserved-id-macro\"\n";
    H << "#endif\n";
    H << "#ifndef " << headerGuard << "\n";
    H << "#define " << headerGuard << "\n\n";
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
    CC << "#include \"" << hfilenamewithoutdir << "\"\n\n";

    CC << PARSIMPACK_BOILERPLATE;

    CC << DESCRIPTOR_BOILERPLATE;

    if (firstNamespace.empty()) {
        H << "\n\n";
        CC << "\n";
        generateTemplates();
    }
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
    H << "/**\n";
    H << " * Class generated from <tt>" << SL(classInfo.astNode->getSourceLocation()) << "</tt> by " PROGRAM ".\n";
    H << generatePreComment(classInfo.astNode);

    if (classInfo.gap) {
        H << " *\n";
        H << " * " << classInfo.msgclass << " is only useful if it gets subclassed, and " << classInfo.realmsgclass << " is derived from it.\n";
        H << " * The minimum code to be written for " << classInfo.realmsgclass << " is the following:\n";
        H << " *\n";
        H << " * <pre>\n";
        H << " * class " << TS(exportDef) << classInfo.realmsgclass << " : public " << classInfo.msgclass << "\n";
        H << " * {\n";
        H << " *   private:\n";
        H << " *     void copy(const " << classInfo.realmsgclass << "& other) { ... }\n\n";
        H << " *   public:\n";
        if (classInfo.iscNamedObject) {
            if (classInfo.keyword == "message" || classInfo.keyword == "packet")
                H << " *     " << classInfo.realmsgclass << "(const char *name=nullptr, short kind=0) : " << classInfo.msgclass << "(name,kind) {}\n";
            else
                H << " *     " << classInfo.realmsgclass << "(const char *name=nullptr) : " << classInfo.msgclass << "(name) {}\n";
        }
        else {
            H << " *     " << classInfo.realmsgclass << "() : " << classInfo.msgclass << "() {}\n";
        }
        H << " *     " << classInfo.realmsgclass << "(const " << classInfo.realmsgclass << "& other) : " << classInfo.msgclass << "(other) {copy(other);}\n";
        H << " *     " << classInfo.realmsgclass << "& operator=(const " << classInfo.realmsgclass << "& other) {if (this==&other) return *this; " << classInfo.msgclass << "::operator=(other); copy(other); return *this;}\n";
        if (classInfo.iscObject)
            H << " *     virtual " << classInfo.realmsgclass << " *dup() const override {return new " << classInfo.realmsgclass << "(*this);}\n";
        H << " *     // ADD CODE HERE to redefine and implement pure virtual functions from " << classInfo.msgclass << "\n";
        H << " * };\n";
        H << " * </pre>\n";
        if (classInfo.iscObject) {
            H << " *\n";
            H << " * The following should go into a .cc (.cpp) file:\n";
            H << " *\n";
            H << " * <pre>\n";
            H << " * Register_Class(" << classInfo.realmsgclass << ")\n";
            H << " * </pre>\n";
        }
    }
    H << " */\n";

    H << "class " << TS(exportDef) << classInfo.msgclass;
    const char *baseclassSepar = " : ";
    if (!classInfo.msgbaseclass.empty()) {
        H << baseclassSepar << "public ::" << classInfo.msgbaseclass;  // make namespace explicit and absolute to disambiguate the way PROGRAM understood it
        baseclassSepar = ", ";
    }

    for (const auto& impl : classInfo.implements) {
        H << baseclassSepar << "public " << impl;
        baseclassSepar = ", ";
    }

    H << "\n{\n";
    H << "  protected:\n";
    for (const FieldInfo& field : classInfo.fieldlist) {
        if (!field.fisabstract) {
            if (field.fisarray && !field.farraysize.empty()) {
                H << "    " << field.datatype << " " << field.var << "[" << field.farraysize << "];\n";
            }
            else if (field.fisarray && field.farraysize.empty()) {
                H << "    " << field.datatype << " *" << field.var << "; // array ptr\n";
                H << "    " << field.fsizetype << " " << field.varsize << ";\n";
            }
            else {
                H << "    " << field.datatype << " " << field.var << ";\n";
            }
        }
    }
    H << "\n";
    H << "  private:\n";
    H << "    void copy(const " << classInfo.msgclass << "& other);\n\n";
    H << "  protected:\n";
    H << "    // protected and unimplemented operator==(), to prevent accidental usage\n";
    H << "    bool operator==(const " << classInfo.msgclass << "&);\n";
    if (classInfo.gap) {
        H << "    // make constructors protected to avoid instantiation\n";
    }
    else {
        H << "\n";
        H << "  public:\n";
    }
    if (classInfo.iscNamedObject) {
        if (classInfo.keyword == "message" || classInfo.keyword == "packet") {
            H << "    " << classInfo.msgclass << "(const char *name=nullptr, short kind=0);\n";
        }
        else {
            H << "    " << classInfo.msgclass << "(const char *name=nullptr);\n";
        }
    }
    else {
        H << "    " << classInfo.msgclass << "();\n";
    }
    H << "    " << classInfo.msgclass << "(const " << classInfo.msgclass << "& other);\n";
    if (classInfo.gap) {
        H << "    // make assignment operator protected to force the user override it\n";
        H << "    " << classInfo.msgclass << "& operator=(const " << classInfo.msgclass << "& other);\n";
        H << "\n";
        H << "  public:\n";
    }
    H << "    virtual ~" << classInfo.msgclass << "();\n";
    if (!classInfo.gap) {
        H << "    " << classInfo.msgclass << "& operator=(const " << classInfo.msgclass << "& other);\n";
    }
    if (classInfo.iscObject) {
        H << "    virtual " << classInfo.msgclass << " *dup() const override ";
        if (classInfo.gap)
            H << "{throw omnetpp::cRuntimeError(\"You forgot to manually add a dup() function to class " << classInfo.realmsgclass << "\");}\n";
        else
            H << "{return new " << classInfo.msgclass << "(*this);}\n";
    }
    std::string maybe_override = classInfo.iscObject ? " override" : "";
    std::string maybe_handleChange = classInfo.beforeChange.empty() ? "" : (classInfo.beforeChange + ";");
    std::string maybe_handleChange_line = classInfo.beforeChange.empty() ? "" : (str("    ") + classInfo.beforeChange + ";\n");
    H << "    virtual void parsimPack(omnetpp::cCommBuffer *b) const" << maybe_override << ";\n";
    H << "    virtual void parsimUnpack(omnetpp::cCommBuffer *b)" << maybe_override << ";\n";
    H << "\n";
    H << "    // field getter/setter methods\n";
    for (const auto& field : classInfo.fieldlist) {
        std::string pure;
        if (field.fisabstract)
            pure = " = 0";
        std::string overrideGetter(field.overrideGetter ? " override" : "");
        std::string overrideSetter(field.overrideSetter ? " override" : "");
        std::string getterIndexVar("");
        std::string getterIndexArg("");
        std::string setterIndexArg("");

        // size setter, size getter
        if (field.fisarray) {
            getterIndexVar = "k";
            getterIndexArg = field.fsizetype + " " + getterIndexVar;
            setterIndexArg = getterIndexArg + ", ";
            if (field.farraysize.empty())
                H << "    virtual void " << field.alloc << "(" << field.fsizetype << " size)" << pure << ";\n";
            H << "    virtual " << field.fsizetype << " " << field.getsize << "() const" << pure << ";\n";
        }

        // getter, setter, remover
        std::string maybeConst = (field.fispointer || !field.byvalue) ? "const " : "";
        H << "    virtual " << maybeConst << field.rettype << " " << field.getter << "(" << getterIndexArg << ") const" << overrideGetter << pure << ";\n";
        if (field.fispointer || !field.byvalue)
            H << "    virtual " << field.rettype << " " << field.mGetter << "(" << getterIndexArg << ")" << overrideGetter << " { " << maybe_handleChange << "return const_cast<" << field.rettype << ">(const_cast<const " << classInfo.msgclass << "*>(this)->" << field.getter << "(" << getterIndexVar << "));}\n";
        if (field.fispointer && field.fisownedpointer)  //TODO should be just "if (fispointer)"?
            H << "    virtual " << field.rettype << " " << field.remover << "(" << getterIndexArg << ")" << overrideGetter << pure << ";\n";
        H << "    virtual void " << field.setter << "(" << setterIndexArg << field.argtype << " " << field.argname << ")" << overrideSetter << pure << ";\n";
    }
    H << "};\n\n";

    if (!classInfo.gap) {
        if (classInfo.iscObject) {
            CC << "Register_Class(" << classInfo.msgclass << ")\n\n";
            H << "inline void doParsimPacking(omnetpp::cCommBuffer *b, const " << classInfo.realmsgclass << "& obj) {obj.parsimPack(b);}\n";
            H << "inline void doParsimUnpacking(omnetpp::cCommBuffer *b, " << classInfo.realmsgclass << "& obj) {obj.parsimUnpack(b);}\n\n";
        }
    }

    // constructor:
    bool isMessage = classInfo.keyword == "message" || classInfo.keyword == "packet";
    std::string ctorArgs = isMessage ? "(const char *name, short kind)" : classInfo.iscNamedObject ? "(const char *name)" : "()";
    std::string baseArgs = isMessage ? "(name, kind)" : classInfo.iscNamedObject ? "(name)" : "()";
    CC << classInfo.msgclass << "::" << classInfo.msgclass << ctorArgs;
    if (classInfo.msgbaseclass != "")
        CC << " : ::" << classInfo.msgbaseclass << baseArgs << "\n";
    CC << "\n";
    CC << "{\n";
    // CC << "    (void)static_cast<cObject *>(this); //sanity check\n" if (info.fieldclasstype == ClassType::COBJECT);
    // CC << "    (void)static_cast<cNamedObject *>(this); //sanity check\n" if (info.fieldclasstype == ClassType::CNAMEDOBJECT);
    // CC << "    (void)static_cast<cOwnedObject *>(this); //sanity check\n" if (info.fieldclasstype == ClassType::COWNEDOBJECT);
    for (const auto& baseclassField : classInfo.baseclassFieldlist)
        CC << "    this->" << baseclassField.setter << "(" << baseclassField.fval << ");\n";
    if (!classInfo.baseclassFieldlist.empty() && !classInfo.fieldlist.empty())
        CC << "\n";
    for (const auto& field : classInfo.fieldlist) {
        if (!field.fisabstract) {
            if (field.fisarray) {
                if (!field.farraysize.empty()) {
                    if (!field.fval.empty()) {
                        CC << "    for (" << field.fsizetype << " i = 0; i < " << field.varsize << "; i++)\n";
                        CC << "        this->" << field.var << "[i] = " << field.fval << ";\n";
                    }
                    if (field.iscOwnedObject || field.fisownedpointer) {
                        CC << "    for (" << field.fsizetype << " i = 0; i < " << field.varsize << "; i++)\n";
                        if (field.fispointer) {
                            if (field.fisownedpointer)
                                CC << "        if (this->" << field.var << " != nullptr) { take(this->" << field.var << "[i]); } // XXX\n";
                        }
                        else
                            CC << "        take(&(this->" << field.var << "[i]));\n";
                    }
                }
                else {
                    CC << "    " << field.varsize << " = 0;\n";
                    CC << "    this->" << field.var << " = nullptr;\n";
                }
            }
            else {
                if (!field.fval.empty()) {
                    CC << "    this->" << field.var << " = " << field.fval << ";\n";
                }
                if (field.iscOwnedObject) {
                    if (field.fispointer)
                        CC << "    if (this->" << field.var << " != nullptr) { take(this->" << field.var << "); }\n";
                    else
                        CC << "    take(&(this->" << field.var << "));\n";
                }
            }
        }
    }
    CC << "}\n\n";

    // copy constructor:
    CC << "" << classInfo.msgclass << "::" << classInfo.msgclass << "(const " << classInfo.msgclass << "& other)";
    if (!classInfo.msgbaseclass.empty()) {
        CC << " : ::" << classInfo.msgbaseclass << "(other)";
        //TODO @implements ???
    }
    CC << "\n{\n";
    for (const auto& field : classInfo.fieldlist) {
        if (!field.fisabstract) {
            if (field.fisarray) {
                if (!field.farraysize.empty()) {
                    if (field.fispointer) {
                        CC << "    for (" << field.fsizetype << " i = 0; i < " << field.varsize << "; i++)\n";
                        CC << "        this->" << field.var << "[i] = nullptr;\n";
                    }
                    else if (field.iscOwnedObject) {
                        CC << "    for (" << field.fsizetype << " i = 0; i < " << field.varsize << "; i++)\n";
                        CC << "        take(&(this->" << field.var << "[i]));\n";
                    }
                }
                else
                {
                    CC << "    " << field.varsize << " = 0;\n";
                    CC << "    this->" << field.var << " = nullptr;\n";
                }
            }
            else {
                if (field.fispointer) {
                    CC << "    this->" << field.var << " = nullptr;\n";
                }
                else if (field.iscOwnedObject) {
                    CC << "    take(&(this->" << field.var << "));\n";
                }
            }
        }
    }
    CC << "    copy(other);\n";
    CC << "}\n\n";

    // destructor:
    CC << "" << classInfo.msgclass << "::~" << classInfo.msgclass << "()\n";
    CC << "{\n";
    for (const auto& field : classInfo.fieldlist) {
        if (!field.fisabstract) {
            if (field.fisarray) {
                std::ostringstream s;
                if (field.fisownedpointer)
                    s << "        dropAndDelete(this->" << field.var << "[i]);\n";
                else if (!field.fispointer && field.iscOwnedObject)
                    s << "        drop(&(this->" << field.var << "[i]));\n";
                if (!s.str().empty()) {
                    CC << "    for (" << field.fsizetype << " i = 0; i < " << field.varsize << "; i++) {\n";
                    CC << s.str();
                    CC << "    }\n";
                }

                if (field.farraysize.empty()) {
                    CC << "    delete [] this->" << field.var << ";\n";
                }
            }
            else {
                if (field.fisownedpointer)
                    CC << "    dropAndDelete(this->" << field.var << ");\n";
                else if (!field.fispointer && field.iscOwnedObject)
                    CC << "    drop(&(this->" << field.var << "));\n";
            }
        }
    }
    CC << "}\n\n";

    // operator = :
    CC << "" << classInfo.msgclass << "& " << classInfo.msgclass << "::operator=(const " << classInfo.msgclass << "& other)\n";
    CC << "{\n";
    CC << "    if (this==&other) return *this;\n";
    if (classInfo.msgbaseclass != "")
        CC << "    ::" << classInfo.msgbaseclass << "::operator=(other);\n";
    CC << "    copy(other);\n";
    CC << "    return *this;\n";
    CC << "}\n\n";

    // copy function:
    CC << "void " << classInfo.msgclass << "::copy(const " << classInfo.msgclass << "& other)\n";
    CC << "{\n";
    for (const auto& field : classInfo.fieldlist) {
        // CC << "// " << field.fname << ":\n";
        if (!field.fisabstract) {
            if (field.fisarray) {
                if (field.fispointer) {
                    if (field.fisownedpointer) {
                        CC << "    for (" << field.fsizetype << " i = 0; i < " << field.varsize << "; i++) {\n";
                        CC << "        dropAndDelete(this->" << field.var << "[i]);\n";
                        CC << "    }\n";
                    }
                }
                if (field.farraysize.empty()) {
                    if (!field.fispointer && field.iscOwnedObject) {
                        CC << "    for (" << field.fsizetype << " i = 0; i < " << field.varsize << "; i++) {\n";
                        CC << "        drop(&(this->" << field.var << "[i]));\n";
                        CC << "    }\n";
                    }
                    CC << "    delete [] this->" << field.var << ";\n";
                    CC << "    this->" << field.var << " = (other." << field.varsize << "==0) ? nullptr : new " << field.datatype << "[other." << field.varsize << "];\n";
                    CC << "    " << field.varsize << " = other." << field.varsize << ";\n";
                    if (!field.fispointer && field.iscOwnedObject) {
                        CC << "    for (" << field.fsizetype << " i = 0; i < " << field.varsize << "; i++) {\n";
                        CC << "        take(&(this->" << field.var << "[i]));\n";
                        CC << "    }\n";
                    }
                }

                CC << "    for (" << field.fsizetype << " i = 0; i < " << field.varsize << "; i++) {\n";
                if (field.fispointer) {
                    if (field.fisownedpointer) {
                        CC << "        this->" << field.var << "[i] = other." << field.var << "[i]->dup();\n";
                        CC << "        take(this->" << field.var << "[i]);\n";
                    }
                    else {
                        CC << "        this->" << field.var << "[i] = other." << field.var << "[i];\n";
                        if (field.iscNamedObject) {
                            CC << "        this->" << field.var << "[i]->setName(other." << field.var << "[i]->getName());\n";
                        }
                    }
                }
                else {
                    CC << "        this->" << field.var << "[i] = other." << field.var << "[i];\n";
                    if (field.iscNamedObject) {
                        CC << "        this->" << field.var << "[i].setName(other." << field.var << "[i].getName());\n";
                    }
                    CC << "    }\n";
                }
            }
            else {
                if (field.fispointer) {
                    if (field.fisownedpointer) {
                        CC << "    dropAndDelete(this->" << field.var << ");\n";
                        CC << "    this->" << field.var << " = other." << field.var << "->dup();\n";
                        CC << "    take(this->" << field.var << ");\n";
                    }
                    else {
                        CC << "    this->" << field.var << " = other." << field.var << ";\n";
                        if (field.iscNamedObject) {
                            CC << "    this->" << field.var << "->setName(other." << field.var << "->getName());\n";
                        }
                    }
                }
                else {
                    CC << "    this->" << field.var << " = other." << field.var << ";\n";
                    if (field.iscNamedObject) {
                        CC << "    this->" << field.var << ".setName(other." << field.var << ".getName());\n";
                    }
                }
            }
        }
    }
    CC << "}\n\n";

    //
    // Note: This class may not be derived from cObject, and then this parsimPack()/
    // parsimUnpack() is NOT that of cObject. However it's still needed because a
    // "friend" doParsimPacking() function could not access protected members otherwise.
    //
    CC << "void " << classInfo.msgclass << "::parsimPack(omnetpp::cCommBuffer *b) const\n";
    CC << "{\n";
    if (classInfo.msgbaseclass != "") {
        if (classInfo.iscObject) {
            if (classInfo.msgbaseclass != "omnetpp::cObject")
                CC << "    ::" << classInfo.msgbaseclass << "::parsimPack(b);\n";
        }
        else {
            CC << "    doParsimPacking(b,(::" << classInfo.msgbaseclass << "&)*this);\n";  // this would do for cOwnedObject too, but the other is nicer
        }
    }
    for (const auto& field : classInfo.fieldlist) {
        if (field.fnopack) {
            // @nopack specified
        }
        else if (field.fisabstract) {
            CC << "    // field " << field.fname << " is abstract -- please do packing in customized class\n";
        }
        else {
            if (field.fisarray) {
                if (field.farraysize.empty())
                    CC << "    b->pack(" << field.varsize << ");\n";
                CC << "    doParsimArrayPacking(b,this->" << field.var << "," << field.varsize << ");\n";
            }
            else {
                CC << "    doParsimPacking(b,this->" << field.var << ");\n";
            }
        }
    }
    CC << "}\n\n";

    CC << "void " << classInfo.msgclass << "::parsimUnpack(omnetpp::cCommBuffer *b)\n";
    CC << "{\n";
    if (classInfo.msgbaseclass != "") {
        if (classInfo.iscObject) {
            if (classInfo.msgbaseclass != "omnetpp::cObject")
                CC << "    ::" << classInfo.msgbaseclass << "::parsimUnpack(b);\n";
        }
        else {
            CC << "    doParsimUnpacking(b,(::" << classInfo.msgbaseclass << "&)*this);\n";  // this would do for cOwnedObject too, but the other is nicer
        }
    }
    for (const auto& field : classInfo.fieldlist) {
        if (field.fnopack) {
            // @nopack specified
        }
        else if (field.fisabstract) {
            CC << "    // field " << field.fname << " is abstract -- please do unpacking in customized class\n";
        }
        else {
            if (field.fisarray) {
                if (!field.farraysize.empty()) {
                    CC << "    doParsimArrayUnpacking(b,this->" << field.var << "," << field.farraysize << ");\n";
                }
                else {
                    CC << "    delete [] this->" << field.var << ";\n";
                    CC << "    b->unpack(" << field.varsize << ");\n";
                    CC << "    if (" << field.varsize << "==0) {\n";
                    CC << "        this->" << field.var << " = nullptr;\n";
                    CC << "    } else {\n";
                    CC << "        this->" << field.var << " = new " << field.datatype << "[" << field.varsize << "];\n";
                    CC << "        doParsimArrayUnpacking(b,this->" << field.var << "," << field.varsize << ");\n";
                    CC << "    }\n";
                }
            }
            else {
                CC << "    doParsimUnpacking(b,this->" << field.var << ");\n";
            }
        }
    }
    CC << "}\n\n";

    for (const auto& field : classInfo.fieldlist) {
        if (!field.fisabstract) {
            std::string idx = (field.fisarray) ? "[k]" : "";
            std::string idxarg = (field.fisarray) ? (field.fsizetype + " k") : std::string("");
            std::string idxarg2 = (field.fisarray) ? (idxarg + ", ") : std::string("");
            std::string indexedVar = str("this->") + field.var + idx;

            // getters:
            if (field.fisarray) {
                CC << "" << field.fsizetype << " " << classInfo.msgclass << "::" << field.getsize << "() const\n";
                CC << "{\n";
                CC << "    return " << field.varsize << ";\n";
                CC << "}\n\n";
            }

            std::string maybeConst = (field.fispointer || !field.byvalue) ? "const " : "";
            CC << maybeConst << field.rettype << " " << classInfo.msgclass << "::" << field.getter << "(" << idxarg << ")" << " const\n";
            CC << "{\n";
            if (field.fisarray)
                CC << "    if (k >= " << field.varsize << ") throw omnetpp::cRuntimeError(\"Array of size " << field.varsize << " indexed by %lu\", (unsigned long)k);\n";
            CC << "    return " << makeFuncall(indexedVar, field.getterconversion) + ";\n";
            CC << "}\n\n";

            // resize:
            if (field.fisarray && field.farraysize.empty()) {
                CC << "void " << classInfo.msgclass << "::" << field.alloc << "(" << field.fsizetype << " size)\n";
                CC << "{\n";
                CC << maybe_handleChange_line;
                CC << "    " << field.datatype << " *" << field.var << "2 = (size==0) ? nullptr : new " << field.datatype << "[size];\n";
                CC << "    " << field.fsizetype << " sz = " << field.varsize << " < size ? " << field.varsize << " : size;\n";
                CC << "    for (" << field.fsizetype << " i = 0; i < sz; i++)\n";
                CC << "        " << field.var << "2[i] = this->" << field.var << "[i];\n";
                if (!field.fval.empty()) {
                    CC << "    for (" << field.fsizetype << " i=sz; i<size; i++)\n";
                    CC << "        " << field.var << "2[i] = " << field.fval << ";\n";
                }
                if (field.iscOwnedObject) {
                    CC << "    for (" << field.fsizetype << " i = sz; i < size; i++)\n";
                    CC << "        take(&(" << field.var << "2[i]));\n";
                }
                CC << "    " << field.varsize << " = size;\n";
                CC << "    delete [] this->" << field.var << ";\n";
                CC << "    this->" << field.var << " = " << field.var << "2;\n";
                CC << "}\n\n";
            }

            // setter:
            CC << "void " << classInfo.msgclass << "::" << field.setter << "(" << idxarg2 << field.argtype << " " << field.argname << ")\n";
            CC << "{\n";
            if (field.fisarray) {
                CC << "    if (k >= " << field.varsize << ") throw omnetpp::cRuntimeError(\"Array of size " << field.farraysize << " indexed by %lu\", (unsigned long)k);\n";
            }
            CC << maybe_handleChange_line;
            if (field.fisownedpointer) {
                CC << "    dropAndDelete(this->" << field.var << idx << ");\n";
            }
            CC << "    " << indexedVar << " = " << field.argname << ";\n";
            if (field.fisownedpointer) {
                CC << "    if (" << indexedVar << " != nullptr)\n";
                CC << "        take(" << indexedVar << ");\n";
            }
            CC << "}\n\n";

            // remover:
            if (field.fisownedpointer) {  //TODO fispointer or fisownedpointer?
                CC << field.rettype << " " << classInfo.msgclass << "::" << field.remover << "(" << idxarg2 << field.argtype << " " << field.argname << ")\n";
                CC << "{\n";
                if (field.fisarray) {
                    CC << "    if (k >= " << field.varsize << ") throw omnetpp::cRuntimeError(\"Array of size " << field.farraysize << " indexed by %lu\", (unsigned long)k);\n";
                }
                CC << maybe_handleChange_line;
                CC << "    " << field.rettype << " retval = " << indexedVar << ";\n";
                CC << "    drop(retval);\n";
                CC << "    " << indexedVar << " = nullptr;\n";
                CC << "    return retval;\n";
                CC << "}\n\n";
            }
        }
    }
}

void MsgCodeGenerator::generateStruct(const ClassInfo& classInfo, const std::string& exportDef)
{
    H << "/**\n";
    H << " * Struct generated from " << SL(classInfo.astNode->getSourceLocation()) << " by " PROGRAM ".\n";
    H << " */\n";
    if (classInfo.msgbaseclass.empty()) {
        H << "struct " << TS(exportDef) << classInfo.msgclass << "\n";
    }
    else {
        H << "struct " << TS(exportDef) << classInfo.msgclass << " : public ::" << classInfo.msgbaseqname << "\n";
    }
    H << "{\n";
    H << "    " << classInfo.msgclass << "();\n";
    for (const auto& field : classInfo.fieldlist) {
        H << "    " << field.datatype << " " << field.var;
        if (field.fisarray)
            H << "[" << field.farraysize << "]";
        H << ";\n";
    }
    H << "};\n\n";

    H << "// helpers for local use\n";
    H << "void " << TS(exportDef) << "__doPacking(omnetpp::cCommBuffer *b, const " << classInfo.msgclass << "& a);\n";
    H << "void " << TS(exportDef) << "__doUnpacking(omnetpp::cCommBuffer *b, " << classInfo.msgclass << "& a);\n\n";

    H << "inline void doParsimPacking(omnetpp::cCommBuffer *b, const " << classInfo.realmsgclass << "& obj) { " << "__doPacking(b, obj); }\n";
    H << "inline void doParsimUnpacking(omnetpp::cCommBuffer *b, " << classInfo.realmsgclass << "& obj) { " << "__doUnpacking(b, obj); }\n\n";

    // Constructor:
    CC << "" << classInfo.msgclass << "::" << classInfo.msgclass << "()\n";
    CC << "{\n";
    // override baseclass fields initial value
    for (const auto& field : classInfo.baseclassFieldlist) {
        CC << "    this->" << field.var << " = " << field.fval << ";\n";
    }
    if (!classInfo.baseclassFieldlist.empty() && !classInfo.fieldlist.empty())
        CC << "\n";

    for (const auto& field : classInfo.fieldlist) {
        Assert(!field.fisabstract); // ensured in the analyzer
        Assert(!field.iscOwnedObject); // ensured in the analyzer
        if (field.fisarray) {
            Assert(!field.farraysize.empty()); // ensured in the analyzer
            if (!field.fval.empty()) {
                CC << "    for (" << field.fsizetype << " i = 0; i < " << field.farraysize << "; i++)\n";
                CC << "        this->" << field.var << "[i] = " << field.fval << ";\n";
            }
        }
        else if (!field.fval.empty()) {
            CC << "    this->" << field.var << " = " << field.fval << ";\n";
        }
    }
    CC << "}\n\n";

    // doPacking/doUnpacking go to the global namespace
    CC << "void __doPacking(omnetpp::cCommBuffer *b, const " << classInfo.msgclass << "& a)\n";
    CC << "{\n";
    if (!classInfo.msgbaseclass.empty())
        CC << "    doParsimPacking(b,(::" << classInfo.msgbaseclass << "&)a);\n";
    for (const auto& field : classInfo.fieldlist) {
        if (field.fisarray)
            CC << "    doParsimArrayPacking(b,a." << field.var << "," << field.farraysize << ");\n";
        else
            CC << "    doParsimPacking(b,a." << field.var << ");\n";
    }
    CC << "}\n\n";

    CC << "void __doUnpacking(omnetpp::cCommBuffer *b, " << classInfo.msgclass << "& a)\n";
    CC << "{\n";
    if (!classInfo.msgbaseclass.empty())
        CC << "    doParsimUnpacking(b,(::" << classInfo.msgbaseclass << "&)a);\n";
    for (const auto& field : classInfo.fieldlist) {
        if (field.fisarray)
            CC << "    doParsimArrayUnpacking(b,a." << field.var << "," << field.farraysize << ");\n";
        else
            CC << "    doParsimUnpacking(b,a." << field.var << ");\n";
    }
    CC << "}\n\n";
}

void MsgCodeGenerator::generateDescriptorClass(const ClassInfo& classInfo)
{
    CC << "class " << classInfo.msgdescclass << " : public omnetpp::cClassDescriptor\n";
    CC << "{\n";
    CC << "  private:\n";
    CC << "    mutable const char **propertynames;\n";
    CC << "  public:\n";
    CC << "    " << classInfo.msgdescclass << "();\n";
    CC << "    virtual ~" << classInfo.msgdescclass << "();\n";
    CC << "\n";
    CC << "    virtual bool doesSupport(omnetpp::cObject *obj) const override;\n";
    CC << "    virtual const char **getPropertyNames() const override;\n";
    CC << "    virtual const char *getProperty(const char *propertyname) const override;\n";
    CC << "    virtual int getFieldCount() const override;\n";
    CC << "    virtual const char *getFieldName(int field) const override;\n";
    CC << "    virtual int findField(const char *fieldName) const override;\n";
    CC << "    virtual unsigned int getFieldTypeFlags(int field) const override;\n";
    CC << "    virtual const char *getFieldTypeString(int field) const override;\n";
    CC << "    virtual const char **getFieldPropertyNames(int field) const override;\n";
    CC << "    virtual const char *getFieldProperty(int field, const char *propertyname) const override;\n";
    CC << "    virtual int getFieldArraySize(void *object, int field) const override;\n";
    CC << "\n";
    CC << "    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;\n";
    CC << "    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;\n";
    CC << "    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;\n";
    CC << "\n";
    CC << "    virtual const char *getFieldStructName(int field) const override;\n";
    CC << "    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;\n";
    CC << "};\n\n";

    // register class
    CC << "Register_ClassDescriptor(" << classInfo.msgdescclass << ")\n\n";

    // ctor, dtor
    size_t fieldcount = classInfo.fieldlist.size();
    std::string qualifiedrealmsgclass = prefixWithNamespace(classInfo.realmsgclass, classInfo.namespacename);
    CC << "" << classInfo.msgdescclass << "::" << classInfo.msgdescclass << "() : omnetpp::cClassDescriptor(\"" << qualifiedrealmsgclass << "\", \"" << classInfo.msgbaseclass << "\")\n";
    CC << "{\n";
    CC << "    propertynames = nullptr;\n";
    CC << "}\n";
    CC << "\n";

    CC << "" << classInfo.msgdescclass << "::~" << classInfo.msgdescclass << "()\n";
    CC << "{\n";
    CC << "    delete[] propertynames;\n";
    CC << "}\n";
    CC << "\n";

    // doesSupport()
    CC << "bool " << classInfo.msgdescclass << "::doesSupport(omnetpp::cObject *obj) const\n";
    CC << "{\n";
    CC << "    return dynamic_cast<" << classInfo.msgclass << " *>(obj)!=nullptr;\n";
    CC << "}\n";
    CC << "\n";

    // getPropertyNames()
    CC << "const char **" << classInfo.msgdescclass << "::getPropertyNames() const\n";
    CC << "{\n";
    CC << "    if (!propertynames) {\n";
    CC << "        static const char *names[] = { ";
    for (const auto& prop : classInfo.props) {
        CC << opp_quotestr(prop.first) << ", ";
    }
    CC << " nullptr };\n";
    CC << "        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;\n";
    CC << "        propertynames = mergeLists(basenames, names);\n";
    CC << "    }\n";
    CC << "    return propertynames;\n";
    CC << "}\n";
    CC << "\n";

    // getProperty()
    CC << "const char *" << classInfo.msgdescclass << "::getProperty(const char *propertyname) const\n";
    CC << "{\n";
    for (const auto& prop : classInfo.props) {
        CC << "    if (!strcmp(propertyname,"<< opp_quotestr(prop.first) << ")) return " << opp_quotestr(prop.second) << ";\n";
    }
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    return basedesc ? basedesc->getProperty(propertyname) : nullptr;\n";
    CC << "}\n";
    CC << "\n";

    // getFieldCount()
    CC << "int " << classInfo.msgdescclass << "::getFieldCount() const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    return basedesc ? " << fieldcount << "+basedesc->getFieldCount() : " << fieldcount << ";\n";
    CC << "}\n";
    CC << "\n";

    // getFieldTypeFlags()
    CC << "unsigned int " << classInfo.msgdescclass << "::getFieldTypeFlags(int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldTypeFlags(field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    if (fieldcount == 0) {
        CC << "    return 0;\n";
    }
    else {
        CC << "    static unsigned int fieldTypeFlags[] = {\n";
        for (auto& field : classInfo.fieldlist) {
            StringVector flags;
            if (field.fisarray)
                flags.push_back("FD_ISARRAY");
            if (!field.fopaque)
                flags.push_back("FD_ISCOMPOUND");
            if (field.fispointer)
                flags.push_back("FD_ISPOINTER");
            if (field.iscObject)
                flags.push_back("FD_ISCOBJECT");
            if (field.iscOwnedObject)
                flags.push_back("FD_ISCOWNEDOBJECT");
            if (field.feditable)
                flags.push_back("FD_ISEDITABLE");
            std::string flagss;
            if (flags.empty())
                flagss = "0";
            else {
                flagss = flags[0];
                for (size_t i = 1; i < flags.size(); i++)
                    flagss = flagss + " | " + flags[i];
            }

            CC << "        " << flagss << ",\n";
        }
        CC << "    };\n";
        CC << "    return (field >= 0 && field < " << fieldcount << ") ? fieldTypeFlags[field] : 0;\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldName()
    CC << "const char *" << classInfo.msgdescclass << "::getFieldName(int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldName(field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    if (fieldcount == 0) {
        CC << "    return nullptr;\n";
    }
    else {
        CC << "    static const char *fieldNames[] = {\n";
        for (const auto& field : classInfo.fieldlist) {
            CC << "        \"" << field.fname << "\",\n";
        }
        CC << "    };\n";
        CC << "    return (field >= 0 && field < " << fieldcount << ") ? fieldNames[field] : nullptr;\n";
    }
    CC << "}\n";
    CC << "\n";

    // findField()
    CC << "int " << classInfo.msgdescclass << "::findField(const char *fieldName) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    if (fieldcount > 0) {
        CC << "    int base = basedesc ? basedesc->getFieldCount() : 0;\n";
        for (size_t i = 0; i < classInfo.fieldlist.size(); ++i) {
            const FieldInfo& field = classInfo.fieldlist[i];
            char c = field.fname[0];
            CC << "    if (fieldName[0] == '" << c << "' && strcmp(fieldName, \""<<field.fname<<"\") == 0) return base+" << i << ";\n";
        }
    }
    CC << "    return basedesc ? basedesc->findField(fieldName) : -1;\n";
    CC << "}\n";
    CC << "\n";

    // getFieldTypeString()
    CC << "const char *" << classInfo.msgdescclass << "::getFieldTypeString(int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldTypeString(field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    if (fieldcount == 0) {
        CC << "    return nullptr;\n";
    }
    else {
        CC << "    static const char *fieldTypeStrings[] = {\n";
        for (const auto& field : classInfo.fieldlist) {
            CC << "        \"" << field.ftypeqname << "\",\n";
        }
        CC << "    };\n";
        CC << "    return (field >= 0 && field < " << fieldcount << ") ? fieldTypeStrings[field] : nullptr;\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldPropertyNames()
    CC << "const char **" << classInfo.msgdescclass << "::getFieldPropertyNames(int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldPropertyNames(field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; ++i) {
        const Properties& ref = classInfo.fieldlist[i].fprops;
        if (!ref.empty()) {
            CC << "        case " << i << ": {\n";
            CC << "            static const char *names[] = { ";
            for (const auto& it : ref)
                CC << opp_quotestr(it.first) << ", ";
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
    CC << "const char *" << classInfo.msgdescclass << "::getFieldProperty(int field, const char *propertyname) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldProperty(field, propertyname);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    switch (field) {\n";

    for (size_t i = 0; i < fieldcount; ++i) {
        const FieldInfo& field = classInfo.fieldlist[i];
        const Properties& ref = field.fprops;
        if (!ref.empty()) {
            CC << "        case " << i << ":\n";
            for (const auto& it : ref) {
                std::string prop = opp_quotestr(it.second);
                CC << "            if (!strcmp(propertyname,\"" << it.first << "\")) return " << prop << ";\n";
            }
            CC << "            return nullptr;\n";
        }
    }

    CC << "        default: return nullptr;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldArraySize()
    CC << "int " << classInfo.msgdescclass << "::getFieldArraySize(void *object, int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldArraySize(object, field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << classInfo.msgclass << " *pp = (" << classInfo.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const FieldInfo& field = classInfo.fieldlist[i];
        if (field.fisarray) {
            CC << "        case " << i << ": ";
            if (!field.farraysize.empty()) {
                CC << "return " << field.farraysize << ";\n";
            }
            else if (!classInfo.isClass) {
                CC << "return pp->" << field.varsize << ";\n";
            }
            else {
                CC << "return " << makeFuncall("pp", field.getsize) << ";\n";
            }
        }
    }
    CC << "        default: return 0;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldDynamicTypeString()
    CC << "const char *" << classInfo.msgdescclass << "::getFieldDynamicTypeString(void *object, int field, int i) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldDynamicTypeString(object,field,i);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << classInfo.msgclass << " *pp = (" << classInfo.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const FieldInfo& field = classInfo.fieldlist[i];
        if (field.fispointer) {
            CC << "        case " << i << ": ";
            CC << "{ const " << field.ftype << " *value = " << makeFuncall("pp", field.getter, field.fisarray) << "; return omnetpp::opp_typename(typeid(*value)); }\n";
        }
    }
    CC << "        default: return nullptr;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldValueAsString()
    CC << "std::string " << classInfo.msgdescclass << "::getFieldValueAsString(void *object, int field, int i) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldValueAsString(object,field,i);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << classInfo.msgclass << " *pp = (" << classInfo.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const FieldInfo& field = classInfo.fieldlist[i];
        CC << "        case " << i << ": ";
        if (!classInfo.isClass && field.fisarray) {
            Assert(!field.farraysize.empty()); // struct may not contain dynamic arrays; checked by analyzer
            CC << "if (i >= " << field.farraysize << ") return \"\";\n                ";
        }
        std::string value = classInfo.isClass ?
                makeFuncall("pp", field.getter, field.fisarray) :
                (str("pp->") + field.var + (field.fisarray ? "[i]" : ""));
        if (!field.tostring.empty())
            CC << "return " << makeFuncall(value, field.tostring) << ";\n";
        else
            CC << "{std::stringstream out; out << " << value << "; return out.str();}\n";
    }
    CC << "        default: return \"\";\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // setFieldValueAsString()
    CC << "bool " << classInfo.msgdescclass << "::setFieldValueAsString(void *object, int field, int i, const char *value) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->setFieldValueAsString(object,field,i,value);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << classInfo.msgclass << " *pp = (" << classInfo.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const FieldInfo& field = classInfo.fieldlist[i];
        if (field.feditable) {
            if (field.fromstring.empty())
                throw opp_runtime_error("Field '%s' is editable, but fromstring() function is unspecified", field.fname.c_str()); // ensured by MsgAnalyzer
            CC << "        case " << i << ": ";
            if (!classInfo.isClass && field.fisarray) {
                Assert(!field.farraysize.empty()); // struct may not contain dynamic arrays; checked by analyzer
                CC << "if (i >= " << field.farraysize << ") return \"\";\n                ";
            }
            std::string fromstringCall = makeFuncall("value", field.fromstring); //TODO use op>> if there's no fromstring?
            if (!classInfo.isClass)
                CC << "pp->" << field.var << (field.fisarray ? "[i]" : "") << " = " << fromstringCall << "; return true;\n";
            else
                CC << makeFuncall("pp", field.setter, field.fisarray, fromstringCall) << "; return true;\n";
        }
    }
    CC << "        default: return false;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldStructName()
    CC << "const char *" << classInfo.msgdescclass << "::getFieldStructName(int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldStructName(field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    if (fieldcount == 0) {
        CC << "    return nullptr;\n";
    }
    else {
        CC << "    switch (field) {\n";
        for (size_t i = 0; i < fieldcount; i++) {
            const FieldInfo& field = classInfo.fieldlist[i];
            if (!field.fopaque && !field.byvalue) {
                CC << "        case " << i << ": return omnetpp::opp_typename(typeid(" << field.ftype << "));\n";
            }
        }
        CC << "        default: return nullptr;\n";
        CC << "    };\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldStructValuePointer()
    CC << "void *" << classInfo.msgdescclass << "::getFieldStructValuePointer(void *object, int field, int i) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldStructValuePointer(object, field, i);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << classInfo.msgclass << " *pp = (" << classInfo.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const FieldInfo& field = classInfo.fieldlist[i];
        if (!field.byvalue) {
            std::string value;
            if (!classInfo.isClass)
                value = str("pp->") + field.var + (field.fisarray ? "[i]" : "");
            else
                value = makeFuncall("pp", field.getter, field.fisarray);
            std::string maybeAddressOf = field.fispointer ? "" : "&";
            CC << "        case " << i << ": return toVoidPtr(" << maybeAddressOf << value << "); break;\n";
        }
    }
    CC << "        default: return nullptr;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";
}

void MsgCodeGenerator::generateEnum(const EnumInfo& enumInfo)
{
    H << "/**\n";
    H << " * Enum generated from <tt>" << SL(enumInfo.astNode->getSourceLocation()) << "</tt> by " PROGRAM ".\n";
    H << generatePreComment(enumInfo.astNode);
    H << " */\n";

    H << "enum " << enumInfo.enumName <<" {\n";
    for (EnumInfo::FieldList::const_iterator it = enumInfo.fieldlist.begin(); it != enumInfo.fieldlist.end(); ) {
        H << "    " << it->name << " = " << it->value;
        if (++it != enumInfo.fieldlist.end())
            H << ",";
        H << "\n";
    }
    H << "};\n\n";

    // TODO generate a Register_Enum() macro call instead
    // TODO why generating the enum and registering its descriptor (cEnum) aren't separate, independently usable operations as with classes?
    CC << "EXECUTE_ON_STARTUP(\n";
    CC << "    omnetpp::cEnum *e = omnetpp::cEnum::find(\"" << enumInfo.enumQName << "\");\n";
    CC << "    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum(\"" << enumInfo.enumQName << "\"));\n";
    // enum inheritance: we should add fields from base enum as well, but that could only be done when importing is in place
    for (const auto& enumItem : enumInfo.fieldlist) {
        CC << "    e->insert(" << enumItem.name << ", \"" << enumItem.name << "\");\n";
    }
    CC << ")\n\n";
}

std::string MsgCodeGenerator::prefixWithNamespace(const std::string& name, const std::string& namespaceName)
{
    return !namespaceName.empty() ? namespaceName + "::" + name : name;  // prefer name from local namespace
}

std::string MsgCodeGenerator::makeFuncall(const std::string& var, const std::string& funcTemplate, bool withIndex, const std::string& value)
{
    if (funcTemplate[0] == '.' || funcTemplate[0] == '-') {
        // ".foo()" becomes "var.foo()", "->foo()" becomes "var->foo()"
        return var + funcTemplate;
    }
    else if (funcTemplate.find('$') != std::string::npos) {
        // "tostring($)" becomes "tostring(var)", "getchild($,i)" becomes "getchild(var,i)"
        return opp_replacesubstring(funcTemplate, "$", var, true);
    }
    else {
        // "foo" is a shorthand for "var->foo()" or "var->foo(i)", depending on flag
        return var + "->" + funcTemplate + "(" + (withIndex ? "i" : "") + ((withIndex && value!="") ? "," : "") + value + ")";
    }
}

void MsgCodeGenerator::generateTemplates()
{
    CC << "// forward\n";
    CC << "template<typename T, typename A>\n";
    CC << "std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);\n\n";

    CC << "// Template rule to generate operator<< for shared_ptr<T>\n";
    CC << "template<typename T>\n";
    CC << "inline std::ostream& operator<<(std::ostream& out,const std::shared_ptr<T>& t) { return out << t.get(); }\n\n";

    CC << "// Template rule which fires if a struct or class doesn't have operator<<\n";
    CC << "template<typename T>\n";
    CC << "inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}\n\n";

    CC << "// operator<< for std::vector<T>\n";
    CC << "template<typename T, typename A>\n";
    CC << "inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)\n";
    CC << "{\n";
    CC << "    out.put('{');\n";
    CC << "    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)\n";
    CC << "    {\n";
    CC << "        if (it != vec.begin()) {\n";
    CC << "            out.put(','); out.put(' ');\n";
    CC << "        }\n";
    CC << "        out << *it;\n";
    CC << "    }\n";
    CC << "    out.put('}');\n";
    CC << "\n";
    CC << "    char buf[32];\n";
    CC << "    sprintf(buf, \" (size=%u)\", (unsigned int)vec.size());\n";
    CC << "    out.write(buf, strlen(buf));\n";
    CC << "    return out;\n";
    CC << "}\n";
    CC << "\n";
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
        H << "namespace " << token << " {\n";
        if (intoCcFile)
            CC << "namespace " << token << " {\n";
    }
    H << std::endl;
    if (intoCcFile)
        CC << std::endl;

    if (intoCcFile)
        generateTemplates();
}

void MsgCodeGenerator::generateNamespaceEnd(const std::string& namespaceName, bool intoCcFile)
{
    auto tokens = opp_split(namespaceName, "::");
    std::reverse(tokens.begin(), tokens.end());
    for (auto token : tokens) {
        H << "} // namespace " << token << std::endl;
        if (intoCcFile)
            CC << "} // namespace " << token << std::endl;
    }
    H << std::endl;
    if (intoCcFile)
        CC << std::endl;
}

void MsgCodeGenerator::generateTypeAnnouncement(const ClassInfo& classInfo)
{
    H << (classInfo.isClass ? "class " : "struct ") << classInfo.msgname << ";\n";
}

void MsgCodeGenerator::generateCplusplusBlock(const std::string& target, const std::string& body0)
{
    std::string body = body0;
    size_t pos0 = body.find_first_not_of("\r\n");
    if (pos0 != body.npos)
        body = body.substr(pos0);
    size_t pos = body.find_last_not_of("\r\n");
    if (pos != body.npos)
        body = body.substr(0, pos+1);
    if (target == "" || target == "h")
        H << "// cplusplus {{\n" << body << "\n// }}\n\n";
    else if (target == "cc")
        CC << "// cplusplus {{\n" << body << "\n// }}\n\n";
    else
        throw opp_runtime_error("unrecognized target '%s' for cplusplus block", target.c_str());
}

}  // namespace nedxml
}  // namespace omnetpp
