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
#include "msgcppgenerator.h"
#include "stringutil.h"
#include "nedutil.h"

NAMESPACE_BEGIN

using std::ostream;

#define H  (*hOutp)
#define CC (*ccOutp)

MsgCppGenerator::MsgCppGenerator(NEDErrorStore *e)
{
    hOutp = ccOutp = NULL;
    errors = e;
}

MsgCppGenerator::~MsgCppGenerator()
{
}

void MsgCppGenerator::generate(MsgFileElement *fileElement, const char *hFile, const char *ccFile)
{
    std::ofstream hStream(hFile);
    std::ofstream ccStream(ccFile);
    generate(fileElement, hStream, ccStream);
    hStream.close();
    ccStream.close();
}

void MsgCppGenerator::generate(MsgFileElement *fileElement, std::ostream& hOut, std::ostream& ccOut)
{
    hOutp = &hOut;
    ccOutp = &ccOut;

    /*
       <!ELEMENT msg-file (comment*, (namespace|property-decl|property|cplusplus|
                           struct-decl|class-decl|message-decl|packet-decl|enum-decl|
                           struct|class|message|packet|enum)*)>
     */
    for (NEDElement *child = fileElement->getFirstChild(); child; child=child->getNextSibling())
    {
        switch (child->getTagCode()) {
            case NED_NAMESPACE:
                //TODO open namespace(s)
                break;

            case NED_CPLUSPLUS:
                //TODO print C++ block
                break;

            case NED_STRUCT_DECL:
            case NED_CLASS_DECL:
            case NED_MESSAGE_DECL:
            case NED_PACKET_DECL:
            case NED_ENUM_DECL:
                //TODO forward declaration -- add to table
                break;

            case NED_ENUM: {
                //TODO generate enum
                break;
            }

            case NED_STRUCT: {
                ClassInfo classInfo = prepareForCodeGeneration(child);
                generateStruct(classInfo);
                generateDescriptorClass(classInfo);
                break;
            }

            case NED_CLASS:
            case NED_MESSAGE:
            case NED_PACKET: {
                ClassInfo classInfo = prepareForCodeGeneration(child);
                generateClass(classInfo);
                generateDescriptorClass(classInfo);
                break;
            }

        }
    }
}

MsgCppGenerator::ClassInfo MsgCppGenerator::prepareForCodeGeneration(NEDElement *node)
{
    ClassInfo classInfo;
    // TODO contents of sub prepareForCodeGeneration() in opp_msgc
    return classInfo;
}

void MsgCppGenerator::generateClass(const ClassInfo& a)
{
    // TODO contents of sub generateClass() in opp_msgc
    H << "class Foo { };\n";
}

void MsgCppGenerator::generateStruct(const ClassInfo& a)
{
    // TODO contents of sub generateStruct() in opp_msgc
    H << "struct Foo { };\n";
}

void MsgCppGenerator::generateDescriptorClass(const ClassInfo& a)
{
    // TODO contents of sub generateDescriptorClass() in opp_msgc
    H << "class FooDescriptor { };\n";
}

NAMESPACE_END

