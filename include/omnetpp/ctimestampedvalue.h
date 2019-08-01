//==========================================================================
//  CTIMESTAMPEDVALUE.H - part of
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

#ifndef __OMNETPP_CTIMESTAMPEDVALUE_H
#define __OMNETPP_CTIMESTAMPEDVALUE_H

#include "clistener.h"  // simsignal_t
#include "simtime_t.h"
#include "cobject.h"

namespace omnetpp {


/**
 * @brief Interface for emitting timestamped values as signals, primarily for
 * result recording.
 *
 * Note: The signalID argument is present in the methods to allow the same
 * object to support multiple signals.
 *
 * @ingroup Signals
 */
class SIM_API cITimestampedValue
{
    public:
        /** Virtual destructor */
        virtual ~cITimestampedValue() {}

        /** Returns the timestamp for the given signal. */
        virtual simtime_t_cref getTimestamp(simsignal_t signalID) const = 0;

        /** Returns the value's data type for the given signal. */
        virtual SimsignalType getValueType(simsignal_t signalID) const = 0;

        /** @name Returns the value for the given signal. */
        //@{
        virtual bool boolValue(simsignal_t signalID) const = 0;
        virtual intval_t intValue(simsignal_t signalID) const = 0;
        virtual uintval_t uintValue(simsignal_t signalID) const = 0;
        virtual double doubleValue(simsignal_t signalID) const = 0;
        virtual SimTime simtimeValue(simsignal_t signalID) const = 0;
        virtual const char *stringValue(simsignal_t signalID) const = 0;
        virtual cObject *objectValue(simsignal_t signalID) const = 0;
        [[deprecated]] intval_t longValue(simsignal_t signalID) const {return intValue(signalID);}
        [[deprecated]] uintval_t unsignedLongValue(simsignal_t signalID) const {return uintValue(signalID);}
        //@}
};

/**
 * @brief A straightforward implementation of cITimestampedValue.
 *
 * @ingroup Signals
 */
class SIM_API cTimestampedValue : public cITimestampedValue, public cObject
{
    public:
        simtime_t timestamp;
        SimsignalType type; // selector for the union
        union {
            bool b;
            intval_t l;
            uintval_t ul;
            double d;
            const char *s;
            cObject *obj;
        };
        SimTime t;  // cannot go into the union because it has a ctor

    public:
        /** @name Constructors */
        //@{
        cTimestampedValue() {type=SIMSIGNAL_UNDEF;}
        cTimestampedValue(simtime_t timestamp, bool b) {set(timestamp,b);}
        cTimestampedValue(simtime_t timestamp, intval_t l) {set(timestamp,l);}
        cTimestampedValue(simtime_t timestamp, uintval_t ul) {set(timestamp,ul);}
        cTimestampedValue(simtime_t timestamp, double d) {set(timestamp,d);}
        cTimestampedValue(simtime_t timestamp, const SimTime& t) {set(timestamp,t);}
        cTimestampedValue(simtime_t timestamp, const char *s) {set(timestamp,s);}
        cTimestampedValue(simtime_t timestamp, cObject *obj) {set(timestamp,obj);}
        //@}

        /** @name Setters */
        //@{
        void set(simtime_t timestamp, bool b) {this->timestamp=timestamp; type=SIMSIGNAL_BOOL; this->b=b;}
        void set(simtime_t timestamp, intval_t l) {this->timestamp=timestamp; type=SIMSIGNAL_INT; this->l=l;}
        void set(simtime_t timestamp, uintval_t ul) {this->timestamp=timestamp; type=SIMSIGNAL_UINT; this->ul=ul;}
        void set(simtime_t timestamp, double d) {this->timestamp=timestamp; type=SIMSIGNAL_DOUBLE; this->d=d;}
        void set(simtime_t timestamp, const SimTime& t) {this->timestamp=timestamp; type=SIMSIGNAL_SIMTIME; this->t=t;}
        void set(simtime_t timestamp, const char *s) {this->timestamp=timestamp; type=SIMSIGNAL_STRING; this->s=s;}
        void set(simtime_t timestamp, cObject *obj) {this->timestamp=timestamp; type=SIMSIGNAL_OBJECT; this->obj=obj;}
        //@}

        /** Returns the stored timestamp. */
        virtual simtime_t_cref getTimestamp(simsignal_t signalID) const override {return timestamp;}

        /** Returns the data type of the stored value. */
        virtual SimsignalType getValueType(simsignal_t signalID) const override {return type;}

        /** @name Getters. Call the one that corresponds to the stored type. */
        //@{
        virtual bool boolValue(simsignal_t signalID) const override {return b;}
        virtual intval_t intValue(simsignal_t signalID) const override {return l;}
        virtual uintval_t uintValue(simsignal_t signalID) const override {return ul;}
        virtual double doubleValue(simsignal_t signalID) const override {return d;}
        virtual SimTime simtimeValue(simsignal_t signalID) const override {return t;}
        virtual const char *stringValue(simsignal_t signalID) const override {return s;}
        virtual cObject *objectValue(simsignal_t signalID) const override {return obj;}
        //@}
};

}  // namespace omnetpp

#endif

