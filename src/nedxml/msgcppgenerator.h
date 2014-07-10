//==========================================================================
//  MSGCPPGENERATOR.H - part of
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

#ifndef __MSGCPPGENERATOR_H
#define __MSGCPPGENERATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "nedelements.h"
#include "nederror.h"

NAMESPACE_BEGIN

/**
 * Generates C++ code from a MSG file object tree.
 * Assumes object tree has already passed all validation stages (DTD, syntax, semantic).
 *
 * @ingroup NEDGenerator
 */
class NEDXML_API MsgCppGenerator
{
  protected:
    std::ostream *hOutp;
    std::ostream *ccOutp;
    NEDErrorStore *errors;

  protected:
    typedef std::map<std::string, std::string> Properties;

    class FieldInfo {
        std::string ftype;
        std::string ftypeqname;
        std::string fval;
        bool fisabstract;
        bool fispointer;
        bool fisarray;
        std::string farraysize;
        std::string fsizetype;
        Properties fprops;

        std::string fkind;
        std::string datatype;
        std::string argtype;
        std::string rettype;
        std::string var;
        std::string argname;
        std::string varsize;
        std::string getter;
        std::string setter;
        std::string alloc;
        std::string getsize;
        std::string tostring;
        std::string fromstring;
        bool maybe_c_str;
    };

    class ClassInfo {
        std::string keyword;
        std::string classtype;
        bool gap;
        bool omitgetverb;
        std::string msgclass;
        std::string realmsgclass;
        std::string msgbaseclass;
        std::string msgdescclass;
        std::vector<FieldInfo> fieldlist;
        Properties props;
    };

  protected:
    ClassInfo prepareForCodeGeneration(NEDElement *node); // accepts StructElement, ClassElement, MessageElement, PacketElement
    void generateClass(const ClassInfo& classInfo);
    void generateStruct(const ClassInfo& classInfo);
    void generateDescriptorClass(const ClassInfo& a);

  public:
    /**
     * Constructor.
     */
    MsgCppGenerator(NEDErrorStore *errors);

    /**
     * Destructor.
     */
    ~MsgCppGenerator();

    /**
     * Generates C++ code from the specified message file. Assumes object tree has already
     * passed DTD and syntax validation.
     */
    void generate(MsgFileElement *fileElement, std::ostream& hOut, std::ostream& ccOut);

    /**
     * TODO
     */
    void generate(MsgFileElement *fileElement, const char *hFile, const char *ccFile);
};

NAMESPACE_END


#endif


