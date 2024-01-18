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

#include <set>
#include <cstring>
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
 * @ingroup Misc
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
    virtual void registerVectorResult(void *vechandle, const cComponent *component, const char *name) = 0;
    virtual void addVectorResult(void *vechandle, const simtime_t& t, double value) = 0;
    virtual void addVisuals() = 0;

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
     * Compares the computed fingerprint with the expected one, and returns
     * true if they match, and false otherwise.
    */
    virtual bool checkFingerprint() const = 0;
};

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
 *  - 'y' display strings of all modules, added on refreshDisplay() calls (see cmdenv-fake-gui config option)
 *  - 'f' essential properties (geometry) of all figures on all module canvases, added on refreshDisplay() calls (see cmdenv-fake-gui config option)
 *  - 'x' extra data provided by modules
 *
 * @ingroup Misc
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
        DISPLAY_STRINGS      = 'y',
        CANVAS_FIGURES       = 'f',
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
    cMatchExpression *eventMatcher = nullptr;
    cMatchExpression *moduleMatcher = nullptr;
    cMatchExpression *resultMatcher = nullptr;
    cHasher hasher_;
    [[deprecated]] cHasher *hasher = &hasher_;
    bool addEvents = false;
    bool addScalarResults = false;
    bool addStatisticResults = false;
    bool addVectorResults = false;
    bool addExtraData_ = false;
    std::set<void*> enabledVecHandles;

  protected:
    virtual FingerprintIngredient validateIngredient(char ch);
    virtual void parseIngredients(const char *s);
    virtual void parseEventMatcher(const char *s);
    virtual void parseModuleMatcher(const char *s);
    virtual void parseResultMatcher(const char *s);
    virtual bool addEventIngredient(cEvent *event, FingerprintIngredient ingredient);
    virtual void addModuleVisuals(cModule *module, bool displayStrings, bool figures);

  public:
    cSingleFingerprintCalculator() {}
    virtual ~cSingleFingerprintCalculator();

    virtual cSingleFingerprintCalculator *dup() const override { return new cSingleFingerprintCalculator(); }
    virtual std::string str() const override;
    virtual void initialize(const char *expectedFingerprints, cConfiguration *cfg, int index=-1) override;

    virtual void addEvent(cEvent *event) override;
    virtual void addScalarResult(const cComponent *component, const char *name, double value) override;
    virtual void addStatisticResult(const cComponent *component, const char *name, const cStatistic *value) override;
    virtual void registerVectorResult(void *vechandle, const cComponent *component, const char *name) override;
    virtual void addVectorResult(void *vechandle, const simtime_t& t, double value) override;
    virtual void addVisuals() override;

    virtual void addExtraData(const char *buffer, size_t length) override { if (addExtraData_) hasher_.add(buffer, length); }
    virtual void addExtraData(char data) override { if (addExtraData_) hasher_.add(data); }
    virtual void addExtraData(short data) override { if (addExtraData_) hasher_.add(data); }
    virtual void addExtraData(int data) override { if (addExtraData_) hasher_.add(data); }
    virtual void addExtraData(long data) override { if (addExtraData_) hasher_.add(data); }
    virtual void addExtraData(long long data) override { if (addExtraData_) hasher_.add(data); }
    virtual void addExtraData(unsigned char data) override { if (addExtraData_) hasher_.add(data); }
    virtual void addExtraData(unsigned short data) override { if (addExtraData_) hasher_.add(data); }
    virtual void addExtraData(unsigned int data) override { if (addExtraData_) hasher_.add(data); }
    virtual void addExtraData(unsigned long data) override { if (addExtraData_) hasher_.add(data); }
    virtual void addExtraData(unsigned long long data) override { if (addExtraData_) hasher_.add(data); }
    virtual void addExtraData(double data) override { if (addExtraData_) hasher_.add(data); }
    virtual void addExtraData(const char *data) override { if (addExtraData_) hasher_.add(data); }

    virtual bool checkFingerprint() const override;

};


/**
 * @brief This class calculates multiple fingerprints simultaneously.
 *
 * The calculator can be configured similarly to the cSingleFingerprintCalculator
 * class, but in this case each option is a comma separated list.
 *
 * @ingroup Misc
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
    virtual void registerVectorResult(void *vechandle, const cComponent *component, const char *name) override;
    virtual void addVectorResult(void *vechandle, const simtime_t& t, double value) override;
    virtual void addVisuals() override;

    virtual void addExtraData(const char *data, size_t length) override { for (auto element: elements) element->addExtraData(data, length); }
    virtual void addExtraData(char data) override { for (auto element: elements) element->addExtraData(data); }
    virtual void addExtraData(short data) override { for (auto element: elements) element->addExtraData(data); }
    virtual void addExtraData(int data) override { for (auto element: elements) element->addExtraData(data); }
    virtual void addExtraData(long data) override { for (auto element: elements) element->addExtraData(data); }
    virtual void addExtraData(long long data) override { for (auto element: elements) element->addExtraData(data); }
    virtual void addExtraData(unsigned char data) override { for (auto element: elements) element->addExtraData(data); }
    virtual void addExtraData(unsigned short data) override { for (auto element: elements) element->addExtraData(data); }
    virtual void addExtraData(unsigned int data) override { for (auto element: elements) element->addExtraData(data); }
    virtual void addExtraData(unsigned long data) override { for (auto element: elements) element->addExtraData(data); }
    virtual void addExtraData(unsigned long long data) override { for (auto element: elements) element->addExtraData(data); }
    virtual void addExtraData(double data) override { for (auto element: elements) element->addExtraData(data); }
    virtual void addExtraData(const char *data) override { for (auto element: elements) element->addExtraData(data); }

    virtual bool checkFingerprint() const override;
};

}  // namespace omnetpp

#endif

