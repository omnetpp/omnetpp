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
    int tags[] = {NED_NED_FILE,NED_MSG_FILE};
    checkChoice(node, tags, sizeof(tags)/sizeof(int), '*');

}

void NEDDTDValidator::validateElement(NedFileNode *node)
{
    int tags[] = {NED_IMPORT,NED_PROPERTYDEF,NED_PROPERTY,NED_CHANNEL,NED_CHANNEL_INTERFACE,NED_SIMPLE,NED_MODULE,NED_INTERFACE};
    checkChoice(node, tags, sizeof(tags)/sizeof(int), '*');

    checkRequiredAttribute(node, "filename");
}

void NEDDTDValidator::validateElement(ImportNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "filename");
}

void NEDDTDValidator::validateElement(PropertydefNode *node)
{
    int tags[] = {NED_KEY_VALUE};
    char mult[] = {'*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(SimpleNode *node)
{
    int tags[] = {,,NED_PARAMETERS,NED_GATES};
    char mult[] = {'?','*','?','?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(InterfaceNode *node)
{
    int tags[] = {,NED_PARAMETERS,NED_GATES};
    char mult[] = {'*','?','?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ModuleNode *node)
{
    int tags[] = {,,NED_PARAMETERS,NED_GATES,NED_SUBMODULES,NED_CONNECTIONS};
    char mult[] = {'?','*','?','?','?','?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ParametersNode *node)
{
    int tags[] = {NED_PROPERTY,NED_PARAM,NED_PARAMETERS_BLOCK};
    checkChoice(node, tags, sizeof(tags)/sizeof(int), '*');

    const char *vals0[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-implicit", vals0, sizeof(vals0)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(ParametersBlockNode *node)
{
