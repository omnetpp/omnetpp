//==========================================================================
// Part of the OMNeT++/OMNEST Discrete Event Simulation System
//
// Generated from ned.dtd by dtdclassgen.pl
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

//
// THIS IS A GENERATED FILE, DO NOT EDIT!
//


#include <stdio.h>
#include "nederror.h"
#include "neddtdvalidator.h"

void NEDDTDValidator::validateElement(FilesNode *node)
{
    int tags[] = {NED_NED_FILE,NED_MSG_FILE, NED_NULL};
    checkChoice(node, tags, '*');

}

void NEDDTDValidator::validateElement(NedFileNode *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_IMPORT, NED_PROPERTY_DECL, NED_PROPERTY, NED_CHANNEL, NED_CHANNEL_INTERFACE, NED_SIMPLE_MODULE, NED_COMPOUND_MODULE, NED_MODULE_INTERFACE, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkRequiredAttribute(node, "filename");
}

void NEDDTDValidator::validateElement(CommentNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "locid");
    checkNMTokenAttribute(node, "locid");
}

void NEDDTDValidator::validateElement(ImportNode *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "import-spec");
}

void NEDDTDValidator::validateElement(PropertyDeclNode *node)
{
    int tags[] = {NED_COMMENT,NED_PROPERTY_KEY,NED_PROPERTY, NED_NULL};
    char mult[] = {'*','*','*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    const char *vals1[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-array", vals1, sizeof(vals1)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(ExtendsNode *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(InterfaceNameNode *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(SimpleModuleNode *node)
{
    int tags[] = {NED_COMMENT,NED_EXTENDS,NED_INTERFACE_NAME,NED_PARAMETERS,NED_GATES, NED_NULL};
    char mult[] = {'*','?','*','?','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    const char *vals1[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-network", vals1, sizeof(vals1)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(ModuleInterfaceNode *node)
{
    int tags[] = {NED_COMMENT,NED_EXTENDS,NED_PARAMETERS,NED_GATES, NED_NULL};
    char mult[] = {'*','*','?','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(CompoundModuleNode *node)
{
    int tags[] = {NED_COMMENT,NED_EXTENDS,NED_INTERFACE_NAME,NED_PARAMETERS,NED_GATES,NED_TYPES,NED_SUBMODULES,NED_CONNECTIONS, NED_NULL};
    char mult[] = {'*','?','*','?','?','?','?','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    const char *vals1[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-network", vals1, sizeof(vals1)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(ChannelInterfaceNode *node)
{
    int tags[] = {NED_COMMENT,NED_EXTENDS,NED_PARAMETERS, NED_NULL};
    char mult[] = {'*','*','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ChannelNode *node)
{
    int tags[] = {NED_COMMENT,NED_EXTENDS,NED_INTERFACE_NAME,NED_PARAMETERS, NED_NULL};
    char mult[] = {'*','?','*','?', 0};
    checkSequence(node, tags, mult);

    const char *vals0[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-withcppclass", vals0, sizeof(vals0)/sizeof(const char *));
    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ParametersNode *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_PROPERTY, NED_PARAM, NED_PATTERN, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    const char *vals0[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-implicit", vals0, sizeof(vals0)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(ParamNode *node)
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

void NEDDTDValidator::validateElement(PatternNode *node)
{
    int tags[] = {NED_COMMENT,NED_EXPRESSION,NED_PROPERTY, NED_NULL};
    char mult[] = {'*','?','*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "pattern");
    const char *vals2[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-default", vals2, sizeof(vals2)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(PropertyNode *node)
{
    int tags[] = {NED_COMMENT,NED_PROPERTY_KEY, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

    const char *vals0[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-implicit", vals0, sizeof(vals0)/sizeof(const char *));
    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNMTokenAttribute(node, "index");
}

void NEDDTDValidator::validateElement(PropertyKeyNode *node)
{
    int tags[] = {NED_COMMENT,NED_LITERAL, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(GatesNode *node)
{
    int tags[] = {NED_COMMENT,NED_GATE, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

}

void NEDDTDValidator::validateElement(GateNode *node)
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

void NEDDTDValidator::validateElement(TypesNode *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_CHANNEL, NED_CHANNEL_INTERFACE, NED_SIMPLE_MODULE, NED_COMPOUND_MODULE, NED_MODULE_INTERFACE, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

}

void NEDDTDValidator::validateElement(SubmodulesNode *node)
{
    int tags[] = {NED_COMMENT,NED_SUBMODULE, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

}

void NEDDTDValidator::validateElement(SubmoduleNode *node)
{
    int tags[] = {NED_COMMENT,NED_EXPRESSION,NED_PARAMETERS,NED_GATES, NED_NULL};
    char mult[] = {'*','*','?','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNMTokenAttribute(node, "type");
    checkNMTokenAttribute(node, "like-type");
}

void NEDDTDValidator::validateElement(ConnectionsNode *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_CONNECTION, NED_CONNECTION_GROUP, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    const char *vals0[] = {"true","false"};
    checkEnumeratedAttribute(node, "allow-unconnected", vals0, sizeof(vals0)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(ConnectionNode *node)
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

void NEDDTDValidator::validateElement(ChannelSpecNode *node)
{
    int tags[] = {NED_COMMENT,NED_EXPRESSION,NED_PARAMETERS, NED_NULL};
    char mult[] = {'*','*','?', 0};
    checkSequence(node, tags, mult);

    checkNMTokenAttribute(node, "type");
    checkNMTokenAttribute(node, "like-type");
}

void NEDDTDValidator::validateElement(ConnectionGroupNode *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_LOOP, NED_CONDITION, NED_NULL}, '*'},
        {{NED_CONNECTION, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

}

void NEDDTDValidator::validateElement(LoopNode *node)
{
    int tags[] = {NED_COMMENT,NED_EXPRESSION, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "param-name");
    checkNameAttribute(node, "param-name");
}

void NEDDTDValidator::validateElement(ConditionNode *node)
{
    int tags[] = {NED_COMMENT,NED_EXPRESSION, NED_NULL};
    char mult[] = {'*','?', 0};
    checkSequence(node, tags, mult);

}

void NEDDTDValidator::validateElement(ExpressionNode *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_OPERATOR, NED_FUNCTION, NED_IDENT, NED_LITERAL, NED_NULL}, '1'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

}

void NEDDTDValidator::validateElement(OperatorNode *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_OPERATOR, NED_FUNCTION, NED_IDENT, NED_LITERAL, NED_NULL}, '+'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkRequiredAttribute(node, "name");
}

void NEDDTDValidator::validateElement(FunctionNode *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_OPERATOR, NED_FUNCTION, NED_IDENT, NED_LITERAL, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(IdentNode *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_OPERATOR, NED_FUNCTION, NED_IDENT, NED_LITERAL, NED_NULL}, '?'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(LiteralNode *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "type");
    const char *vals0[] = {"double","int","string","bool","spec"};
    checkEnumeratedAttribute(node, "type", vals0, sizeof(vals0)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(MsgFileNode *node)
{
    Choice choices[] = {
        {{NED_COMMENT, NED_NULL}, '*'},
        {{NED_CPLUSPLUS, NED_STRUCT_DECL, NED_CLASS_DECL, NED_MESSAGE_DECL, NED_ENUM_DECL, NED_ENUM, NED_MESSAGE, NED_CLASS, NED_STRUCT, NED_NULL}, '*'},
    };
    checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));

}

void NEDDTDValidator::validateElement(CplusplusNode *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "body");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(StructDeclNode *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(ClassDeclNode *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    const char *vals1[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-cobject", vals1, sizeof(vals1)/sizeof(const char *));
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(MessageDeclNode *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(EnumDeclNode *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(EnumNode *node)
{
    int tags[] = {NED_COMMENT,NED_ENUM_FIELDS, NED_NULL};
    char mult[] = {'*','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(EnumFieldsNode *node)
{
    int tags[] = {NED_COMMENT,NED_ENUM_FIELD, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(EnumFieldNode *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(MessageNode *node)
{
    int tags[] = {NED_COMMENT,NED_PROPERTIES,NED_FIELDS, NED_NULL};
    char mult[] = {'*','?','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(ClassNode *node)
{
    int tags[] = {NED_COMMENT,NED_PROPERTIES,NED_FIELDS, NED_NULL};
    char mult[] = {'*','?','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(StructNode *node)
{
    int tags[] = {NED_COMMENT,NED_PROPERTIES,NED_FIELDS, NED_NULL};
    char mult[] = {'*','?','?', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(FieldsNode *node)
{
    int tags[] = {NED_COMMENT,NED_FIELD, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(FieldNode *node)
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
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(PropertiesNode *node)
{
    int tags[] = {NED_COMMENT,NED_MSGPROPERTY, NED_NULL};
    char mult[] = {'*','*', 0};
    checkSequence(node, tags, mult);

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(MsgpropertyNode *node)
{
    int tags[] = {NED_COMMENT, NED_NULL};
    char mult[] = {'*', 0};
    checkSequence(node, tags, mult);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(UnknownNode *node)
{
    // ANY

    checkRequiredAttribute(node, "element");
    checkNMTokenAttribute(node, "element");
}

