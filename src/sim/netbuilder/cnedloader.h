//==========================================================================
// CNEDLOADER.H -
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


#ifndef __CNEDLOADER_H
#define __CNEDLOADER_H

#include "simkerneldefs.h"
#include "nedresourcecache.h"
#include "cneddeclaration.h"

NAMESPACE_BEGIN

class cProperties;
class cProperty;

class NEDElement;
class ParamElement;
class GateElement;
class PropertyElement;



/**
 * Stores dynamically loaded NED files, and one can look up NED declarations
 * of modules, channels, module interfaces and channel interfaces in them.
 * NED declarations are wrapped in cNEDDeclaration objects, which
 * point back into the NEDElement trees of the loaded NED files.
 *
 * This cNEDLoader class extends nedxml's NEDResourceCache, and
 * cNEDDeclaration extends nexml's corresponding NEDTypeInfo.
 */
class SIM_API cNEDLoader : public NEDResourceCache
{
  public:
    class ComponentTypeNames : public INEDTypeNames {
      public:
        virtual bool contains(const char *qname) const  {return componentTypes.instance()->lookup(qname)!=NULL;}
        virtual int size() const  {return componentTypes.instance()->size();}
        virtual const char *get(int k) const  {return componentTypes.instance()->get(k)->fullName();}
    };

  protected:
    // the singleton instance
    static cNEDLoader *instance_;

    struct PendingNedType {
        std::string qname;
        NEDElement *node;
        PendingNedType(const char *q, NEDElement *e) {qname=q;node=e;}
    };

    // storage for NED components not resolved yet because of missing dependencies
    std::vector<PendingNedType> pendingList;

    // maps the loaded source NED folders (as absolute paths, canonical representation)
    // to package names
    typedef std::map<std::string,std::string> StringMap;
    StringMap folderPackages;

  protected:
    /** Redefined to return a cNEDDeclaration. */
    virtual void addNedType(const char *qname, NEDElement *node);

  protected:
    // utility functions
    virtual void registerBuiltinDeclarations();
    virtual NEDElement *parseAndValidateNedFile(const char *nedfname, bool isXML);
    virtual bool areDependenciesResolved(const char *qname, NEDElement *node);
    virtual void registerNedTypes();
    virtual void registerNedType(const char *qname, NEDElement *node);
    virtual int doLoadNedSourceFolder(const char *foldername, const char *expectedPackage);
    virtual void doLoadNedFile(const char *nedfname, const char *expectedPackage, bool isXML);
    virtual std::string determineRootPackageName(const char *foldername);

    // constructor is protected, because we want only one instance
    cNEDLoader();

  public:
    /** Access to the singleton instance */
    static cNEDLoader *instance();

    /** Disposes of the singleton instance */
    static void clear();

    /**
     * Like lookup(), but throws an error if the declaration is not found,
     * so it never returns NULL.
     */
    virtual cNEDDeclaration *getDecl(const char *qname) const;

    /**
     * Load all NED files from a NED source folder. This involves visiting
     * each subdirectory, and loading all "*.ned" files from there.
     * The given folder is assumed to be the root of the NED package hierarchy.
     * Returns the number of files loaded.
     */
    int loadNedSourceFolder(const char *foldername);

    /**
     * Issues errors for components that are unresolved because of missing
     * dependencies.
     */
    void doneLoadingNedFiles();

    /**
     * For loading additional NED files after doneLoadingNedFiles().
     * This method resolves dependencies (base types, etc) immediately,
     * and throws an error if something is missing. (So this method is
     * not useful if two or more NED files mutually depend on each other.)
     * If the expected package is given (non-NULL), it should match the
     * package declaration inside the NED file.
     */
    void loadNedFile(const char *nedfname, const char *expectedPackage, bool isXML);

    /**
     * Returns the NED package that corresponds to the given folder. Returns ""
     * for the default package, and "-" if the folder is outside all NED folders.
     */
    std::string getNedPackageForFolder(const char *folder) const;

    /**
     * Resolves NED module/channel/moduleinterface/channelinterface type name,
     * based on the NED files loaded. This lookup is need for resolving inheritance
     * ("extends", "like").
     */
    virtual std::string resolveNedType(const NEDLookupContext& context, const char *nedtypename) {
        return NEDResourceCache::resolveNedType(context, nedtypename, &NEDResourceCache::CachedTypeNames(this));
    }

    /**
     * Resolve a NED module/channel type name, for a submodule or channel
     * instance. Lookup is based on component names registered in the simkernel,
     * NOT on the NED files loaded. This allows the user to instantiate
     * cModuleTypes/cChannelTypes which are not declared in NED.
     */
    virtual std::string resolveComponentType(const NEDLookupContext& context, const char *nedtypename) {
        return NEDResourceCache::resolveNedType(context, nedtypename, &ComponentTypeNames());
    }

    /**
     * Utility method, useful with resolveNedType()/resolveComponentType()
     */
    static NEDLookupContext getParentContextOf(const char *qname, NEDElement *node);

};

NAMESPACE_END


#endif

