//==========================================================================
// CNEDRESOURCECACHE.H -
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


#ifndef __CNEDRESOURCECACHE_H
#define __CNEDRESOURCECACHE_H

#include "defs.h"
#include "nedresourcecache.h"
#include "cnedcomponent.h"
#include "cneddeclaration.h"

class cProperties;
class cProperty;

class NEDElement;
class ParamNode;
class GateNode;
class PropertyNode;

/**
 * Stores dynamically loaded NED files. Extends nedxml's NEDResourceCache,
 * so that we can subclass NEDComponent.
 */
class SIM_API cNEDResourceCache : public NEDResourceCache
{
  protected:
    // storage for NED components not resolved yet because of missing dependencies
    std::vector<NEDElement *> pendingList;

  protected:
    /** Redefined to return a cNEDComponent. */
    virtual void addComponent(const char *name, NEDElement *node);

  protected:
    // utility functions
    NEDElement *parseAndValidateNedFile(const char *nedfname, bool isXML);
    bool areDependenciesResolved(NEDElement *node);
    void tryResolvePendingDeclarations();
    cNEDDeclaration *buildNEDDeclaration(NEDElement *node);
    cNEDDeclaration::ParamDescription extractParamDescription(ParamNode *paramNode);
    cNEDDeclaration::GateDescription extractGateDescription(GateNode *gateNode);
    cProperties *extractProperties(NEDElement *parent);
    cProperty *extractProperty(PropertyNode *propNode);

  public:
    /** Constructor */
    cNEDResourceCache() {}

    /** Just a typecast on base class's lookup(), utilizing covariant return types */
    virtual cNEDComponent *lookup(const char *name);

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

