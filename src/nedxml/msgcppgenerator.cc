//==========================================================================
//  MSGCPPGENERATOR.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <set>

#include "msgcppgenerator.h"
#include "ned2generator.h"
#include "nedexception.h"
#include "nedutil.h"
#include "stringtokenizer.h"
#include "stringutil.h"
#include "platmisc.h"  // unlink()

#include "../../include/simkerneldefs.h"

#ifdef COMPATIBILITY_MODE
// removes ":<linenumber>" from source location string
inline std::string SL(const std::string& s)
{
    return s.substr(0, s.find_last_of(':'));
}
#else
#define SL(x)  (x)
#endif

inline std::string TS(const std::string& s)
{
    return s.empty() ? s : s + " ";
}

char charToNameFilter(char ch)
{
    return (isalnum(ch)) ? ch : '_';
}

std::string canonicalizeQName(const std::string& namespac, const std::string& name)
{
    std::string qname;
    if (name.find("::") != name.npos) {
        qname = name.substr(0, 2) == "::" ? name.substr(2) : name; // remove leading "::", because names in @classes don't have it either
    }
    else if (!namespac.empty() && !name.empty()) {
        qname = namespac + "::" + name;
    }
    else
        qname = name;
    return qname;
}

std::ostream& operator << (std::ostream& o, const std::pair<std::string,int>& p)
{
    return o << '(' << p.first << ':' << p.second << ')';
}

std::string ptr2str(const char *ptr)
{
    return ptr ? ptr : "";
}


NAMESPACE_BEGIN

using std::ostream;

#define H  (*hOutp)
#define CC (*ccOutp)

// compatibility mode makes output more similar to opp_msgc's
//#define COMPATIBILITY_MODE

#ifdef COMPATIBILITY_MODE
#define PROGRAM "opp_msgc"
#else
#define PROGRAM "nedtool"
#endif

template <typename T>
std::string join(const T& v, const std::string& delim)
{
    std::ostringstream s;
    for (typename T::const_iterator i=v.begin(); i != v.end(); ++i) {
        if (i != v.begin()) {
            s << delim;
        }
        s << *i;
    }
    return s.str();
}

// Note: based on omnetpp 5.x checkandcast.h (merged check_and_cast and check_and_cast_failure)
template<class P, class T>
P check_and_cast(T *p)
{
    assert(p);
    P ret = dynamic_cast<P>(p);
    assert(ret);
    return ret;
}

template <typename T, typename U>
class create_map
{
private:
    std::map<T, U> m_map;
public:
    create_map(const T& key, const U& val)
    {
        m_map[key] = val;
    }

    create_map<T, U>& operator()(const T& key, const U& val)
    {
        m_map[key] = val;
        return *this;
    }

    operator std::map<T, U>()
    {
        return m_map;
    }
};

const MsgCppGenerator::TypeMap MsgCppGenerator::PRIMITIVE_TYPES =
    create_map<std::string, MsgCppGenerator::TypeDesc>
        ("bool",            TypeDesc("string2bool",     "bool2string"))
        ("float",           TypeDesc("string2double",   "double2string"))
        ("double",          TypeDesc("string2double",   "double2string"))
        ("simtime_t",       TypeDesc("string2double",   "double2string"))
        ("string",          TypeDesc("",                "oppstring2string"))
        ("char",            TypeDesc("string2long",     "long2string"))
        ("short",           TypeDesc("string2long",     "long2string"))
        ("int",             TypeDesc("string2long",     "long2string"))
        ("long",            TypeDesc("string2long",     "long2string"))
        ("int8",            TypeDesc("string2long",     "long2string"))
        ("int8_t",          TypeDesc("string2long",     "long2string"))
        ("int16",           TypeDesc("string2long",     "long2string"))
        ("int16_t",         TypeDesc("string2long",     "long2string"))
        ("int32",           TypeDesc("string2long",     "long2string"))
        ("int32_t",         TypeDesc("string2long",     "long2string"))
        ("unsigned char",   TypeDesc("string2ulong",    "ulong2string"))
        ("unsigned short",  TypeDesc("string2ulong",    "ulong2string"))
        ("unsigned int",    TypeDesc("string2ulong",    "ulong2string"))
        ("unsigned long",   TypeDesc("string2ulong",    "ulong2string"))
        ("uint8",           TypeDesc("string2ulong",    "ulong2string"))
        ("uint8_t",         TypeDesc("string2ulong",    "ulong2string"))
        ("uint16",          TypeDesc("string2ulong",    "ulong2string"))
        ("uint16_t",        TypeDesc("string2ulong",    "ulong2string"))
        ("uint32",          TypeDesc("string2ulong",    "ulong2string"))
        ("uint32_t",        TypeDesc("string2ulong",    "ulong2string"))
        ("int64",           TypeDesc("string2int64",    "int642string"))
        ("int64_t",         TypeDesc("string2int64",    "int642string"))
        ("uint64",          TypeDesc("string2uint64",   "uint642string"))
        ("uint64_t",        TypeDesc("string2uint64",   "uint642string"))
        ;

static const std::string _RESERVED_WORDS[] =
       {"namespace", "cplusplus", "struct", "message", "packet", "class", "noncobject",
        "enum", "extends", "abstract", "readonly", "properties", "fields", "bool", "char", "short",
        "int", "long", "double", "unsigned", "string", "true", "false",
        "float", "int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64",
        "int8_t", "int16_t", "int32_t", "int64_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t",
        "for", "while", "if", "else", "do", "enum", "class", "struct", "typedef", "public", "private",
        "protected", "auto", "register", "sizeof", "void", "new", "delete", "explicit", "static",
        "extern", "return", "try", "catch"};

static const std::set<std::string> RESERVED_WORDS(_RESERVED_WORDS, _RESERVED_WORDS + sizeof(_RESERVED_WORDS) / sizeof(_RESERVED_WORDS[0]));

MsgCppGenerator::MsgCppGenerator(NEDErrorStore *e, const MsgCppGeneratorOptions& options)
{
    opts = options;

    hOutp = ccOutp = NULL;
    errors = e;

    // pre-register some OMNeT++ classes so that one doesn't need to announce them
    //
    // @classes contains fully qualified names (ie with namespace); keys to the other hashes are fully qualified as well
    //
    // note: $classtype values:
    //  'cownedobject' ==> subclasses from cOwnedObject
    //  'cnamedobject' ==> subclasses from cNamedObject but NOT from cOwnedObject
    //  'cobject'      ==> subclasses from cObject but NOT from cNamedObject
    //  'foreign'      ==> non-cObject class (classes announced as "class noncobject" or "extends void")
    //  'struct'       ==> struct (no member functions)
    //
    classtype[OPP_PREFIX "cObject"] = COBJECT;
    classtype[OPP_PREFIX "cNamedObject"] = CNAMEDOBJECT;
    classtype[OPP_PREFIX "cOwnedObject"] = COWNEDOBJECT;
    classtype[OPP_PREFIX "cMessage"] = COWNEDOBJECT;
    classtype[OPP_PREFIX "cPacket"] = COWNEDOBJECT;
}

MsgCppGenerator::~MsgCppGenerator()
{
}

void MsgCppGenerator::generate(MsgFileElement *fileElement, const char *hFile, const char *ccFile)
{
    hFilename = hFile;
    ccFilename = ccFile;
    std::ofstream hStream(hFile);
    std::ofstream ccStream(ccFile);
    hOutp = &hStream;
    ccOutp = &ccStream;
    generate(fileElement);
    hStream.close();
    ccStream.close();

    if (errors->containsError()) {
        // remove output files when error occured:
        unlink(hFile);
        unlink(ccFile);
    }
}

void MsgCppGenerator::extractClassDecl(NEDElement *child)
{
    std::string name = ptr2str(child->getAttribute("name"));
    if (RESERVED_WORDS.find(name) == RESERVED_WORDS.end())
    {
        std::string type0 = child->getTagName();
        std::string myclass = name;
        std::string baseclass = ptr2str(child->getAttribute("extends-name"));
        ClassType type;
        bool isCobject = (ptr2str(child->getAttribute("is-cobject")) == "true");

        std::string classqname = canonicalizeQName(namespacename, myclass);

        if (type0 == "struct-decl") {
            type = STRUCT;
        } else if (type0 == "message-decl" || type0 == "packet-decl") {
            type = COWNEDOBJECT;
        } else if (type0 == "class-decl") {
            if (!isCobject) {
                type = FOREIGN;
                if (!baseclass.empty()) {
                    errors->addError(child, "'%s': the keywords noncobject and extends cannot be used together", name.c_str());
                }
            }
            else if (baseclass == "") {
                type = COWNEDOBJECT;
            } else if (baseclass == "void") {
                type = FOREIGN;
            } else {
                StringVector found = lookupExistingClassName(baseclass);
                if (found.size() == 1) {
                    type = getClassType(found[0]);
                } else if (found.empty()) {
                    errors->addError(child, "'%s': unknown ancestor class '%s'\n", myclass.c_str(), baseclass.c_str());
                    type = COBJECT;
                } else {
                    errors->addWarning(child, "'%s': ambiguous ancestor class '%s'; possibilities: '%s'\n", myclass.c_str(), baseclass.c_str(), join(found, "','").c_str());
                    type = getClassType(found[0]);
                }
            }
        } else {
            errors->addError(child, "invalid type '%s' in class '%s'\n", type0.c_str(), myclass.c_str());
        }

        if (classtype.find(classqname) != classtype.end()) {
            if (classtype[classqname] != type) {
                errors->addError(child, "different declarations for '%s(=%s)' are inconsistent\n", myclass.c_str(), classqname.c_str());
            }
        } else {
            //print "DBG: classtype{$type0 myclass $baseclass} = $type\n";
            classtype[classqname] = type;
        }
    }
    else
    {
        errors->addError(child, "type name '%s' is a reserved word\n", name.c_str());
    }
}

void MsgCppGenerator::generate(MsgFileElement *fileElement)
{
    std::string hfilenamewithoutdir = hFilename;
    size_t pos = hfilenamewithoutdir.find_last_of('/');
    if (pos != hfilenamewithoutdir.npos)
        hfilenamewithoutdir = hfilenamewithoutdir.substr(pos+1);
    std::string hdef;
    hdef = std::string("_") + hfilenamewithoutdir + "_";

    // add first namespace to headerguard
    NamespaceElement *firstNS = fileElement->getFirstNamespaceChild();
    if (firstNS)
        hdef = std::string("_") + ptr2str(firstNS->getAttribute("name")) + hdef;

    // replace non-alphanumeric characters to '_'
    std::transform(hdef.begin(), hdef.end(), hdef.begin(), charToNameFilter);
    // capitalize
    std::transform(hdef.begin(), hdef.end(), hdef.begin(), ::toupper);

    H << "//\n// Generated file, do not edit! Created by " PROGRAM " " << (OMNETPP_VERSION / 0x100) << "." << (OMNETPP_VERSION % 0x100)
      << " from " << fileElement->getFilename() << ".\n//\n\n";
    H << "#ifndef " << hdef << "\n";
    H << "#define " << hdef << "\n\n";
    H << "#include <omnetpp.h>\n";
    H << "\n";
    H << "// " PROGRAM " version check\n";
    H << "#define MSGC_VERSION 0x" << std::hex << std::setfill('0') << std::setw(4) << OMNETPP_VERSION << "\n";
    H << "#if (MSGC_VERSION!=OMNETPP_VERSION)\n";
    H << "#    error Version mismatch! Probably this file was generated by an earlier version of " PROGRAM ": 'make clean' should help.\n";
    H << "#endif\n";
    H << "\n";

    if (opts.exportdef.length() > 4 && opts.exportdef.substr(opts.exportdef.length()-4) == "_API") {
        //# generate boilerplate code for dll export
        std::string exportbase = opts.exportdef.substr(0, opts.exportdef.length()-4);
        H << "// dll export symbol\n";
        H << "#ifndef " << opts.exportdef << "\n";
        H << "#  if defined(" << exportbase << "_EXPORT)\n";
        H << "#    define " << opts.exportdef << "  OPP_DLLEXPORT\n";
        H << "#  elif defined(" << exportbase << "_IMPORT)\n";
        H << "#    define " << opts.exportdef << "  OPP_DLLIMPORT\n";
        H << "#  else\n";
        H << "#    define " << opts.exportdef << "\n";
        H << "#  endif\n";
        H << "#endif\n";
        H << "\n";
    }

    CC << "//\n// Generated file, do not edit! Created by " PROGRAM " " << (OMNETPP_VERSION / 0x100) << "." << (OMNETPP_VERSION % 0x100) << " from " << fileElement->getFilename() << ".\n//\n\n";
    CC << "// Disable warnings about unused variables, empty switch stmts, etc:\n";
    CC << "#ifdef _MSC_VER\n";
    CC << "#  pragma warning(disable:4101)\n";
    CC << "#  pragma warning(disable:4065)\n";
    CC << "#endif\n\n";

    CC << "#include <iostream>\n";
    CC << "#include <sstream>\n";
    CC << "#include \"" << hfilenamewithoutdir << "\"\n\n";

    CC << "USING_NAMESPACE\n\n";

    if (opts.generate_classes)
    {
        CC << "\n";
        CC << "// Another default rule (prevents compiler from choosing base class' doPacking())\n";
        CC << "template<typename T>\n";
        CC << "void doPacking(cCommBuffer *, T& t) {\n";
        CC << "    throw cRuntimeError(\"Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)\",opp_typename(typeid(t)));\n";
        CC << "}\n\n";
        CC << "template<typename T>\n";
        CC << "void doUnpacking(cCommBuffer *, T& t) {\n";
        CC << "    throw cRuntimeError(\"Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)\",opp_typename(typeid(t)));\n";
        CC << "}\n";
        CC << "\n";
    }
    CC << "\n\n";

    if (!firstNS)
    {
        H << "\n\n";
        CC << "\n";
        generateTemplates();
    }

    /*
       <!ELEMENT msg-file (comment*, (namespace|property-decl|property|cplusplus|
                           struct-decl|class-decl|message-decl|packet-decl|enum-decl|
                           struct|class|message|packet|enum)*)>
     */
    for (NEDElement *child = fileElement->getFirstChild(); child; child=child->getNextSibling())
    {
        switch (child->getTagCode()) {
            case NED_NAMESPACE:
                // open namespace(s)
                if (!namespacename.empty())
                {
                    generateNamespaceEnd();
                }
                namespacename = ptr2str(child->getAttribute("name"));
                generateNamespaceBegin(child);
                break;

            case NED_CPLUSPLUS:
            {
                // print C++ block
                std::string body = ptr2str(child->getAttribute("body"));
                body = body.substr(body.find_first_not_of("\r\n"));
                size_t pos = body.find_last_not_of("\r\n");
                if (pos != body.npos)
                    body = body.substr(0, pos+1);
                H << "// cplusplus {{\n"
                  << body
                  << "\n// }}\n\n"
                  ;
                break;
            }

            case NED_STRUCT_DECL:
            case NED_CLASS_DECL:
            case NED_MESSAGE_DECL:
            case NED_PACKET_DECL:
                extractClassDecl(child);
                break;

            case NED_ENUM_DECL:
            {
                // forward declaration -- add to table
                std::string name = ptr2str(child->getAttribute("name"));
                if (RESERVED_WORDS.find(name) == RESERVED_WORDS.end())
                    enumtype[name] = canonicalizeQName(namespacename, name);
                else
                {
                    errors->addError(child, "Namespace name is reserved word: '%s'", name.c_str());
                }
                break;
            }

            case NED_ENUM: {
                EnumInfo info = extractEnumInfo(check_and_cast<EnumElement *>(child));
                enumtype[info.enumName] = info.enumQName;
                generateEnum(info);
                break;
            }

            case NED_STRUCT: {
                ClassInfo classInfo = extractClassInfo(child);
                prepareForCodeGeneration(classInfo);
                classtype[classInfo.msgname] = classInfo.classtype;
                if (classInfo.generate_class)
                    generateStruct(classInfo);
                if (classInfo.generate_descriptor)
                    generateDescriptorClass(classInfo);
                break;
            }

            case NED_CLASS:
            case NED_MESSAGE:
            case NED_PACKET: {
                ClassInfo classInfo = extractClassInfo(child);
                prepareForCodeGeneration(classInfo);
                classtype[classInfo.msgname] = classInfo.classtype;
                if (classInfo.generate_class)
                    generateClass(classInfo);
                if (classInfo.generate_descriptor)
                    generateDescriptorClass(classInfo);
                break;
            }

        }
    }

    generateNamespaceEnd();

#ifdef COMPATIBILITY_MODE
    H << "#endif // " << hdef << "\n";
#else
    H << "#endif // ifndef " << hdef << "\n\n";
#endif
}

MsgCppGenerator::Properties MsgCppGenerator::extractPropertiesOf(NEDElement *node)
{
    Properties props;

    for (PropertyElement *child = static_cast<PropertyElement *>(node->getFirstChildWithTag(NED_PROPERTY)); child; child = child->getNextPropertySibling())
    {
        std::string propname = ptr2str(child->getAttribute("name"));
        std::string propval;
        for (PropertyKeyElement *key = child->getFirstPropertyKeyChild(); key; key = key->getNextPropertyKeySibling())
        {
            std::string keyname = ptr2str(key->getAttribute("name"));
            if (keyname.empty())
            {
                const char *sep = "";
                for (LiteralElement *lit = key->getFirstLiteralChild(); lit; lit = lit->getNextLiteralSibling())
                {
                    std::string s = ptr2str(lit->getAttribute("value"));
                    propval = propval + sep + s;
                    sep = ",";
                }
            }
        }
        if (props.find(propname) == props.end()) {
            props[propname] = propval;
        }
        else {
            errors->addError(child, "the property '%s' is duplicated", propname.c_str());
        }
    }
    return props;
}

MsgCppGenerator::ClassInfo MsgCppGenerator::extractClassInfo(NEDElement *node)
{
    ClassInfo classInfo;
    classInfo.nedElement = node;

    classInfo.keyword = node->getTagName();
    classInfo.msgname = ptr2str(node->getAttribute("name"));
    classInfo.msgbase = ptr2str(node->getAttribute("extends-name"));
    classInfo.props = extractPropertiesOf(node);

    for (NEDElement *child = node->getFirstChild(); child; child = child->getNextSibling())
    {
        switch (child->getTagCode()) {
            case NED_FIELD:
            {
                ClassInfo::FieldInfo f;
                f.nedElement = child;
                f.fname = ptr2str(child->getAttribute("name"));
                f.datatype = ptr2str(child->getAttribute("data-type"));  // ha ez nincs, eltunnek a struct mezoi....
                f.ftype = ptr2str(child->getAttribute("data-type"));
                f.fval = ptr2str(child->getAttribute("default-value"));
                f.fisabstract = ptr2str(child->getAttribute("is-abstract")) == "true";
                f.fispointer = (f.ftype[f.ftype.length()-1] == '*');
                if (f.fispointer)
                    f.ftype = f.ftype.substr(0, f.ftype.find_last_not_of(" \t*")+1);
                f.fisarray = ptr2str(child->getAttribute("is-vector")) == "true";
                f.farraysize = ptr2str(child->getAttribute("vector-size"));

                f.fprops = extractPropertiesOf(child);

                if (f.ftype.empty())
                    classInfo.baseclassFieldlist.push_back(f);
                else
                    classInfo.fieldlist.push_back(f);
                break;
            }
            case NED_PROPERTY:
                //skip properties here, properties already extracted
                break;
            case NED_PROPERTY_DECL:
                errors->addError(child, "syntax error: property '%s' declaration unaccepted here", child->getTagName());
                break;
            case NED_PROPERTY_KEY:
                errors->addError(child, "syntax error: property key '%s' unaccepted here", child->getTagName());
                break;
            case NED_COMMENT:
                break;
            default:
                errors->addError(child, "unaccepted element: '%s'", child->getTagName());
                break;
        }
    }
    return classInfo;
}

void MsgCppGenerator::prepareFieldForCodeGeneration(ClassInfo& info, ClassInfo::FieldInfo *it)
{
    if (it->fisabstract && !info.gap) {
        errors->addError(it->nedElement, "abstract fields need '@customize(true)' property in '%s'\n", info.msgname.c_str());
    }

    if (info.generate_class) {
        ClassType classType = getClassType(it->ftype);
        if (classType == COWNEDOBJECT && (info.classtype != COWNEDOBJECT)) {
            errors->addError(it->nedElement, "cannot use cOwnedObject field '%s %s' in struct or non-cOwnedObject class '%s'\n", it->ftype.c_str(), it->fname.c_str(), info.msgname.c_str());
        }
    }

    it->fnopack = getPropertyAsBool(it->fprops, "nopack", false);
    it->feditable = getPropertyAsBool(it->fprops, "editable", false);
    it->fopaque = getPropertyAsBool(it->fprops, "opaque", false);

    // resolve enum namespace
    it->enumname = getProperty(it->fprops, "enum");
    if (!it->enumname.empty()) {
        StringVector found = lookupExistingEnumName(it->enumname);
        if (found.size() == 1) {
            it->enumqname = found[0];
        } else if (found.empty()) {
            errors->addError(it->nedElement, "undeclared enum '%s' in field '%s' in '%s'\n", it->enumname.c_str(), it->fname.c_str(), info.msgname.c_str());
            it->enumqname = "";
            CC << "\n\n/*\n Undeclared enum: " << it->enumname << "\n";
            CC << "  Declared enums:\n";
            for (std::map<std::string,std::string>::iterator x=enumtype.begin(); x != enumtype.end(); ++x)
                CC << "    " << x->first << " : " << x->second << "\n";
            CC << "\n*/\n\n";
        } else {
            errors->addWarning(it->nedElement, "ambiguous enum '%s' in field '%s' in '%s';  possibilities: %s\n", it->enumname.c_str(), it->fname.c_str(), info.msgname.c_str(), join(found, ", ").c_str());
            it->enumqname = found[0];
        }
        it->fprops["enum"] = it->enumqname; // need to modify the property in place
    }

    // variable name
    if (info.classtype == STRUCT) {
        it->var = it->fname;
    } else {
        it->var = it->fname + "_var";
        it->argname = it->fname;
    }

    it->varsize = it->fname + "_arraysize";
    std::string sizetypeprop = getProperty(it->fprops, "sizetype");
    it->fsizetype = !sizetypeprop.empty() ? sizetypeprop : "unsigned int";  //TODO change to size_t

    // default method names
    if (info.classtype != STRUCT) {
        std::string capfieldname = it->fname;
        capfieldname[0] = toupper(capfieldname[0]);
        it->setter = std::string("set") + capfieldname;
        it->alloc = std::string("set") + capfieldname + "ArraySize";
        if (info.omitgetverb) {
            it->getter = it->fname;
            it->getsize = it->fname + "ArraySize";
        } else {
            it->getter = std::string("get") + capfieldname;
            it->getsize = std::string("get") + capfieldname + "ArraySize";
        }

        // allow customization of names
        if (getProperty(it->fprops, "setter") != "") {
            it->setter = getProperty(it->fprops, "setter");
        }
        if (getProperty(it->fprops, "getter") != "") {
            it->getter = getProperty(it->fprops, "getter");
        }
        if (getProperty(it->fprops, "sizeSetter") != "") {
            it->alloc = getProperty(it->fprops, "sizeSetter");
        }
        if (getProperty(it->fprops, "sizeGetter") != "") {
            it->getsize = getProperty(it->fprops, "sizeGetter");
        }
    }

    // determine field data type
    TypeMap::const_iterator tdIt= PRIMITIVE_TYPES.find(it->ftype);
    if (tdIt != PRIMITIVE_TYPES.end()) {
        it->fkind = "basic";
        it->ftypeqname = "";  // unused
    }
    else {
        it->fkind = "struct";

        // $ftypeqname
        StringVector found = lookupExistingClassName(it->ftype);
        if (found.size() == 1) {
            it->ftypeqname = "::" + found[0];
        } else if (found.empty()) {
            errors->addError(it->nedElement, "unknown type '%s' for field '%s' in '%s'\n", it->ftype.c_str(), it->fname.c_str(), info.msgname.c_str());
            it->ftypeqname = "::cObject";
        } else {
            errors->addError(it->nedElement, "unknown type '%s' for field '%s' in '%s'; possibilities: %s\n", it->ftype.c_str(), it->fname.c_str(), info.msgname.c_str(), join(found, ", ").c_str());
            it->ftypeqname = "::" + found[0];
        }
    }

    // data type, argument type, conversion to/from string...
    it->maybe_c_str = "";
    if (it->fkind == "struct") {
        it->datatype = it->ftype;
        it->argtype = std::string("const ") + it->ftype + "&";
        it->rettype = it->ftype + "&";
        it->tostring = "";
        it->fromstring = "";
        //it->fval = "" unless (it->fval != "");
    } else if (it->fkind == "basic") {
        if (tdIt == PRIMITIVE_TYPES.end())
            throw NEDException("internal error - unknown primitive data type '%s'", it->ftype.c_str());
        // defaults:
        it->datatype = it->ftype;
        it->argtype = it->ftype;
        it->rettype = it->ftype;
        if (it->fval.empty())
            it->fval = "0";
        it->tostring = tdIt->second.tostring;
        it->fromstring = tdIt->second.fromstring;

        if (it->ftype == "bool") {
            // $datatype, $argtype, $rettype: default (same as $ftype)
            if (it->fval.empty())
                it->fval = "false";
        } else if (it->ftype == "string") {
            it->datatype = "opp_string";
            it->argtype = "const char *";
            it->rettype = "const char *";
            if (it->fval.empty())
                it->fval = "\"\"";
            it->maybe_c_str = ".c_str()";
        }
    } else {
        throw NEDException("internal error");
    }
}

void MsgCppGenerator::prepareForCodeGeneration(ClassInfo& info)
{
    info.msgqname = prefixWithNamespace(info.msgname);

    // determine info.msgbaseqname
    if (info.msgbase != "void") {
        StringVector found = lookupExistingClassName(info.msgbase);
        if (found.size() == 1) {
            info.msgbaseqname = found[0];
        } else if (found.empty()) {
            errors->addError(info.nedElement, "'%s': unknown base class '%s', available classes '%s'", info.msgname.c_str(), info.msgbase.c_str(), join(classtype, "','").c_str());
            info.msgbaseqname = "cMessage";
        } else {
            errors->addError(info.nedElement, "'%s': ambiguous base class '%s'; possibilities: '%s'",
                             info.msgname.c_str(), info.msgbase.c_str(), join(found, "','").c_str());
            info.msgbaseqname = found[0];
        }
    }

    // check base class and determine type of object
    if (info.msgqname == "cObject" || info.msgqname == "cNamedObject" || info.msgqname == "cOwnedObject") {
        info.classtype = getClassType(info.msgqname);  // only for sim_std.msg
    }
    else if (info.msgbase == "") {
        if (info.keyword == "message" || info.keyword == "packet") {
            info.classtype = COWNEDOBJECT;
        } else if (info.keyword == "class") {
            info.classtype = COBJECT; // Note: we never generate non-cObject classes
        } else if (info.keyword == "struct") {
            info.classtype = STRUCT;
        } else {
            throw NEDException("internal error: invalid keyword:'%s' at '%s'", info.keyword.c_str(), info.msgclass.c_str());
        }
        // if announced earlier as noncpolymorphic, accept that.
        if (isClassDeclared(info.msgqname)) {
            if (getClassType(info.msgqname) == FOREIGN && info.classtype == COBJECT) {
                info.classtype = FOREIGN;
            }
        }
    }
    else if (info.msgbase == "void") {
        info.classtype = FOREIGN;
    }
    else if (info.msgbaseqname != "") {
        info.classtype = getClassType(info.msgbaseqname);
    }
    else {
        errors->addError(info.nedElement, "unknown base class '%s' for '%s'\n", info.msgbase.c_str(), info.msgname.c_str());
        info.classtype = COBJECT;
    }

    // check earlier declarations and register this class
    if (isClassDeclared(info.msgqname)) {
        if (0) { // XXX add condition
            errors->addError(info.nedElement, "attempt to redefine '%s'\n", info.msgname.c_str());
        } else if (getClassType(info.msgqname) != info.classtype) {
            errors->addError(info.nedElement, "definition of '%s' inconsistent with earlier declaration(s)\n", info.msgname.c_str());
        }
    } else {
        classtype[info.msgqname] = info.classtype;
    }

    //
    // produce all sorts of derived names
    //
    info.generate_class = opts.generate_classes && !getPropertyAsBool(info.props, "existingClass", false);
    info.generate_descriptor = opts.generate_descriptors && getPropertyAsBool(info.props, "descriptor", true);
    info.generate_setters_in_descriptor = opts.generate_setters_in_descriptors && (getProperty(info.props, "descriptor") != "readonly");

    if (getPropertyAsBool(info.props, "customize", false)) {
        info.gap = 1;
        info.msgclass = info.msgname + "_Base";
        info.realmsgclass = info.msgname;
        info.msgdescclass = info.realmsgclass + "Descriptor";
    } else {
        info.gap = 0;
        info.msgclass = info.msgname;
        info.realmsgclass = info.msgname;
        info.msgdescclass = info.msgclass + "Descriptor";
    }
    if (info.msgbase == "") {
        if (info.msgqname == "cObject") {
            info.msgbaseclass = "";
        } else if (info.keyword == "message") {
            info.msgbaseclass = "cMessage";
        } else if (info.keyword == "packet") {
            info.msgbaseclass = "cPacket";
        } else if (info.keyword == "class") {
            info.msgbaseclass = "cObject";  // note: all classes we generate subclass from cObject!
        } else if (info.keyword == "struct") {
            info.msgbaseclass = "";
        } else {
            throw NEDException("internal error");
        }
    } else if (info.msgbase == "void") {
        info.msgbaseclass = "";
    } else {
        info.msgbaseclass = info.msgbaseqname;
    }

    info.omitgetverb = getPropertyAsBool(info.props, "omitGetVerb", false);

    std::string s = getProperty(info.props, "implements");
    if (!s.empty()) {
        info.implements = StringTokenizer(s.c_str(), ",").asVector();
    }

    for (ClassInfo::Fieldlist::iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
        prepareFieldForCodeGeneration(info, &(*it));
    }
    for (ClassInfo::Fieldlist::iterator it = info.baseclassFieldlist.begin(); it != info.baseclassFieldlist.end(); ++it)
    {
        prepareFieldForCodeGeneration(info, &(*it));
    }
}

std::string MsgCppGenerator::generatePreComment(NEDElement *nedElement)
{
    std::ostringstream s;
    NED2Generator(errors).generate(s, nedElement, "");
    std::string str = s.str();

#ifdef COMPATIBILITY_MODE
    // remove comments
    size_t p1;
    while ((p1 = str.find("//")) != str.npos)
    {
        size_t p2 = str.find("\n", p1);
        std::string s2 = str.substr(0, p1) + str.substr(p2);
        str = s2;
    }
#endif

    std::ostringstream o;
    o << " * <pre>\n";
    o << opp_indentlines(opp_trim(opp_replacesubstring(str.c_str(), "*/", "  ", true).c_str()).c_str(), " * ");
//    o << ';';   //FIXME HACK for reduce difference
    o << "\n";
    o << " * </pre>\n";
    return o.str();
}

void MsgCppGenerator::generateClass(const ClassInfo& info)
{
    H << "/**\n";
    H << " * Class generated from <tt>" << SL(info.nedElement->getSourceLocation()) << "</tt> by " PROGRAM ".\n";
    H << generatePreComment(info.nedElement);

    if (info.gap)
    {
        H << " *\n";
        H << " * " << info.msgclass << " is only useful if it gets subclassed, and " << info.realmsgclass << " is derived from it.\n";
        H << " * The minimum code to be written for " << info.realmsgclass << " is the following:\n";
        H << " *\n";
        H << " * <pre>\n";
        H << " * class " << TS(opts.exportdef) << info.realmsgclass << " : public " << info.msgclass << "\n";
        H << " * {\n";
        H << " *   private:\n";
        H << " *     void copy(const " << info.realmsgclass << "& other) { ... }\n\n";
        H << " *   public:\n";
        if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT) {
            if (info.keyword == "message" || info.keyword == "packet") {
                H << " *     " << info.realmsgclass << "(const char *name=NULL, int kind=0) : " << info.msgclass << "(name,kind) {}\n";
            } else {
                H << " *     " << info.realmsgclass << "(const char *name=NULL) : " << info.msgclass << "(name) {}\n";
            }
        } else {
            H << " *     " << info.realmsgclass << "() : " << info.msgclass << "() {}\n";
        }
        H << " *     " << info.realmsgclass << "(const " << info.realmsgclass << "& other) : " << info.msgclass << "(other) {copy(other);}\n";
        H << " *     " << info.realmsgclass << "& operator=(const " << info.realmsgclass << "& other) {if (this==&other) return *this; " << info.msgclass << "::operator=(other); copy(other); return *this;}\n";
        if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT || info.classtype == COBJECT) {
            H << " *     virtual " << info.realmsgclass << " *dup() const {return new " << info.realmsgclass << "(*this);}\n";
        }
        H << " *     // ADD CODE HERE to redefine and implement pure virtual functions from " << info.msgclass << "\n";
        H << " * };\n";
        H << " * </pre>\n";
        if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT || info.classtype == COBJECT) {
            H << " *\n";
            H << " * The following should go into a .cc (.cpp) file:\n";
            H << " *\n";
            H << " * <pre>\n";
            H << " * Register_Class(" << info.realmsgclass << ");\n";
            H << " * </pre>\n";
        }
    }
    H << " */\n";

    H << "class " << TS(opts.exportdef) << info.msgclass;
    const char *baseclassSepar = " : ";
    if (!info.msgbaseclass.empty()) {
        H << baseclassSepar << "public ::" << info.msgbaseclass;  // make namespace explicit and absolute to disambiguate the way PROGRAM understood it
        baseclassSepar = ", ";
    }

    for (StringVector::const_iterator it = info.implements.begin(); it != info.implements.end(); ++it) {
        H << baseclassSepar << "public " << *it;
        baseclassSepar = ", ";
    }

    H << "\n{\n";
    H << "  protected:\n";
    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it) {
        if (it->fispointer) {
            errors->addError(it->nedElement, "pointers not supported yet in '%s'\n", info.msgname.c_str()); return;
        }
        if (!it->fisabstract) {
            if (it->fisarray && !it->farraysize.empty()) {
                H << "    " << it->datatype << " " << it->var << "[" << it->farraysize << "];\n";
            } else if (it->fisarray && it->farraysize.empty()) {
                H << "    " << it->datatype << " *" << it->var << "; // array ptr\n";
                H << "    " << it->fsizetype << " " << it->varsize << ";\n";
            } else {
                H << "    " << it->datatype << " " << it->var << ";\n";
            }
        }
    }
    H << "\n";
    H << "  private:\n";
    H << "    void copy(const " << info.msgclass << "& other);\n\n";
    H << "  protected:\n";
    H << "    // protected and unimplemented operator==(), to prevent accidental usage\n";
    H << "    bool operator==(const " << info.msgclass << "&);\n";
    if (info.gap) {
        H << "    // make constructors protected to avoid instantiation\n";
    } else {
        H << "\n";
        H << "  public:\n";
    }
    if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT) {
        if (info.keyword == "message" || info.keyword == "packet") {
            H << "    " << info.msgclass << "(const char *name=NULL, int kind=0);\n";
        } else {
            H << "    " << info.msgclass << "(const char *name=NULL);\n";
        }
    } else {
        H << "    " << info.msgclass << "();\n";
    }
    H << "    " << info.msgclass << "(const " << info.msgclass << "& other);\n";
    if (info.gap) {
        H << "    // make assignment operator protected to force the user override it\n";
        H << "    " << info.msgclass << "& operator=(const " << info.msgclass << "& other);\n";
        H << "\n";
        H << "  public:\n";
    }
    H << "    virtual ~" << info.msgclass << "();\n";
    if (!info.gap) {
        H << "    " << info.msgclass << "& operator=(const " << info.msgclass << "& other);\n";
    }
    if (info.gap) {
        H << "    virtual " << info.msgclass << " *dup() const {throw cRuntimeError(\"You forgot to manually add a dup() function to class " << info.realmsgclass << "\");}\n";
    } else {
        H << "    virtual " << info.msgclass << " *dup() const {return new " << info.msgclass << "(*this);}\n";
    }
    H << "    virtual void parsimPack(cCommBuffer *b);\n";
    H << "    virtual void parsimUnpack(cCommBuffer *b);\n";
    H << "\n";
    H << "    // field getter/setter methods\n";
    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
        std::string pure;
        if (it->fisabstract)
            pure = " = 0";

        bool isstruct = (it->fkind == "struct");
        std::string constifprimitivetype = (!isstruct ? " const" : "");
        if (it->fisarray && !it->farraysize.empty()) {
            H << "    virtual " << it->fsizetype << " " << it->getsize << "() const" << pure << ";\n";
            H << "    virtual " << it->rettype << " " << it->getter << "(" << it->fsizetype << " k)" << constifprimitivetype << "" << pure << ";\n";
            if (isstruct)
                H << "    virtual const " << it->rettype << " " << it->getter << "(" << it->fsizetype << " k) const {return const_cast<" << info.msgclass << "*>(this)->" << it->getter << "(k);}\n";
            H << "    virtual void " << it->setter << "(" << it->fsizetype << " k, " << it->argtype << " " << it->argname << ")" << pure << ";\n";
        } else if (it->fisarray && it->farraysize.empty()) {
            H << "    virtual void " << it->alloc << "(" << it->fsizetype << " size)" << pure << ";\n";
            H << "    virtual " << it->fsizetype << " " << it->getsize << "() const" << pure << ";\n";
            H << "    virtual " << it->rettype << " " << it->getter << "(" << it->fsizetype << " k)" << constifprimitivetype << "" << pure << ";\n";
            if (isstruct)
                H << "    virtual const " << it->rettype << " " << it->getter << "(" << it->fsizetype << " k) const {return const_cast<" << info.msgclass << "*>(this)->" << it->getter << "(k);}\n";
            H << "    virtual void " << it->setter << "(" << it->fsizetype << " k, " << it->argtype << " " << it->argname << ")" << pure << ";\n";
        } else {
            H << "    virtual " << it->rettype << " " << it->getter << "()" << constifprimitivetype << "" << pure << ";\n";
            if (isstruct)
                H << "    virtual const " << it->rettype << " " << it->getter << "() const {return const_cast<" << info.msgclass << "*>(this)->" << it->getter << "();}\n";
            H << "    virtual void " << it->setter << "(" << it->argtype << " " << it->argname << ")" << pure << ";\n";
        }
    }
    H << "};\n\n";

    if (!info.gap) {
        if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT || info.classtype == COBJECT) {
            CC << "Register_Class(" << info.msgclass << ");\n\n";
        }
        H << "inline void doPacking(cCommBuffer *b, " << info.realmsgclass << "& obj) {obj.parsimPack(b);}\n";
        H << "inline void doUnpacking(cCommBuffer *b, " << info.realmsgclass << "& obj) {obj.parsimUnpack(b);}\n\n";
    }

    if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT) {
        if (info.keyword == "message" || info.keyword == "packet") {
            // CAREFUL when assigning values to existing members gets implemented!
            // The msg kind passed to the ctor should take priority!!!
            CC << "" << info.msgclass << "::" << info.msgclass << "(const char *name, int kind) : ::" << info.msgbaseclass << "(name,kind)\n";
        } else {
            if (info.msgbaseclass == "") {
                CC << "" << info.msgclass << "::" << info.msgclass << "(const char *name)\n";
            } else {
                CC << "" << info.msgclass << "::" << info.msgclass << "(const char *name) : ::" << info.msgbaseclass << "(name)\n";
            }
        }
    } else {
        if (info.msgbaseclass == "") {
            CC << "" << info.msgclass << "::" << info.msgclass << "()\n";
        } else {
            CC << "" << info.msgclass << "::" << info.msgclass << "() : ::" << info.msgbaseclass << "()\n";
        }
    }
    CC << "{\n";
    //CC << "    (void)static_cast<cObject *>(this); //sanity check\n" if (info.fieldclasstype == COBJECT);
    //CC << "    (void)static_cast<cNamedObject *>(this); //sanity check\n" if (info.fieldclasstype == CNAMEDOBJECT);
    //CC << "    (void)static_cast<cOwnedObject *>(this); //sanity check\n" if (info.fieldclasstype == COWNEDOBJECT);
    for (ClassInfo::Fieldlist::const_iterator it = info.baseclassFieldlist.begin(); it != info.baseclassFieldlist.end(); ++it)
    {
        std::string capfieldname = it->fname;
        capfieldname[0] = toupper(capfieldname[0]);
        std::string setter = "set" + capfieldname;
        //FIXME setter function name - ezt a base class leirobol kellene venni
        CC << "    this->" << setter << "(" << it->fval << ");\n";
    }
    if (!info.baseclassFieldlist.empty() && !info.fieldlist.empty())
        CC << "\n";
    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
        if (!it->fisabstract) {
            if (it->fisarray && !it->farraysize.empty()) {
                if (it->fkind == "basic") {
                    CC << "    for (" << it->fsizetype << " i=0; i<" << it->farraysize << "; i++)\n";
                    CC << "        this->" << it->var << "[i] = " << it->fval << ";\n";
                }
                if (getClassType(it->ftype) == COWNEDOBJECT) {
                  CC << "    for (" << it->fsizetype << " i=0; i<" << it->farraysize << "; i++)\n";
                  CC << "        take(&(this->" << it->var << "[i]));\n";
                }
            } else if (it->fisarray && it->farraysize.empty()) {
                CC << "    " << it->varsize << " = 0;\n";
                CC << "    this->" << it->var << " = 0;\n";
            } else {
                if (!it->fval.empty()) {
                  CC << "    this->" << it->var << " = " << it->fval << ";\n";
                }
                if (getClassType(it->ftype) == COWNEDOBJECT) {
                  CC << "    take(&(this->" << it->var << "));\n";
                }
            }
        }
    }
    CC << "}\n\n";

    CC << "" << info.msgclass << "::" << info.msgclass << "(const " << info.msgclass << "& other)";
    if (!info.msgbaseclass.empty()) {
        CC << " : ::" << info.msgbaseclass << "(other)";
    }
    CC << "\n{\n";
    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
      if (!it->fisabstract) {
        if (it->fisarray && !it->farraysize.empty()) {
          if (getClassType(it->ftype) == COWNEDOBJECT) {
            CC << "    for (" << it->fsizetype << " i=0; i<" << it->farraysize << "; i++)\n";
            CC << "        take(&(this->" << it->var << "[i]));\n";
          }
        } else if (it->fisarray && it->farraysize.empty()) {
          CC << "    " << it->varsize << " = 0;\n";
          CC << "    this->" << it->var << " = 0;\n";
        } else if (!it->fisarray && getClassType(it->ftype) == COWNEDOBJECT) {
          CC << "    take(&(this->" << it->var << "));\n";
        }
      }
    }
    CC << "    copy(other);\n";
    CC << "}\n\n";

    CC << "" << info.msgclass << "::~" << info.msgclass << "()\n";
    CC << "{\n";
    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
        if (!it->fisabstract) {
            if (getClassType(it->ftype) == COWNEDOBJECT) {
                if (!it->fisarray) {
                    CC << "    drop(&(this->" << it->var << "));\n";
                } else if (!it->farraysize.empty()) {
                    CC << "    for (" << it->fsizetype << " i=0; i<" << it->farraysize << "; i++)\n";
                    CC << "        drop(&(this->" << it->var << "[i]));\n";
                } else {
                    CC << "    for (" << it->fsizetype << " i=0; i<" << it->varsize << "; i++)\n";
                    CC << "        drop(&(this->" << it->var << "[i]));\n";
                }
            }
            if (it->fisarray && it->farraysize.empty()) {
                CC << "    delete [] " << it->var << ";\n";
            }
        }
    }
    CC << "}\n\n";

    CC << "" << info.msgclass << "& " << info.msgclass << "::operator=(const " << info.msgclass << "& other)\n";
    CC << "{\n";
    CC << "    if (this==&other) return *this;\n";
    if (info.msgbaseclass != "") {
        CC << "    ::" << info.msgbaseclass << "::operator=(other);\n";
    }
    CC << "    copy(other);\n";
    CC << "    return *this;\n";
    CC << "}\n\n";

    CC << "void " << info.msgclass << "::copy(const " << info.msgclass << "& other)\n";
    CC << "{\n";
    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
        if (!it->fisabstract) {
            ClassType classType = getClassType(it->ftype);
            if (it->fisarray && !it->farraysize.empty()) {
                CC << "    for (" << it->fsizetype << " i=0; i<" << it->farraysize << "; i++)\n";
                CC << "        this->" << it->var << "[i] = other." << it->var << "[i];\n";
                if (classType == COWNEDOBJECT) {
                    CC << "    for (" << it->fsizetype << " i=0; i<" << it->farraysize << "; i++)\n";
                    CC << "        this->" << it->var << "[i].setName(other." << it->var << "[i].getName());\n";
                }
            } else if (it->fisarray && it->farraysize.empty()) {
                CC << "    delete [] this->" << it->var << ";\n";
                CC << "    this->" << it->var << " = (other." << it->varsize << "==0) ? NULL : new " << it->datatype << "[other." << it->varsize << "];\n";
                CC << "    " << it->varsize << " = other." << it->varsize << ";\n";
                CC << "    for (" << it->fsizetype << " i=0; i<" << it->varsize << "; i++)\n";
                if (classType == COWNEDOBJECT) {
                    CC << "    {\n";
                    CC << "        take(&(this->" << it->var << "[i]));\n";
                    CC << "        this->" << it->var << "[i] = other." << it->var << "[i];\n";
                    CC << "        this->" << it->var << "[i].setName(other." << it->var << "[i].getName());\n";
                    CC << "    }\n";
                } else {
                    CC << "        this->" << it->var << "[i] = other." << it->var << "[i];\n";
                }
            } else {
                CC << "    this->" << it->var << " = other." << it->var << ";\n";
                if (!it->fisarray && (classType == COWNEDOBJECT || classType == CNAMEDOBJECT)) {
                    CC << "    this->" << it->var << ".setName(other." << it->var << ".getName());\n";
                }
            }
        }
    }
    CC << "}\n\n";

    //
    // Note: This class may not be derived from cOwnedObject, and then this parsimPack()/
    // parsimUnpack() is NOT that of cOwnedObject. However it's still needed because a
    // "friend" doPacking() function could not access protected members otherwise.
    //
    CC << "void " << info.msgclass << "::parsimPack(cCommBuffer *b)\n";
    CC << "{\n";
    if (info.msgbaseclass != "") {
        if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT || info.classtype == COBJECT) {
            if (info.msgbaseclass != "cObject")
                CC << "    ::" << info.msgbaseclass << "::parsimPack(b);\n" ;
        } else {
            CC << "    doPacking(b,(::" << info.msgbaseclass << "&)*this);\n"; // this would do for cOwnedObject too, but the other is nicer
        }
    }
    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
        if (it->fnopack) {
            // @nopack specified
        } else if (it->fisabstract) {
            CC << "    // field " << it->fname << " is abstract -- please do packing in customized class\n";
        } else {
            if (it->fisarray && !it->farraysize.empty()) {
                CC << "    doPacking(b,this->" << it->var << "," << it->farraysize << ");\n";
            } else if (it->fisarray && it->farraysize.empty()) {
                CC << "    b->pack(" << it->varsize << ");\n";
                CC << "    doPacking(b,this->" << it->var << "," << it->varsize << ");\n";
            } else {
                CC << "    doPacking(b,this->" << it->var << ");\n";
            }
        }
    }
    CC << "}\n\n";

    CC << "void " << info.msgclass << "::parsimUnpack(cCommBuffer *b)\n";
    CC << "{\n";
    if (info.msgbaseclass != "") {
        if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT || info.classtype == COBJECT) {
            if (info.msgbaseclass != "cObject")
                CC << "    ::" << info.msgbaseclass << "::parsimUnpack(b);\n";
        } else {
            CC << "    doUnpacking(b,(::" << info.msgbaseclass << "&)*this);\n"; // this would do for cOwnedObject too, but the other is nicer
        }
    }
    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
        if (it->fnopack) {
            // @nopack specified
        } else if (it->fisabstract) {
            CC << "    // field " << it->fname << " is abstract -- please do unpacking in customized class\n";
        } else {
            if (it->fisarray && !it->farraysize.empty()) {
                CC << "    doUnpacking(b,this->" << it->var << "," << it->farraysize << ");\n";
            } else if (it->fisarray && it->farraysize.empty()) {
                CC << "    delete [] this->" << it->var << ";\n";
                CC << "    b->unpack(" << it->varsize << ");\n";
                CC << "    if (" << it->varsize << "==0) {\n";
                CC << "        this->" << it->var << " = 0;\n";
                CC << "    } else {\n";
                CC << "        this->" << it->var << " = new " << it->datatype << "[" << it->varsize << "];\n";
                CC << "        doUnpacking(b,this->" << it->var << "," << it->varsize << ");\n";
                CC << "    }\n";
            } else {
                CC << "    doUnpacking(b,this->" << it->var << ");\n";
            }
        }
    }
    CC << "}\n\n";

    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
        if (!it->fisabstract) {
            bool isstruct = (it->fkind == "struct");
            const char *constifprimitivetype = (!isstruct ? " const" : "");
            if (it->fisarray && !it->farraysize.empty()) {
                CC << "" << it->fsizetype << " " << info.msgclass << "::" << it->getsize << "() const\n";
                CC << "{\n";
                CC << "    return " << it->farraysize << ";\n";
                CC << "}\n\n";
                CC << "" << it->rettype << " " << info.msgclass << "::" << it->getter << "(" << it->fsizetype << " k)" << constifprimitivetype << "\n";
                CC << "{\n";
                CC << "    if (k>=" << it->farraysize << ") throw cRuntimeError(\"Array of size " << it->farraysize << " indexed by %lu\", (unsigned long)k);\n";
                CC << "    return " << it->var << "[k]" << it->maybe_c_str << ";\n";
                CC << "}\n\n";
                CC << "void " << info.msgclass << "::" << it->setter << "(" << it->fsizetype << " k, " << it->argtype << " " << it->argname << ")\n";
                CC << "{\n";
                CC << "    if (k>=" << it->farraysize << ") throw cRuntimeError(\"Array of size " << it->farraysize << " indexed by %lu\", (unsigned long)k);\n";
                CC << "    this->" << it->var << "[k] = " << it->argname << ";\n";
                CC << "}\n\n";
            } else if (it->fisarray && it->farraysize.empty()) {
                CC << "void " << info.msgclass << "::" << it->alloc << "(" << it->fsizetype << " size)\n";
                CC << "{\n";
                CC << "    " << it->datatype << " *" << it->var << "2 = (size==0) ? NULL : new " << it->datatype << "[size];\n";
                CC << "    " << it->fsizetype << " sz = " << it->varsize << " < size ? " << it->varsize << " : size;\n";
                CC << "    for (" << it->fsizetype << " i=0; i<sz; i++)\n";
                CC << "        " << it->var << "2[i] = this->" << it->var << "[i];\n";
                if (it->fkind == "basic") {
                    CC << "    for (" << it->fsizetype << " i=sz; i<size; i++)\n";
                    CC << "        " << it->var << "2[i] = 0;\n";
                }
                if (getClassType(it->ftype) == COWNEDOBJECT) {
                    CC << "    for (" << it->fsizetype << " i=sz; i<size; i++)\n";
                    CC << "        take(&(" << it->var << "2[i]));\n";
                }
                CC << "    " << it->varsize << " = size;\n";
                CC << "    delete [] this->" << it->var << ";\n";
                CC << "    this->" << it->var << " = " << it->var << "2;\n";
                CC << "}\n\n";
                CC << "" << it->fsizetype << " " << info.msgclass << "::" << it->getsize << "() const\n";
                CC << "{\n";
                CC << "    return " << it->varsize << ";\n";
                CC << "}\n\n";
                CC << "" << it->rettype << " " << info.msgclass << "::" << it->getter << "(" << it->fsizetype << " k)" << constifprimitivetype << "\n";
                CC << "{\n";
                CC << "    if (k>=" << it->varsize << ") throw cRuntimeError(\"Array of size %d indexed by %d\", " << it->varsize << ", k);\n";
                CC << "    return " << it->var << "[k]" << it->maybe_c_str << ";\n";
                CC << "}\n\n";
                CC << "void " << info.msgclass << "::" << it->setter << "(" << it->fsizetype << " k, " << it->argtype << " " << it->argname << ")\n";
                CC << "{\n";
                CC << "    if (k>=" << it->varsize << ") throw cRuntimeError(\"Array of size %d indexed by %d\", " << it->varsize << ", k);\n";
                CC << "    this->" << it->var << "[k] = " << it->argname << ";\n";
                CC << "}\n\n";
            } else {
                CC << "" << it->rettype << " " << info.msgclass << "::" << it->getter << "()" << constifprimitivetype << "\n";
                CC << "{\n";
                CC << "    return " << it->var << "" << it->maybe_c_str << ";\n";
                CC << "}\n\n";
                CC << "void " << info.msgclass << "::" << it->setter << "(" << it->argtype << " " << it->argname << ")\n";
                CC << "{\n";
                CC << "    this->" << it->var << " = " << it->argname << ";\n";
                CC << "}\n\n";
            }
        }
    }
}

#define DD(x)
void MsgCppGenerator::generateStruct(const ClassInfo& info)
{
    H << "/**\n";
    H << " * Struct generated from " << SL(info.nedElement->getSourceLocation()) << " by " PROGRAM ".\n";
    H << " */\n";
    if (info.msgbaseclass.empty()) {
        H << "struct " << TS(opts.exportdef) << info.msgclass << "\n";
    } else {
        H << "struct " << TS(opts.exportdef) << info.msgclass << " : public ::" << info.msgbaseqname << "\n";
    }
    H << "{\n";
    H << "    " << info.msgclass << "();\n";
    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
        H << "    " << it->datatype << " " << it->var;
        if (it->fisarray) {
            H << "[" << it->farraysize << "]";
        }
        H << ";\n";
    }
    H << "};\n\n";

    H << "void " << TS(opts.exportdef) << "doPacking(cCommBuffer *b, " << info.msgclass << "& a);\n";
    H << "void " << TS(opts.exportdef) << "doUnpacking(cCommBuffer *b, " << info.msgclass << "& a);\n\n";

    // Constructor:
    CC << "" << info.msgclass << "::" << info.msgclass << "()\n";
    CC << "{\n";
    // override baseclass fields initial value
    for (ClassInfo::Fieldlist::const_iterator it = info.baseclassFieldlist.begin(); it != info.baseclassFieldlist.end(); ++it)
    {
        CC << "    this->" << it->var << " = " << it->fval << ";\n";
    }
    if (!info.baseclassFieldlist.empty() && !info.fieldlist.empty())
        CC << "\n" ;

    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
        if (it->fisabstract)
            throw NEDException("abstract field not possible in struct");
        if (getClassType(it->ftype) == COWNEDOBJECT)
            throw NEDException("cOwnedObject field not possible in struct");
        if (it->fisarray && it->farraysize.empty())
            throw NEDException("dynamic array not possible in struct");
        if (it->fisarray && !it->farraysize.empty()) {
            if (it->fkind == "basic") {
                CC << "    for (" << it->fsizetype << " i=0; i<" << it->farraysize << "; i++)\n";
                CC << "        " << it->var << "[i] = " << it->fval << ";\n";
            }
        } else {
            if (!it->fval.empty()) {
                CC << "    " << it->var << " = " << it->fval << ";\n";
            }
        }
    }
    CC << "}\n\n";
    CC << "void doPacking(cCommBuffer *b, " << info.msgclass << "& a)\n";
    CC << "{\n";
    if (!info.msgbaseclass.empty()) {
        CC << "    doPacking(b,(::" << info.msgbaseclass << "&)a);\n";
    }

    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
        if (it->fisarray) {
            CC << "    doPacking(b,a." << it->var << "," << it->farraysize << ");\n";
        } else {
            CC << "    doPacking(b,a." << it->var << ");\n";
        }
    }
    CC << "}\n\n";

    CC << "void doUnpacking(cCommBuffer *b, " << info.msgclass << "& a)\n";
    CC << "{\n";
    if (!info.msgbaseclass.empty()) {
        CC << "    doUnpacking(b,(::" << info.msgbaseclass << "&)a);\n";
    }

    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
    {
        if (it->fisarray) {
            CC << "    doUnpacking(b,a." << it->var << "," << it->farraysize << ");\n";
        } else {
            CC << "    doUnpacking(b,a." << it->var << ");\n";
        }
    }
    CC << "}\n\n";
}

void MsgCppGenerator::generateDescriptorClass(const ClassInfo& info)
{
    CC << "class " << info.msgdescclass << " : public cClassDescriptor\n";
    CC << "{\n";
    CC << "  public:\n";
    CC << "    " << info.msgdescclass << "();\n";
    CC << "    virtual ~" << info.msgdescclass << "();\n";
    CC << "\n";
    CC << "    virtual bool doesSupport(cObject *obj) const;\n";
    CC << "    virtual const char *getProperty(const char *propertyname) const;\n";
    CC << "    virtual int getFieldCount(void *object) const;\n";
    CC << "    virtual const char *getFieldName(void *object, int field) const;\n";
    CC << "    virtual int findField(void *object, const char *fieldName) const;\n";
    CC << "    virtual unsigned int getFieldTypeFlags(void *object, int field) const;\n";
    CC << "    virtual const char *getFieldTypeString(void *object, int field) const;\n";
    CC << "    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;\n";
    CC << "    virtual int getArraySize(void *object, int field) const;\n";
    CC << "\n";
    CC << "    virtual std::string getFieldAsString(void *object, int field, int i) const;\n";
    CC << "    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;\n";
    CC << "\n";
    CC << "    virtual const char *getFieldStructName(void *object, int field) const;\n";
    CC << "    virtual void *getFieldStructPointer(void *object, int field, int i) const;\n";
    CC << "};\n\n";

    // register class
    CC << "Register_ClassDescriptor(" << info.msgdescclass << ");\n\n";

    // ctor, dtor
    size_t fieldcount = info.fieldlist.size();
    std::string qualifiedrealmsgclass = prefixWithNamespace(info.realmsgclass);
    CC << "" << info.msgdescclass << "::" << info.msgdescclass << "() : cClassDescriptor(\"" << qualifiedrealmsgclass << "\", \"" << info.msgbaseclass << "\")\n";
    CC << "{\n";
    CC << "}\n";
    CC << "\n";

    CC << "" << info.msgdescclass << "::~" << info.msgdescclass << "()\n";
    CC << "{\n";
    CC << "}\n";
    CC << "\n";

    // doesSupport()
    CC << "bool " << info.msgdescclass << "::doesSupport(cObject *obj) const\n";
    CC << "{\n";
    CC << "    return dynamic_cast<" << info.msgclass << " *>(obj)!=NULL;\n";
    CC << "}\n";
    CC << "\n";

    // getProperty()
    CC << "const char *" << info.msgdescclass << "::getProperty(const char *propertyname) const\n";
    CC << "{\n";

    //Properties
    for (Properties::const_iterator key = info.props.begin(); key != info.props.end(); ++key) {
        CC << "    if (!strcmp(propertyname,"<< opp_quotestr(key->first.c_str()) << ")) return " << opp_quotestr(key->second.c_str()) << ";\n";
    }

    CC << "    cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    return basedesc ? basedesc->getProperty(propertyname) : NULL;\n";
    CC << "}\n";
    CC << "\n";

    // getFieldCount()
    CC << "int " << info.msgdescclass << "::getFieldCount(void *object) const\n";
    CC << "{\n";
    CC << "    cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    return basedesc ? " << fieldcount << "+basedesc->getFieldCount(object) : " << fieldcount << ";\n";
    CC << "}\n";
    CC << "\n";

    // getFieldTypeFlags()
    CC << "unsigned int " << info.msgdescclass << "::getFieldTypeFlags(void *object, int field) const\n";
    CC << "{\n";
    CC << "    cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount(object))\n";
    CC << "            return basedesc->getFieldTypeFlags(object, field);\n";
    CC << "        field -= basedesc->getFieldCount(object);\n";
    CC << "    }\n";
    if (fieldcount == 0) {
        CC << "    return 0;\n";
    } else {
        CC << "    static unsigned int fieldTypeFlags[] = {\n";
        for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
        {
            StringVector flags;
            ClassType classType = getClassType(it->ftype);
            if (it->fisarray)
                flags.push_back("FD_ISARRAY");
            if (it->fkind == "struct")
                flags.push_back("FD_ISCOMPOUND");
            if (it->fispointer)
                flags.push_back("FD_ISPOINTER");
            if (classType == COBJECT || classType == CNAMEDOBJECT)
                flags.push_back("FD_ISCOBJECT");
            if (classType == COWNEDOBJECT)
                flags.push_back("FD_ISCOBJECT | FD_ISCOWNEDOBJECT");

            if (it->feditable || (info.generate_setters_in_descriptor && it->fkind == "basic"))
                flags.push_back("FD_ISEDITABLE");
            std::string flagss;
            if (flags.empty())
                flagss = "0" ;
            else
            {
                flagss = flags[0];
                for (size_t i=1; i<flags.size(); i++)
                    flagss = flagss + " | " + flags[i];
            }

            CC << "        " << flagss << ",\n";
        }
        CC << "    };\n";
        CC << "    return (field>=0 && field<" << fieldcount << ") ? fieldTypeFlags[field] : 0;\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldName()
    CC << "const char *" << info.msgdescclass << "::getFieldName(void *object, int field) const\n";
    CC << "{\n";
    CC << "    cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount(object))\n";
    CC << "            return basedesc->getFieldName(object, field);\n";
    CC << "        field -= basedesc->getFieldCount(object);\n";
    CC << "    }\n";
    if (fieldcount == 0) {
        CC << "    return NULL;\n";
    } else {
        CC << "    static const char *fieldNames[] = {\n";
        for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
        {
            CC << "        \"" << it->fname << "\",\n";
        }
        CC << "    };\n";
        CC << "    return (field>=0 && field<" << fieldcount << ") ? fieldNames[field] : NULL;\n";
    }
    CC << "}\n";
    CC << "\n";

    // findField()
    CC << "int " << info.msgdescclass << "::findField(void *object, const char *fieldName) const\n";
    CC << "{\n";
    CC << "    cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    if (fieldcount > 0) {
        CC << "    int base = basedesc ? basedesc->getFieldCount(object) : 0;\n";
        for (size_t i=0; i < info.fieldlist.size(); ++i)
        {
            char c = info.fieldlist[i].fname[0];
            CC << "    if (fieldName[0]=='" << c << "' && strcmp(fieldName, \""<<info.fieldlist[i].fname<<"\")==0) return base+" << i << ";\n";
        }
    }
    CC << "    return basedesc ? basedesc->findField(object, fieldName) : -1;\n";
    CC << "}\n";
    CC << "\n";

    // getFieldTypeString()
    CC << "const char *" << info.msgdescclass << "::getFieldTypeString(void *object, int field) const\n";
    CC << "{\n";
    CC << "    cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount(object))\n";
    CC << "            return basedesc->getFieldTypeString(object, field);\n";
    CC << "        field -= basedesc->getFieldCount(object);\n";
    CC << "    }\n";
    if (fieldcount == 0) {
        CC << "    return NULL;\n";
    } else {
        CC << "    static const char *fieldTypeStrings[] = {\n";
        for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it)
        {
            CC << "        \"" << it->ftype << "\",\n"; // note: NOT $fieldtypeqname! that's getFieldStructName()
        }
        CC << "    };\n";
        CC << "    return (field>=0 && field<" << fieldcount << ") ? fieldTypeStrings[field] : NULL;\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldProperty()
    CC << "const char *" << info.msgdescclass << "::getFieldProperty(void *object, int field, const char *propertyname) const\n";
    CC << "{\n";
    CC << "    cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount(object))\n";
    CC << "            return basedesc->getFieldProperty(object, field, propertyname);\n";
    CC << "        field -= basedesc->getFieldCount(object);\n";
    CC << "    }\n";
    CC << "    switch (field) {\n";

    for (size_t i=0; i < fieldcount; ++i)
    {
        const Properties &ref = info.fieldlist[i].fprops;
        if (!ref.empty()) {
            CC << "        case " << i << ":\n";
            for (Properties::const_iterator it = ref.begin(); it != ref.end(); ++it) {
                std::string prop = opp_quotestr(it->second.c_str());
                CC << "            if (!strcmp(propertyname,\"" << it->first << "\")) return " << prop << ";\n";
            }
            CC << "            return NULL;\n";
        }
    }

    CC << "        default: return NULL;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getArraySize()
    CC << "int " << info.msgdescclass << "::getArraySize(void *object, int field) const\n";
    CC << "{\n";
    CC << "    cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount(object))\n";
    CC << "            return basedesc->getArraySize(object, field);\n";
    CC << "        field -= basedesc->getFieldCount(object);\n";
    CC << "    }\n";
    CC << "    " << info.msgclass << " *pp = (" << info.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i=0; i < fieldcount; i++) {
        if (info.fieldlist[i].fisarray) {
            CC << "        case " << i << ": ";
            if (!info.fieldlist[i].farraysize.empty()) {
                CC << "return " << info.fieldlist[i].farraysize << ";\n";
            } else if (info.classtype == STRUCT) {
                CC << "return pp->" << info.fieldlist[i].varsize << ";\n";
            } else {
                CC << "return pp->" << info.fieldlist[i].getsize << "();\n";
            }
        }
    }
    CC << "        default: return 0;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldAsString()
    CC << "std::string " << info.msgdescclass << "::getFieldAsString(void *object, int field, int i) const\n";
    CC << "{\n";
    CC << "    cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount(object))\n";
    CC << "            return basedesc->getFieldAsString(object,field,i);\n";
    CC << "        field -= basedesc->getFieldCount(object);\n";
    CC << "    }\n";
    CC << "    " << info.msgclass << " *pp = (" << info.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i=0; i<fieldcount; i++)
    {
        if (info.fieldlist[i].fkind == "basic") {
            CC << "        case " << i << ": ";
            if (info.classtype == STRUCT) {
                if (info.fieldlist[i].fisarray) {
                    if (!info.fieldlist[i].farraysize.empty()) {
                        CC << "if (i>=" << info.fieldlist[i].farraysize << ") return \"\";\n";
                    } else {
                        CC << "if (i>=pp->" << info.fieldlist[i].varsize << ") return \"\";\n";
                    }
                    CC << "                return " << info.fieldlist[i].tostring << "(pp->" << info.fieldlist[i].var << "[i]);\n";
                } else {
                    CC << "return " << info.fieldlist[i].tostring << "(pp->" << info.fieldlist[i].var << ");\n";
                }
            } else {
                if (info.fieldlist[i].fisarray) {
                    CC << "return " << info.fieldlist[i].tostring << "(pp->" << info.fieldlist[i].getter << "(i));\n";
                } else {
                    CC << "return " << info.fieldlist[i].tostring << "(pp->" << info.fieldlist[i].getter << "());\n";
                }
            }
        } else if (info.fieldlist[i].fkind == "struct") {
            CC << "        case " << i << ": ";
            if (info.classtype == STRUCT) {
                if (info.fieldlist[i].fisarray) {
                    CC << "{std::stringstream out; out << pp->" << info.fieldlist[i].var << "[i]; return out.str();}\n";
                } else {
                    CC << "{std::stringstream out; out << pp->" << info.fieldlist[i].var << "; return out.str();}\n";
                }
            } else {
                if (info.fieldlist[i].fisarray) {
                    CC << "{std::stringstream out; out << pp->" << info.fieldlist[i].getter << "(i); return out.str();}\n";
                } else {
                    CC << "{std::stringstream out; out << pp->" << info.fieldlist[i].getter << "(); return out.str();}\n";
                }
            }
        } else {
            throw NEDException("internal error");
        }
    }
    CC << "        default: return \"\";\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // setFieldAsString()
    CC << "bool " << info.msgdescclass << "::setFieldAsString(void *object, int field, int i, const char *value) const\n";
    CC << "{\n";
    CC << "    cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount(object))\n";
    CC << "            return basedesc->setFieldAsString(object,field,i,value);\n";
    CC << "        field -= basedesc->getFieldCount(object);\n";
    CC << "    }\n";
    CC << "    " << info.msgclass << " *pp = (" << info.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i=0; i < fieldcount; i++)
    {
        if (info.fieldlist[i].feditable || (info.generate_setters_in_descriptor && info.fieldlist[i].fkind == "basic")) {
            CC << "        case " << i << ": ";
            if (info.classtype == STRUCT) {
                if (info.fieldlist[i].fisarray) {
                    if (!info.fieldlist[i].farraysize.empty()) {
                        CC << "if (i>=" << info.fieldlist[i].farraysize << ") return false;\n";
                    } else {
                        CC << "if (i>=pp->" << info.fieldlist[i].varsize << ") return false;\n";
                    }
                    CC << "                pp->"<<info.fieldlist[i].var << "[i] = " << info.fieldlist[i].fromstring << "(value); return true;\n";
                } else {
                    CC << "pp->" << info.fieldlist[i].var << " = " << info.fieldlist[i].fromstring << "(value); return true;\n";
                }
            } else {
                if (info.fieldlist[i].fisarray) {
                    CC << "pp->" << info.fieldlist[i].setter << "(i," << info.fieldlist[i].fromstring << "(value)); return true;\n";
                } else {
                    CC << "pp->" << info.fieldlist[i].setter << "(" << info.fieldlist[i].fromstring << "(value)); return true;\n";
                }
            }
        }
    }
    CC << "        default: return false;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldStructName()
    CC << "const char *" << info.msgdescclass << "::getFieldStructName(void *object, int field) const\n";
    CC << "{\n";
    CC << "    cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount(object))\n";
    CC << "            return basedesc->getFieldStructName(object, field);\n";
    CC << "        field -= basedesc->getFieldCount(object);\n";
    CC << "    }\n";
    if (fieldcount == 0) {
        CC << "    return NULL;\n";
    } else {
        CC << "    switch (field) {\n";
        for (size_t i=0; i < fieldcount; i++)
        {
            bool opaque = info.fieldlist[i].fopaque;  // TODO: @opaque should rather be the attribute of the field's type, not the field itself
            if (info.fieldlist[i].fkind == "struct" && !opaque) {
                CC << "        case " << i << ": return opp_typename(typeid(" << info.fieldlist[i].ftype << "));\n";
            }
        }
        CC << "        default: return NULL;\n";
        CC << "    };\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldStructPointer()
    CC << "void *" << info.msgdescclass << "::getFieldStructPointer(void *object, int field, int i) const\n";
    CC << "{\n";
    CC << "    cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount(object))\n";
    CC << "            return basedesc->getFieldStructPointer(object, field, i);\n";
    CC << "        field -= basedesc->getFieldCount(object);\n";
    CC << "    }\n";
    CC << "    " << info.msgclass << " *pp = (" << info.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i=0; i < fieldcount; i++)
    {
        bool opaque = info.fieldlist[i].fopaque;  //# TODO: @opaque should rather be the attribute of the field's type, not the field itself
        if (info.fieldlist[i].fkind == "struct" && !opaque) {
            std::string cast;
            std::string value;
            if (info.classtype == STRUCT) {
                if (info.fieldlist[i].fisarray) {
                    value = std::string("pp->") + info.fieldlist[i].var + "[i]";
                } else {
                    value = std::string("pp->") + info.fieldlist[i].var;
                }
            } else {
                if (info.fieldlist[i].fisarray) {
                    value = std::string("pp->") + info.fieldlist[i].getter + "(i)";
                } else {
                    value = std::string("pp->") + info.fieldlist[i].getter + "()";
                }
            }
            ClassType fieldclasstype = getClassType(info.fieldlist[i].ftype);
            cast = "(void *)";
            if (fieldclasstype == COBJECT || fieldclasstype == CNAMEDOBJECT || fieldclasstype == COWNEDOBJECT)
                cast = cast + "static_cast<cObject *>";
            if (info.fieldlist[i].fispointer) {
                CC << "        case " << i << ": return " << cast << "(" << value << "); break;\n";
            } else {
                CC << "        case " << i << ": return " << cast << "(&" << value << "); break;\n";
            }
        }
    }
    CC << "        default: return NULL;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";
}

MsgCppGenerator::EnumInfo MsgCppGenerator::extractEnumInfo(EnumElement *node)
{
    EnumInfo info;
    info.nedElement = node;
    info.enumName = ptr2str(node->getAttribute("name"));
    info.enumQName = canonicalizeQName(namespacename, info.enumName);

    // prepare enum items:
    for (NEDElement *child = node->getFirstChild(); child; child = child->getNextSibling())
    {
        switch (child->getTagCode()) {
            case NED_ENUM_FIELDS:
                for (NEDElement *e = child->getFirstChild(); e; e = e->getNextSibling())
                {
                    switch (e->getTagCode()) {
                        case NED_ENUM_FIELD:
                        {
                            EnumInfo::EnumItem f;
                            f.nedElement = e;
                            f.name = ptr2str(e->getAttribute("name"));
                            f.value = ptr2str(e->getAttribute("value"));
                            info.fieldlist.push_back(f);
                            break;
                        }
                        case NED_COMMENT:
                            break;
                        default:
                            errors->addError(e, "unaccepted element '%s'", e->getTagName());
                    }
                }
                break;
            case NED_COMMENT:
                break;
            default:
                errors->addError(child, "unaccepted element '%s'", child->getTagName());
                break;
        }
    }
    return info;
}

void MsgCppGenerator::generateEnum(const EnumInfo& enumInfo)
{
    H << "/**\n";
    H << " * Enum generated from <tt>" << SL(enumInfo.nedElement->getSourceLocation()) << "</tt> by " PROGRAM ".\n";
    H << generatePreComment(enumInfo.nedElement);
    H << " */\n";

    H << "enum " << enumInfo.enumName <<" {\n";
    for (EnumInfo::FieldList::const_iterator it = enumInfo.fieldlist.begin(); it != enumInfo.fieldlist.end(); )
    {
        H << "    " << it->name << " = " << it->value;
        if (++it != enumInfo.fieldlist.end())
            H << ",";
        H << "\n";
    }
    H << "};\n\n";

    CC << "EXECUTE_ON_STARTUP(\n";
    CC << "    cEnum *e = cEnum::find(\"" << enumInfo.enumQName << "\");\n";
    CC << "    if (!e) enums.getInstance()->add(e = new cEnum(\"" << enumInfo.enumQName << "\"));\n";
    // enum inheritance: we should add fields from base enum as well, but that could only be done when importing is in place
    for (EnumInfo::FieldList::const_iterator it = enumInfo.fieldlist.begin(); it != enumInfo.fieldlist.end(); ++it)
    {
        CC << "    e->insert(" << it->name << ", \"" << it->name << "\");\n";
    }
    CC << ");\n\n";
}


std::string MsgCppGenerator::prefixWithNamespace(const std::string& name)
{
    return !namespacename.empty() ? namespacename + "::" + name : name; // prefer name from local namespace
}

bool MsgCppGenerator::getPropertyAsBool(const Properties& p, const char *name, bool defval)
{
    Properties::const_iterator it = p.find(name);
    if (it == p.end())
        return defval;
    if (it->second == "false")
        return false;
    return true;
}

std::string MsgCppGenerator::getProperty(const Properties& p, const char *name, const std::string& defval)
{
    Properties::const_iterator it = p.find(name);
    if (it == p.end())
        return defval;
    return it->second;
}

MsgCppGenerator::StringVector MsgCppGenerator::lookupExistingClassName(const std::string& name)
{
    StringVector ret;
    std::map<std::string,ClassType>::iterator it;

    if (name.empty())
    {
        ret.push_back(name);
        return ret;
    }

    // if $name contains "::" then user means explicitly qualified name; otherwise he means 'in whichever namespace it is'
    if (name.find("::") != name.npos) {
        std::string qname = name.substr(0, 2) == "::" ? name.substr(2) : name; // remove leading "::", because names in @classes don't have it either
        it = classtype.find(qname);
        if (it != classtype.end())
        {
            ret.push_back(it->first);
            return ret;
        }
    }
    else {
        std::string qname = prefixWithNamespace(name);
        it = classtype.find(qname);
        if (it != classtype.end())
        {
            ret.push_back(it->first);
            return ret;
        }
    }
    size_t namelength = name.length();
    for (it = classtype.begin(); it != classtype.end(); ++it)
    {
        size_t l = it->first.length();
        if (l >= namelength)
        {
            size_t pos = l - namelength;
            if ((pos == 0 || it->first[pos-1] == ':') && (it->first.substr(pos) == name))
            {
                ret.push_back(it->first);
            }
        }
    }
    return ret;
}

MsgCppGenerator::StringVector MsgCppGenerator::lookupExistingEnumName(const std::string& name)
{
    StringVector ret;
    std::map<std::string,std::string>::iterator it;

    if (name.empty())
    {
        //ret.push_back(name);
        return ret;
    }

    // if $name contains "::" then user means explicitly qualified name; otherwise he means 'in whichever namespace it is'
    if (name.find("::") != name.npos) {
        std::string qname = name.substr(0, 2) == "::" ? name.substr(2) : name; // remove leading "::", because names in @classes don't have it either
        it = enumtype.find(qname);
        if (it != enumtype.end())
        {
            ret.push_back(it->second);
            return ret;
        }
    }
    else {
        std::string qname = prefixWithNamespace(name); // prefer name from local namespace
        it = enumtype.find(qname);
        if (it != enumtype.end())
        {
            ret.push_back(it->second);
            return ret;
        }
    }

    size_t namelength = name.length();
    for (it = enumtype.begin(); it != enumtype.end(); ++it)
    {
        size_t l = it->second.length();
        if (l >= namelength)
        {
            size_t pos = l - namelength;
            if ((pos == 0 || it->second[pos-1] == ':') && (it->second.substr(pos) == name))
            {
                ret.push_back(it->second);
            }
        }
    }
    return ret;
}

void MsgCppGenerator::generateTemplates()
{
    CC << "// Template rule for outputting std::vector<T> types\n";
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
    CC << "    \n";
    CC << "    char buf[32];\n";
    CC << "    sprintf(buf, \" (size=%u)\", (unsigned int)vec.size());\n";
    CC << "    out.write(buf, strlen(buf));\n";
    CC << "    return out;\n";
    CC << "}\n";
    CC << "\n";

    CC << "// Template rule which fires if a struct or class doesn't have operator<<\n";
    CC << "template<typename T>\n";
    CC << "inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}\n";
    CC << "\n";
}

void MsgCppGenerator::generateNamespaceBegin(NEDElement *element)
{
    if (namespacename.empty()) {
        errors->addError(element, "namespace name is empty\n");
    }

    // split namespacename into namespacenamevector
    for (size_t pos = 0; ; ) {
        size_t colonPos = namespacename.find("::", pos);
        if (colonPos != namespacename.npos) {
            namespacenamevector.push_back(namespacename.substr(pos, colonPos-pos));
            pos = colonPos + 2;
        }
        else {
            namespacenamevector.push_back(namespacename.substr(pos));
            break;
        }
    }

    // output namespace-begin lines; also check for reserved words
    H  << std::endl;
    for (StringVector::const_iterator it = namespacenamevector.begin(); it != namespacenamevector.end(); ++it)
    {
        if (RESERVED_WORDS.find(*it) != RESERVED_WORDS.end())
            errors->addError(element, "namespace name '%s' is a reserved word\n", (*it).c_str());
        H  << "namespace " << *it << " {\n";
        CC << "namespace " << *it << " {\n";
    }
    H  << std::endl;
    CC << std::endl;

    generateTemplates();

}

void MsgCppGenerator::generateNamespaceEnd()
{
#ifdef COMPATIBILITY_MODE
    for (StringVector::const_iterator it = namespacenamevector.begin(); it != namespacenamevector.end(); ++it)
    {
        H  << "}; // end namespace " << *it << std::endl;
        CC << "}; // end namespace " << *it << std::endl;
    }
#else
    for (StringVector::const_reverse_iterator it = namespacenamevector.rbegin(); it != namespacenamevector.rend(); ++it)
    {
        H  << "} // namespace " << *it << std::endl;
        CC << "} // namespace " << *it << std::endl;
    }
#endif
    H  << std::endl;
    CC << std::endl;
}

MsgCppGenerator::ClassType MsgCppGenerator::getClassType(const std::string& s)
{
    std::map<std::string,ClassType>::iterator it = classtype.find(s);
    return it != classtype.end() ? it->second : UNKNOWN;
}

NAMESPACE_END

