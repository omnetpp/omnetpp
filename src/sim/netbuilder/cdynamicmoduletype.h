//==========================================================================
//  CDYNAMICMODULETYPE.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CDYNAMICMODULETYPE_H
#define __OMNETPP_CDYNAMICMODULETYPE_H

#include "omnetpp/cmodule.h"
#include "omnetpp/ccomponenttype.h"
#include "cneddeclaration.h"
#include "cnednetworkbuilder.h"

namespace omnetpp {


/**
 * @brief NEDXML-based cModuleType: takes all info from cNedLoader
 */
class SIM_API cDynamicModuleType : public cModuleType
{
  protected:
    /** Redefined from cModuleType */
    virtual cModule *createModuleObject() override;

    /** Redefined from cModuleType */
    virtual void addParametersAndGatesTo(cModule *module) override;

    /** Redefined from cComponentType */
    virtual void applyPatternAssignments(cComponent *component) override;

    /** Redefined from cModuleType */
    virtual void setupGateVectors(cModule *module) override;

    /** Redefined from cModuleType */
    virtual void buildInside(cModule *module) override;

    // internal utility function
    cNedDeclaration *getDecl() const;

    // methods redefined from cComponentType
    virtual cProperties *getProperties() const override;
    virtual cProperties *getParamProperties(const char *paramName) const override;
    virtual cProperties *getGateProperties(const char *gateName) const override;
    virtual cProperties *getSubmoduleProperties(const char *submoduleName, const char *submoduleType) const override;
    virtual cProperties *getConnectionProperties(int connectionId, const char *channelType) const override;
    virtual std::string getPackageProperty(const char *name) const override;
    virtual const char *getImplementationClassName() const override;
    virtual std::string getCxxNamespaceForType(const char *type) const override;
    virtual std::string getCxxNamespace() const override;
    virtual const char *getSourceFileName() const override;
    virtual bool isInnerType() const override;
    virtual void clearSharedParImpls() override;

  public:
    /**
     * Constructor.
     */
    cDynamicModuleType(const char *name);

    /**
     * Produces a one-line description.
     */
    virtual std::string str() const override;

    /**
     * Returns the NED source code of the component.
     */
    virtual std::string getNedSource() const override;

    /**
     * Returns the documentation of the component.
     */
    virtual std::string getDocumentation() const override;

    /**
     * Returns true if the module type was declared with the "network" keyword.
     */
    virtual bool isNetwork() const override;

    /**
     * Returns true if this object represents a simple module type.
     */
    virtual bool isSimple() const override;
};

}  // namespace omnetpp


#endif


