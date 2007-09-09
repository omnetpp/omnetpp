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
#include "fileutil.h"


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

bool NEDResourceCache::addFile(const char *fname, NEDElement *node)
{
    std::string key = tidyFilename(absolutePath(fname).c_str());
    NEDFileMap::iterator it = files.find(key);
    if (it!=files.end())
        return false; // already added

    files[key] = node;

    collectComponents(node, "");
    return true;
}

NEDElement *NEDResourceCache::getFile(const char *fname)
{
    // hash table lookup
    std::string key = tidyFilename(absolutePath(fname).c_str());
    NEDFileMap::iterator i = files.find(key);
    return i==files.end() ? NULL : i->second;
}

NEDComponent *NEDResourceCache::lookup(const char *qname) const
{
    // hash table lookup
    NEDComponentMap::const_iterator i = components.find(qname);
    return i==components.end() ? NULL : i->second;
}

void NEDResourceCache::addComponent(const char *qname, NEDElement *node)
{
    NEDComponent *component = new NEDComponent(node);
    components[qname] = component;
}

void NEDResourceCache::collectComponents(NEDElement *node, const std::string& namespaceprefix)
{
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        int tag = child->getTagCode();
        if (tag==NED_CHANNEL || tag==NED_CHANNEL_INTERFACE || tag==NED_SIMPLE_MODULE ||
            tag==NED_COMPOUND_MODULE || tag==NED_MODULE_INTERFACE ||
            tag==NED_ENUM || tag==NED_STRUCT || tag==NED_CLASS || tag==NED_MESSAGE)
        {
            std::string name = namespaceprefix + child->getAttribute("name");
            if (lookup(name.c_str()))
                throw NEDException("redeclaration of %s %s", child->getTagName(), name.c_str()); //XXX maybe just NEDError?

            addComponent(name.c_str(), child);

            NEDElement *types = child->getFirstChildWithTag(NED_TYPES);
            if (types)
                collectComponents(types, name+"::");
        }
    }
}

