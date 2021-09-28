//==========================================================================
// NEDRESOURCECACHE.H -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __OMNETPP_NEDXML_NEDRESOURCECACHE_H
#define __OMNETPP_NEDXML_NEDRESOURCECACHE_H

#include <map>
#include <vector>
#include <string>
#include "nedelements.h"
#include "nedtypeinfo.h"

namespace omnetpp {
namespace nedxml {

class ErrorStore;

/**
 * @brief Context of NED type lookup, for NedResourceCache.
 *
 * @ingroup NedResources
 */
struct NedLookupContext
{
    ASTNode *element;  // compound module or NED file
    std::string qname;    // fully qualified name, or (for NED files) package name
    NedLookupContext(ASTNode *e, const char *q) {element=e;qname=q;}
};

/**
 * @brief Stores loaded NED files, and keeps track of components in them.
 *
 * This class can be turned into a cache (discarding and reloading
 * NED files on demand) if such need arises.
 *
 * @ingroup NedResources
 */
class NEDXML_API NedResourceCache
{
  public:
      /** Interface that enumerates NED types; used by resolveType() */
      class INedTypeNames {
        public:
          /** Returns true if the given fully qualified name is an existing NED type */
          virtual bool contains(const char *qname) const = 0;

          /** Returns the number of NED type names */
          virtual int size() const = 0;

          /** Returns the kth fully qualified NED type name */
          virtual const char *get(int k) const = 0;

          virtual ~INedTypeNames() {}  // make the compiler happy
      };

      class CachedTypeNames : public INedTypeNames {
        protected:
          NedResourceCache *p;
        public:
          CachedTypeNames(NedResourceCache *p) {this->p=p;}
          virtual bool contains(const char *qname) const override {return p->lookup(qname)!=nullptr;}
          virtual int size() const override {return p->getTypeNames().size();}
          virtual const char *get(int k) const override {return p->getTypeNames()[k].c_str();}
      };

  protected:
    // table of loaded NED files (by NED file name as absolute path, canonical representation)
    std::map<std::string,NedFileElement*> nedFiles;

    // "package.ned" files by package name (only one per package accepted)
    std::map<std::string, NedFileElement*> packageDotNedFiles;

    // true after doneLoadingNedFiles() has been called
    bool doneLoadingNedFilesCalled = false;

    // table of NED type declarations; key is fully qualified name, and elements point into nedFiles
    std::map<std::string, NedTypeInfo*> nedTypes;

    // cached keys of the nedTypes map, for getTypeNames(); zero size means out of date
    mutable std::vector<std::string> nedTypeNames;

    // maps the loaded source NED folders (as absolute paths, canonical representation)
    // to package names
    typedef std::map<std::string,std::string> StringMap;
    StringMap folderPackages;

    struct PendingNedType {
        std::string qname;
        bool isInnerType;
        ASTNode *node;
        PendingNedType(const char *q, bool inner, ASTNode *e) {qname=q;isInnerType=inner;node=e;}
    };

    // storage for NED components not resolved yet because of missing dependencies
    std::vector<PendingNedType> pendingList;

  protected:
    virtual void addFile(const char *fname, NedFileElement *node);
    virtual void registerBuiltinDeclarations();
    virtual int doLoadNedSourceFolder(const char *foldername, const char *expectedPackage, const std::vector<std::string>& excludedFolders);
    virtual void doLoadNedFileOrText(const char *nedfname, const char *nedtext, const char *expectedPackage, bool isXML);
    virtual NedFileElement *parseAndValidateNedFileOrText(const char *nedfname, const char *nedtext, bool isXML);
    virtual std::string determineRootPackageName(const char *nedSourceFolderName);
    virtual std::string getNedSourceFolderForFolder(const char *folder) const;
    virtual void collectNedTypesFrom(ASTNode *node, const std::string& packagePrefix, bool areInnerTypes);
    virtual void collectNedType(const char *qname, bool isInnerType, ASTNode *node);
    virtual bool areDependenciesResolved(const char *qname, ASTNode *node);
    virtual void registerPendingNedTypes();
    virtual void registerNedType(const char *qname, bool isInnerType, ASTNode *node);
    virtual std::string getFirstError(ErrorStore *errors, const char *prefix=nullptr);

  public:
    /** Constructor */
    NedResourceCache() {}

    /** Destructor */
    virtual ~NedResourceCache();

    /**
     * Load all NED files from a NED source folder. This involves visiting
     * each subdirectory, and loading all "*.ned" files from there.
     * The given folder is assumed to be the root of the NED package hierarchy.
     * A list of packages to skip may be specified in the excludedPackages parameter
     * (items must be separated with a semicolon).
     *
     * The function returns the number of NED files loaded.
     *
     * Note: doneLoadingNedFiles() must be called after the last
     * loadNedSourceFolder()/loadNedFile()/loadNedText() call.
     */
    virtual int loadNedSourceFolder(const char *foldername, const char *excludedPackages);

    /**
     * Load a single NED file. If the expected package is given (non-nullptr),
     * it should match the package declaration inside the NED file.
     *
     * Note: doneLoadingNedFiles() must be called after the last
     * loadNedSourceFolder()/loadNedFile()/loadNedText() call.
     */
    virtual void loadNedFile(const char *nedfname, const char *expectedPackage, bool isXML);

    /**
     * Parses and loads the NED source code passed in the nedtext argument.
     * The name argument will be used as filename in error messages, and
     * and should be unique among the files loaded. If the expected package
     * is given (non-nullptr), it should match the package declaration inside
     * the NED file.
     *
     * Note: doneLoadingNedFiles() must be called after the last
     * loadNedSourceFolder()/loadNedFile()/loadNedText() call.
     */
    virtual void loadNedText(const char *name, const char *nedtext, const char *expectedPackage, bool isXML);

    /**
     * To be called after all NED folders / files have been loaded. May be
     * redefined to issue errors for components that could not be fully
     * resolved because of missing base types or interfaces.
     */
    virtual void doneLoadingNedFiles();

    /**
     * Return a list of "package.ned" files relevant for the given package.
     * Files are in bottom-up order, the order they should be searched for package properties.
     */
    std::vector<NedFileElement*> getPackageNedListForLookup(const char *packageName) const;

    /** Look up a fully qualified NED type name from the cache. Returns nullptr if not found. */
    virtual NedTypeInfo *lookup(const char *qname) const;

    /** Like lookup(), but asserts non-nullptr return value */
    virtual NedTypeInfo *getDecl(const char *qname) const;

    /** Resolves the given NED type name in the given context, among the given type names. Returns "" if not found. */
    virtual std::string resolveNedType(const NedLookupContext& context, const char *nedtypename, INedTypeNames *qnames);

    /** Resolves NED type name, based on the NED files loaded */
    virtual std::string resolveNedType(const NedLookupContext& context, const char *nedtypename) {
        CachedTypeNames names(this);
        return resolveNedType(context, nedtypename, &names);
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
    static NedLookupContext getParentContextOf(const char *qname, ASTNode *node);

};

}  // namespace nedxml
}  // namespace omnetpp


#endif

