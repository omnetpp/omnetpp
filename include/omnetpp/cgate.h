//==========================================================================
//   CGATE.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CGATE_H
#define __OMNETPP_CGATE_H

#include <set>
#include <map>
#include "cobject.h"
#include "opp_string.h"
#include "simtime_t.h"
#include "cexception.h"

namespace omnetpp {

class cGate;
class cModule;
class cMessage;
class cChannelType;
class cChannel;
class cProperties;
class cDisplayString;
class cIdealChannel;
class cDatarateChannel;
struct SendOptions;

//
// internal: gateId bitfield macros.
// See note in cgate.cc
//
#define GATEID_LBITS  20
#define GATEID_HBITS  (8*sizeof(int)-GATEID_LBITS)   // default 12
#define GATEID_HMASK  ((~0U)<<GATEID_LBITS)          // default 0xFFF00000
#define GATEID_LMASK  (~GATEID_HMASK)                // default 0x000FFFFF

#define MAX_VECTORGATES  ((1<<(GATEID_HBITS-1))-2)   // default 2046
#define MAX_SCALARGATES  ((1<<(GATEID_LBITS-1))-2)   // default ~500000
#define MAX_VECTORGATESIZE ((1<<(GATEID_LBITS-1))-1) // default ~500000

/**
 * @brief Represents a module gate.
 *
 * cGate object are created and managed by modules; the user typically
 * does not want to directly create or destroy cGate objects. However,
 * they are important if a simple module algorithm needs to know about
 * its surroundings.
 *
 * @ingroup ModelComponents
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
    // Note2: to reduce sizeof(Desc), "size" might be stored in input.gatev[0],
    // although it might not be worthwhile the extra complication and CPU cycles.
    //
    struct Desc
    {
        cModule *owner;
        Name *name;  // pooled (points into cModule::namePool)
        int vectorSize; // gate vector size, or -1 if scalar gate; actually allocated size is capacityFor(size)
        union Gates { cGate *gate; cGate **gatev; };
        Gates input;
        Gates output;

        Desc() {owner=nullptr; vectorSize=-1; name=nullptr; input.gate=output.gate=nullptr;}
        bool inUse() const {return name!=nullptr;}
        Type getType() const {return name->type;}
        bool isVector() const {return vectorSize>=0;}
        const char *nameFor(Type t) const {return (t==INOUT||name->type!=INOUT) ? name->name.c_str() : t==INPUT ? name->namei.c_str() : name->nameo.c_str();}
        int indexOf(const cGate *g) const {ASSERT(isVector()); return g->pos >> 2;}
        bool deliverOnReceptionStart(const cGate *g) const {return g->pos&2;}
        Type getTypeOf(const cGate *g) const {return (g->pos&1)==0 ? INPUT : OUTPUT;}
        bool isInput(const cGate *g) const {return (g->pos&1)==0;}
        bool isOutput(const cGate *g) const {return (g->pos&1)==1;}
        int gateSize() const {ASSERT(isVector()); return vectorSize;}
        void setInputGate(cGate *g) {ASSERT(getType()!=OUTPUT && !isVector()); input.gate=g; g->desc=this; g->pos=(-(1<<2));}
        void setOutputGate(cGate *g) {ASSERT(getType()!=INPUT && !isVector()); output.gate=g; g->desc=this; g->pos=(-(1<<2))|1;}
        void setInputGate(cGate *g, int index) {ASSERT(getType()!=OUTPUT && isVector()); input.gatev[index]=g; g->desc=this; g->pos=(index<<2);}
        void setOutputGate(cGate *g, int index) {ASSERT(getType()!=INPUT && isVector()); output.gatev[index]=g; g->desc=this; g->pos=(index<<2)|1;}
        static int capacityFor(int size) {return size<8 ? (size+1)&~1 : size<32 ? (size+3)&~3 : size<256 ? (size+15)&~15 : (size+63)&~63;}
    };

  protected:
    Desc *desc = nullptr; // descriptor of the gate or gate vector, stored in cModule
    int pos = 0;          // b0: input(0) or output(1); b1: deliverOnReceptionStart bit;
                          // rest (pos>>2): array index, or -1 if scalar gate

    int connectionId = -1;       // uniquely identifies the connection between *this and *nextgatep; -1 if unconnected
    cChannel *channel = nullptr; // channel object (if exists)
    cGate *prevGate = nullptr;   // previous and next gate in the path
    cGate *nextGate = nullptr;

    static int lastConnectionId;

  protected:
    // internal: constructor is protected because only cModule is allowed to create instances
    explicit cGate() {}

    // also protected: only cModule is allowed to delete gates
    virtual ~cGate();

    // internal
    static void clearFullnamePool();

    // internal
    void installChannel(cChannel *chan);

    // internal
    void checkChannels() const;

#ifdef SIMFRONTEND_SUPPORT
    // internal
    virtual bool hasChangedSince(int64_t lastRefreshSerial);
#endif

  public:
    /** @name Redefined cObject member functions */
    //@{
    /**
     * Returns the name of the the gate without the gate index in brackets.
     */
    virtual const char *getName() const override;

    /**
     * Returns the full name of the gate, which is getName() plus the
     * index in square brackets (e.g. "out[4]"). Redefined to add the
     * index.
     */
    virtual const char *getFullName() const override;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v) override;

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;

    /**
     * Returns the owner module of this gate.
     */
    virtual cObject *getOwner() const override; // note: cannot return cModule* (covariant return type) due to declaration order
    //@}

    /**
     * This function is called internally by the send() functions and
     * channel classes' deliver() to deliver the message to its destination.
     * A false return value means that the message object should be deleted
     * by the caller. (This is used e.g. with parallel simulation, for
     * messages leaving the partition.)
     */
    virtual bool deliver(cMessage *msg, const SendOptions& options, simtime_t at);

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
     * argument cannot be nullptr, that is, you cannot use this function
     * to disconnect a gate; use disconnect() for that.
     *
     * Note: When you set channel parameters after channel initialization,
     * make sure the channel class is implemented so that the changes take
     * effect; i.e. the channel should either override and properly handle
     * handleParameterChange(), or should not cache any values from parameters.
     */
    cChannel *connectTo(cGate *gate, cChannel *channel=nullptr, bool leaveUninitialized=false);

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
     * given channel object (if not nullptr). The gate must be connected.
     *
     * @see connectTo()
     */
    cChannel *reconnectWith(cChannel *channel, bool leaveUninitialized=false);
    //@}

    /** @name Information about the gate. */
    //@{
    /**
     * Returns true if this gate is half of an "inout gate" (which is made up
     * of an input and an output gate object). The name of such a gate includes
     * the "$i" (for the input half) or "$o" (for the output half) suffix.
     *
     * @see getBaseName(), getNameSuffix(), getOtherHalf()
     */
    bool isGateHalf() const;

    /**
     * If this gate is one half of an "inout gate" (see isGateHalf()), this method
     * returns the other half. For example, for a gate named "eth$o[5]" it returns
     * the gate "eth$i[5]" and vice versa; otherwise it returns nullptr.
     */
    cGate *getOtherHalf() const;

    /**
     * Returns the gate name without index and potential "$i"/"$o" suffix.
     */
    const char *getBaseName() const;

    /**
     * Returns the suffix part of the gate name ("$i", "$o" or "").
     */
    const char *getNameSuffix() const;

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
     * Returns the given type as a string.
     */
    static const char *getTypeName(Type t);

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
     * If the gate is part of a gate vector, returns the ID of the first
     * element in the gate vector. Otherwise, it returns the gate's ID.
     */
    int getBaseId() const;

    /**
     * If the gate is part of a gate vector, returns the gate's index in the vector.
     * For non-vector gates it throws an error.
     */
    int getIndex() const;

    /**
     * If the gate is part of a gate vector, returns the size of the vector.
     * For non-vector gates it throws an error.
     *
     * @see cModule::gateSize()
     */
    int getVectorSize() const;

    /**
     * Alias for getVectorSize().
     */
    int size() const  {return getVectorSize();}

    /**
     * Returns the channel object attached to this gate, or nullptr if there is
     * no channel. This is the channel between this gate and this->getNextGate(),
     * that is, channels are stored on the "from" side of the connections.
     */
    cChannel *getChannel() const  {return channel;}

    /**
     * This method may only be invoked on input gates of simple modules,
     * and sets the packet reception mode. The two packet reception modes are
     * the default a.k.a. at-end mode (arg=false), and immediate mode (arg=true).
     * See getDeliverImmediately() for the description of their operations.
     *
     * @see getDeliverImmediately()
     */
    void setDeliverImmediately(bool d);

    /**
     * Indicates reception mode for messages with nonzero transmission duration,
     * i.e. packets. The default setting is false.
     *
     * In the default a.k.a. at-end mode (retval=false), packets delivered to
     * this gate are handed to the module when they are completely received.
     * That is, the arrival time of the message time will include both the
     * propagation delay and the transmission duration. For transmission
     * update packets (where cPacket::isUpdate()==true), this mode ensures
     * that the module only receives the final transmission update
     * (the original packet and intermediate transmission updates will
     * be swallowed by the simulation kernel). The duration of the packet
     * can be obtained from the cPacket::getDuration() method. The
     * cPacket::getRemainingDuration() method will return zero.
     *
     * In immediate mode (retval=true), packets and transmission updates
     * will be delivered to the module "immediately", after applying the
     * propagation delay only, and excluding the transmission duration.
     * The remaining transmission duration can be obtained from the
     * cPacket::getRemainingDuration() method. The total transmission duration
     * of the packet can be obtained from the cPacket::getDuration() method.
     * For non-update packets (cPacket::isUpdate()==false), the two methods
     * will return the same value.
     *
     * @see setDeliverImmediately()
     */
    bool getDeliverImmediately() const  {return pos&2;}

    /**
     * Renamed to setDeliverImmediately() -- please use the new name.
     */
    [[deprecated("Renamed to setDeliverImmediately() -- please use the new name")]]
    void setDeliverOnReceptionStart(bool d) {setDeliverImmediately(d);}

    /**
     * Renamed to getDeliverImmediately() -- please use the new name.
     */
    [[deprecated("Renamed to getDeliverImmediately() -- please use the new name")]]
    bool getDeliverOnReceptionStart() const  {return getDeliverImmediately();}
    //@}

    /** @name Transmission state. */
    //@{
    /**
     * Typically invoked on an output gate, this method returns <i>the</i>
     * channel in the connection path that supports datarate (as determined
     * by cChannel::isTransmissionChannel(); it is guaranteed that there can be
     * at most one such channel per path). If there is no such channel,
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
     *
     * @see cChannel::isTransmissionChannel()
     */
    cChannel *getTransmissionChannel() const;

    /**
     * Like getTransmissionChannel(), but returns nullptr instead of throwing
     * an error if there is no transmission channel in the path.
     */
    cChannel *findTransmissionChannel() const;

    /**
     * Typically invoked on an input gate, this method searches the reverse
     * path (i.e. calls getPreviousGate() repeatedly) for the transmission
     * channel. It is guaranteed that there can be at most one such channel
     * per path. If no transmission channel is found, the method throws an error.
     *
     * @see getTransmissionChannel(), cChannel::isTransmissionChannel()
     */
    cChannel *getIncomingTransmissionChannel() const;

    /**
     * Like getIncomingTransmissionChannel(), but returns nullptr instead of
     * throwing an error if there is no transmission channel in the reverse
     * path.
     */
    cChannel *findIncomingTransmissionChannel() const;
    //@}

    /** @name Gate connectivity. */
    //@{

    /**
     * Returns the previous gate in the series of connections (the path) that
     * contains this gate, or nullptr if this gate is the first one in the path.
     * (E.g. for a simple module output gate, this function will return nullptr.)
     */
    cGate *getPreviousGate() const {return prevGate;}

    /**
     * Returns the next gate in the series of connections (the path) that
     * contains this gate, or nullptr if this gate is the last one in the path.
     * (E.g. for a simple module input gate, this function will return nullptr.)
     */
    cGate *getNextGate() const   {return nextGate;}

    /**
     * Returns an ID that uniquely identifies the connection between this gate
     * and the next gate in the path (see getNextGate()) during the lifetime of
     * the simulation. (Disconnecting and then reconnecting the gate results
     * in a new connection ID being assigned.) The method returns -1 if the gate
     * is unconnected.
     */
    int getConnectionId() const  {return connectionId;}

    /**
     * Return the ultimate source of the series of connections
     * (the path) that contains this gate.
     */
    cGate *getPathStartGate() const;

    /**
     * Return the ultimate destination of the series of connections
     * (the path) that contains this gate.
     */
    cGate *getPathEndGate() const;

    /**
     * Determines if a given module is in the path containing this gate.
     */
    bool pathContains(cModule *module, int gateId=-1);

    /**
     * Returns true if the gate is connected outside (i.e. to one of its
     * sibling modules or to the parent module).
     *
     * This means that for an input gate, getPreviousGate() must be non-nullptr; for an output
     * gate, getNextGate() must be non-nullptr.
     */
    bool isConnectedOutside() const;

    /**
     * Returns true if the gate (of a compound module) is connected inside
     * (i.e. to one of its submodules).
     *
     * This means that for an input gate, getNextGate() must be non-nullptr; for an output
     * gate, getPreviousGate() must be non-nullptr.
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
     * in the channel associated with the connection. If there is no channel,
     * this call creates an installs a cIdealChannel to hold the display string.
     */
    cDisplayString& getDisplayString();

    /**
     * Shortcut to <tt>getDisplayString().set(dispstr)</tt>.
     */
    void setDisplayString(const char *dispstr);
    //@}
};

}  // namespace omnetpp

#endif
