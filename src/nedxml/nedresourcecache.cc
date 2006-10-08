//==========================================================================
// NEDRESOURCECACHE.CC -
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
#include "nedresourcecache.h"


NEDResourceCache::NEDResourceCache()
{
}

NEDResourceCache::~NEDResourceCache()
{
    for (NEDFileMap::iterator i = files.begin(); i!=files.end(); ++i)
        delete i->second;
    for (NEDComponentMap::iterator i = components.begin(); i!=components.end(); ++i)
        delete i->second;
}

NEDComponent *NEDResourceCache::createNEDComponent(NEDElement *tree)
{
    return new NEDComponent(tree);
}

bool NEDResourceCache::addFile(const char *name, NedFileNode *node)
{
    NEDFileMap::iterator it = files.find(name);
    if (it!=files.end())
        return false;

    files[name] = node;

    collectComponents(node, "");
    return true;
}

NedFileNode *NEDResourceCache::getFile(const char *name)
{
    // hash table lookup
    NEDFileMap::iterator i = files.find(name);
    return i==files.end() ? NULL : i->second;
}

NEDComponent *NEDResourceCache::getComponent(const char *name)
{
    // hash table lookup
    NEDComponentMap::iterator i = components.find(name);
    return i==components.end() ? NULL : i->second;
}

void NEDResourceCache::collectComponents(NEDElement *node, const std::string& namespaceprefix)
{
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int tag = child->getTagCode();
        if (tag==NED_CHANNEL || tag==NED_CHANNEL_INTERFACE || tag==NED_SIMPLE_MODULE ||
            tag==NED_COMPOUND_MODULE || tag==NED_MODULE_INTERFACE)
        {
            std::string name = namespaceprefix + child->getAttribute("name");
            if (getComponent(name.c_str()))
                throw new NEDException("redeclaration of %s %s", child->getTagName(), name.c_str()); //XXX maybe just NEDError?

            NEDComponent *component = createNEDComponent(child);
            components[name] = component;

            NEDElement *types = child->getFirstChildWithTag(NED_TYPES);
            if (types)
                collectComponents(types, name+".");
        }
        else if (tag==NED_ENUM || tag==NED_STRUCT || tag==NED_CLASS || tag==NED_MESSAGE)
        {
            //XXX for now, msg file contents are ignored -- can be added later if there's a need
        }
    }
}
