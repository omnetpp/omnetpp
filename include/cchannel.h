//==========================================================================
//   CCHANNEL.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cChannel : channel class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCHANNEL_H
#define __CCHANNEL_H

#include "ccomponent.h"
#include "ccomponenttype.h"

class cGate;
class cMessage;


/**
 * Base class for channels. FIXME docu!!!!
 *
 * @ingroup SimCore
 */
class SIM_API cChannel : public cComponent //noncopyable
{
  protected:
    cGate *fromgatep;  // gate the channel is attached to
    // TBD: add on/off modelling? or to cGate?

  public:
    // internal: called from cGate
    void setFromGate(cGate *g) {fromgatep=g;}

  public:
    /** @name Constructors, destructor */
    //@{
    /**
     * Constructor.
     */
    explicit cChannel(const char *name=NULL);

    /**
     * Destructor.
     */
    virtual ~cChannel();
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of object contents.
     * See cOwnedObject for more details.
     */
    virtual std::string info() const;

    /**
     * Calls v->visit(this) for each contained object.
     * See cOwnedObject for more details.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Serializes the object into a buffer.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a buffer.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Public methods for invoking initialize()/finish(), redefined from cComponent.
     * initialize(), numInitStages(), and finish() are themselves also declared in
     * cComponent, and can be redefined in channel classes by the user to perform
     * initialization and finalization (result recording, etc) tasks.
     */
    //@{
    /**
     * Interface for calling initialize() from outside.
     */
    virtual void callInitialize();

    /**
     * Interface for calling initialize() from outside.
     */
    virtual bool callInitialize(int stage);

    /**
     * Interface for calling finish() from outside.
     */
    virtual void callFinish();
    //@}

    /** @name Channel information. */
    //@{
    /**
     * Returns the compound module containing this channel. That is,
     * the channel is either between two submodules of parentModule(),
     * or between parentModule() and one of its submodules.
     * (For completeness, it may also connect two gates of parentModule()
     * on the inside).
     */
    virtual cModule *parentModule() const;

    /**
     * Convenience method: casts the return value of componentType() to cChannelType.
     */
    cChannelType *channelType() const  {return (cChannelType *)componentType();}

    /**
     * Returns the gate this channel is attached to.
     */
    cGate *fromGate() const  {return fromgatep;}
    //@}

    /** @name Internally used methods. */
    //@{
    /**
     * This function is called internally by the simulation kernel for
     * transmission modelling. A return value of false means that the message
     * object should be deleted by the caller; this can be used to model that
     * the message gets lost in the channel.
     */
    virtual bool deliver(cMessage *msg, simtime_t at);
    //@}
};

#endif


