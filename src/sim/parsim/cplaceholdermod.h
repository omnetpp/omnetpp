//==========================================================================
//  CPLACEHOLDERMOD.H  -  header for
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CPLACEHOLDERMOD_H
#define __OMNETPP_CPLACEHOLDERMOD_H

#include "omnetpp/cmodule.h"
#include "omnetpp/csimulation.h"

namespace omnetpp {

/**
 * @brief In distributed parallel simulation, modules of the network
 * are distributed across partitions.
 *
 * Represents a module which was instantiated on a remote partition.
 *
 * @ingroup Parsim
 */
class SIM_API cPlaceholderModule : public cModule // so, noncopyable
{
  protected:
    // internal: "virtual ctor" for cGate: creates cProxyGate
    virtual cGate *createGateObject(cGate::Type type) override;

    // placeholder modules have no submodules
    virtual void doBuildInside() override {}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor. Note that module objects should not be created directly,
     * only via their cModuleType objects. See cModule constructor for more info.
     */
    cPlaceholderModule() {}

    /**
     * Destructor.
     */
    virtual ~cPlaceholderModule() {}
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;
    //@}

    /** @name Redefined cModule functions */
    //@{
    /**
     * Redefined to return true.
     */
    virtual bool isPlaceholder() const override  {return true;}

    /**
     * Not implemented: throws an exception when called.
     */
    virtual void arrived(cMessage *msg, cGate *ongate, const SendOptions& options, simtime_t t) override;

    /**
     * Does nothing.
     */
    virtual void scheduleStart(simtime_t t) override;
    //@}
};


}  // namespace omnetpp


#endif

