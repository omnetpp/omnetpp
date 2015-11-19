//==========================================================================
//   CFINGERPRINT.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CFINGERPRINT_H
#define __CFINGERPRINT_H

#include <string.h>
#include "simkerneldefs.h"
#include "cevent.h"
#include "cmessage.h"
#include "chasher.h"
#include "cmatchexpression.h"

namespace omnetpp {

/**
 * This class defines the interface for fingerprint calculators.
 */
class SIM_API cFingerprint : public cObject
{
  public:
    virtual ~cFingerprint() {}

    virtual void initialize(const char *expectedFingerprints, cConfiguration *cfg, int index=-1) = 0;

    virtual void addEvent(cEvent *event) = 0;
    virtual void addScalarResult(const cComponent *component, const char *name, double value) = 0;
    virtual void addStatisticResult(const cComponent *component, const char *name, const cStatistic *value) = 0;
    virtual void addVectorResult(const cComponent *component, const char *name, const simtime_t& t, double value) = 0;

    virtual void addExtraData(const char *data, size_t length) = 0;
    virtual void addExtraData(char data) = 0;
    virtual void addExtraData(short data) = 0;
    virtual void addExtraData(int data) = 0;
    virtual void addExtraData(long data) = 0;
    virtual void addExtraData(long long data) = 0;
    virtual void addExtraData(unsigned char data) = 0;
    virtual void addExtraData(unsigned short data) = 0;
    virtual void addExtraData(unsigned int data) = 0;
    virtual void addExtraData(unsigned long data) = 0;
    virtual void addExtraData(unsigned long long data) = 0;
    virtual void addExtraData(double data) = 0;
    virtual void addExtraData(const char *data) = 0;

    virtual bool checkFingerprint() const = 0;
};

#ifdef USE_OMNETPP4x_FINGERPRINTS

class SIM_API cOmnetpp4xFingerprint : public cFingerprint
{
  protected:
    std::string expectedFingerprints;
    cHasher *hasher;

  public:
    cOmnetpp4xFingerprint();
    virtual ~cOmnetpp4xFingerprint();

    virtual cOmnetpp4xFingerprint *dup() const override { return new cOmnetpp4xFingerprint(); }
    virtual std::string info() const override { return hasher->str(); }
    virtual void initialize(const char *expectedFingerprints, cConfiguration *cfg, int index=-1) override;

    virtual void addEvent(cEvent *event) override;
    virtual void addScalarResult(const cComponent *component, const char *name, double value) override {}
    virtual void addStatisticResult(const cComponent *component, const char *name, const cStatistic *value) override {}
    virtual void addVectorResult(const cComponent *component, const char *name, const simtime_t& t, double value) override {}

    virtual void addExtraData(const char *buffer, size_t length) override { hasher->add(buffer, length); }
    virtual void addExtraData(char data) override { hasher->add(data); }
    virtual void addExtraData(short data) override { hasher->add(data); }
    virtual void addExtraData(int data) override { hasher->add(data); }
    virtual void addExtraData(long data) override { hasher->add(data); }
    virtual void addExtraData(long long data) override { hasher->add(data); }
    virtual void addExtraData(unsigned char data) override { hasher->add(data); }
    virtual void addExtraData(unsigned short data) override { hasher->add(data); }
    virtual void addExtraData(unsigned int data) override { hasher->add(data); }
    virtual void addExtraData(unsigned long data) override { hasher->add(data); }
    virtual void addExtraData(unsigned long long data) override { hasher->add(data); }
    virtual void addExtraData(double data) override { hasher->add(data); }
    virtual void addExtraData(const char *data) override { hasher->add(data); }

    virtual bool checkFingerprint() const override;

};

#else // if !USE_OMNETPP4x_FINGERPRINTS

/**
 * This class calculates the "fingerprint" of a simulation. The
 * fingerprint is a 32-bit hash value calculated from various data
 * of the simulation events and simulation results. The calculator
 * can be configured to consider only certain events, modules, and
 * results using filter expressions.
 *
 * The available categories are:
 *  - 'e' event number
 *  - 't' simulation time
 *
 *  - 'n' message (event) full name
 *  - 'c' message (event) class name
 *  - 'k' message kind
 *  - 'l' message bit length
 *  - 'o' message control info class name
 *  - 'd' message data (uses parsimPack() by default but can be overridden)
 *
 *  - 'i' module id
 *  - 'm' module full name
 *  - 'p' module full path
 *  - 'a' module class name
 *
 *  - 'r' random numbers drawn
 *  - 's' scalar results
 *  - 'z' statistic results
 *  - 'v' vector results
 *
 *  - 'x' extra data provided by modules
 */
class SIM_API cSingleFingerprint : public cFingerprint
{
  protected:
    enum FingerprintCategory {
        EVENT_NUMBER         = 'e',
        SIMULATION_TIME      = 't',
        MESSAGE_FULL_NAME    = 'n',
        MESSAGE_CLASS_NAME   = 'c',
        MESSAGE_KIND         = 'k',
        MESSAGE_BIT_LENGTH   = 'l',
        MESSAGE_CONTROL_INFO_CLASS_NAME = 'o',
        MESSAGE_DATA         = 'd',
        MODULE_ID            = 'i',
        MODULE_FULL_NAME     = 'm',
        MODULE_FULL_PATH     = 'p',
        MODULE_CLASS_NAME    = 'a',
        RANDOM_NUMBERS_DRAWN = 'r',
        RESULT_SCALAR        = 's',
        RESULT_STATISTIC     = 'z',
        RESULT_VECTOR        = 'v',
        EXTRA_DATA           = 'x',
        CLEAN_HASHER         = '0'
    };

    class MatchableObject : public cMatchExpression::Matchable
    {
      private:
        const cObject *object;
        mutable std::string attributeValue;

      public:
        MatchableObject(const cObject *object) : object(object) {}

        virtual const char *getAsString() const;
        virtual const char *getAsString(const char *attribute) const;
    };

  protected:
    std::string expectedFingerprints;
    std::string categories;
    cMatchExpression *eventMatcher;
    cMatchExpression *moduleMatcher;
    cMatchExpression *resultMatcher;
    cHasher *hasher;
    bool addEvents;
    bool addScalarResults;
    bool addStatisticResults;
    bool addVectorResults;
    bool addExtraData_;

  protected:
    virtual FingerprintCategory validateCategory(char ch);
    virtual void parseCategories(const char *s);
    virtual void parseEventMatcher(const char *s);
    virtual void parseModuleMatcher(const char *s);
    virtual void parseResultMatcher(const char *s);
    virtual bool addEventCategory(cEvent *event, FingerprintCategory category);

  public:
    cSingleFingerprint();
    virtual ~cSingleFingerprint();

    virtual cSingleFingerprint *dup() const override { return new cSingleFingerprint(); }
    virtual std::string info() const override;
    virtual void initialize(const char *expectedFingerprints, cConfiguration *cfg, int index=-1) override;

    virtual void addEvent(cEvent *event) override;
    virtual void addScalarResult(const cComponent *component, const char *name, double value) override;
    virtual void addStatisticResult(const cComponent *component, const char *name, const cStatistic *value) override;
    virtual void addVectorResult(const cComponent *component, const char *name, const simtime_t& t, double value) override;

    virtual void addExtraData(const char *buffer, size_t length) override { if (addExtraData_) hasher->add(buffer, length); }
    virtual void addExtraData(char data) override { if (addExtraData_) hasher->add(data); }
    virtual void addExtraData(short data) override { if (addExtraData_) hasher->add(data); }
    virtual void addExtraData(int data) override { if (addExtraData_) hasher->add(data); }
    virtual void addExtraData(long data) override { if (addExtraData_) hasher->add(data); }
    virtual void addExtraData(long long data) override { if (addExtraData_) hasher->add(data); }
    virtual void addExtraData(unsigned char data) override { if (addExtraData_) hasher->add(data); }
    virtual void addExtraData(unsigned short data) override { if (addExtraData_) hasher->add(data); }
    virtual void addExtraData(unsigned int data) override { if (addExtraData_) hasher->add(data); }
    virtual void addExtraData(unsigned long data) override { if (addExtraData_) hasher->add(data); }
    virtual void addExtraData(unsigned long long data) override { if (addExtraData_) hasher->add(data); }
    virtual void addExtraData(double data) override { if (addExtraData_) hasher->add(data); }
    virtual void addExtraData(const char *data) override { if (addExtraData_) hasher->add(data); }

    virtual bool checkFingerprint() const override;

};


/**
 * This class calculates multiple fingerprints simultaneously. The calculator
 * can be configured similarly to the cSingleFingerprint class, but in this case
 * each option is a comma separated list.
 */
class SIM_API cMultiFingerprint : public cFingerprint
{
  protected:
    cFingerprint *prototype;
    std::vector<cFingerprint *> elements;

  public:
    cMultiFingerprint(cFingerprint *prototype);
    virtual ~cMultiFingerprint();

    virtual cMultiFingerprint *dup() const override { return new cMultiFingerprint(static_cast<cFingerprint *>(prototype->dup())); }
    virtual std::string info() const override;
    virtual void initialize(const char *expectedFingerprints, cConfiguration *cfg, int index=-1) override;

    virtual void addEvent(cEvent *event) override;
    virtual void addScalarResult(const cComponent *component, const char *name, double value) override;
    virtual void addStatisticResult(const cComponent *component, const char *name, const cStatistic *value) override;
    virtual void addVectorResult(const cComponent *component, const char *name, const simtime_t& t, double value) override;

#define for_each_element(CODE) for (std::vector<cFingerprint *>::iterator it = elements.begin(); it != elements.end(); ++it) { cFingerprint *element = *it; CODE; }
    virtual void addExtraData(const char *data, size_t length) override { for_each_element(element->addExtraData(data, length)); }
    virtual void addExtraData(char data) override { for_each_element(element->addExtraData(data)); }
    virtual void addExtraData(short data) override { for_each_element(element->addExtraData(data)); }
    virtual void addExtraData(int data) override { for_each_element(element->addExtraData(data)); }
    virtual void addExtraData(long data) override { for_each_element(element->addExtraData(data)); }
    virtual void addExtraData(long long data) override { for_each_element(element->addExtraData(data)); }
    virtual void addExtraData(unsigned char data) override { for_each_element(element->addExtraData(data)); }
    virtual void addExtraData(unsigned short data) override { for_each_element(element->addExtraData(data)); }
    virtual void addExtraData(unsigned int data) override { for_each_element(element->addExtraData(data)); }
    virtual void addExtraData(unsigned long data) override { for_each_element(element->addExtraData(data)); }
    virtual void addExtraData(unsigned long long data) override { for_each_element(element->addExtraData(data)); }
    virtual void addExtraData(double data) override { for_each_element(element->addExtraData(data)); }
    virtual void addExtraData(const char *data) override { for_each_element(element->addExtraData(data)); }
#undef for_each_element

    virtual bool checkFingerprint() const override;
};

#endif // !USE_OMNETPP4x_FINGERPRINTS


} // namespace omnetpp

#endif

