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
#include "ccompoundmodule.h"
#include "ccomponenttype.h"
#include "cneddeclaration.h"

#include "cnednetworkbuilder.h"


/**
 * NEDXML-based cModuleType: takes all info from cNEDLoader
 */
class cDynamicModuleType : public cModuleType
{
  protected:
    /** Redefined from cModuleType */
    virtual cModule *createModuleObject();

    /** Redefined from cModuleType */
    virtual void addParametersGatesTo(cModule *module);

    /** Redefined from cModuleType */
    virtual void buildInside(cModule *module);

    // internal utility function
    cNEDDeclaration *getDecl() const;

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
     * Returns the NED declaration
     */
    virtual cNEDDeclarationBase *declaration() const {return getDecl();}  //XXX merge the two funcs

    virtual cProperties *properties() const;
    virtual cProperties *paramProperties(const char *paramname) const;
    virtual cProperties *gateProperties(const char *gatename) const;
    virtual cProperties *subcomponentProperties(const char *subcomponentname) const;
    virtual cProperties *subcomponentParamProperties(const char *subcomponentname, const char *paramname) const;
    virtual cProperties *subcomponentGateProperties(const char *subcomponentname, const char *gatename) const;

};

#endif


