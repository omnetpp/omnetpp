//==========================================================================
//   CCOMPOUNDMODULE.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCOMPOUNDMODULE_H
#define __CCOMPOUNDMODULE_H

#include "cmodule.h"


/**
 * Represents a compound module in the simulation.
 *
 * @ingroup SimCore
 */
class SIM_API cCompoundModule : public cModule // noncopyable
{
    friend class TCompoundModInspector;

  protected:
    // internal use
    virtual void arrived(cMessage *msg,int n,simtime_t t);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor. Note that module objects should not be created directly,
     * only via their cModuleType objects. See cModule constructor for more info.
     */
    cCompoundModule();

    /**
     * Destructor.
     */
    virtual ~cCompoundModule();
    //@}

    /** @name Redefined cObject functions. */
    //@{
    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual std::string info() const;
    //@}

    /** @name Redefined cModule functions. */
    //@{

    /**
     * Calls scheduleStart() recursively for all its (immediate)
     * submodules. This is used with dynamically created modules.
     */
    virtual void scheduleStart(simtime_t t);
    //@}
};

#endif

