//==========================================================================
//  CDYNAMICMODULE.H - part of
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

#ifndef __CDYNAMICMODULE_H
#define __CDYNAMICMODULE_H

#include "cmodule.h"
#include "ctypes.h"

#include "cnednetworkbuilder.h"


/**
 * Special cModuleType: takes all info from cNEDNetworkBuilder
 */
class cDynamicModuleType : public cModuleType
{
  protected:
    CompoundModuleNode *modulenode;  // contains NEDElement tree

    virtual cModule *createModuleObject();

  public:
    cDynamicModuleType(const char *name, CompoundModuleNode *moduleNode);
    virtual ~cDynamicModuleType();

    /**
     * Build submodules and connections inside the module passed,
     * based on info in the stored CompoundModuleNode.
     */
    virtual void buildInside(cModule *module);
};


/**
 *
 */
class cDynamicCompoundModule : public cCompoundModule
{
    friend class cDynamicModuleType;
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

    /**
     * Copy constructor.
     */
    cDynamicCompoundModule(const cDynamicCompoundModule& mod);

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cDynamicCompoundModule& operator=(const cDynamicCompoundModule& mod);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * We lie about the class name here, and return the module type instead
     * (which is the name of the cModuleType object).
     */
    virtual const char *className() const;

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPolymorphic *dup() const   {return new cDynamicCompoundModule(*this);}
    //@}
};


#endif


