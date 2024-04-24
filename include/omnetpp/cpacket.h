//==========================================================================
//   CPACKET.H  -  header for
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

#ifndef __OMNETPP_CPACKET_H
#define __OMNETPP_CPACKET_H

#include "cmessage.h"

namespace omnetpp {


/**
 * @brief A subclass of cMessage to represent packets, frames, datagrams,
 * application messages, and similar data.
 *
 * cPacket adds length (measured in bits or bytes), bit error flag, and
 * encapsulation capability to cMessage. Length and bit error flag are
 * significant when the packet travels through a cDatarateChannel or
 * another channel that supports data rate and/or error modelling.
 *
 * cPacket is rarely used "as is". Typically the user wants to subclass it
 * to create specific packet types for various protocols being modelled.
 * The most convenient way to do that are via message declaration files
 * (.msg), which are translated into C++ classes.
 * An example message declaration:
 *
 * ```
 * packet Datagram
 * {
 *     int destAddr = -1; // destination address
 *     int srcAddr = -1;  // source address
 *     int ttl =  32;     // time to live
 * }
 * ```
 *
 * @see cSimpleModule, cDatarateChannel, cPacketQueue
 *
 * @ingroup SimProgr
 */
class SIM_API cPacket : public cMessage
{
  private:
    enum {
        FL_BITERROR = 8, // has bit errors
        FL_TXCHANNELSEEN = 16, // encountered a transmission channel during its last send
        FL_ISUPDATE = 32, // if true, this packet is a transmission update
    };

    int64_t bitLength;    // length of the packet in bits -- used for bit error and transmission delay modeling
    simtime_t duration;   // transmission duration on last channel with datarate
    cPacket *encapsulatedPacket = nullptr; // ptr to the encapsulated message
    unsigned short shareCount = 0; // num of messages MINUS ONE that have this message encapsulated.
                                   // 0: not shared (not encapsulated or encapsulated in one message);
                                   // 1: shared once (shared among two messages);
                                   // 2: shared twice (shared among three messages); etc.
                                   // on reaching max sharecount a new packet gets created
    txid_t transmissionId = -1;  // for pairing transmission updates with the original transmission
    simtime_t remainingDuration; // if transmission update: remaining duration (otherwise it must be equal to the duration)

  private:
    void copy(const cPacket& packet);

  public:
    // internal: setters used from within send()/sendDirect() and channel code
    void setDuration(simtime_t d) {duration = d;}
    void setRemainingDuration(simtime_t d) {remainingDuration = d;}
    void setTransmissionId(txid_t id) {transmissionId = id;}
    void setIsUpdate(bool b) {setFlag(FL_ISUPDATE, b);}
    void setTxChannelEncountered(bool b) {setFlag(FL_TXCHANNELSEEN, b);}
    void clearTxChannelEncountered() {flags &= ~FL_TXCHANNELSEEN;}
    void setTxChannelEncountered() {flags |= FL_TXCHANNELSEEN;}

    // internal (for now)
    bool getTxChannelEncountered() const {return flags&FL_TXCHANNELSEEN;}

    // internal convenience method: returns the getId() of the innermost encapsulated message,
    // or itself if there is no encapsulated message
    msgid_t getEncapsulationId() const;

    // internal convenience method: returns getTreeId() of the innermost encapsulated message,
    // or itself if there is no encapsulated message
    msgid_t getEncapsulationTreeId() const;

    cPacket *_getEncapMsg() { return encapsulatedPacket; }

    // internal: if encapmsg is shared (sharecount>0), creates a private copy for this packet,
    // and in any case it sets encapmsg's owner to be this object. This method
    // has to be called before any operation on encapmsg, to prevent trouble
    // that may arise from accessing shared message instances. E.g. without calling
    // _detachEncapMsg(), encapmsg's ownerp is unpredictable (may be any previous owner,
    // possibly not even existing any more) which makes even a call to its getFullPath()
    // method dangerous.
    void _detachEncapMsg();

    // internal: delete encapmsg, paying attention to its sharecount (assumes encapmsg!=nullptr)
    void _deleteEncapMsg();

    // internal: only to be used by test cases
    int getShareCount() const {return shareCount;}

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Copy constructor.
     */
    cPacket(const cPacket& packet);

    /**
     * Constructor. It takes the packet name, message kind value, and the
     * packet length in bits; all optional.
     */
    explicit cPacket(const char *name=nullptr, short kind=0, int64_t bitLength=0);

    /**
     * Destructor
     */
    virtual ~cPacket();

    /**
     * Assignment operator. The name member is not copied; see cNamedObject::operator=() for details.
     */
    cPacket& operator=(const cPacket& packet);
    //@}

    /** @name Redefined cObject/cMessage member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPacket *dup() const override  {return new cPacket(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v) override;

    /**
     * Serializes the object into an MPI send buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;

    /**
     * Returns true.
     */
    virtual bool isPacket() const override {return true;}

    /**
     * Returns the encapsulated packet's display string if it has one,
     * otherwise returns the empty string. Override as needed.
     */
    virtual const char *getDisplayString() const override;
    //@}

    /** @name Length and bit error flag */
    //@{
    /**
     * Sets packet length (in bits). When the packet is sent through a
     * channel, packet length affects the transmission duration and the
     * probability of setting the bit error flag.
     */
    virtual void setBitLength(int64_t l);

    /**
     * Sets packet length (bytes). This is just a convenience function which
     * invokes setBitLength() with 8*l as argument. The caller must take care
     * that the result does not overflow (i.e. fits into an int64_t).
     */
    void setByteLength(int64_t l)  {setBitLength(l<<3);}

    /**
     * Changes packet length by the given value (bits). This is useful for
     * modeling encapsulation/decapsulation. (See also encapsulate() and
     * decapsulate().) The caller must take care that the result does not
     * overflow (i.e. fits into an int64_t).
     *
     * The value may be negative (packet length may be decreased too).
     * If the resulting length would be negative, the method throws a
     * cRuntimeError.
     */
    virtual void addBitLength(int64_t delta);

    /**
     * Changes packet length by the given value (bytes). This is just a
     * convenience function which invokes addBitLength() with 8*l as argument.
     * The caller must take care that the result does not overflow (i.e.
     * fits into an int64_t).
     */
    void addByteLength(int64_t delta)  {addBitLength(delta<<3);}

    /**
     * Returns the packet length (in bits).
     */
    virtual int64_t getBitLength() const  {return bitLength;}

    /**
     * Returns the packet length in bytes, that is, bitlength/8. If bitlength
     * is not a multiple of 8, the result is rounded up.
     */
    int64_t getByteLength() const  {return (getBitLength()+7)>>3;}

    /**
     * Sets the bit error flag.
     */
    virtual void setBitError(bool e) {setFlag(FL_BITERROR,e);}

    /**
     * Returns the bit error flag.
     */
    virtual bool hasBitError() const {return flags&FL_BITERROR;}
    //@}

    /** @name Message encapsulation. */
    //@{

    /**
     * Encapsulates packet in the packet. The packet length gets increased
     * by the length of the encapsulated packet.
     * 
     * Important: IT IS FORBIDDEN TO KEEP A POINTER TO A MESSAGE
     * AFTER IT WAS ENCAPSULATED. For performance reasons, encapsulated
     * packets are reference counted, meaning that the encapsulated
     * packet is not duplicated when you duplicate a packet, but rather,
     * both (all) copies share the same packet instance. Any change done
     * to the encapsulated packet would affect other packets as well.
     * Decapsulation (and even calling getEncapsulatedPacket()) will create an
     * own (non-shared) copy of the packet.
     */
    virtual void encapsulate(cPacket *packet);

    /**
     * Decapsulates a packet from the packet object. The length of
     * this packet will be decreased by the length of the encapsulated
     * packet, except if it was zero. If the length would become
     * negative, cRuntimeError is thrown. If there is no encapsulated
     * packet, the method returns nullptr.
     */
    virtual cPacket *decapsulate();

    /**
     * Returns a pointer to the encapsulated packet, or nullptr if there
     * is no encapsulated packet.
     *
     * Note: See notes at encapsulate() about reference counting
     * of encapsulated packets.
     */
    virtual cPacket *getEncapsulatedPacket() const;

    /**
     * Returns true if the packet contains an encapsulated packet, and false
     * otherwise. This method is potentially more efficient than
     * `getEncapsulatedPacket()!=nullptr`, because it does not need to
     * unshare a shared encapsulated packet (see note at encapsulate()).
     */
    virtual bool hasEncapsulatedPacket() const;
    //@}

    /** @name Transmission state */
    //@{
    /**
     * Returns true if this packet is a transmission update, and false otherwise.
     * Transmission updates are packets sent with the SendOptions::updateTx()
     * or SendOptions::finishTx() option.
     *
     * @see getRemainingDuration(), cSimpleModule::send(), cSimpleModule::sendDirect(),
     * SendOptions
     */
    bool isUpdate() const {return flags&FL_ISUPDATE;}

    /**
     * If the packet is a transmission update (isUpdate() would return true),
     * this method returns the original packet ID supplied in the send call.
     *
     * @see cSimpleModule::send(), cSimpleModule::sendDirect(), SendOptions
     */
    txid_t getTransmissionId() const {return transmissionId;}

    /**
     * Returns the transmission duration of the whole packet after the packet
     * was sent through a transmission channel such as cDatarateChannel.
     *
     * Contrast that with getRemainingDuration() which, when called on
     * transmission update packets, returns the remaining transmission time
     * for the packet. Naturally, remainingDuration <= duration.
     *
     * After a sending not involving a transmission channel, the duration
     * is set to zero.
     *
     * @see getRemainingDuration(), isReceptionStart(), getArrivalTime(),
     * cDatarateChannel
     */
    simtime_t_cref getDuration() const {return duration;}

    /**
     * Returns the remaining transmission time for the packet after the packet
     * was sent through a transmission channel such as cDatarateChannel.
     *
     * For packets delivered to a gate with the default (at-end) delivery
     * mode, the remaining transmission time is zero.
     *
     * For packets delivered to a gate with the immediate delivery mode,
     * the remaining duration is the full transmission duration if the packet
     * is a normal (i.e. non-update) packet. If the packet is a transmission
     * update (isUpdate() would return true), the remaining transmission time
     * is determined by the send options passed the send call.
     *
     * After a sending not involving a transmission channel, the remaining
     * duration will be zero.
     *
     * @see isUpdate(), getDuration(), getArrivalTime(), cDatarateChannel,
     * cGate::setDeliverImmediately(), cSimpleModule::send(),
     * cSimpleModule::sendDirect(), SendOptions
     */
    simtime_t_cref getRemainingDuration() const {return remainingDuration;}

    /**
     * Returns true if the packet's delivery to the module represents the beginning
     * of the reception of the packet, and false otherwise.
     */
    bool isReceptionStart() const {return duration == remainingDuration;}

    /**
     * Returns true if the packet's delivery to the module represents the end
     * of the reception of the packet, and false otherwise.
     */
    bool isReceptionEnd() const {return remainingDuration.isZero();}
    //@}
};

}  // namespace omnetpp

#endif


