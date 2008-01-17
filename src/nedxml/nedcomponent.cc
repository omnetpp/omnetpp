//==========================================================================
// NEDCOMPONENT.CC -
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

#include <stdio.h>
#include <string.h>
#include "nederror.h"
#include "nedcomponent.h"

USING_NAMESPACE


NEDComponent::NEDComponent(NEDElement *tree)
{
    this->tree = tree;
}

NEDComponent::~NEDComponent()
{
    // nothing -- we don't manage the tree, only cache a pointer to it
}

NEDElement *NEDComponent::getTree() const
{
    return tree;
}




