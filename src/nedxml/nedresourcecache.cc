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
#include "stringutil.h"
#include "patternmatcher.h"

USING_NAMESPACE


NEDResourceCache::NEDResourceCache()
{
}

NEDResourceCache::~NEDResourceCache()
{
    for (NEDFileMap::iterator i = files.begin(); i!=files.end(); ++i)
        delete i->second;
    for (NEDTypeInfoMap::iterator i = nedTypes.begin(); i!=nedTypes.end(); ++i)
        delete i->second;
}

bool NEDResourceCache::addFile(const char *fname, NEDElement *node)
{
    std::string key = tidyFilename(absolutePath(fname).c_str());
    NEDFileMap::iterator it = files.find(key);
    if (it!=files.end())
        return false; // already added

    files[key] = node;

    PackageNode *packageDecl = (PackageNode *) node->getFirstChildWithTag(NED_PACKAGE);
    std::string packagePrefix = packageDecl ? packageDecl->getName() : "";
    if (!packagePrefix.empty())
        packagePrefix += ".";

    collectComponents(node, packagePrefix);
    return true;
}

NEDElement *NEDResourceCache::getFile(const char *fname)
{
    // hash table lookup
    std::string key = tidyFilename(absolutePath(fname).c_str());
    NEDFileMap::iterator i = files.find(key);
    return i==files.end() ? NULL : i->second;
}

NEDTypeInfo *NEDResourceCache::lookup(const char *qname) const
{
    // hash table lookup
    NEDTypeInfoMap::const_iterator i = nedTypes.find(qname);
    return i==nedTypes.end() ? NULL : i->second;
}

void NEDResourceCache::addNedType(const char *qname, NEDElement *node)
{
    NEDTypeInfo *component = new NEDTypeInfo(qname, node);
    nedTypes[qname] = component;
    nedTypeNames.clear(); // invalidate
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
            std::string qname = namespaceprefix + child->getAttribute("name");
            if (lookup(qname.c_str()))
                throw NEDException("redeclaration of %s %s", child->getTagName(), qname.c_str()); //XXX maybe just NEDError?

            addNedType(qname.c_str(), child);

            NEDElement *types = child->getFirstChildWithTag(NED_TYPES);
            if (types)
                collectComponents(types, qname+".");
        }
    }
}

std::string NEDResourceCache::resolveNedType(const NEDLookupContext& context, const char *nedtypename, INEDTypeNames *qnames)
{
    // note: this method is to be kept consistent with NEDResources.lookupNedType() in the Java code
    // note2: partially qualified names are not supported: name must be either simplename or fully qualified
    if (!strchr(nedtypename, '.'))
    {
        // no dot: name is an unqualified name (simple name); so, it can be:
        // (a) inner type, (b) from the same package, (c) an imported type, (d) from the default package

        // inner type?  FIXME only if context is NOT a nedfile
        std::string qname = context.qname + "." + nedtypename;
        if (qnames->contains(qname.c_str()))
            return qname;

        NedFileNode *nedfileNode = dynamic_cast<NedFileNode *>(context.element->getParentWithTag(NED_NED_FILE));

        // from the same package?
        PackageNode *packageNode = nedfileNode->getFirstPackageChild();
        const char *packageName = packageNode ? packageNode->getName() : "";
        qname = opp_isempty(packageName) ? nedtypename : std::string(packageName) + "." + nedtypename;
        if (qnames->contains(qname.c_str()))
            return qname;

        // collect imports, for convenience
        std::vector<const char *> imports;
        for (ImportNode *import = nedfileNode->getFirstImportChild(); import; import = import->getNextImportNodeSibling())
            imports.push_back(import->getImportSpec());

        // imported type?
        // try a shortcut first: if the import doesn't contain wildcards
        std::string dot_nedtypename = std::string(".")+nedtypename;
        for (int i=0; i<imports.size(); i++)
            if (qnames->contains(imports[i]) && (opp_stringendswith(imports[i], dot_nedtypename.c_str()) || strcmp(imports[i], nedtypename)==0))
                return imports[i];

        // try harder, using wildcards
        for (int i=0; i<imports.size(); i++) {
            PatternMatcher importpattern(imports[i], true, true, true);
            for (int j=0; j<qnames->size(); j++) {
                const char *qname = qnames->get(j);
                if ((opp_stringendswith(qname, dot_nedtypename.c_str()) || strcmp(qname, nedtypename)==0))
                    if (importpattern.matches(qname))
                        return qname;
            }
        }
    }

    // fully qualified name?
    if (qnames->contains(nedtypename))
        return nedtypename;

    return "";
}

const std::vector<std::string>& NEDResourceCache::getTypeNames() const
{
    if (nedTypeNames.empty() && !nedTypes.empty()) 
    {
        // fill in nedTypeNames vector
        for (NEDTypeInfoMap::const_iterator i=nedTypes.begin(); i!=nedTypes.end(); ++i)
            nedTypeNames.push_back(i->first);
    }
    return nedTypeNames;
}
