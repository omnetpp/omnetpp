//==========================================================================
// NEDRESOURCECACHE.H -
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


#ifndef __NEDRESOURCECACHE_H
#define __NEDRESOURCECACHE_H

#include <map>
#include <vector>
#include <string>
#include "nedelements.h"
#include "nedcomponent.h"


/**
 * Stores loaded NED files, and keeps track of components in them.
 *
 * This class could be turned into a cache (discarding and reloading
 * NED files on demand) if such need arises.
 *
 * @ingroup NEDCompiler
 */
class NEDXML_API NEDResourceCache
{
  protected:
    typedef std::map<std::string, NEDElement *> NEDFileMap;
    typedef std::map<std::string, NEDComponent *> NEDComponentMap;

    // table of loaded NED files; maps file name to NED element.
    NEDFileMap files;

    // table of component declarations; key is fully qualified name, and
    // elements point into the files map
    NEDComponentMap components;

  protected:
    virtual void collectComponents(NEDElement *node, const std::string& namespaceprefix);

    /**
     * Wrap the given NEDElement into a NEDComponent and add it to the table.
     * Redefine it if you want to subclass NEDComponent.
     */
    virtual void addComponent(const char *qname, NEDElement *node);

  public:
    /** Constructor */
    NEDResourceCache();

    /** Destructor */
    virtual ~NEDResourceCache();

    /**
     * Add a file (parsed into an object tree) to the cache. If the file
     * was already added, no processing takes place and the function
     * returns false; otherwise it returns true.
     */
    virtual bool addFile(const char *fname, NEDElement *node);

    /** Get a file (represented as object tree) from the cache */
    virtual NEDElement *getFile(const char *fname);

    /** Get a component by fully qualified name from the cache */
    virtual NEDComponent *lookup(const char *qname) const;
};

#endif

