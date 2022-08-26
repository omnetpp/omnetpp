//==========================================================================
//  CDYNAMICCHANNELTYPE.H - part of
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

#ifndef __OMNETPP_CDYNAMICCHANNELTYPE_H
#define __OMNETPP_CDYNAMICCHANNELTYPE_H

#include "omnetpp/cchannel.h"
#include "omnetpp/ccomponenttype.h"
#include "cnednetworkbuilder.h"
#include "cneddeclaration.h"

namespace omnetpp {


/**
 * @brief NEDXML-based cChannelType: takes all info from cNedLoader
 */
class SIM_API cDynamicChannelType : public cChannelType
{
  protected:
    /** Redefined from cChannelType */
    virtual cChannel *createChannelObject() override;

    /** Redefined from cChannelType */
    virtual void addParametersTo(cChannel *channel) override;

    /** Redefined from cComponentType */
    virtual void applyPatternAssignments(cComponent *component) override;

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
    cDynamicChannelType(const char *name);

    /**
     * Produces a one-line description.
     */
    virtual std::string str() const override;

    /**
     * Returns the NED source code of the component.
     */
    virtual std::string getNedSource() const override;
    //@}
};


}  // namespace omnetpp


#endif


