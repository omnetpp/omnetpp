//==========================================================================
//   CDYNAMICMODULE.H -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDYNAMICMODULE_H
#define __CDYNAMICMODULE_H

#include "cmodule.h"
#include "ctypes.h"

#include "cnednetworkbuilder.h"


/**
 * How does it work?
 *
 * - before network setup, we read some NED files, and register
 *   the modules types and network types in them (by creating cDynamicModuleType
 *   objects and adding them to the 'networktypes' list).
 *
 * - then we can instantiate _any_ network: it may contain module types that were
 *   read in dynamically, it'll work without problem.

  FIXME put this comment somewhere...
 */

/**
 * Special cModuleType: takes all info from cNEDNetworkBuilder
 */
class cDynamicModuleType : public cModuleType
{
  protected:
    CompoundModuleNode *modulenode;  // contains NEDElement tree

    virtual cModule *createModuleObject(const char *modname, cModule *parentmod);
    virtual void addParametersGatesTo(cModule *mod);

  public:
    cDynamicModuleType(const char *name, CompoundModuleNode *moduleNode);
    virtual ~cDynamicModuleType();

    /**
     * FIXME comment
     */
    virtual void buildInside(cModule *module);

    /**
     * Returns NULL. FIXME more comment
     */
    virtual cModuleInterface *moduleInterface();
};


/**
 *
 */
class cDynamicCompoundModule : public cCompoundModule
{
    friend cDynamicModuleType;
  protected:
    /**
     * FIXME comment
     */
    void doBuildInside();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor. Note that module objects should not be created directly,
     * only via their cModuleType objects. See cModule constructor for more info.
     */
    cDynamicCompoundModule(const char *name, cModule *parentmod);

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
    virtual cObject *dup() const   {return new cDynamicCompoundModule(*this);}
    //@}

};


#endif


