//==========================================================================
//  CCONFIGURATION.H - part of
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

#ifndef __OMNETPP_CCONFIGURATION_H
#define __OMNETPP_CCONFIGURATION_H

#include <string>
#include <map>
#include <vector>
#include "simkerneldefs.h"
#include "cobject.h"

namespace omnetpp {

class cConfigOption;

/**
 * @brief "General", the name of the default configuration
 */
#define CFGNAME_GENERAL "General"

/**
 * @brief Predefined configuration variables; see cConfigurationEx::getVariable().
 * Variables can be referred to using the ${...} syntax in the configuration.
 */
//@{
#define CFGVAR_RUNID            "runid"
#define CFGVAR_INIFILE          "inifile"
#define CFGVAR_CONFIGNAME       "configname"
#define CFGVAR_RUNNUMBER        "runnumber"
#define CFGVAR_NETWORK          "network"
#define CFGVAR_EXPERIMENT       "experiment"
#define CFGVAR_MEASUREMENT      "measurement"
#define CFGVAR_REPLICATION      "replication"
#define CFGVAR_PROCESSID        "processid"
#define CFGVAR_DATETIME         "datetime"
#define CFGVAR_RESULTDIR        "resultdir"
#define CFGVAR_REPETITION       "repetition"
#define CFGVAR_SEEDSET          "seedset"
#define CFGVAR_ITERATIONVARS    "iterationvars"   // without $repetition
#define CFGVAR_ITERATIONVARSF   "iterationvarsf"  // same, sanitized for use in file names
//@}

/**
 * @brief Represents the configuration, as accessed by the simulation kernel.
 *
 * This class provides access to configuration data for components such as
 * scheduler classes, parallel simulation algorithms, etc. Model code (simple
 * modules) should not directly read the configuration, they are expected
 * to rely on module parameters for input.
 *
 * This class logically belongs to the cEnvir facade, and the configuration
 * instance can be accessed with getEnvir()->getConfig().
 *
 * @see cConfigurationEx, cEnvir::getConfig()
 * @ingroup ExtensionPoints
 */
class SIM_API cConfiguration : public cObject
{
  public:
    /**
     * @brief Describes a configuration entry.
     */
    class KeyValue {
      public:
        virtual ~KeyValue() {}
        virtual const char *getKey() const = 0;
        virtual const char *getValue() const = 0;
        virtual const char *getBaseDirectory() const = 0;
    };

    /**
     * @brief Struct used by unrollConfig() to return information.
     */
    struct RunInfo {
        std::string info; // concatenated
        std::map<std::string,std::string> runAttrs; // run attributes
        std::string configBrief; // config options that contain inifile variables (${foo}), expanded
    };

  public:
    /** @name String-based getters for configuration options */
    //@{
    /**
     * Returns a configuration value. Valid keys do not contain dots or wildcard characters.
     * Returns nullptr if key is not found.
     */
    virtual const char *getConfigValue(const char *key) const = 0;

    /**
     * Like getConfigValue(), but this one returns information about the
     * whole inifile entry, not just the value string.
     * If the key is not found, a special KeyValue object is returned
     * where both key and value are nullptr.
     *
     * Lifetime of the returned object might be limited, so clients
     * should not store references to it. Copying the object is not allowed
     * either, because KeyValue is a polymorphic type (object slicing!).
     */
    virtual const KeyValue& getConfigEntry(const char *key) const = 0;

    /**
     * Returns a per-object configuration value. Valid keysuffixes do not contain
     * dots or wildcard characters. Returns nullptr if key is not found.
     * keySuffix is something like "vector-recording-intervals", "ev-output", etc.
     */
    virtual const char *getPerObjectConfigValue(const char *objectFullPath, const char *keySuffix) const = 0;

    /**
     * Like getPerObjectConfigValue(), but this one returns information about the
     * whole inifile entry, not just the value string.
     * If the key is not found, a special KeyValue object is returned
     * where both key and value are nullptr.
     *
     * Lifetime of the returned object might be limited, so clients
     * should not store references to it. Copying the object is not allowed
     * either, because KeyValue is a polymorphic type (object slicing!).
     */
    virtual const KeyValue& getPerObjectConfigEntry(const char *objectFullPath, const char *keySuffix) const = 0;
    //@}

    /** @name Utility functions for parsing config entries */
    //@{
    static bool parseBool(const char *s, const char *defaultValue, bool fallbackValue=false);
    static long parseLong(const char *s, const char *defaultValue, long fallbackValue=0);
    static double parseDouble(const char *s, const char *unit, const char *defaultValue, double fallbackValue=0);
    static std::string parseString(const char *s, const char *defaultValue, const char *fallbackValue="");
    static std::string parseFilename(const char *s, const char *baseDir, const char *defaultValue);
    static std::vector<std::string> parseFilenames(const char *s, const char *baseDir, const char *defaultValue);
    static std::string adjustPath(const char *s, const char *baseDir, const char *defaultValue);
    //@}

    /** @name Getters for configuration options */
    //@{
//FIXME why aren't these methods all const???
    /**
     * Returns a config value without any conversion.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual const char *getAsCustom(cConfigOption *option, const char *fallbackValue=nullptr) const;

    /**
     * Returns a config value as bool.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual bool getAsBool(cConfigOption *option, bool fallbackValue=false) const;

    /**
     * Returns a config value as long.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual long getAsInt(cConfigOption *option, long fallbackValue=0) const;

    /**
     * Returns a config value as double.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual double getAsDouble(cConfigOption *option, double fallbackValue=0) const;

    /**
     * Returns a config value as string.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual std::string getAsString(cConfigOption *option, const char *fallbackValue="") const;

    /**
     * Interprets the config value as a path. If it is relative, then it will be
     * converted to an absolute path, using the base directory (see the
     * getBaseDirectoryFor() method).
     */
    virtual std::string getAsFilename(cConfigOption *option) const;

    /**
     * Interprets the config value as a list of paths (file or directory names,
     * possibly containing wildcards), separated by spaces. The relative paths
     * in the list will be converted to absolute, using the base directory
     * (see getBaseDirectoryFor() method).
     */
    virtual std::vector<std::string> getAsFilenames(cConfigOption *option) const;

    /**
     * Interprets the config value as a list of directory names, possibly
     * containing wildcards, and separated by ";" (Windows), or ":" or ";"
     * (other OSes). The relative names in the list will be converted to absolute,
     * using the base directory (see getBaseDirectoryFor() method).
     */
    virtual std::string getAsPath(cConfigOption *option) const;
    //@}

    /** @name Getters for per-object configuration options */
    //@{
    /**
     * Returns a per-object config value without any conversion.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual const char *getAsCustom(const char *objectFullPath, cConfigOption *option, const char *fallbackValue=nullptr) const;

    /**
     * Returns a per-object config value as bool.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual bool getAsBool(const char *objectFullPath, cConfigOption *option, bool fallbackValue=false) const;

    /**
     * Returns a per-object config value as long.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual long getAsInt(const char *objectFullPath, cConfigOption *option, long fallbackValue=0) const;

    /**
     * Returns a per-object config value as double.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual double getAsDouble(const char *objectFullPath, cConfigOption *option, double fallbackValue=0) const;

    /**
     * Returns a per-object config value as string.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual std::string getAsString(const char *objectFullPath, cConfigOption *option, const char *fallbackValue="") const;

    /**
     * Interprets the per-object config value as a path. If it is relative,
     * then it will be converted to an absolute path, using the base directory
     * (see KeyValue::getBaseDirectory()).
     */
    virtual std::string getAsFilename(const char *objectFullPath, cConfigOption *option) const;

    /**
     * Interprets the per-object config value as a list of paths (file or directory
     * names, possibly containing wildcards), separated by spaces. The relative paths
     * in the list will be converted to absolute, using the base directory
     * (see KeyValue::getBaseDirectory()).
     */
    virtual std::vector<std::string> getAsFilenames(const char *objectFullPath, cConfigOption *option) const;

    /**
     * Interprets the per-object config value as a list of directory names, possibly
     * containing wildcards, and separated by ";" (Windows), or ":" or ";"
     * (other OSes). The relative names in the list will be converted to absolute,
     * using the base directory (see getBaseDirectoryFor() method).
     */
    virtual std::string getAsPath(const char *objectFullPath, cConfigOption *option) const;
    //@}

    /** @name Other methods */
    //@{
    /**
     * Substitutes ${} variables into the given string. The resulting string
     * is stored inside the configuration object.
     */
    virtual const char *substituteVariables(const char *value) const = 0;
    //@}
};


/**
 * @brief Represents a configuration suitable for use with the Envir library.
 *
 * This class extends cConfiguration with the following functionality:
 *   - Methods for reading module parameters; cEnvir's readParameter()
 *     method delegates to them by default.
 *   - Support for multiple configurations (enumeration, activation, etc.)
 *   - Parameter Study support: run numbers, iteration variables, unrolling, etc.
 *   - Other utility functions like dump()
 *
 * @see cEnvir::getConfigEx()
 * @ingroup ExtensionPoints
 */
class SIM_API cConfigurationEx : public cConfiguration
{
  public:
    /**
     * Initializes configuration object from "boot-time" configuration
     * (omnetpp.ini). For example, if a particular cConfiguration subclass
     * uses a database as data source, it may take the connection parameters
     * from the "boot-time" configuration. This method makes global config
     * immediately available, i.e. there is no need for an additional
     * activateConfig() call.
     */
    virtual void initializeFrom(cConfiguration *bootConfig) = 0;

    /**
     * Returns the name of the configuration file. Returns nullptr if this object is
     * not using a configuration file.
     */
    virtual const char *getFileName() const = 0;

    /**
     * Validates the configuration: reports obsolete config keys,
     * cycles in the section fallback chain, unrecognized keys, etc
     * as exceptions.
     *
     * ignorableConfigKeys is the list of config keys whose presence in the
     * configuration should not be reported as errors even if they are not
     * declared using cConfigOptions. The list is space-separated, and items may
     * contain wildcards. Typically, this list will contain "cmdenv-*" when
     * Cmdenv is unavailable (not linked in), "tkenv-*" when Tkenv is unavailable,
     * etc, so that validate() does not report those keys in omnetpp.ini as errors.
     */
    virtual void validate(const char *ignorableConfigKeys=nullptr) const = 0;

    /** @name Activating a configuration */
    //@{
    /**
     * Returns the names of the available configurations.
     */
    virtual std::vector<std::string> getConfigNames() = 0;

    /**
     * Activates the [Config \<name\>] section. If it does not exist, an error
     * is thrown. [General] is treated as short for [Config General].
     * The runNumber must be between 0 and getNumRunsInConfig(name)-1.
     */
    virtual void activateConfig(const char *configName, int runNumber=0) = 0;

    /**
     * Returns the description of the given configuration.
     */
    virtual std::string getConfigDescription(const char *configName) const = 0;

    /**
     * Returns the names of the configurations the given configuration extends directly.
     * Only names of *existing* configurations are returned (that is,
     * if "extends" is bogus and refers to a nonexistent configuration,
     * this method omits that configuration name; also, "General" is only returned
     * if such configuration actually exists.)
     */
    virtual std::vector<std::string> getBaseConfigs(const char *configName) const = 0;

    /**
     * Returns the names of the configurations the given configuration extends transitively.
     * This is the search order of parameter lookups from the given configuration.
     * Only names of *existing* configurations are returned.
     */
    virtual std::vector<std::string> getConfigChain(const char * configName) const = 0;

    /**
     * Generates Cartesian product of all iterations within the given config,
     * and counts them. If the config does not exist, an error is thrown.
     * [General] is treated as short for [Config General].
     */
    virtual int getNumRunsInConfig(const char *configName) const = 0;

    /**
     * Generates all runs in the given configuration, and returns a string for each.
     * When detailed==false, each run will generate a one-line string with the
     * iteration variables; with detailed==true, each run generates a multi-line
     * string containing the config entries that contain iterations or iteration
     * variable references. This method is primarily for debugging purposes.
     *
     * TODO update description
     */
    virtual std::vector<RunInfo> unrollConfig(const char *configName) const = 0;

    /**
     * Returns the name of the currently active configuration.
     */
    virtual const char *getActiveConfigName() const = 0;

    /**
     * Returns currently active run number. This is the number passed to
     * activateConfig(), or 0 if activateConfig() has not been called.
     */
    virtual int getActiveRunNumber() const = 0;

    /**
     * After activating a configuration, this method can be used to query
     * iteration variables and predefined variables. These are the variables
     * that can be referred to using the "${...}" syntax in the configuration.
     * If the variable does not exist, nullptr is returned.
     *
     * Some of the predefined variables are: "configname", "runnumber", "network",
     * "processid", "datetime", "runid", "repetition", "iterationvars";
     * these names are also available as symbolic constants, see CFGVAR_CONFIGNAME
     * and other CFGVAR_xxx names.
     */
    virtual const char *getVariable(const char *varname) const = 0;

    /**
     * Returns the names of all iteration variables in the activated configuration.
     */
    virtual std::vector<const char *> getIterationVariableNames() const = 0;

    /**
     * Returns the names of all predefined variables in the activated configuration.
     * See getVariable().
     */
    virtual std::vector<const char *> getPredefinedVariableNames() const = 0;

    /**
     * Returns the description of the given variable in the activated configuration.
     * Returns nullptr if the variable does not exist or no description is available.
     */
    virtual const char *getVariableDescription(const char *varname) const = 0;

    /**
     * For debugging.
     */
    virtual void dump() const = 0;
    //@}

    /** @name Getting values from the currently active configuration */
    //@{

    /**
     * Returns the list of config keys that match the given wildcard pattern.
     * The returned keys can be passed to getConfigValue().
     */
    virtual std::vector<const char *> getMatchingConfigKeys(const char *pattern) const = 0;

    /**
     * Looks up the value of the given parameter in the inifile. The argument
     * hasDefaultValue controls whether "=default" entries need to be considered.
     * Return value is nullptr if the parameter is not specified in the inifile,
     * otherwise returns the string after the equal sign.
     */
    virtual const char *getParameterValue(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const = 0;

    /**
     * Like getParameterValue(), but this one returns information about the
     * whole inifile entry, not just the value string.
     * If the key is not found, a special KeyValue object is returned
     * where both key and value are nullptr.
     *
     * Lifetime of the returned object might be limited, so clients
     * should not store references to it. Copying the object is not allowed
     * either, because KeyValue is a polymorphic type (object slicing!).
     */
    virtual const KeyValue& getParameterEntry(const char *moduleFullPath, const char *paramName, bool hasDefaultValue) const = 0;

    /**
     * This method returns an array of the following form: (key1, value1,
     * key2, value2,...), where keys and values correspond to entries in the
     * active configuration.
     */
    virtual std::vector<const char *> getKeyValuePairs() const = 0;

    /**
     * This method returns an array of the following form: (key1, value1,
     * key2, value2,...), where keys and values correspond to parameter
     * assignments in the active configuration. This method could be
     * useful for writing the header of result files.
     */
    virtual std::vector<const char *> getParameterKeyValuePairs() const = 0;

    /**
     * Returns the list of config keys that match the wildcard pattern
     * objectFullPath + dot + keySuffixPattern. objectFullPath may either be
     * a concrete string without wildcards, or "**" to match anything.
     * The returned keys may be used with getPerObjectConfigValue() to obtain
     * the corresponding values.
     */
    virtual std::vector<const char *> getMatchingPerObjectConfigKeys(const char *objectFullPath, const char *keySuffixPattern) const = 0;

    /**
     * Like getMatchingPerObjectConfigKeys(), but returns the suffixes instead
     * of the keys. Note: the result may contain duplicates. The returned suffixes
     * may be used with getPerObjectConfigValue() to obtain the corresponding values.
     */
    virtual std::vector<const char *> getMatchingPerObjectConfigKeySuffixes(const char *objectFullPath, const char *keySuffixPattern) const = 0;
    //@}
};

}  // namespace omnetpp

#endif


