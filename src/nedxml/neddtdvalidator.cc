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
