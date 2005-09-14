//==========================================================================
//   CPLACEHOLDERMOD.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPLACEHOLDERMOD_H
#define __CPLACEHOLDERMOD_H

#include "cmodule.h"
#include "csimul.h"

/**
 * In distributed parallel simulation, modules of the network
 * are distributed across partitions.
 *
 * Represents a module which was instantiated on a remote partition.
 *
 * @ingroup Parsim
 */
class SIM_API cPlaceHolderModule : public cModule
{
  protected:
    // internal: "virtual ctor" for cGate: creates cProxyGate
    virtual cGate *createGateObject(const char *gname, char tp);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cPlaceHolderModule(const cPlaceHolderModule& mod);

    /**
     * Constructor. Note that module objects should not be created directly,
     * only via their cModuleType objects. See cModule constructor for more info.
     */
    cPlaceHolderModule();

    /**
     * Destructor.
     */
    virtual ~cPlaceHolderModule();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cPlaceHolderModule& operator=(const cPlaceHolderModule& mod);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPolymorphic *dup() const   {return new cPlaceHolderModule(*this);}
    //@}

    /** @name Redefined cModule functions */
    //@{

    /**
     * Not implemented: throws an exception when called.
     */
    virtual void arrived(cMessage *msg,int n,simtime_t t);

    /**
     * Does nothing.
     */
    virtual bool callInitialize(int stage);

    /**
     * Does nothing.
     */
    virtual void callFinish();

    /**
     * Does nothing.
     */
    virtual void scheduleStart(simtime_t t);
    //@}
};


#endif

