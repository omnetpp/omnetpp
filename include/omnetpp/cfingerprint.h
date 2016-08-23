//==========================================================================
//   CFINGERPRINT.H  - part of
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
 * @brief This class defines the interface for fingerprint calculators.
 *
 * @see cSimulation::getFingerprintCalculator()
 * @ingroup ExtensionPoints
 */
class SIM_API cFingerprintCalculator : public cObject, noncopyable
{
  public:
    /** @name Constructor, destructor. */
    //@{
    virtual ~cFingerprintCalculator() {}
    //@}

    /**
     * Initialization.
     */
    virtual void initialize(const char *expectedFingerprints, cConfiguration *cfg, int index=-1) = 0;

    /** @name Updating the fingerprint value */
    //@{
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
    //@}

    /**
     * Compare computed fingerprint with the expected one, and throw
     * error if it doesn't match.
     */
    virtual bool checkFingerprint() const = 0;
};

#ifdef USE_OMNETPP4x_FINGERPRINTS

/**
 * @brief Computes \opp 4.x compatible fingerprints.
 *
 * This class is only available when \opp was compiled with USE_OMNETPP4x_FINGERPRINTS
 * defined. Note that USE_OMNETPP4x_FINGERPRINTS affects other parts of the \opp
 * codebase as well (e.g. SimTime), not only the availability of this class.
 *
 * @ingroup Internals
 */
class SIM_API cOmnetpp4xFingerprintCalculator : public cFingerprintCalculator
{
  protected:
    std::string expectedFingerprints;
    cHasher *hasher;

  public:
    cOmnetpp4xFingerprintCalculator();
    virtual ~cOmnetpp4xFingerprintCalculator();

    virtual cOmnetpp4xFingerprintCalculator *dup() const override { return new cOmnetpp4xFingerprintCalculator(); }
    virtual std::string str() const override { return hasher->str(); }
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
 * @brief This class calculates the "fingerprint" of a simulation.
 *
 * The fingerprint is a 32-bit hash value calculated from various data
 * of the simulation events and simulation results. The calculator
 * can be configured to consider only certain events, modules, and
 * results using filter expressions.
 *
 * The available ingredients are:
 *  - 'e' event number
 *  - 't' simulation time
 *  - 'n' message (event) full name
 *  - 'c' message (event) class name
 *  - 'k' message kind
 *  - 'l' message bit length
 *  - 'o' message control info class name
 *  - 'd' message data (uses parsimPack() by default but can be overridden)
 *  - 'i' module id
 *  - 'm' module full name
 *  - 'p' module full path
 *  - 'a' module class name
 *  - 'r' random numbers drawn
 *  - 's' scalar results
 *  - 'z' statistic results
 *  - 'v' vector results
 *  - 'x' extra data provided by modules
 *
 * @ingroup Internals
 */
class SIM_API cSingleFingerprintCalculator : public cFingerprintCalculator
{
  protected:
    enum FingerprintIngredient {
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
    std::string ingredients;
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
    virtual FingerprintIngredient validateIngredient(char ch);
    virtual void parseIngredients(const char *s);
    virtual void parseEventMatcher(const char *s);
    virtual void parseModuleMatcher(const char *s);
    virtual void parseResultMatcher(const char *s);
    virtual bool addEventIngredient(cEvent *event, FingerprintIngredient ingredient);

  public:
    cSingleFingerprintCalculator();
    virtual ~cSingleFingerprintCalculator();

    virtual cSingleFingerprintCalculator *dup() const override { return new cSingleFingerprintCalculator(); }
    virtual std::string str() const override;
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
 * @brief This class calculates multiple fingerprints simultaneously.
 *
 * The calculator can be configured similarly to the cSingleFingerprintCalculator
 * class, but in this case each option is a comma separated list.
 *
 * @ingroup Internals
 */
class SIM_API cMultiFingerprintCalculator : public cFingerprintCalculator
{
  protected:
    cFingerprintCalculator *prototype;
    std::vector<cFingerprintCalculator *> elements;

  public:
    cMultiFingerprintCalculator(cFingerprintCalculator *prototype);
    virtual ~cMultiFingerprintCalculator();

    virtual cMultiFingerprintCalculator *dup() const override { return new cMultiFingerprintCalculator(static_cast<cFingerprintCalculator *>(prototype->dup())); }
    virtual std::string str() const override;
    virtual void initialize(const char *expectedFingerprints, cConfiguration *cfg, int index=-1) override;

    virtual void addEvent(cEvent *event) override;
    virtual void addScalarResult(const cComponent *component, const char *name, double value) override;
    virtual void addStatisticResult(const cComponent *component, const char *name, const cStatistic *value) override;
    virtual void addVectorResult(const cComponent *component, const char *name, const simtime_t& t, double value) override;

#define for_each_element(CODE) for (std::vector<cFingerprintCalculator *>::iterator it = elements.begin(); it != elements.end(); ++it) { cFingerprintCalculator *element = *it; CODE; }
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

