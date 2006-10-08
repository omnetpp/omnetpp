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
class NEDResourceCache
{
  protected:
    typedef std::map<std::string, NedFileNode *> NEDFileMap;
    typedef std::map<std::string, NEDComponent *> NEDComponentMap;

    // table of loaded NED files; maps file name to NED element.
    NEDFileMap files;

    // table of component declarations; elements point into the files map
    NEDComponentMap components;

  protected:
    virtual void collectComponents(NEDElement *node, const std::string& namespaceprefix);

    /**
     * Factory method for creating NEDComponent objects. Redefine if you want
     * to add new stuff to NEDComponent by subclassing it.
     */
    virtual NEDComponent *createNEDComponent(NEDElement *tree);

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
    virtual bool addFile(const char *name, NedFileNode *node);

    /** Get a file (represented as object tree) from the cache */
    virtual NedFileNode *getFile(const char *name);

    /** Get a component from the cache */
    virtual NEDComponent *getComponent(const char *name);
};

#endif

