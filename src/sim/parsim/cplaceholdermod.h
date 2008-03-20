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
#include "csimulation.h"

NAMESPACE_BEGIN

/**
 * In distributed parallel simulation, modules of the network
 * are distributed across partitions.
 *
 * Represents a module which was instantiated on a remote partition.
 *
 * @ingroup Parsim
 */
class SIM_API cPlaceHolderModule : public cModule // so, noncopyable
{
  protected:
    // internal: "virtual ctor" for cGate: creates cProxyGate
    virtual cGate *createGateObject(cGate::Desc *desc);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor. Note that module objects should not be created directly,
     * only via their cModuleType objects. See cModule constructor for more info.
     */
    cPlaceHolderModule();

    /**
     * Destructor.
     */
    virtual ~cPlaceHolderModule();
    //@}


    //FIXME XXX override className() like in cDynamicModule(), to return NED type!
    //XXX also add isPlaceHolder() then? otherwise it's indistinguishable from "normal" modules!


    /** @name Redefined cModule functions */
    //@{

    /**
     * Not implemented: throws an exception when called.
     */
    virtual void arrived(cMessage *msg,int n,simtime_t t);

    /**
     * Does nothing.
     */
    virtual void scheduleStart(simtime_t t);
    //@}
};


NAMESPACE_END


#endif

