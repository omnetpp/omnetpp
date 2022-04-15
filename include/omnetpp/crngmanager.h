//==========================================================================
//   CRNGMANAGER.H  - part of
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

#ifndef __OMNETPP_CRNGMANAGER_H
#define __OMNETPP_CRNGMANAGER_H

#include "cownedobject.h"

namespace omnetpp {

class cConfiguration;
class cRNG;

/**
 * @brief Provides RNG access to simulation components (modules and channels).
 *
 * @ingroup SimSupport
 */
class SIM_API cIRngManager : public cNoncopyableOwnedObject
{
  public:
    /** @name Constructor, destructor. */
    //@{
    /**
     * Constructor.
     */
    cIRngManager() {}

    /**
     * Destructor.
     */
    virtual ~cIRngManager();
    //@}

    /** @name Setting up and accessing RNGs. */
    //@{
    /**
     * Sets the configuration to be used for configuring this object.
     */
    virtual void configure(cSimulation *simulation, cConfiguration *cfg, int parsimProcId, int parsimNumPartitions) = 0;

    /**
     * Sets up RNGs for the given component.
     */
    virtual void configureRNGs(cComponent *component) = 0;

    /**
     * Returns the number of RNGs available for the given component.
     */
    virtual int getNumRNGs(const cComponent *component) const = 0;

    /**
     * Returns the kth RNG of the given component.
     */
    virtual cRNG *getRNG(const cComponent *component, int k) = 0;

    /**
     * Returns a hash computed from RNG usage, suitable as fingerprint
     * ingredient.
     */
    virtual uint32_t getHash() const = 0;
    //@}
};


/**
 * @brief The default RNG manager.
 *
 * @ingroup SimSupport
 */
class SIM_API cRngManager : public cIRngManager
{
  private:
    cConfiguration *cfg = nullptr;
    int numRNGs = 0;
    cRNG **rngs = nullptr;

  public:
    /** Constructor */
    cRngManager() {}

    /** Destructor */
    virtual ~cRngManager();

    /** @name Redefined cIRngManager methods. */
    //@{
    virtual void configure(cSimulation *simulation, cConfiguration *cfg, int parsimProcId, int parsimNumPartitions) override;
    virtual void configureRNGs(cComponent *component) override;
    virtual int getNumRNGs(const cComponent *component) const override;
    virtual cRNG *getRNG(const cComponent *component, int k) override;
    virtual uint32_t getHash() const  override;
    //@}

    /** @name Additional methods. */
    //@{
    virtual int getNumRNGs() const;
    virtual cRNG *getRNG(int rngId);
    //@}
};

}  // namespace omnetpp

#endif


