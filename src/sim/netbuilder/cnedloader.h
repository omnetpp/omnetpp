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

class cProperties;
class cProperty;

class NEDElement;
class ParamNode;
class GateNode;
class PropertyNode;

/**
 * Stores dynamically loaded NED files, and one can look up NED declarations
 * of modules, channels, module interfaces and channel interfaces in them.
 * NED declarations are wrapped in cNEDDeclaration objects, which
 * point back into the NEDElement trees of the loaded NED files.
 *
 * This cNEDLoader class extends nedxml's NEDResourceCache, and
 * cNEDDeclaration extends nexml's corresponding NEDComponent.
 */
class SIM_API cNEDLoader : public NEDResourceCache
{
  protected:
    // the singleton instance
    static cNEDLoader *instance_;

    // storage for NED components not resolved yet because of missing dependencies
    std::vector<NEDElement *> pendingList;

  protected:
    /** Redefined to return a cNEDDeclaration. */
    virtual void addComponent(const char *qname, NEDElement *node);

  protected:
    // utility functions
    void registerBuiltinDeclarations();
    NEDElement *parseAndValidateNedFile(const char *nedfname, bool isXML);
    bool areDependenciesResolved(NEDElement *node);
    void tryResolvePendingDeclarations();
    cNEDDeclaration *buildNEDDeclaration(const char *qname, NEDElement *node);
    int doLoadNedSourceFolder(const char *foldername);

    // constructor is protected, because we want only one instance
    cNEDLoader();

  public:
    /** Access to singleton instance */
    static cNEDLoader *instance();

    /** Disposes of singleton instance */
    static void clear();

    /**
     * A call to base class's lookup(), plus a cast. Implementation note: we cannot just
     * override lookup() with covariant return types, because VC 7.1 is buggy and
     * cannot handle it properly.
     */
    virtual cNEDDeclaration *lookup2(const char *qname) const;

    /**
     * Like lookup2(), but throws an error if the declaration is not found,
     * so it never returns NULL.
     */
    virtual cNEDDeclaration *getDecl(const char *qname) const;

    /**
     * Parses the given NED file, and converts modules/channels/interfaces to
     * cNEDDeclaration. Components that depend on other components
     * (e.g. contain "extends") may be temporarily put off until all
     * dependencies are loaded.
     *
     * Invokes base class's addNEDFile() internally.
     */
    void loadNedFile(const char *nedfname, bool isXML);

    /**
     * Load all NED files from a NED source folder. This involves visiting
     * each subdirectory, and loading all "*.ned" files from there.
     * The given folder is assumed to be the root of the NED package hierarchy.
     * Returns the number of files loaded.
     */
    int loadNedSourceFolder(const char *foldername);

    /**
     * Issues errors for components that are still unresolved because of
     * missing (unloaded) dependencies.
     */
    void done();
};

#endif

