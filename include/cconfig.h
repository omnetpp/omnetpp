//==========================================================================
//   CCONFIG.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cConfiguration
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCONFIG_H
#define __CCONFIG_H

#include <vector>
#include "defs.h"
#include "opp_string.h"
#include "cpolymorphic.h"


/**
 * This class logically belongs to the cEnvir facade. (cEnvir presents
 * to the simulation kernel the UI, or generally, the program which embeds
 * the simulation.) This class provides access to configuration data for
 * components such as scheduler classes, parallel simulation algorithms, etc.
 * Model code (simple modules) should not directly read the configuration --
 * they should rely on module parameters for input.
 *
 * This is an abstract base class, which means functionality is provided by
 * subclasses. The subclass used by default is cInifile which reads config
 * files in the omnetpp.ini format. Other subclasses which use different data
 * sources (e.g. database) can be created and plugged in for flexibility.
 *
 * @ingroup EnvirExtensions
 */
class ENVIR_API cConfiguration : public cPolymorphic
{
  public:
    /**
     * Virtual destructor
     */
    virtual ~cConfiguration() {}

    /**
     * Initializes configuration object from "boot-time" configuration
     * (omnetpp.ini). For example, if a particular cConfiguration subclass
     * uses a database as data source, it may take the connection parameters
     * from the "boot-time" configuration.
     */
    virtual void initializeFrom(cConfiguration *conf) = 0;

    /** @name Sections */
    //@{
    /**
     * Returns the number of sections available in configuration
     */
    virtual int getNumSections() = 0;

    /**
     * Returns the name of section k. 0<=k<getNumSections().
     */
    virtual const char *getSectionName(int k) = 0;
    //@}

    /** @name Getter methods */
    //@{
    /**
     * FIXME comments...
     */
    virtual bool exists(const char *section, const char *key) = 0;
    /**
     * FIXME comments...
     */
    virtual bool getAsBool(const char *section, const char *key, bool defaultvalue=false) = 0;
    virtual long getAsInt(const char *section, const char *key, long defaultvalue=0) = 0;
    virtual double getAsDouble(const char *section, const char *key, double defaultvalue=0.0) = 0;
    virtual double getAsTime(const char *sect, const char *key, double defaultvalue=0.0) = 0;
    virtual const char *getAsString(const char *section, const char *key, const char *defaultvalue="") = 0; // quotes stripped (if any)
    virtual const char *getAsCustom(const char *section, const char *key, const char *defaultvalue=NULL) = 0; // with quotes (if any)

    /**
     * Returns a collection of entries together with their values. Entry names
     * must match keypart1 + keypart2 + some suffix. Pairs of "some suffix" and
     * their matching values are returned.
     *
     * Example: The config contains "net.host1.gen.rng-0 = value1",
     * "net.host1.gen.rng-2 = value2" and "net.host1.gen.rng-5 = value3".
     * getEntriesWithPrefix() with keypart1="net.host1.gen" and keypart2=".rng-"
     * is invoked. The call returns this: ("0" --> value1, "2" --> value2, "5" --> value3).
     * Returned array contains "0", "2", "5" at odd indices, and
     * value1, value2, value3 at even indices.
     */
    virtual std::vector<opp_string> getEntriesWithPrefix(const char *section, const char *keypart1, const char *keypart2) = 0;

    /**
     * Returns true if the last "get" call didn't find the configuration entry
     * and returned the default value.
     */
    virtual bool notFound() = 0;
    //@}

    /** @name Getter methods, with fallback to another section */
    //@{
    // get an entry from [section1] or if it isn't there, from [section2]
    virtual bool exists2(const char *section1, const char *section2, const char *key) = 0;
    virtual bool getAsBool2(const char *section1, const char *section2, const char *key, bool defaultvalue=false) = 0;
    virtual long getAsInt2(const char *section1, const char *section2, const char *key, long defaultvalue=0) = 0;
    virtual double getAsDouble2(const char *section1, const char *section2, const char *key, double defaultvalue=0.0) = 0;
    virtual double getAsTime2(const char *section1, const char *section2, const char *key, double defaultvalue=0.0) = 0;
    virtual const char *getAsString2(const char *section1, const char *section2, const char *key, const char *defaultvalue="") = 0;
    virtual const char *getAsCustom2(const char *section1, const char *section2, const char *key, const char *defaultvalue="") = 0;

    virtual std::vector<opp_string> getEntriesWithPrefix(const char *section1, const char *section2, const char *keypart1, const char *keypart2) = 0;
    //@}

    /**
     * Returns the name of the configuration file. Returns NULL if this object is
     * using a configuration file.
     */
    virtual const char *fileName() const = 0;
};

#endif


