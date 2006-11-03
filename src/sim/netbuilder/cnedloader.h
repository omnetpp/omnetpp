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

#include "defs.h"
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
    virtual void addComponent(const char *name, NEDElement *node);

  protected:
    // utility functions
    void registerBuiltinDeclarations();
    NEDElement *parseAndValidateNedFile(const char *nedfname, bool isXML);
    bool areDependenciesResolved(NEDElement *node);
    void tryResolvePendingDeclarations();
    cNEDDeclaration *buildNEDDeclaration(NEDElement *node);
    cNEDDeclaration::ParamDescription extractParamDescription(ParamNode *paramNode);
    cNEDDeclaration::GateDescription extractGateDescription(GateNode *gateNode);
    void updateProperties(NEDElement *parent, cProperties *props);
    void updateProperty(PropertyNode *propNode, cProperty *prop);
    void updateDisplayProperty(PropertyNode *propNode, cProperty *prop);

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
    virtual cNEDDeclaration *lookup2(const char *name) const;

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
     * Issues errors for components that are still unresolved because of
     * missing (unloaded) dependencies.
     */
    void done();
};

#endif

