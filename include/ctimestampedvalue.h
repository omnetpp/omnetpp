//==========================================================================
//  CTIMESTAMPEDVALUE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CTIMESTAMPEDVALUE_H
#define __CTIMESTAMPEDVALUE_H

#include "clistener.h"  // simsignal_t
#include "simtime_t.h"
#include "cobject.h"

NAMESPACE_BEGIN


/**
 * Interface for emitting timestamped values as signals, primarily for
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
        enum Type {UNDEF, LONG, ULONG, DOUBLE, SIMTIME, STRING, OBJECT};

    public:
        /** Virtual destructor */
        virtual ~cITimestampedValue() {}

        /** Returns the timestamp for the given signal. */
        virtual simtime_t_cref getTimestamp(simsignal_t signalID) const = 0;

        /** Returns the value's data type for the given signal. */
        virtual Type getValueType(simsignal_t signalID) const = 0;

        /** @name Returns the value for the given signal. */
        //@{
        virtual long longValue(simsignal_t signalID) const = 0;
        virtual unsigned long unsignedLongValue(simsignal_t signalID) const = 0;
        virtual double doubleValue(simsignal_t signalID) const = 0;
        virtual SimTime simtimeValue(simsignal_t signalID) const = 0;
        virtual const char *stringValue(simsignal_t signalID) const = 0;
        virtual cObject *objectValue(simsignal_t signalID) const = 0;
        //@}
};

/**
 * A straightforward implementation of cITimestampedValue.
 *
 * @ingroup Signals
 */
class SIM_API cTimestampedValue : public cITimestampedValue, public cObject
{
    public:
        simtime_t timestamp;
        Type type; // selector for the union
        union {
            long l;
            unsigned long ul;
            double d;
            const char *s;
            cObject *obj;
        };
        SimTime t;  // cannot go into the union because it has a ctor

    public:
        /** @name Constructors */
        //@{
        cTimestampedValue() {type=UNDEF;}
        cTimestampedValue(simtime_t timestamp, long l) {set(timestamp,l);}
        cTimestampedValue(simtime_t timestamp, unsigned long ul) {set(timestamp,ul);}
        cTimestampedValue(simtime_t timestamp, double d) {set(timestamp,d);}
        cTimestampedValue(simtime_t timestamp, const SimTime& t) {set(timestamp,t);}
        cTimestampedValue(simtime_t timestamp, const char *s) {set(timestamp,s);}
        cTimestampedValue(simtime_t timestamp, cObject *obj) {set(timestamp,obj);}
        //@}

        /** @name Setters */
        //@{
        void set(simtime_t timestamp, long l) {this->timestamp=timestamp; type=LONG; this->l=l;}
        void set(simtime_t timestamp, unsigned long ul) {this->timestamp=timestamp; type=ULONG; this->ul=ul;}
        void set(simtime_t timestamp, double d) {this->timestamp=timestamp; type=DOUBLE; this->d=d;}
        void set(simtime_t timestamp, const SimTime& t) {this->timestamp=timestamp; type=SIMTIME; this->t=t;}
        void set(simtime_t timestamp, const char *s) {this->timestamp=timestamp; type=STRING; this->s=s;}
        void set(simtime_t timestamp, cObject *obj) {this->timestamp=timestamp; type=OBJECT; this->obj=obj;}
        //@}

        /** Returns the stored timestamp. */
        virtual simtime_t_cref getTimestamp(simsignal_t signalID) const {return timestamp;}

        /** Returns the data type of the stored value. */
        virtual Type getValueType(simsignal_t signalID) const {return type;}

        /** @name Getters. Call the one that corresponds to the stored type. */
        //@{
        virtual long longValue(simsignal_t signalID) const {return l;}
        virtual unsigned long unsignedLongValue(simsignal_t signalID) const {return ul;}
        virtual double doubleValue(simsignal_t signalID) const {return d;}
        virtual SimTime simtimeValue(simsignal_t signalID) const {return t;}
        virtual const char *stringValue(simsignal_t signalID) const {return s;}
        virtual cObject *objectValue(simsignal_t signalID) const {return obj;}
        //@}
};

NAMESPACE_END

#endif

