//==========================================================================
//  CDYNAMICMODULETYPE.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

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
    virtual void addParametersTo(cModule *module);

    /** Redefined from cModuleType */
    virtual void addGatesTo(cModule *module);

    /** Redefined from cModuleType */
    virtual void buildInside(cModule *module);

    // internal utility function
    cNEDDeclaration *getDecl() const;

    // methods redefined from cComponentType
    virtual cProperties *properties() const;
    virtual cProperties *paramProperties(const char *paramName) const;
    virtual cProperties *gateProperties(const char *gateName) const;
    virtual cProperties *submoduleProperties(const char *submoduleName, const char *submoduleType) const;
    virtual cProperties *connectionProperties(const char *connectionId, const char *channelType) const;

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
};

NAMESPACE_END


#endif


