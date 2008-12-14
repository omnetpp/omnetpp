//==========================================================================
// Part of the OMNeT++/OMNEST Discrete Event Simulation System
//
// Generated from ned.dtd by dtdclassgen.pl
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

//
// THIS IS A GENERATED FILE, DO NOT EDIT!
//


#include <stdio.h>
#include "nederror.h"
#include "neddtdvalidator.h"

NAMESPACE_BEGIN

void NEDDTDValidator::validateElement(FilesElement *node)
{
    int tags[] = {NED_NED_FILE,NED_MSG_FILE, NED_NULL};
    checkChoice(node, tags, '*');

}

void NEDDTDValidator::validateElement(NedFileElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_PACKAGE, NED_IMPORT, NED_PROPERTY_DECL, NED_PROPERTY, NED_CHANNEL, NED_CHANNEL_INTERFACE, NED_SIMPLE_MODULE, NED_COMPOUND_MODULE, NED_MODULE_INTERFACE, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkRequiredAttribute(node, "filename");
}

void NEDDTDValidator::validateElement(CommentElement *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "locid");
    checkNameAttribute(node, "locid");
}

void NEDDTDValidator::validateElement(PackageElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ImportElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "import-spec");
}

void NEDDTDValidator::validateElement(PropertyDeclElement *node)
{
    int tags[] = {NED_COMMENT,NED_PROPERTY_KEY,NED_PROPERTY, NED_NULL};
    char mult[] = {'*','*','*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    const char *vals1[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-array", vals1, sizeof(vals1)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(ExtendsElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
}

void NEDDTDValidator::validateElement(InterfaceNameElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
}

void NEDDTDValidator::validateElement(SimpleModuleElement *node)
{
    int tags[] = {NED_COMMENT,NED_EXTENDS,NED_INTERFACE_NAME,NED_PARAMETERS,NED_GATES, NED_NULL};
    char mult[] = {'*','?','*','?','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ModuleInterfaceElement *node)
{
    int tags[] = {NED_COMMENT,NED_EXTENDS,NED_PARAMETERS,NED_GATES, NED_NULL};
    char mult[] = {'*','*','?','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(CompoundModuleElement *node)
{
    int tags[] = {NED_COMMENT,NED_EXTENDS,NED_INTERFACE_NAME,NED_PARAMETERS,NED_GATES,NED_TYPES,NED_SUBMODULES,NED_CONNECTIONS, NED_NULL};
    char mult[] = {'*','?','*','?','?','?','?','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ChannelInterfaceElement *node)
{
    int tags[] = {NED_COMMENT,NED_EXTENDS,NED_PARAMETERS, NED_NULL};
    char mult[] = {'*','*','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ChannelElement *node)
{
    int tags[] = {NED_COMMENT,NED_EXTENDS,NED_INTERFACE_NAME,NED_PARAMETERS, NED_NULL};
    char mult[] = {'*','?','*','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ParametersElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_PROPERTY, NED_PARAM, NED_PATTERN, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    const char *vals0[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-implicit", vals0, sizeof(vals0)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(ParamElement *node)
{
    int tags[] = {NED_COMMENT,NED_EXPRESSION,NED_PROPERTY, NED_NULL};
    char mult[] = {'*','?','*', 0};
    checkSequence(node, tags, mult);

    const char *vals0[] = {"double","int","string","bool","xml",""};
    checkEnumeratedAttribute(node, "type", vals0, sizeof(vals0)/sizeof(const char *));
    const char *vals1[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-volatile", vals1, sizeof(vals1)/sizeof(const char *));
    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    const char *vals4[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-default", vals4, sizeof(vals4)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(PatternElement *node)
{
    int tags[] = {NED_COMMENT,NED_EXPRESSION,NED_PROPERTY, NED_NULL};
    char mult[] = {'*','?','*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "pattern");
    const char *vals2[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-default", vals2, sizeof(vals2)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(PropertyElement *node)
{
    int tags[] = {NED_COMMENT,NED_PROPERTY_KEY, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

    const char *vals0[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-implicit", vals0, sizeof(vals0)/sizeof(const char *));
    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "index");
}

void NEDDTDValidator::validateElement(PropertyKeyElement *node)
{
    int tags[] = {NED_COMMENT,NED_LITERAL, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

}

void NEDDTDValidator::validateElement(GatesElement *node)
{
    int tags[] = {NED_COMMENT,NED_GATE, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

}

void NEDDTDValidator::validateElement(GateElement *node)
{
    int tags[] = {NED_COMMENT,NED_EXPRESSION,NED_PROPERTY, NED_NULL};
    char mult[] = {'*','?','*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    const char *vals1[] = {"input","output","inout",""};
    checkEnumeratedAttribute(node, "type", vals1, sizeof(vals1)/sizeof(const char *));
    const char *vals2[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-vector", vals2, sizeof(vals2)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(TypesElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_CHANNEL, NED_CHANNEL_INTERFACE, NED_SIMPLE_MODULE, NED_COMPOUND_MODULE, NED_MODULE_INTERFACE, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

}

void NEDDTDValidator::validateElement(SubmodulesElement *node)
{
    int tags[] = {NED_COMMENT,NED_SUBMODULE, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

}

void NEDDTDValidator::validateElement(SubmoduleElement *node)
{
    int tags[] = {NED_COMMENT,NED_EXPRESSION,NED_PARAMETERS,NED_GATES, NED_NULL};
    char mult[] = {'*','*','?','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ConnectionsElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_CONNECTION, NED_CONNECTION_GROUP, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    const char *vals0[] = {"true","false"};
    checkEnumeratedAttribute(node, "allow-unconnected", vals0, sizeof(vals0)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(ConnectionElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_EXPRESSION, NED_NULL}, '*'},
        {{NED_CHANNEL_SPEC, NED_NULL}, '?'},
        {{NED_LOOP, NED_CONDITION, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkNameAttribute(node, "src-module");
    checkRequiredAttribute(node, "src-gate");
    checkNameAttribute(node, "src-gate");
    const char *vals3[] = {"true","false"};
    checkEnumeratedAttribute(node, "src-gate-plusplus", vals3, sizeof(vals3)/sizeof(const char *));
    const char *vals5[] = {"i","o",""};
    checkEnumeratedAttribute(node, "src-gate-subg", vals5, sizeof(vals5)/sizeof(const char *));
    checkNameAttribute(node, "dest-module");
    checkRequiredAttribute(node, "dest-gate");
    checkNameAttribute(node, "dest-gate");
    const char *vals9[] = {"true","false"};
    checkEnumeratedAttribute(node, "dest-gate-plusplus", vals9, sizeof(vals9)/sizeof(const char *));
    const char *vals11[] = {"i","o",""};
    checkEnumeratedAttribute(node, "dest-gate-subg", vals11, sizeof(vals11)/sizeof(const char *));
    checkRequiredAttribute(node, "arrow-direction");
    const char *vals12[] = {"l2r","r2l","bidir"};
    checkEnumeratedAttribute(node, "arrow-direction", vals12, sizeof(vals12)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(ChannelSpecElement *node)
{
    int tags[] = {NED_COMMENT,NED_EXPRESSION,NED_PARAMETERS, NED_NULL};
    char mult[] = {'*','*','?', 0};
    checkSequence(node, tags, mult);

}

void NEDDTDValidator::validateElement(ConnectionGroupElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_LOOP, NED_CONDITION, NED_NULL}, '*'},
        {{NED_CONNECTION, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

}

void NEDDTDValidator::validateElement(LoopElement *node)
{
    int tags[] = {NED_COMMENT,NED_EXPRESSION, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "param-name");
    checkNameAttribute(node, "param-name");
}

void NEDDTDValidator::validateElement(ConditionElement *node)
{
    int tags[] = {NED_COMMENT,NED_EXPRESSION, NED_NULL};
    char mult[] = {'*','?', 0};
    checkSequence(node, tags, mult);

}

void NEDDTDValidator::validateElement(ExpressionElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_OPERATOR, NED_FUNCTION, NED_IDENT, NED_LITERAL, NED_NULL}, '1'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

}

void NEDDTDValidator::validateElement(OperatorElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_OPERATOR, NED_FUNCTION, NED_IDENT, NED_LITERAL, NED_NULL}, '+'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkRequiredAttribute(node, "name");
}

void NEDDTDValidator::validateElement(FunctionElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_OPERATOR, NED_FUNCTION, NED_IDENT, NED_LITERAL, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(IdentElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_OPERATOR, NED_FUNCTION, NED_IDENT, NED_LITERAL, NED_NULL}, '?'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(LiteralElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "type");
    const char *vals0[] = {"double","int","string","bool","spec"};
    checkEnumeratedAttribute(node, "type", vals0, sizeof(vals0)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(MsgFileElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_NAMESPACE, NED_PROPERTY_DECL, NED_PROPERTY, NED_CPLUSPLUS, NED_STRUCT_DECL, NED_CLASS_DECL, NED_MESSAGE_DECL, NED_PACKET_DECL, NED_ENUM_DECL, NED_STRUCT, NED_CLASS, NED_MESSAGE, NED_PACKET, NED_ENUM, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

}

void NEDDTDValidator::validateElement(NamespaceElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
}

void NEDDTDValidator::validateElement(CplusplusElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "body");
}

void NEDDTDValidator::validateElement(StructDeclElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ClassDeclElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    const char *vals1[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-cobject", vals1, sizeof(vals1)/sizeof(const char *));
    checkNameAttribute(node, "extends-name");
}

void NEDDTDValidator::validateElement(MessageDeclElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(PacketDeclElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(EnumDeclElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(EnumElement *node)
{
    int tags[] = {NED_COMMENT,NED_ENUM_FIELDS, NED_NULL};
    char mult[] = {'*','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
}

void NEDDTDValidator::validateElement(EnumFieldsElement *node)
{
    int tags[] = {NED_COMMENT,NED_ENUM_FIELD, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

}

void NEDDTDValidator::validateElement(EnumFieldElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(MessageElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_PROPERTY, NED_FIELD, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
}

void NEDDTDValidator::validateElement(PacketElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_PROPERTY, NED_FIELD, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
}

void NEDDTDValidator::validateElement(ClassElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_PROPERTY, NED_FIELD, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
}

void NEDDTDValidator::validateElement(StructElement *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_PROPERTY, NED_FIELD, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
}

void NEDDTDValidator::validateElement(FieldElement *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    const char *vals2[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-abstract", vals2, sizeof(vals2)/sizeof(const char *));
    const char *vals3[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-readonly", vals3, sizeof(vals3)/sizeof(const char *));
    const char *vals4[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-vector", vals4, sizeof(vals4)/sizeof(const char *));
    checkNameAttribute(node, "enum-name");
}

void NEDDTDValidator::validateElement(UnknownElement *node)
{
    // ANY

    checkRequiredAttribute(node, "element");
}

NAMESPACE_END

