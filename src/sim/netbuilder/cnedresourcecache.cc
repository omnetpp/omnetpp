//==========================================================================
// CNEDRESOURCECACHE.CC -
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

#include "cnedresourcecache.h"
#include "cnedcomponent.h"


void NEDResourceCache::addComponent(const char *name, NEDElement *node)
{
    cNEDComponent *component = new cNEDComponent(node);
    components[name] = component;
}

cNEDComponent *NEDResourceCache::lookup(const char *name)
{
    return dynamic_cast<cNEDComponent *>(NEDResourceCache::lookup(name));
}

