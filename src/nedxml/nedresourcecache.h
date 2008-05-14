//==========================================================================
// NEDRESOURCECACHE.H -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __NEDRESOURCECACHE_H
#define __NEDRESOURCECACHE_H

#include <map>
#include <vector>
#include <string>
#include "nedelements.h"
#include "nedtypeinfo.h"

NAMESPACE_BEGIN


/**
 * Context of NED type lookup
 */
struct NEDLookupContext
{
    NEDElement *element;  // compound module or NED file
    std::string qname;    // fully qualified name, or (for NED files) package name
    NEDLookupContext(NEDElement *e, const char *q) {element=e;qname=q;}
};

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
      class INEDTypeNames {
        public:
          /** Returns true if the given fully qualified name is an existing NED type */
          virtual bool contains(const char *qname) const = 0;

          /** Returns the number of NED type names */
          virtual int size() const = 0;

          /** Returns the kth fully qualified NED type name */
          virtual const char *get(int k) const = 0;

          virtual ~INEDTypeNames() {}  // make the compiler happy
      };

      class CachedTypeNames : public INEDTypeNames {
        protected:
          NEDResourceCache *p;
        public:
          CachedTypeNames(NEDResourceCache *p) {this->p=p;}
          virtual bool contains(const char *qname) const {return p->lookup(qname)!=NULL;}
          virtual int size() const {return p->getTypeNames().size();}
          virtual const char *get(int k) const {return p->getTypeNames()[k].c_str();}
      };

  protected:
    typedef std::map<std::string, NEDElement *> NEDFileMap;
    typedef std::map<std::string, NEDTypeInfo *> NEDTypeInfoMap;

    // table of loaded NED files; maps file name to NED element.
    NEDFileMap files;

    // table of NED type declarations; key is fully qualified name, and
    // elements point into the files map
    NEDTypeInfoMap nedTypes;

    // cached keys of the nedTypes map, for getTypeNames(); zero size means out of date
    mutable std::vector<std::string> nedTypeNames;

    // maps the loaded source NED folders (as absolute paths, canonical representation)
    // to package names
    typedef std::map<std::string,std::string> StringMap;
    StringMap folderPackages;

    struct PendingNedType {
        std::string qname;
        NEDElement *node;
        PendingNedType(const char *q, NEDElement *e) {qname=q;node=e;}
    };

    // storage for NED components not resolved yet because of missing dependencies
    std::vector<PendingNedType> pendingList;

  protected:
    virtual void registerBuiltinDeclarations();
    virtual int doLoadNedSourceFolder(const char *foldername, const char *expectedPackage);
    virtual void doLoadNedFile(const char *nedfname, const char *expectedPackage, bool isXML);
    virtual NEDElement *parseAndValidateNedFile(const char *nedfname, bool isXML);
    virtual std::string determineRootPackageName(const char *foldername);
    virtual void collectNedTypes(NEDElement *node, const std::string& namespaceprefix);
    virtual void collectNedType(const char *qname, NEDElement *node);
    virtual bool areDependenciesResolved(const char *qname, NEDElement *node);
    virtual void registerPendingNedTypes();
    virtual void registerNedType(const char *qname, NEDElement *node);

  public:
    /** Constructor */
    NEDResourceCache();

    /** Destructor */
    virtual ~NEDResourceCache();

    /**
     * Load all NED files from a NED source folder. This involves visiting
     * each subdirectory, and loading all "*.ned" files from there.
     * The given folder is assumed to be the root of the NED package hierarchy.
     * Returns the number of files loaded.
     */
    virtual int loadNedSourceFolder(const char *foldername);

    /**
     * To be called after all NED folders / files have been loaded. May be redefined
     * to issue errors for components that are unresolved because of missing
     * dependencies.
     */
    virtual void doneLoadingNedFiles();

    /**
     * For loading additional NED files after doneLoadingNedFiles().
     * This method resolves dependencies (base types, etc) immediately,
     * and throws an error if something is missing. (So this method is
     * not useful if two or more NED files mutually depend on each other.)
     * If the expected package is given (non-NULL), it should match the
     * package declaration inside the NED file.
     */
    virtual void loadNedFile(const char *nedfname, const char *expectedPackage, bool isXML);

    /**
     * Add a file (parsed into an object tree) to the cache. If the file
     * was already added, no processing takes place and the function
     * returns false; otherwise it returns true.
     */
    virtual bool addFile(const char *fname, NEDElement *node);  //XXX make protected?

    /** Get a file (represented as object tree) from the cache */
    virtual NEDElement *getFile(const char *fname);

    /** Returns the package.ned file for the given package, or NULL. */
    virtual NEDElement *getPackageNedFile(const char *packagename) const;

    /** Look up a fully qualified NED type name from the cache. Returns NULL if not found. */
    virtual NEDTypeInfo *lookup(const char *qname) const;

    /** Like lookup(), but asserts non-NULL return value */
    virtual NEDTypeInfo *getDecl(const char *qname) const;

    /** Resolves the given NED type name in the given context, among the given type names. Returns "" if not found. */
    virtual std::string resolveNedType(const NEDLookupContext& context, const char *nedtypename, INEDTypeNames *qnames);

    /** Resolves NED type name, based on the NED files loaded */
    virtual std::string resolveNedType(const NEDLookupContext& context, const char *nedtypename) {
        NEDResourceCache::CachedTypeNames names(this);
        return NEDResourceCache::resolveNedType(context, nedtypename, &names);
    }

    /** Available NED type names */
    virtual const std::vector<std::string>& getTypeNames() const;

    /**
     * Returns the NED package that corresponds to the given folder. Returns ""
     * for the default package, and "-" if the folder is outside all NED folders.
     */
    virtual std::string getNedPackageForFolder(const char *folder) const;

    /**
     * Utility method, useful with resolveNedType()/resolveComponentType()
     */
    static NEDLookupContext getParentContextOf(const char *qname, NEDElement *node);

};

NAMESPACE_END


#endif

