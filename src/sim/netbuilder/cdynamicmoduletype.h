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

#include "cnednetworkbuilder.h"


/**
 * NEDXML-based cModuleType: takes all info from cNEDResourceCache
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

  public:
    cDynamicModuleType(const char *name);
};


/**
 *
 */
//FIXME clarify
//FIXME do we need such channel too?
//FIXME do subclassed simple modules need to lie about their classname too?
class cDynamicCompoundModule : public cCompoundModule
{
    friend class cDynamicCompoundModuleType;
  protected:
    void doBuildInside();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor. Note that module objects should not be created directly,
     * only via their cModuleType objects. See cModule constructor for more info.
     */
    cDynamicCompoundModule();
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * We lie about the class name here, and return the module type instead
     * (which is the name of the cModuleType object).
     */
    virtual const char *className() const;
    //@}
};


#endif


