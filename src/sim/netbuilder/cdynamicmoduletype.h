//==========================================================================
//  CDYNAMICMODULETYPE.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDYNAMICMODULETYPE_H
#define __CDYNAMICMODULETYPE_H

#include "cmodule.h"
#include "ccomponenttype.h"
#include "cneddeclaration.h"
#include "cnednetworkbuilder.h"

NAMESPACE_BEGIN


/**
 * NEDXML-based cModuleType: takes all info from cNEDLoader
 */
class SIM_API cDynamicModuleType : public cModuleType
{
  protected:
    /** Redefined from cModuleType */
    virtual cModule *createModuleObject();

    /** Redefined from cModuleType */
    virtual void addParametersAndGatesTo(cModule *module);

    /** Redefined from cComponentType */
    virtual void applyPatternAssignments(cComponent *component);

    /** Redefined from cModuleType */
    virtual void setupGateVectors(cModule *module);

    /** Redefined from cModuleType */
    virtual void buildInside(cModule *module);

    // internal utility function
    cNEDDeclaration *getDecl() const;

    // methods redefined from cComponentType
    virtual cProperties *getProperties() const;
    virtual cProperties *getParamProperties(const char *paramName) const;
    virtual cProperties *getGateProperties(const char *gateName) const;
    virtual cProperties *getSubmoduleProperties(const char *submoduleName, const char *submoduleType) const;
    virtual cProperties *getConnectionProperties(int connectionId, const char *channelType) const;
    virtual std::string getPackageProperty(const char *name) const;
    virtual const char *getImplementationClassName() const;
    virtual std::string getCxxNamespace() const;
    virtual bool isInnerType() const;

  public:
    /**
     * Constructor.
     */
    cDynamicModuleType(const char *name);

    /**
     * Produces a one-line description.
     */
    virtual std::string info() const;

    /**
     * Produces a detailed, multi-line description.
     */
    virtual std::string detailedInfo() const;

    /**
     * Returns true if the module type was declared with the "network" keyword.
     */
    virtual bool isNetwork() const;

    /**
     * Returns true if this object represents a simple module type.
     */
    virtual bool isSimple() const;
};

NAMESPACE_END


#endif


