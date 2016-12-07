#ifndef __CGATEWRAPPER_H
#define __CGATEWRAPPER_H

#include <omnetpp.h>

/**
 * Utility class to wrap a gate used for transmissions. This class assumes
 * that cDelayChannels and cDatarateChannels are in use, and that topology
 * and channel parameters do not change during simulation. If there is
 * any such change, refresh() must be called on this object.
 */
class cTransmissionGateWrapper
{
  protected:
    cGate *gate;
    cDatarateChannel *datarateChannel;
    simtime_t leadingDelay;

  public:
    /**
     * Default constructor. When used, setGate() needs to be called
     * before the object can be used.
     */
    cTransmissionGateWrapper() {gate = NULL; datarateChannel = NULL;}

    /**
     * Constructor; takes the gate on which to operate.
     */
    cTransmissionGateWrapper(cGate *g) {gate = g; refresh();}

    /**
     * Sets the gate on which to operate.
     */
    void setGate(cGate *g) {gate = g; refresh();}

    /**
     * Returns the transmission channel from the path; it must be a
     * cDatarateChannel.
     */
    cDatarateChannel *getDatarateChannel() {return datarateChannel;}

    /**
     * Returns the sum of the propagation delays (cDelayChannel) up to
     * the transmission channel.
     */
    simtime_t getLeadingDelay() {return leadingDelay;}

    /**
     * Calculates the transmission duration of the packet with the current
     * channel configuration (datarate, etc). Does not check or modify
     * channel state.
     */
    simtime_t calculateDuration(cPacket *pk) {return pk->getBitLength() / datarateChannel->getDatarate();}

    /**
     * Returns whether this gate is currently transmitting.
     */
    bool isBusy() {return getTransmissionFinishTime() < simTime();}

    /**
     * Returns the time this gate will finish transmitting.
     */
    simtime_t getTransmissionFinishTime() {return datarateChannel->getTransmissionFinishTime() - leadingDelay;}

    /**
     * Must be called on changes in the connection path.
     */
    void refresh();
};

#endif

