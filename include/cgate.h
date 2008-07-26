//==========================================================================
//   CGATE.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cGate       : module gate
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CGATE_H
#define __CGATE_H

#include <set>
#include <map>
#include "cobject.h"
#include "cstringpool.h"
#include "opp_string.h"
#include "simtime_t.h"

NAMESPACE_BEGIN

class cGate;
class cModule;
class cMessage;
class cChannelType;
class cChannel;
class cProperties;
class cDisplayString;
class cIdealChannel;
class cDatarateChannel;


//
// internal: gateId bitfield macros.
// See note in cgate.cc
//
#define GATEID_LBITS  20
#define GATEID_HBITS  (8*sizeof(int)-GATEID_LBITS)   // usually 12
#define GATEID_HMASK  ((~0)<<GATEID_LBITS)           // usually 0xFFF00000
#define GATEID_LMASK  (~GATEID_HMASK)                // usually 0x000FFFFF

#define MAX_VECTORGATES  ((1<<(GATEID_HBITS-1))-2)   // usually 2046
#define MAX_SCALARGATES  ((1<<(GATEID_LBITS-1))-2)   // usually ~500000
#define MAX_VECTORGATESIZE ((1<<(GATEID_LBITS-1))-1) // usually ~500000

/**
 * Represents a module gate. cGate object are created and managed by modules;
 * the user typically does not want to directly create or destroy cGate
 * objects. However, they are important if a simple module algorithm
 * needs to know about its surroundings.
 *
 * @ingroup SimCore
 */
class SIM_API cGate : public cObject, noncopyable
{
    friend class cModule;
    friend class cModuleGates;
    friend class cPlaceholderModule;

  public:
    /**
     * Gate type
     */
    enum Type {
        NONE = 0,
        INPUT = 'I',
        OUTPUT = 'O',
        INOUT = 'B'
    };

  protected:
    // internal
    struct SIM_API Name
    {
        opp_string name;  // "foo"
        opp_string namei; // "foo$i"
        opp_string nameo; // "foo$o"
        Type type;
        Name(const char *name, Type type);
        bool operator<(const Name& other) const;
    };

  public:
    // Internal data structure, only public for technical reasons (GateIterator).
    // One instance per module and per gate vector/gate pair/gate.
    // Note: gate name and type are factored out to a global pool.
    // Note2: to reduce sizeof(Desc), "size" might be stored in inputgatev[0],
    // although it might not be worthwhile the extra complication and CPU cycles.
    //
    struct Desc
    {
        cModule *ownerp;
        Name *namep;  // pooled
        int size; // gate vector size, or -1 if scalar gate; actually allocated size is capacityFor(size)
        union { cGate *inputgate; cGate **inputgatev; };
        union { cGate *outputgate; cGate **outputgatev; };

        Desc() {ownerp=NULL; size=-1; namep=NULL; inputgate=outputgate=NULL;}
        bool inUse() const {return namep!=NULL;}
        Type getType() const {return namep->type;}
        bool isVector() const {return size>=0;}
        const char *nameFor(Type t) const {return (t==INOUT||namep->type!=INOUT) ? namep->name.c_str() : t==INPUT ? namep->namei.c_str() : namep->nameo.c_str();}
        int indexOf(const cGate *g) const {return (g->pos>>2)==-1 ? 0 : g->pos>>2;}
        bool deliverOnReceptionStart(const cGate *g) const {return g->pos&2;}
        Type getTypeOf(const cGate *g) const {return (g->pos&1)==0 ? INPUT : OUTPUT;}
        bool isInput(const cGate *g) const {return (g->pos&1)==0;}
        bool isOutput(const cGate *g) const {return (g->pos&1)==1;}
        int gateSize() const {return size>=0 ? size : 1;}
        void setInputGate(cGate *g) {ASSERT(getType()!=OUTPUT && !isVector()); inputgate=g; g->desc=this; g->pos=(-1<<2);}
        void setOutputGate(cGate *g) {ASSERT(getType()!=INPUT && !isVector()); outputgate=g; g->desc=this; g->pos=(-1<<2)|1;}
        void setInputGate(cGate *g, int index) {ASSERT(getType()!=OUTPUT && isVector()); inputgatev[index]=g; g->desc=this; g->pos=(index<<2);}
        void setOutputGate(cGate *g, int index) {ASSERT(getType()!=INPUT && isVector()); outputgatev[index]=g; g->desc=this; g->pos=(index<<2)|1;}
        static int capacityFor(int size) {return size<8 ? (size+1)&~1 : size<32 ? (size+3)&~3 : size<256 ? (size+15)&~15 : (size+63)&~63;}
    };

  protected:
    Desc *desc; // descriptor of gate/gate vector, stored in cModule
    int pos;    // b0: input(0) or output(1); b1: deliverOnReceptionStart bit;
                // rest (pos>>2): array index, or -1 if scalar gate

    cChannel *channelp; // channel object (if exists)
    cGate *fromgatep;   // previous and next gate in the path
    cGate *togatep;

  protected:
    // internal: constructor is protected because only cModule is allowed to create instances
    explicit cGate();

    // also protected: only cModule is allowed to delete gates
    virtual ~cGate();

    // internal
    static void clearFullnamePool();

    // internal
    void installChannel(cChannel *chan);

    // internal
    void checkChannels() const;

  public:
    /** @name Redefined cObject member functions */
    //@{
    /**
     * Returns the name of the the gate without the gate index in brackets.
     */
    virtual const char *getName() const;

    /**
     * Returns the full name of the gate, which is getName() plus the
     * index in square brackets (e.g. "out[4]"). Redefined to add the
     * index.
     */
    virtual const char *getFullName() const;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Produces a one-line description of object contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Returns the owner module of this gate.
     */
    virtual cObject *getOwner() const;
    //@}

    /**
     * This function is called internally by the send() functions and
     * channel classes' deliver() to deliver the message to its destination.
     * A false return value means that the message object should be deleted
     * by the caller. (This is used e.g. with parallel simulation, for
     * messages leaving the partition.)
     */
    virtual bool deliver(cMessage *msg, simtime_t at);

    /** @name Connecting the gate. */
    //@{
    /**
     * Connects the gate to another gate, using the given channel object
     * (if one is specified). This method can be used to manually create
     * connections for dynamically created modules.
     *
     * This method invokes callInitialize() on the channel object, unless the
     * compound module containing this connection is not yet initialized
     * (then it assumes that this channel will be initialized as part of the
     * compound module initialization process.) To leave the channel
     * uninitialized, specify true for the leaveUninitialized parameter.
     *
     * If the gate is already connected, an error will occur. The gate
     * argument cannot be NULL, that is, you cannot use this function
     * to disconnect a gate; use disconnect() for that.
     *
     * Note: When you set channel parameters are after channel initialization,
     * make sure the channel class is implemented so that the changes take
     * effect; i.e. the channel should either override and properly handle
     * handleParameterChange(), or should not cache any values from parameters.
     */
    cChannel *connectTo(cGate *gate, cChannel *channel=NULL, bool leaveUninitialized=false);

    /**
     * Disconnects the gate, and also deletes the associated channel object
     * if one has been set. disconnect() must be invoked on the source gate
     * ("from" side) of the connection.
     *
     * The method has no effect if the gate is not connected.
     */
    void disconnect();

    /**
     * Disconnects the gate, then connects it again to the same gate, with the
     * given channel object (if not NULL). The gate must be connected.
     *
     * @see connectTo()
     */
    cChannel *reconnectWith(cChannel *channel, bool leaveUninitialized=false);

    /**
     * DEPRECATED: Use reconnectWith() instead.
     */
    _OPPDEPRECATED void setChannel(cChannel *channel) {reconnectWith(channel);}
    //@}

    /** @name Information about the gate. */
    //@{
    /**
     * Returns the gate name without index and potential "$i"/"$o" suffix.
     */
    const char *getBaseName() const;

    /**
     * Returns the properties for this gate. Properties cannot be changed
     * at runtime.
     */
    cProperties *getProperties() const;

    /**
     * Returns the gate's type, cGate::INPUT or cGate::OUTPUT. (It never returns
     * cGate::INOUT, because a cGate object is always either the input or
     * the output half of an inout gate ("name$i" or "name$o").
     */
    Type getType() const  {return desc->getTypeOf(this);}

    /**
     * Returns a pointer to the owner module of the gate.
     */
    cModule *getOwnerModule() const;

    /**
     * Returns the gate ID, which uniquely identifies the gate within the
     * module. IDs are guaranteed to be contiguous within a gate vector:
     * <tt>module->gate(id+index) == module->gate(id)+index</tt>.
     *
     * Gate IDs are stable: they are guaranteed not to change during
     * simulation. (This is a new feature of \opp 4.0. In earlier releases,
     * gate IDs could change when the containing gate vector was resized.)
     *
     * Note: As of \opp 4.0, gate IDs are no longer small integers, and
     * cannot be used for iterating over the gates of a module.
     * Use cModule::GateIterator for iteration.
     */
    int getId() const;

    /**
     * Returns true if the gate is part of a gate vector.
     */
    bool isVector() const  {return desc->isVector();}

    /**
     * If the gate is part of a gate vector, returns the gate's index in the vector.
     * Otherwise, it returns 0.
     */
    int getIndex() const  {return desc->indexOf(this);}

    /**
     * If the gate is part of a gate vector, returns the size of the vector.
     * For non-vector gates it returns 1.
     *
     * The gate vector size can also be obtained by calling the cModule::gateSize().
     */
    int getVectorSize() const  {return desc->gateSize();}

    /**
     * Alias for getVectorSize().
     */
    int size() const  {return getVectorSize();}

    /**
     * Returns the channel object attached to this gate, or NULL if there's
     * no channel. This is the channel between this gate and this->getToGate(),
     * that is, channels are stored on the "from" side of the connections.
     */
    cChannel *getChannel() const  {return channelp;}

    /**
     * This method may only be invoked on input gates of simple modules.
     * Messages with nonzero length then have a nonzero
     * transmission duration (and thus, reception duration on the other
     * side of the connection). By default, the delivery of the message
     * to the module marks the end of the reception. Setting this bit will cause
     * the channel to deliver the message to the module at the start of the
     * reception. The duration that the reception will take can be extracted
     * from the message object, by its getDuration() method.
     */
    void setDeliverOnReceptionStart(bool d);

    /**
     * Returns whether messages delivered through this gate will mark the
     * start or the end of the reception process (assuming nonzero message length
     * and data rate on the channel.)
     *
     * @see setDeliverOnReceptionStart()
     */
    bool getDeliverOnReceptionStart() const  {return pos&2;}
    //@}

    /** @name Transmission state. */
    //@{
    /**
     * Usually invoked on an output gate, this method returns <i>the</i>
     * channel in the connection path that supports datarate (as determined
     * by cChannel::supportsDatarate(); it is guaranteed that there can be
     * at most one such channel per path). If there is no suh channel,
     * an error is thrown.
     *
     * This method only checks the segment of the connection path that
     * <i>starts</i> at this gate, so, for example, it is an error to invoke
     * it on a simple module input gate.
     *
     * Note: this method searches the connection path linearly, so at
     * performance-critical places it may be better to cache its return
     * value (provided that connections are not removed or created dynamically
     * during simulation.)
     */
    cChannel *getDatarateChannel() const;

    /**
     * Usually only meaningful on an output gate, this method returns
     * whether the datarate channel in the connection path starting at
     * this gate is currently transmitting. If there is no datarate channel
     * in the path, this method throws an error.
     *
     * It is equivalent to <tt>getDatarateChannel()->isBusy()</tt>.
     */
    bool isBusy() const;

    /**
     * Usually only meaningful on an output gate, this method returns
     * when the datarate channel in the connection path starting at
     * this gate will finish transmitting. If there is no datarate channel
     * in the path, this method throws an error.
     *
     * It is equivalent to <tt>getDatarateChannel()->getTransmissionFinishTime()</tt>.
     */
    simtime_t getTransmissionFinishTime() const;
    //@}

    /** @name Gate connectivity. */
    //@{

    /**
     * Returns the previous gate in the series of connections (the path) that
     * contains this gate, or a NULL pointer if this gate is the first one in the path.
     * (E.g. for a simple module output gate, this function will return NULL.)
     */
    cGate *getFromGate() const {return fromgatep;}

    /**
     * Returns the next gate in the series of connections (the path) that
     * contains this gate, or a NULL pointer if this gate is the last one in the path.
     * (E.g. for a simple module input gate, this function will return NULL.)
     */
    cGate *getToGate() const   {return togatep;}

    /**
     * Return the ultimate source of the series of connections
     * (the path) that contains this gate.
     */
    cGate *getSourceGate() const;

    /**
     * Return the ultimate destination of the series of connections
     * (the path) that contains this gate.
     */
    cGate *getDestinationGate() const;

    /**
     * Determines if a given module is in the path containing this gate.
     */
    bool pathContains(cModule *module, int gateId=-1);

    /**
     * Returns true if the gate is connected outside (i.e. to one of its
     * sibling modules or to the parent module).
     *
     * This means that for an input gate, getFromGate() must be non-NULL; for an output
     * gate, getToGate() must be non-NULL.
     */
    bool isConnectedOutside() const;

    /**
     * Returns true if the gate (of a compound module) is connected inside
     * (i.e. to one of its submodules).
     *
     * This means that for an input gate, getToGate() must be non-NULL; for an output
     * gate, getFromGate() must be non-NULL.
     */
    bool isConnectedInside() const;

    /**
     * Returns true if the gate fully connected. For a compound module gate
     * this means both isConnectedInside() and isConnectedOutside() are true;
     * for a simple module, only isConnectedOutside() is checked.
     */
    bool isConnected() const;

    /**
     * Returns true if the path (chain of connections) containing this gate
     * starts and ends at a simple module.
     */
    bool isPathOK() const;
    //@}

    /** @name Display string. */
    //@{
    /**
     * Returns the display string for the gate, which controls the appearance
     * of the connection arrow starting from gate. The display string is stored
     * in the channel associated with the connection. If there's no channel,
     * this call creates an installs a cIdealChannel to hold the display string.
     */
    cDisplayString& getDisplayString();

    /**
     * Shortcut to <tt>getDisplayString().set(dispstr)</tt>.
     */
    void setDisplayString(const char *dispstr);
    //@}
};

NAMESPACE_END

#endif

