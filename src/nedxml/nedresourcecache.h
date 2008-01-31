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

NAMESPACE_BEGIN


/**
 * Stores loaded NED files, and keeps track of components in them.
 *
 * This class can be turned into a cache (discarding and reloading
 * NED files on demand) if such need arises.
 *
 * @ingroup NEDCompiler
 */
class NEDXML_API NEDResourceCache
{
  public:
	  /** Interface that enumerates NED types; used by resolveType() */
	  class NEDTypeNames {
	  public:
		/** Returns true if the given fully qualified name is an existing NED type */
		virtual bool contains(const char *qname) const = 0;
		
		/** Returns the number of NED type names */
		virtual int size() const = 0;
		
		/** Returns the kth fully qualified NED type name */
		virtual const char *get(int k) const = 0;
	  };
  protected:
    typedef std::map<std::string, NEDElement *> NEDFileMap;
    typedef std::map<std::string, NEDTypeInfo *> NEDTypeInfoMap;

    // table of loaded NED files; maps file name to NED element.
    NEDFileMap files;

    // table of NED type declarations; key is fully qualified name, and
    // elements point into the files map
    NEDTypeInfoMap nedTypes;

  protected:
    virtual void collectComponents(NEDElement *node, const std::string& namespaceprefix);

    /**
     * Wrap the given NEDElement into a NEDTypeInfo and add it to the table.
     * Redefine it if you want to subclass NEDTypeInfo.
     */
    virtual void addNedType(const char *qname, NEDElement *node);

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

    /** Look up a fully qualified NED type name from the cache. Returns NULL if not found. */
    virtual NEDTypeInfo *lookup(const char *qname) const;
    
    /** Resolves the given NED type name in the given context. Returns "" if not found. */ 
    virtual std::string resolveNedType(NEDTypeInfo *context, const char *nedtypename, NEDTypeNames *qnames);
};

NAMESPACE_END


#endif

