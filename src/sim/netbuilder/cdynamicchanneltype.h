//==========================================================================
//  CDYNAMICCHANNELTYPE.H - part of
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

#ifndef __CDYNAMICCHANNELTYPE_H
#define __CDYNAMICCHANNELTYPE_H

#include "cchannel.h"
#include "ccomponenttype.h"
#include "cnednetworkbuilder.h"
#include "cneddeclaration.h"

NAMESPACE_BEGIN


/**
 * NEDXML-based cChannelType: takes all info from cNEDLoader
 */
class SIM_API cDynamicChannelType : public cChannelType
{
  protected:
    /** Redefined from cChannelType */
    virtual cChannel *createChannelObject();

    /** Redefined from cChannelType */
    virtual void addParametersTo(cChannel *module);

    // internal utility function
    cNEDDeclaration *getDecl() const;

    // methods redefined from cComponentType
    virtual cProperties *properties() const;
    virtual cProperties *paramProperties(const char *paramName) const;
    virtual cProperties *gateProperties(const char *gateName) const;
    virtual cProperties *submoduleProperties(const char *submoduleName, const char *submoduleType) const;
    virtual cProperties *connectionProperties(int connectionId, const char *channelType) const;

  public:
    /**
     * Constructor.
     */
    cDynamicChannelType(const char *name);

    /**
     * Produces a one-line description.
     */
    virtual std::string info() const;

    /**
     * Produces a detailed, multi-line description.
     */
    virtual std::string detailedInfo() const;
    //@}
};


NAMESPACE_END


#endif


