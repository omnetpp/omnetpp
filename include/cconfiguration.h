//==========================================================================
//  CCONFIGURATION.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cConfiguration
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCONFIGURATION_H
#define __CCONFIGURATION_H

#include <vector>
#include "simkerneldefs.h"
#include "cobject.h"

class cConfigEntry;


/**
 * "General", the name of the default configuration
 */
#define CFGNAME_GENERAL "General"

/**
 * Abstract base class for representing the configuration (omnetpp.ini).
 * This class logically belongs to the cEnvir facade. (cEnvir presents
 * to the simulation kernel the UI, or generally, the program which embeds
 * the simulation.) This class provides access to configuration data for
 * components such as scheduler classes, parallel simulation algorithms, etc.
 * Model code (simple modules) should not directly read the configuration --
 * they should rely on module parameters for input.
 *
 * Two layers etc!!!
 * ([General] is treated as if it was short for [Config General].)
 *
 *XXX refine
 * This is an abstract base class, which means functionality is provided by
 * subclasses. The subclass used by default is cInifile which reads config
 * files in the omnetpp.ini format. One can create other subclasses which use
 * different data sources (e.g. database) or different data format (e.g. XML).
 *
 * To switch to your own configuration storage (e.g. database or XML files)
 * from omnetpp.ini, subclass cConfiguration, register your new class with
 * the Register_Class() macro, then create the following <tt>omnetpp.ini</tt>:
 *
 * <pre>
 * [General]
 * configuration-class="MyClass"
 * </pre>
 *
 * \opp will get the rest of the configuration from your configuration
 * class.
 *
 * See also: cEnvir::config()
 *
 * @ingroup EnvirExtensions
 */
//FIXME explain what is cConfigEntry (ie purpose: to detect unrecognized entries (ie caused by typos) in omnetpp.ini)
//XXX cConfigEntry: rename to cConfigEntryDecl?
//XXX refine the above docu!
class ENVIR_API cConfiguration : public cObject
{
  public:
    /**
     * Describes a configuration entry.
     */
    class KeyValue {
      public:
        virtual ~KeyValue() {}
        virtual const char *getKey() const = 0;
        virtual const char *getValue() const = 0;
        virtual const char *getBaseDirectory() const = 0;
    };

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

    /**
     * Returns the name of the configuration file. Returns NULL if this object is
     * not using a configuration file.
     */
    virtual const char *getFileName() const = 0;

    /** @name Activating a configuration or scenario */
    //@{
    /**
     * Returns the names of the available configurations and scenarios.
     */
    virtual std::vector<std::string> getConfigNames() = 0;

    /**
     * Activates the [Scenario <name>] section, or if it does not exist,
     * the [Config <name>] section. If neither exists, an error is thrown.
     * ([General] is treated as if it was short for [Config General].)
     * The runNumber must be between 0 and getNumRunsInScenario(name)-1.
     */
    virtual void activateConfig(const char *scenarioOrConfigName, int runNumber=0) = 0;

    /**
     * Returns the description of the given configuration or scenario.
     */
    virtual std::string getConfigDescription(const char *scenarioOrConfigName) const = 0;

    /**
     * Returns the name of the config the given configuration or scenario extends.
     * Only names of *existing* configuration names are returned (that is,
     * if "extends" is bogus and refers to a nonexistent configuration,
     * this method returns the empty string; also, "General" is only returned
     * if such configuration actually exists.)
     */
    virtual std::string getBaseConfig(const char *scenarioOrConfigName) const = 0;

    /**
     * Generates Cartesian product of all iterations within the scenario, and counts them.
     */
    virtual int getNumRunsInScenario(const char *scenarioName) const = 0;

    /**
     * Generates all runs in the given scenario, and returns a string for each.
     * When detailed==false, each run will generate a one-line string with the
     * iteration variables; with detailed==true, each run generates a multi-line
     * string containing the config entries that contain iterations or iteration
     * variable references. This method is primarily for debugging purposes.
     */
    virtual std::vector<std::string> unrollScenario(const char *scenarioName, bool detailed=true) const = 0;

    /**
     * Returns the name of the currently active configuration or scenario.
     */
    virtual const char *getActiveConfigName() const = 0;

    /**
     * Returns currently active run number. This is the number passed to
     * activateScenario(), or 0 if activateConfig() was called.
     */
    virtual int getActiveRunNumber() const = 0;

    /**
     * For debugging.
     */
    virtual void dump() const = 0;
    //@}

    /** @name Getting values from the currently active configuration or scenario */
    //@{
    /**
     * Returns a configuration value. Valid keys don't contain dots or wildcard characters.
     * Returns NULL if key is not found.
     */
    virtual const char *getConfigValue(const char *key) const = 0;

    /**
     * Like getConfigValue(), but this one returns information about the
     * whole inifile entry, not just the value string.
     * If the key is not found, a special KeyValue object is returned
     * where both key and value are NULL.
     *
     * Lifetime of the returned object might be limited, so clients
     * should not store references to it.
     */
    virtual const KeyValue& getConfigEntry(const char *key) const = 0;

    /**
     * Returns the list of config keys that match the given wildcard pattern.
     * The returned keys can be passed to getConfigValue().
     */
    virtual std::vector<const char *> getMatchingConfigKeys(const char *pattern) const = 0;

    /**
     * Looks up the value of the given parameter in the inifile. The argument
     * hasDefaultValue controls whether .apply-default entries need to be considered.
     * Return value is NULL if the parameter is not specified in the inifile;
     * it is empty string "" if the default needs to be applied (Note that the
     * empty string as value is syntactically not allowed in ini files; for string
     * parameters the empty value is two quotes (the C++ "\"\"" literal)).
     */
    virtual const char *getParameterValue(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const = 0;

    /**
     * Like getParameterValue(), but this one returns information about the
     * whole inifile entry, not just the value string.
     * If the key is not found, a special KeyValue object is returned
     * where both key and value are NULL.
     *
     * Lifetime of the returned object might be limited, so clients
     * should not store references to it.
     */
    virtual const KeyValue& getParameterEntry(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const = 0;

    /**
     * TODO
     * keySuffix is something like "record-interval", "ev-output", etc.
     */
    virtual const char *getPerObjectConfigValue(const char *objectFullPath, const char *keySuffix) const = 0;

    /**
     * Like getPerObjectConfigValue(), but this one returns information about the
     * whole inifile entry, not just the value string.
     * If the key is not found, a special KeyValue object is returned
     * where both key and value are NULL.
     *
     * Lifetime of the returned object might be limited, so clients
     * should not store references to it.
     */
   virtual const KeyValue& getPerObjectConfigEntry(const char *objectFullPath, const char *keySuffix) const = 0;

    /**
     * Returns the list of config keys that match the given wildcard pattern.
     * The returned keys can be passed to getPerObjectConfigValue().
     * objectFullPath may not contain wildcards.
     */
    virtual std::vector<const char *> getMatchingPerObjectConfigKeys(const char *objectFullPath, const char *keySuffixPattern) const = 0;
    //@}

    /** @name XXX */
    //@{
    static bool parseBool(const char *s, const char *defaultValue, bool fallbackValue=false);
    static long parseLong(const char *s, const char *defaultValue, long fallbackValue=0);
    static double parseDouble(const char *s, const char *unit, const char *defaultValue, double fallbackValue=0);
    static std::string parseString(const char *s, const char *defaultValue, const char *fallbackValue="");
    static std::string parseFilename(const char *s, const char *baseDir, const char *defaultValue);
    static std::vector<std::string> parseFilenames(const char *s, const char *baseDir, const char *defaultValue);
    //@}

    /** @name XXX */
    //@{
    /**
     * Returns a config value without any conversion.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual const char *getAsCustom(cConfigEntry *entry, const char *fallbackValue=NULL);

    /**
     * Returns a config value as bool.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual bool getAsBool(cConfigEntry *entry, bool fallbackValue=false);

    /**
     * Returns a config value as long.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual long getAsInt(cConfigEntry *entry, long fallbackValue=0);

    /**
     * Returns a config value as double.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual double getAsDouble(cConfigEntry *entry, double fallbackValue=0);

    /**
     * Returns a config value as string.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual std::string getAsString(cConfigEntry *entry, const char *fallbackValue="");

    /**
     * Interprets the config value as a path. If it is relative, then it will be
     * converted to an absolute path, using the base directory (see the
     * getBaseDirectoryFor() method).
     */
    virtual std::string getAsFilename(cConfigEntry *entry);

    /**
     * Interprets the config value as a list of paths (file or directory names,
     * possibly containing wildcards), separated by spaces. The relative paths
     * in the list will be converted to absolute, using the base directory
     * (see getBaseDirectoryFor() method).
     */
    virtual std::vector<std::string> getAsFilenames(cConfigEntry *entry);
    //@}

    /** @name XXX */
    //@{
    /**
     * Returns a per-object config value without any conversion.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual const char *getAsCustom(const char *objectFullPath, cConfigEntry *entry, const char *fallbackValue=NULL);

    /**
     * Returns a per-object config value as bool.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual bool getAsBool(const char *objectFullPath, cConfigEntry *entry, bool fallbackValue=false);

    /**
     * Returns a per-object config value as long.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual long getAsInt(const char *objectFullPath, cConfigEntry *entry, long fallbackValue=0);

    /**
     * Returns a per-object config value as double.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual double getAsDouble(const char *objectFullPath, cConfigEntry *entry, double fallbackValue=0);

    /**
     * Returns a per-object config value as string.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual std::string getAsString(const char *objectFullPath, cConfigEntry *entry, const char *fallbackValue="");

    /**
     * Interprets the per-object config value as a path. If it is relative,
     * then it will be converted to an absolute path, using the base directory
     * (see KeyValue::getBaseDirectory()).
     */
    virtual std::string getAsFilename(const char *objectFullPath, cConfigEntry *entry);

    /**
     * Interprets the per-object config value as a list of paths (file or directory
     * names, possibly containing wildcards), separated by spaces. The relative paths
     * in the list will be converted to absolute, using the base directory
     * (see KeyValue::getBaseDirectory()).
     */
    virtual std::vector<std::string> getAsFilenames(const char *objectFullPath, cConfigEntry *entry);

    //@}
};

#endif


