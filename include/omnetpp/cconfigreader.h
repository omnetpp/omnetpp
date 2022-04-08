//==========================================================================
//  CCONFIGREADER.H - part of
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

#ifndef __OMNETPP_CCONFIGREADER_H
#define __OMNETPP_CCONFIGREADER_H

#include "simkerneldefs.h"
#include "fileline.h"

namespace omnetpp {

class cConfiguration;

/**
 * @brief Abstract base class for configuration readers for SectionBasedConfiguration.
 *
 * This class presents configuration contents as key-value pairs grouped
 * into sections. This class does not try to make sense of section/key names.
 *
 * The default implementation is InifileReader, but other variants (e.g. those
 * that read the configuration from a database) can also be implemented.
 *
 * @ingroup EnvirAndExtensions
 */
class /* NOT SIM_API */ cConfigurationReader
{
  public:
    /**
     * @brief Abstract base class for representing a key-value pair in the configuration.
     */
    class /* NOT SIM_API */ KeyValue {
      public:
        virtual ~KeyValue() {}
        virtual const char *getKey() const = 0;
        virtual const char *getValue() const = 0;
        virtual const char *getBaseDirectory() const = 0;
        virtual FileLine getSourceLocation() const = 0;
    };

  public:
    /**
     * Virtual destructor
     */
    virtual ~cConfigurationReader() {}

    /**
     * Initializes the object from a "boot-time" configuration (omnetpp.ini).
     * For example, if a particular cConfigurationReader class uses a database
     * as data source, it may take the connection parameters from the
     * "boot-time" configuration.
     */
    virtual void initializeFrom(cConfiguration *bootConfig) = 0;

    /**
     * Returns the name of the configuration file. Returns nullptr if this object is
     * not using a configuration file.
     */
    virtual const char *getFileName() const = 0;

    /**
     * Returns the directory defaulted filenames should be understood to be
     * relative to. For example, if the name of the output vector file is
     * not explicitly configured, it defaults to "omnetpp.vec" in the
     * default base directory.
     */
    virtual const char *getDefaultBaseDirectory() const = 0;

    /**
     * Returns the number of sections in the configuration.
     */
    virtual int getNumSections() const = 0;

    /**
     * Returns the name of the given section. sectionId should be in
     * 0..getNumSections()-1.
     */
    virtual const char *getSectionName(int sectionId) const = 0;

    /**
     * Returns the number of entries in the given section. sectionId should be
     * in 0..getNumSections()-1.
     */
    virtual int getNumEntries(int sectionId) const = 0;

    /**
     * Returns the given entry in the in the given section. sectionId should be
     * in 0..getNumSections()-1. The lifetime of the returned entry may be
     * limited, so references to entries should not be cached.
     */
    virtual const KeyValue& getEntry(int sectionId, int entryId) const = 0;

    /**
     * Prints the configuration to the standard output. This can be useful
     * for tracking down problems.
     */
    virtual void dump() const = 0;
};

}  // namespace omnetpp

#endif


