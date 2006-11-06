//==========================================================================
//   CPPMSGGENERATOR.CC
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


/***** UNFINISHED CODE -- DEVELOPMENT IN PROGRESS *****/


#include <string.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

#include "nedelements.h"
#include "cppgenerator.h"
#include "nederror.h"

using std::ostream;

inline bool strnotnull(const char *s)
{
    return s && s[0];
}

inline ostream& operator<< (ostream& out, const std::string& str)
{
    out << str.c_str();
    return out;
}

//---------------------------------------

void NEDCppGenerator::doCplusplus(CplusplusNode *node, const char *indent, int mode, const char *)
{
    out << "// cplusplus {{\n";
    out << node->getBody();
    out << "// }}\n";
}

void NEDCppGenerator::doStructDecl(StructDeclNode *node, const char *indent, int mode, const char *)
{
    // nothing to output
}

void NEDCppGenerator::doClassDecl(ClassDeclNode *node, const char *indent, int mode, const char *)
{
    // nothing to output
}

void NEDCppGenerator::doMessageDecl(MessageDeclNode *node, const char *indent, int mode, const char *)
{
    // nothing to output
}

void NEDCppGenerator::doEnumDecl(EnumDeclNode *node, const char *indent, int mode, const char *)
{
    // nothing to output
}

void NEDCppGenerator::doEnum(EnumNode *node, const char *indent, int mode, const char *)
{
    outh << "enum " << node->getName() << " {\n";
    if (!strnotnull(node->getExtendsName()))
        out << "static sEnumBuilder _" << node->getName() << "( \"" << node->getName() << "\",\n";
    else
        out << "static sEnumBuilder _" << node->getExtendsName() << "_" << node->getName() << "( \"" << node->getExtendsName() << "\",\n";

    generateChildren(node, indent, mode);

    outh << "};\n\n";
    out << "    0, NULL\n";
    out << ");\n\n";
}

void NEDCppGenerator::doEnumFields(EnumFieldsNode *node, const char *indent, int mode, const char *)
{
    generateChildren(node, indent, mode);
}

void NEDCppGenerator::doEnumField(EnumFieldNode *node, const char *indent, int mode, const char *)
{
    outh << indent << node->getName() << " = " << node->getValue() << ",\n";
    out << indent << node->getName() << ", \"" << node->getName() << "\",\n";
}


enum
{
    CLASSTYPE_COBJECT,
    CLASSTYPE_NONCOBJECT,
    CLASSTYPE_STRUCT
};

struct NEDCppGenerator::ClassDesc
{
    std::string msgname; // name of class in NED file
    std::string msgbase; // name of base class in NED file ('extends' name)
    std::string keyword;   // "message", "class", "struct"

    int classtype;              // CLASSTYPE_COBJECT, CLASSTYPE_NONCOBJECT, CLASSTYPE_STRUCT
    bool usegap;
    std::string msgclass;
    std::string realmsgclass;
    std::string msgbaseclass;

    std::string msgdescclass;
    std::string msgbasedescclass;
    bool hasbasedescriptor;
};


enum
{
    FIELDTYPE_BASIC,
    FIELDTYPE_STRUCT,
    FIELDTYPE_SPECIAL
};

struct NEDCppGenerator::FieldDesc
{
    std::string fieldname;
    std::string ftype;
    std::string fval;
    bool fisabstract;
    bool fisarray;
    std::string farraysize;
    std::string fenumname;
    bool fispointer;

    int fkind;                  // FIELDTYPE_BASIC, FIELDTYPE_STRUCT, FIELDTYPE_SPECIAL
    int classtype; // if fkind==FIELDTYPE_STRUCT
    std::string datatype;
    std::string argtype;
    std::string var;
    std::string varsize;
    std::string getter;
    std::string setter;
    std::string alloc;
    std::string getsize;
    std::string tostring;
    std::string fromstring;
};

void NEDCppGenerator::doMessage(MessageNode *node, const char *, int, const char *)
{
    ClassDesc cld;
    FieldDesc *fld;
    int numfields;
    prepareForCodeGeneration(node, cld, fld, numfields);
    generateClass(cld, fld, numfields);
    generateDescriptorClass(cld, fld, numfields);
    delete [] fld;
}

void NEDCppGenerator::doClass(ClassNode *node, const char *, int, const char *)
{
    ClassDesc cld;
    FieldDesc *fld;
    int numfields;
    prepareForCodeGeneration(node, cld, fld, numfields);
    generateClass(cld, fld, numfields);
    generateDescriptorClass(cld, fld, numfields);
    delete [] fld;
}

void NEDCppGenerator::doStruct(StructNode *node, const char *, int, const char *)
{
    ClassDesc cld;
    FieldDesc *fld;
    int numfields;
    prepareForCodeGeneration(node, cld, fld, numfields);
    generateStruct(cld, fld, numfields);
    generateDescriptorClass(cld, fld, numfields);
    delete [] fld;
}


void NEDCppGenerator::prepareForCodeGeneration(NEDElement *node, NEDCppGenerator::ClassDesc& cld, NEDCppGenerator::FieldDesc *&fld, int& numfields)
{
    cld.keyword = node->getTagName();     // "message", "class", "struct"
    cld.msgname = node->getAttribute("name");
    cld.msgbase = node->getAttribute("extends-name");

    // check base class and determine type of object
    if (cld.msgbase == "")
    {
        if (cld.keyword == "message")
        {
            cld.classtype = CLASSTYPE_COBJECT;
        }
        else if (cld.keyword == "class")
        {
            cld.classtype = CLASSTYPE_NONCOBJECT;
        }
        else if (cld.keyword == "struct")
        {
            cld.classtype = CLASSTYPE_STRUCT;
        }
        else
        {
            INTERNAL_ERROR0(node, "prepareForCodeGeneration(): cannot determine classtype");
        }
    }
    else
    {
#if 0
        // TBD use symboltable here!
        cld.classtype = cld.classtype[[cld.msgbase]];
#endif
        cld.classtype = CLASSTYPE_COBJECT;
    }

#if 0
    // check earlier declarations and register this class
    if (grep(/^\Qmsgname \ E /, @classes))
    {
        if (hasdescriptor[[cld.msgname]])
        {
            INTERNAL_ERROR2(node, "attempt to redefine '%s'", cld.msgname.c_str());
            ret = 1;
        }
        else if (cld.classtype[[cld.msgname]] ne cld.classtype)
        {
            INTERNAL_ERROR2(node, "definition of '%s' inconsistent with earlier declaration(s)", cld.msgname.c_str());
            ret = 1;
        }
        else
        {
            // OK
            hasdescriptor[[cld.msgname]] = 1;
        }
    }
    else
    {
        push(@classes, cld.msgname);
        cld.classtype[[cld.msgname]] = cld.classtype;
        hasdescriptor[[cld.msgname]] = 1;
    }
#endif

    //
    // produce all sorts of derived names
    //
    cld.usegap = false; // pval[["customize"]]=="true"

    if (cld.usegap)
    {
        cld.msgclass = cld.msgname + "_Base";
    }
    else
    {
        cld.msgclass = cld.msgname;
    }

    cld.realmsgclass = cld.msgname;
    cld.msgdescclass = cld.msgname + "Descriptor";

    if (cld.msgbase == "")
    {
        if (cld.keyword == "message")
        {
            cld.msgbaseclass = "cMessage";
        }
        else if (cld.keyword == "class")
        {
            cld.msgbaseclass = "";
        }
        else if (cld.keyword == "struct")
        {
            cld.msgbaseclass = "";
        }
        else
        {
            INTERNAL_ERROR0(node, "internal error");
        }
    }
    else
    {
        cld.msgbaseclass = cld.msgbase;
    }

    cld.hasbasedescriptor = false; // should be hasdescriptor[[cld.msgbaseclass]];
    if (cld.hasbasedescriptor)
    {
        cld.msgbasedescclass = cld.msgbaseclass + "Descriptor";
    }
    else
    {
        cld.msgbasedescclass = "cClassDescriptor";
    }


    //
    // process fields
    //
    numfields = 0;
    FieldsNode *fieldsnode = (FieldsNode *)node->getFirstChildWithTag(NED_FIELDS);
    if (fieldsnode && fieldsnode->getFirstFieldChild())
    {
        // count fields
        FieldNode *field;
        for (field=fieldsnode->getFirstFieldChild(); field; field=field->getNextFieldNodeSibling())
            numfields++;

        // allocate array and fill fld[] array
        fld = new FieldDesc[numfields];
        int i;
        for (field=fieldsnode->getFirstFieldChild(),i=0; field; field=field->getNextFieldNodeSibling(),i++)
        {
            fld[i].fieldname = field->getName();
            fld[i].ftype = field->getDataType();
            fld[i].fval = field->getDefaultValue();
            fld[i].fisabstract = field->getIsAbstract();
            fld[i].fisarray = field->getIsVector();
            fld[i].farraysize = field->getVectorSize();
            fld[i].fenumname = field->getEnumName();
        }

        for (i=0; i<numfields; i++)
        {
#if 0
            if (fld[i].fisabstract && !cld.usegap)
                INTERNAL_ERROR0(NULL, "virtual fields assume 'customize=true' property in 'cld.msgname'");
            if (fld[i].fval != "" && cld.classtype == CLASSTYPE_STRUCT)
                INTERNAL_ERROR0(NULL, "default values not possible with structs (no constructor is generated!) in 'cld.msgname'");
            if (fld[i].fenumname != "" && !grep(/^\Qfld[i].fenumname \ E /, @enums))
                INTERNAL_ERROR0(NULL, "undeclared enum 'fld[i].fenumname' used in 'cld.msgname'");
#endif

            // variable name
            fld[i].var = fld[i].fieldname;
            fld[i].varsize = fld[i].var + "_arraysize";

            // method names
            if (cld.classtype != CLASSTYPE_STRUCT)
            {
                std::string capfieldname = fld[i].fieldname;
                capfieldname[0] = toupper(capfieldname[0]);
                fld[i].getter = "get" + capfieldname;
                fld[i].setter = "set" + capfieldname;
                fld[i].alloc = "set" + capfieldname + "ArraySize";
                fld[i].getsize = "get" + capfieldname + "ArraySize";
            }

            // pointer
            fld[i].fispointer = false;
            if (strchr(fld[i].ftype.c_str(), '*'))  // fld[i].ftype contains '*'
            {
                // todo: cut trailing '*' from fld[i].ftype
                fld[i].fispointer = true;
                INTERNAL_ERROR1(node, "pointers not supported yet in '%s'", cld.msgname.c_str());
            }

            // data type, argument type, conversion to/from string...
            if (1 /* grep(/^\Qfld[i].ftype\E/,@classes) */ )
            {
                fld[i].fkind = FIELDTYPE_STRUCT;
            }
            else
            {
                fld[i].fkind = FIELDTYPE_BASIC;
            }
            if (fld[i].fkind == FIELDTYPE_STRUCT)
            {
                fld[i].datatype = fld[i].ftype;
                fld[i].argtype = "const " + fld[i].ftype + "&";
                fld[i].tostring = fld[i].ftype + "2string";
                fld[i].fromstring = "string2" + fld[i].ftype;
                //fld[i].fval = "" unless (fld[i].fval!="");
            }
            else if (fld[i].fkind == FIELDTYPE_BASIC)
            {
                if (fld[i].ftype == "bool")
                {
                    fld[i].datatype = "bool";
                    fld[i].argtype = "bool";
                    fld[i].tostring = "bool2string";
                    fld[i].fromstring = "string2bool";
                    if (fld[i].fval=="")
                        fld[i].fval = "false";
                }
                else if (fld[i].ftype == "char")
                {
                    fld[i].datatype = "char";
                    fld[i].argtype = "char";
                    fld[i].tostring = "long2string";
                    fld[i].fromstring = "string2long";
                    if (fld[i].fval=="")
                        fld[i].fval = "0";
                }
                else if (fld[i].ftype == "unsigned char")
                {
                    fld[i].datatype = "unsigned char";
                    fld[i].argtype = "unsigned char";
                    fld[i].tostring = "long2string";
                    fld[i].fromstring = "string2long";
                    if (fld[i].fval=="")
                        fld[i].fval = "0";
                }
                else if (fld[i].ftype == "short")
                {
                    fld[i].datatype = "short";
                    fld[i].argtype = "short";
                    fld[i].tostring = "long2string";
                    fld[i].fromstring = "string2long";
                    if (fld[i].fval=="")
                        fld[i].fval = "0";
                }
                else if (fld[i].ftype == "unsigned short")
                {
                    fld[i].datatype = "unsigned short";
                    fld[i].argtype = "unsigned short";
                    fld[i].tostring = "long2string";
                    fld[i].fromstring = "string2long";
                    if (fld[i].fval=="")
                        fld[i].fval = "0";
                }
                else if (fld[i].ftype == "int")
                {
                    fld[i].datatype = "int";
                    fld[i].argtype = "int";
                    fld[i].tostring = "long2string";
                    fld[i].fromstring = "string2long";
                    if (fld[i].fval=="")
                        fld[i].fval = "0";
                }
                else if (fld[i].ftype == "unsigned int")
                {
                    fld[i].datatype = "unsigned int";
                    fld[i].argtype = "unsigned int";
                    fld[i].tostring = "long2string";
                    fld[i].fromstring = "string2long";
                    if (fld[i].fval=="")
                        fld[i].fval = "0";
                }
                else if (fld[i].ftype == "long")
                {
                    fld[i].datatype = "long";
                    fld[i].argtype = "long";
                    fld[i].tostring = "long2string";
                    fld[i].fromstring = "string2long";
                    if (fld[i].fval=="")
                        fld[i].fval = "0";
                }
                else if (fld[i].ftype == "unsigned long")
                {
                    fld[i].datatype = "unsigned long";
                    fld[i].argtype = "unsigned long";
                    fld[i].tostring = "long2string";
                    fld[i].fromstring = "string2long";
                    if (fld[i].fval=="")
                        fld[i].fval = "0";
                }
                else if (fld[i].ftype == "double")
                {
                    fld[i].datatype = "double";
                    fld[i].argtype = "double";
                    fld[i].tostring = "double2string";
                    fld[i].fromstring = "string2double";
                    if (fld[i].fval=="")
                        fld[i].fval = "0";
                }
                else if (fld[i].ftype == "string")
                {
                    fld[i].datatype = "opp_string";
                    fld[i].argtype = "const char *";
                    fld[i].tostring = "oppstring2string";
                    fld[i].fromstring = "opp_string";
                    if (fld[i].fval=="")
                        fld[i].fval = "\"\"";
                }
                else
                {
                    INTERNAL_ERROR1(node, "unknown data type '%s' (is it struct?)", fld[i].ftype.c_str());
                    // is following needed here?
                    fld[i].datatype = fld[i].ftype;
                    fld[i].argtype = fld[i].ftype;
                    fld[i].tostring = "";
                    fld[i].fromstring = "";
                    if (fld[i].fval != "")
                        fld[i].fval = "0";
                }
            }
            else if (fld[i].fkind == FIELDTYPE_SPECIAL)
            {
                // ...
            }
            else
            {
                INTERNAL_ERROR1(node,"prepareForCodeGeneration(): invalid fieldtype %d", fld[i].fkind);
            }
        }
    } // if (fieldsnode)
}

void NEDCppGenerator::generateClass(NEDCppGenerator::ClassDesc& cld, NEDCppGenerator::FieldDesc *&fld, int numfields)
{
    int i;

    //
    // Header
    //
    if (cld.msgbaseclass == "")
        outh << "class " << cld.msgclass << "\n";
    else
        outh << "class " << cld.msgclass << " : public " << cld.msgbaseclass << "\n";
    outh << "{\n";
    outh << "  protected:\n";

    for (i=0; i<numfields; i++)
    {
        if (!fld[i].fisabstract)
        {
            if (fld[i].fisarray && fld[i].farraysize != "")
            {
                outh << "    " << fld[i].datatype << " " << fld[i].var << "[" << fld[i].farraysize << "];\n";
            }
            else if (fld[i].fisarray && fld[i].farraysize == "")
            {
                outh << "    " << fld[i].datatype << " *" << fld[i].var << "; // array ptr\n";
                outh << "    unsigned int " << fld[i].varsize << ";\n";
            }
            else
            {
                outh << "    " << fld[i].datatype << " " << fld[i].var << ";\n";
            }
        }
    }

    // ctor declaration
    if (cld.usegap)
        outh << "    // make constructors protected to avoid instantiation\n";
    else
        outh << "  public:\n";

    if (cld.classtype == CLASSTYPE_COBJECT)
        outh << "    " << cld.msgclass << "(const char *name=NULL);\n";
    else
        outh << "    " << cld.msgclass << "();\n";
    outh << "    " << cld.msgclass << "(const " << cld.msgclass << "& other);\n";

    // dtor declaration
    if (cld.usegap)
    {
        outh << "  public:\n";
    }
    outh << "    virtual ~" << cld.msgclass << "();\n";
    if (cld.classtype == CLASSTYPE_COBJECT)
    {
        outh << "    virtual const char *className() const {return \"" << cld.realmsgclass << "\";}\n";
    }

    // operator=, dup
    outh << "    " << cld.msgclass << "& operator=(const " << cld.msgclass << "& other);\n";
    if (cld.classtype == CLASSTYPE_COBJECT && !cld.usegap)
    {
        outh << "    virtual cPolymorphic *dup() const {return new " << cld.msgclass << "(*this);}\n";
    }
    outh << "\n";

    // field getters/setters
    outh << "    // field getter/setter methods\n";
    for (i=0; i<numfields; i++)
    {
        const char *pure = fld[i].fisabstract ? " = 0" : "";
        if (fld[i].fisarray && fld[i].farraysize != "")
        {
            outh << "    virtual unsigned int " << fld[i].getsize << "() const" << pure << ";\n";
            outh << "    virtual " << fld[i].argtype << " " << fld[i].getter << "(unsigned int k) const" << pure << ";\n";
            outh << "    virtual void " << fld[i].setter << "(unsigned int k, " << fld[i].argtype << " " << fld[i].var << ")" << pure << ";\n";
        }
        else if (fld[i].fisarray && fld[i].farraysize == "")
        {
            outh << "    virtual void " << fld[i].alloc << "(unsigned int size)" << pure << ";\n";
            outh << "    virtual unsigned int " << fld[i].getsize << "() const" << pure << ";\n";
            outh << "    virtual " << fld[i].argtype << " " << fld[i].getter << "(unsigned int k) const" << pure << ";\n";
            outh << "    virtual void " << fld[i].setter << "(unsigned int k, " << fld[i].argtype << " " << fld[i].var << ")" << pure << ";\n";
        }
        else
        {
            outh << "    virtual " << fld[i].argtype << " " << fld[i].getter << "() const" << pure << ";\n";
            outh << "    virtual void " << fld[i].setter << "(" << fld[i].argtype << " " << fld[i].var << ")" << pure << ";\n";
        }
    }
    outh << "};\n\n";

    // example for generation gap
    if (cld.usegap)
    {
        outh << "/*\n";
        outh << "* The minimum code to be written for " << cld.realmsgclass << ":\n";
        outh << "* (methods that cannot be inherited from " << cld.msgclass << ")\n";
        outh << "*\n";
        outh << "* class " << cld.realmsgclass << " : public " << cld.msgclass << "\n";
        outh << "* {\n";
        outh << "*   public:\n";
        if (cld.classtype == CLASSTYPE_COBJECT)
            outh << "*     " << cld.realmsgclass << "(const char *name=NULL) : " << cld.msgclass << "(name) {}\n";
        else
            outh << "*     " << cld.realmsgclass << "() : " << cld.msgclass << "() {}\n";
        outh << "*     " << cld.realmsgclass << "(const " << cld.realmsgclass << "& other) : " << cld.msgclass << "(other) {}\n";
        outh << "*     " << cld.realmsgclass << "& operator=(const " << cld.realmsgclass << "& other) {" << cld.msgclass << "::operator=(other); return *this;}\n";
        if (cld.classtype == CLASSTYPE_COBJECT)
            outh << "*     virtual cPolymorphic *dup() {return new " << cld.realmsgclass << "(*this);}\n";
        outh << "* };\n";
        if (cld.classtype == CLASSTYPE_COBJECT)
            outh << "* Register_Class(" << cld.realmsgclass << ");\n";
        outh << "*/\n\n";
    }

    //
    // Implementation
    //
    if (cld.classtype == CLASSTYPE_COBJECT && !cld.usegap)
        out << "Register_Class(" << cld.msgclass << ");\n\n";

    // ctor
    if (cld.classtype == CLASSTYPE_COBJECT)
    {
        if (cld.msgbaseclass == "")
            out << cld.msgclass << "::" << cld.msgclass << "(const char *name)\n";
        else
            out << cld.msgclass << "::" << cld.msgclass << "(const char *name) : " << cld.msgbaseclass << "(name)\n";
    }
    else
    {
        if (cld.msgbaseclass == "")
            out << cld.msgclass << "::" << cld.msgclass << "()\n";
        else
            out << cld.msgclass << "::" << cld.msgclass << "() : " << cld.msgbaseclass << "()\n";
    }
    out << "{\n";
    for (i=0; i<numfields; i++)
    {
        if (!fld[i].fisabstract)
        {
            if (fld[i].fisarray && fld[i].farraysize != "")
            {
                if (fld[i].fval != "")
                {
                    out << "    for (int i=0; i<" << fld[i].farraysize << "; i++)\n";
                    out << "        " << fld[i].var << "[i] = 0;\n";
                }
                if (fld[i].classtype == CLASSTYPE_COBJECT)
                {
                    out << "    for (int i=0; i<" << fld[i].farraysize << "; i++)\n";
                    out << "        " << fld[i].var << "[i].setOwner(this);\n";
                }
            }
            else if (fld[i].fisarray && fld[i].farraysize == "")
            {
                out << "    " << fld[i].varsize << " = 0;\n";
                out << "    " << fld[i].var << " = 0;\n";
            }
            else
            {
                if (fld[i].fval != "")
                {
                    out << "    " << fld[i].var << " = " << fld[i].fval << ";\n";
                }
                if (fld[i].classtype == CLASSTYPE_COBJECT) // ?
                {
                    out << "    " << fld[i].var << ".setOwner(this);\n";
                }
            }
        }
    }
    out << "}\n\n";

    // copy ctor
    if (cld.msgbaseclass == "")
    {
        out << cld.msgclass << "::" << cld.msgclass << "(const " << cld.msgclass << "& other)\n";
    }
    else
    {
        out << cld.msgclass << "::" << cld.msgclass << "(const " << cld.msgclass << "& other) : " << cld.msgbaseclass << "()\n";
    }
    out << "{\n";
    if (cld.classtype == CLASSTYPE_COBJECT)
    {
        out << "    setName(other.name());\n";
    }
    for (i=0; i<numfields; i++)
    {
        if (!fld[i].fisabstract)
        {
            if (fld[i].fisarray && fld[i].farraysize != "")
            {
                if (fld[i].classtype == CLASSTYPE_COBJECT)
                {
                    out << "    for (int i=0; i<" << fld[i].farraysize << "; i++)\n";
                    out << "        " << fld[i].var << "[i].setOwner(this);\n";
                }
            }
            else if (fld[i].fisarray && fld[i].farraysize == "")
            {
                out << "    " << fld[i].varsize << " = 0;\n";
                out << "    " << fld[i].var << " = 0;\n";
            }
            else if (!fld[i].fisarray && fld[i].classtype == CLASSTYPE_COBJECT)
            {
                out << "    " << fld[i].var << ".setOwner(this);\n";
            }
        }
    }
    out << "    operator=(other);\n";
    out << "}\n\n";

    // dtor
    out << cld.msgclass << "::~" << cld.msgclass << "()\n";
    out << "{\n";
    for (i=0; i<numfields; i++)
        if (!fld[i].fisabstract && fld[i].fisarray && fld[i].farraysize == "")
            out << "    delete [] " << fld[i].var << ";\n";
    out << "}\n\n";

    // operator=
    out << cld.msgclass << "& " << cld.msgclass << "::operator=(const " << cld.msgclass << "& other)\n";
    out << "{\n";
    out << "    if (this==&other) return *this;\n";
    if (cld.msgbaseclass != "")
        out << "    " << cld.msgbaseclass << "::operator=(other);\n";
    for (i=0; i<numfields; i++)
    {
        if (!fld[i].fisabstract)
        {
            if (fld[i].fisarray && fld[i].farraysize != "")
            {
                out << "    for (int i=0; i<" << fld[i].farraysize << "; i++)\n";
                out << "        " << fld[i].var << "[i] = other." << fld[i].var << "[i];\n";
            }
            else if (fld[i].fisarray && fld[i].farraysize == "")
            {
                out << "    delete [] " << fld[i].var << ";\n";
                out << "    " << fld[i].var << " = new " << fld[i].datatype << "[other." << fld[i].varsize << "];\n";
                out << "    " << fld[i].varsize << " = other." << fld[i].varsize << ";\n";
                out << "    for (unsigned int i=0; i<" << fld[i].varsize << "; i++)\n";
                out << "        " << fld[i].var << "[i] = other." << fld[i].var << "[i];\n";
            }
            else
            {
                out << "    " << fld[i].var << " = other." << fld[i].var << ";\n";
            }
        }
    }
    out << "    return *this;\n";
    out << "}\n\n";

    // field getters/setters
    for (i=0; i<numfields; i++)
    {
        if (!fld[i].fisabstract)
        {
            if (fld[i].fisarray && fld[i].farraysize != "")
            {
                out << "unsigned int " << cld.msgclass << "::" << fld[i].getsize << "() const\n";
                out << "{\n";
                out << "    return " << fld[i].farraysize << ";\n";
                out << "}\n\n";
                out << fld[i].argtype << " " << cld.msgclass << "::" << fld[i].getter << "(unsigned int k) const\n";
                out << "{\n";
                out << "    if (k>=" << fld[i].farraysize << ") opp_error(\"Array of size " << fld[i].farraysize << " indexed by %d\", k);\n";
                out << "    return " << fld[i].var << "[k];\n";
                out << "}\n\n";
                out << "void " << cld.msgclass << "::" << fld[i].setter << "(unsigned int k, " << fld[i].argtype << " " << fld[i].var << ")\n";
                out << "{\n";
                out << "    if (k>=" << fld[i].farraysize << ") opp_error(\"Array of size " << fld[i].farraysize << " indexed by %d\", k);\n";
                out << "    this->" << fld[i].var << "[k] = " << fld[i].var << ";\n";
                out << "}\n\n";
            }
            else if (fld[i].fisarray && fld[i].farraysize == "")
            {
                out << "void " << cld.msgclass << "::" << fld[i].alloc << "(unsigned int size)\n";
                out << "{\n";
                out << "    " << fld[i].datatype << " *" << fld[i].var << "2 = new " << fld[i].datatype << "[size];\n";
                out << "    unsigned int sz = " << fld[i].varsize << " > size ? " << fld[i].varsize << " : size;\n";
                out << "    for (unsigned int i=0; i<sz; i++)\n";
                out << "        " << fld[i].var << "2[i] = " << fld[i].var << "[i];\n";
                out << "    for (unsigned int i=sz; i<size; i++)\n";
                out << "        " << fld[i].var << "2[i] = 0;\n";
                out << "    " << fld[i].varsize << " = size;\n";
                out << "    delete [] " << fld[i].var << ";\n";
                out << "    " << fld[i].var << " = " << fld[i].var << "2;\n";
                out << "}\n\n";
                out << "unsigned int " << cld.msgclass << "::" << fld[i].getsize << "() const\n";
                out << "{\n";
                out << "    return " << fld[i].varsize << ";\n";
                out << "}\n\n";
                out << fld[i].argtype << " " << cld.msgclass << "::" << fld[i].getter << "(unsigned int k) const\n";
                out << "{\n";
                out << "    if (k>=" << fld[i].varsize << ") opp_error(\"Array of size " << fld[i].varsize << " indexed by %d, k\");\n";
                out << "    return " << fld[i].var << "[k];\n";
                out << "}\n\n";
                out << "void " << cld.msgclass << "::" << fld[i].setter << "(unsigned int k, " << fld[i].argtype << " " << fld[i].var << ")\n";
                out << "{\n";
                out << "    if (k>=" << fld[i].varsize << ") opp_error(\"Array of size " << fld[i].varsize << " indexed by %d, k\");\n";
                out << "    this->" << fld[i].var << "[k]=" << fld[i].var << ";\n";
                out << "}\n\n";
            }
            else
            {
                out << fld[i].argtype << " " << cld.msgclass << "::" << fld[i].getter << "() const\n";
                out << "{\n";
                out << "    return " << fld[i].var << ";\n";
                out << "}\n\n";
                out << "void " << cld.msgclass << "::" << fld[i].setter << "(" << fld[i].argtype << " " << fld[i].var << ")\n";
                out << "{\n";
                out << "    this->" << fld[i].var << " = " << fld[i].var << ";\n";
                out << "}\n\n";
            }
        }
    }
}


void NEDCppGenerator::generateStruct(NEDCppGenerator::ClassDesc& cld, NEDCppGenerator::FieldDesc *&fld, int numfields)
{
    int i;

    //
    // Struct declaration
    //
    if (cld.msgbaseclass == "")
        outh << "struct " << cld.msgclass << "\n";
    else
        outh << "struct " << cld.msgclass << " : public " << cld.msgbaseclass << "\n";
    outh << "{\n";
    for (i=0; i<numfields; i++)
    {
        if (!fld[i].fisabstract)
        {
            if (fld[i].fisarray && fld[i].farraysize != "")
            {
                outh << "    " << fld[i].datatype << " " << fld[i].var << "[" << fld[i].farraysize << "];\n";
            }
            else if (fld[i].fisarray && fld[i].farraysize == "")
            {
                outh << "    " << fld[i].datatype << " *" << fld[i].var << "; // array ptr\n";
                outh << "    unsigned int " << fld[i].varsize << ";\n";
            }
            else
            {
                outh << "    " << fld[i].datatype << " " << fld[i].var << ";\n";
            }
        }
    }
    outh << "};\n\n";

    if (cld.usegap)
    {
        outh << "/*\n";
        outh << "* You need to subclass " << cld.msgclass << " to produce " << cld.realmsgclass << ":\n";
        outh << "*\n";
        outh << "* struct " << cld.realmsgclass << " : public " << cld.msgclass << "\n";
        outh << "* {\n";
        outh << "*     // ...\n";
        outh << "* };\n";
        outh << "*/\n\n";
    }
}


void NEDCppGenerator::generateDescriptorClass(NEDCppGenerator::ClassDesc& cld, NEDCppGenerator::FieldDesc *&fld, int numfields)
{
    int i;

    // descriptor class declaration (goes to cc file)
    out << "class " << cld.msgdescclass << " : public " << cld.msgbasedescclass << "\n";
    out << "{\n";
    out << "  public:\n";
    out << "    " << cld.msgdescclass << "(void *p=NULL);\n";
    out << "    virtual ~" << cld.msgdescclass << "();\n";
    out << "    virtual const char *className() const {return \"" << cld.msgdescclass << "\";}\n";
    out << "    " << cld.msgdescclass << "& operator=(const " << cld.msgdescclass << "& other);\n";
    out << "    virtual cPolymorphic *dup() const {return new " << cld.msgdescclass << "(*this);}\n";
    out << "\n";
    out << "    virtual int getFieldCount();\n";
    out << "    virtual const char *getFieldName(int field);\n";
    out << "    virtual int getFieldType(int field);\n";
    out << "    virtual const char *getFieldTypeString(int field);\n";
    out << "    virtual const char *getFieldEnumName(int field);\n";
    out << "    virtual int getArraySize(int field);\n";
    out << "\n";
    out << "    virtual bool getFieldAsString(int field, int i, char *resultbuf, int bufsize);\n";
    out << "    virtual bool setFieldAsString(int field, int i, const char *value);\n";
    out << "\n";
    out << "    virtual const char *getFieldStructName(int field);\n";
    out << "    virtual void *getFieldStructPointer(int field, int i);\n";
    out << "    virtual sFieldWrapper *getFieldWrapper(int field, int i);\n";
    out << "};\n\n";

    // register descriptor class
    out << "Register_Class(" << cld.msgdescclass << ");\n\n";

    // ctor, dtor
    out << cld.msgdescclass << "::" << cld.msgdescclass << "(void *p) : " << cld.msgbasedescclass << "(p)\n";
    out << "{\n";
    out << "}\n";
    out << "\n";

    out << cld.msgdescclass << "::~" << cld.msgdescclass << "()\n";
    out << "{\n";
    out << "}\n";
    out << "\n";

    // getFieldCount()
    out << "int " << cld.msgdescclass << "::getFieldCount()\n";
    out << "{\n";
    if (cld.hasbasedescriptor)
        out << "    return " << cld.msgbasedescclass << "::getFieldCount() + " << numfields << ";\n";
    else
        out << "    return " << numfields << ";\n";
    out << "}\n";
    out << "\n";

    // getFieldType()
    out << "int " << cld.msgdescclass << "::getFieldType(int field)\n";
    out << "{\n";
    if (cld.hasbasedescriptor)
    {
        out << "    if (field < " << cld.msgbasedescclass << "::getFieldCount())\n";
        out << "        return " << cld.msgbasedescclass << "::getFieldType(field);\n";
        out << "    field -= " << cld.msgbasedescclass << "::getFieldCount();\n";
    }
    out << "    switch (field) {\n";
    for (i=0; i<numfields; i++)
    {
        const char *arr;
        if (fld[i].fisarray)
            arr = "_ARRAY";
        else
            arr = "";

        if (fld[i].fkind == FIELDTYPE_BASIC)
            out << "        case " << i << ": return FT_BASIC" << arr << ";\n";
        else if (fld[i].fkind == FIELDTYPE_STRUCT)
            out << "        case " << i << ": return FT_STRUCT" << arr << ";\n";
        else if (fld[i].fkind == FIELDTYPE_SPECIAL)
            out << "        case " << i << ": return FT_SPECIAL" << arr << ";\n";
        else
            INTERNAL_ERROR1(NULL,"generateDescriptorClass(): invalid fieldtype %d", fld[i].fkind);
    }
    out << "        default: return FT_INVALID;\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";

    // getFieldName()
    out << "const char *" << cld.msgdescclass << "::getFieldName(int field)\n";
    out << "{\n";
    if (cld.hasbasedescriptor)
    {
        out << "    if (field < " << cld.msgbasedescclass << "::getFieldCount())\n";
        out << "        return " << cld.msgbasedescclass << "::getFieldName(field);\n";
        out << "    field -= " << cld.msgbasedescclass << "::getFieldCount();\n";
    }
    out << "    switch (field) {\n";
    for (i=0; i<numfields; i++)
        out << "        case " << i << ": return \"" << fld[i].fieldname << "[" << i << "]\";\n";
    out << "        default: return NULL;\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";

    // getFieldTypeString()
    out << "const char *" << cld.msgdescclass << "::getFieldTypeString(int field)\n";
    out << "{\n";
    if (cld.hasbasedescriptor)
    {
        out << "    if (field < " << cld.msgbasedescclass << "::getFieldCount())\n";
        out << "        return " << cld.msgbasedescclass << "::getFieldTypeString(field);\n";
        out << "    field -= " << cld.msgbasedescclass << "::getFieldCount();\n";
    }
    out << "    switch (field) {\n";
    for (i=0; i<numfields; i++)
        out << "        case " << i << ": return \"" << fld[i].ftype << "\";\n";
    out << "        default: return NULL;\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";

    // getFieldEnumName()
    out << "const char *" << cld.msgdescclass << "::getFieldEnumName(int field)\n";
    out << "{\n";
    if (cld.hasbasedescriptor)
    {
        out << "    if (field < " << cld.msgbasedescclass << "::getFieldCount())\n";
        out << "        return " << cld.msgbasedescclass << "::getFieldEnumName(field);\n";
        out << "    field -= " << cld.msgbasedescclass << "::getFieldCount();\n";
    }
    out << "    switch (field) {\n";
    for (i=0; i<numfields; i++)
        if (fld[i].fenumname != "")
            out << "        case " << i << ": return \"" << fld[i].fenumname << "\";\n";
    out << "        default: return NULL;\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";

    // getArraySize()
    out << "int " << cld.msgdescclass << "::getArraySize(int field)\n";
    out << "{\n";
    if (cld.hasbasedescriptor)
    {
        out << "    if (field < " << cld.msgbasedescclass << "::getFieldCount())\n";
        out << "        return " << cld.msgbasedescclass << "::getArraySize(field);\n";
        out << "    field -= " << cld.msgbasedescclass << "::getFieldCount();\n";
    }
    out << "    " << cld.msgclass << " *pp = (" << cld.msgclass << " *)p;\n";
    out << "    switch (field) {\n";
    for (i=0; i<numfields; i++)
    {
        if (fld[i].fisarray)
        {
            if (fld[i].farraysize != "")
                out << "        case " << i << ": return " << fld[i].farraysize << ";\n";
            else if (cld.classtype == CLASSTYPE_STRUCT)
                out << "        case " << i << ": return pp->" << fld[i].varsize << ";\n";
            else
                out << "        case " << i << ": return pp->" << fld[i].getsize << "();\n";
        }
    }
    out << "        default: return 0;\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";

    // getFieldAsString()
    out << "bool " << cld.msgdescclass << "::getFieldAsString(int field, int i, char *resultbuf, int bufsize)\n";
    out << "{\n";
    if (cld.hasbasedescriptor)
    {
        out << "    if (field < " << cld.msgbasedescclass << "::getFieldCount())\n";
        out << "        return " << cld.msgbasedescclass << "::getFieldAsString(field,i,resultbuf,bufsize);\n";
        out << "    field -= " << cld.msgbasedescclass << "::getFieldCount();\n";
    }
    out << "    " << cld.msgclass << " *pp = (" << cld.msgclass << " *)p;\n";
    out << "    switch (field) {\n";
    for (i=0; i<numfields; i++)
    {
        if (fld[i].fkind == FIELDTYPE_BASIC)
        {
            if (cld.classtype == CLASSTYPE_STRUCT)
            {
                if (fld[i].fisarray)
                {
                    if (fld[i].farraysize != "")
                        out << "        case " << i << ": if (i>=" << fld[i].farraysize << ") return false;\n";
                    else
                        out << "        case " << i << ": if (i>=pp->" << fld[i].varsize << ") return false;\n";
                    out << "                " << fld[i].tostring << "(pp->" << fld[i].var << "[i],resultbuf,bufsize); return true;\n";
                }
                else
                {
                    out << "        case " << i << ": " << fld[i].tostring << "(pp->" << fld[i].var << ",resultbuf,bufsize); return true;\n";
                }
            }
            else
            {
                if (fld[i].fisarray)
                    out << "        case " << i << ": " << fld[i].tostring << "(pp->" << fld[i].getter << "(i),resultbuf,bufsize); return true;\n";
                else
                    out << "        case " << i << ": " << fld[i].tostring << "(pp->" << fld[i].getter << "(),resultbuf,bufsize); return true;\n";
            }
        }
        else if (fld[i].fkind == FIELDTYPE_STRUCT)
        {
            out << "        case " << i << ": return false;\n";
        }
        else if (fld[i].fkind == FIELDTYPE_SPECIAL)
        {
            out << "        case " << i << ": return false; //TBD!!!\n";
        }
        else
        {
            INTERNAL_ERROR1(NULL,"generateDescriptorClass(): invalid fieldtype %d", fld[i].fkind);
        }
    }
    out << "        default: return false;\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";

    // setFieldAsString()
    out << "bool " << cld.msgdescclass << "::setFieldAsString(int field, int i, const char *value)\n";
    out << "{\n";
    if (cld.hasbasedescriptor)
    {
        out << "    if (field < " << cld.msgbasedescclass << "::getFieldCount())\n";
        out << "        return " << cld.msgbasedescclass << "::setFieldAsString(field,i,value);\n";
        out << "    field -= " << cld.msgbasedescclass << "::getFieldCount();\n";
    }
    out << "    " << cld.msgclass << " *pp = (" << cld.msgclass << " *)p;\n";
    out << "    switch (field) {\n";
    for (i=0; i<numfields; i++)
    {
        if (fld[i].fkind == FIELDTYPE_BASIC)
        {
            if (cld.classtype == CLASSTYPE_STRUCT)
            {
                if (fld[i].fisarray)
                {
                    if (fld[i].farraysize != "")
                        out << "        case " << i << ": if (i>=" << fld[i].farraysize << ") return false;\n";
                    else
                        out << "        case " << i << ": if (i>=pp->" << fld[i].varsize << ") return false;\n";
                    out << "                pp->" << fld[i].var << "[i] = " << fld[i].fromstring << "(value); return true;\n";
                }
                else
                {
                    out << "        case " << i << ": pp->" << fld[i].var << " = " << fld[i].fromstring << "(value); return true;\n";
                }
            }
            else
            {
                if (fld[i].fisarray)
                    out << "        case " << i << ": pp->" << fld[i].setter << "(i," << fld[i].fromstring << "(value)); return true;\n";
                else
                    out << "        case " << i << ": pp->" << fld[i].setter << "(" << fld[i].fromstring << "(value)); return true;\n";
            }
        }
        else if (fld[i].fkind == FIELDTYPE_STRUCT)
        {
            out << "        case " << i << ": return false;\n";
        }
        else if (fld[i].fkind == FIELDTYPE_SPECIAL)
        {
            out << "        case " << i << ": return false; //TBD!!!\n";
        }
        else
        {
            INTERNAL_ERROR1(NULL,"generateDescriptorClass(): invalid fieldtype %d", fld[i].fkind);
        }
    }
    out << "        default: return false;\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";

    // getFieldStructName()
    out << "const char *" << cld.msgdescclass << "::getFieldStructName(int field)\n";
    out << "{\n";
    if (cld.hasbasedescriptor)
    {
        out << "    if (field < " << cld.msgbasedescclass << "::getFieldCount())\n";
        out << "        return " << cld.msgbasedescclass << "::getFieldStructName(field);\n";
        out << "    field -= " << cld.msgbasedescclass << "::getFieldCount();\n";
    }
    out << "    switch (field) {\n";
    for (i=0; i<numfields; i++)
        if (fld[i].fkind == FIELDTYPE_STRUCT)
            out << "        case " << i << ": return \"" << fld[i].ftype << "\"; break;\n";
    out << "        default: return NULL;\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";

    // getFieldStructPointer()
    out << "void *" << cld.msgdescclass << "::getFieldStructPointer(int field, int i)\n";
    out << "{\n";
    if (cld.hasbasedescriptor)
    {
        out << "    if (field < " << cld.msgbasedescclass << "::getFieldCount())\n";
        out << "        return " << cld.msgbasedescclass << "::getFieldStructPointer(field, i);\n";
        out << "    field -= " << cld.msgbasedescclass << "::getFieldCount();\n";
    }
    out << "    " << cld.msgclass << " *pp = (" << cld.msgclass << " *)p;\n";
    out << "    switch (field) {\n";
    for (i=0; i<numfields; i++)
    {
        if (fld[i].fkind == FIELDTYPE_STRUCT)
        {
            if (cld.classtype == CLASSTYPE_STRUCT)
            {
                if (fld[i].fisarray)
                    out << "        case " << i << ": return (void *)&" << fld[i].var << "[i]; break;\n";
                else
                    out << "        case " << i << ": return (void *)&" << fld[i].var << "; break;\n";
            }
            else
            {
                if (fld[i].fisarray)
                    out << "        case " << i << ": return (void *)&pp->" << fld[i].getter << "(i); break;\n";
                else
                    out << "        case " << i << ": return (void *)&pp->" << fld[i].getter << "(); break;\n";
            }
        }
    }
    out << "        default: return NULL;\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";

    // getFieldWrapper()
    out << "sFieldWrapper *" << cld.msgdescclass << "::getFieldWrapper(int field, int i)\n";
    out << "{\n";
    out << "    return NULL;\n";
    out << "}\n\n";
}


