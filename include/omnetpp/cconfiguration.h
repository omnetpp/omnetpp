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

class FileLine;
class cConfigOption;

/**
 * @brief "General", the name of the default configuration
 */
#define CFGNAME_GENERAL "General"

/**
 * @brief Predefined configuration variables; see InifileContents::getVariable().
 * Variables can be referred to using the ${...} syntax in the configuration.
 *
 * @defgroup ConfigVars Predefined configuration variables
 * @ingroup EnvirAndExtensions
 * @{
 */
#define CFGVAR_RUNID            "runid"           ///< Name of the "runid" configuration variable and run attribute
#define CFGVAR_INIFILE          "inifile"         ///< Name of the "inifile" configuration variable and run attribute
#define CFGVAR_CONFIGNAME       "configname"      ///< Name of the "configname" configuration variable and run attribute
#define CFGVAR_RUNNUMBER        "runnumber"       ///< Name of the "runnumber" configuration variable and run attribute
#define CFGVAR_DESCRIPTION      "description"     ///< Name of the "description" configuration variable and run attribute
#define CFGVAR_NETWORK          "network"         ///< Name of the "network" configuration variable and run attribute
#define CFGVAR_EXPERIMENT       "experiment"      ///< Name of the "experiment" configuration variable and run attribute
#define CFGVAR_MEASUREMENT      "measurement"     ///< Name of the "measurement" configuration variable and run attribute
#define CFGVAR_REPLICATION      "replication"     ///< Name of the "replication" configuration variable and run attribute
#define CFGVAR_PROCESSID        "processid"       ///< Name of the "processid" configuration variable and run attribute
#define CFGVAR_DATETIME         "datetime"        ///< Name of the "datetime" configuration variable and run attribute
#define CFGVAR_DATETIMEF        "datetimef"       ///< Name of the "datetimef" configuration variable and run attribute
#define CFGVAR_RESULTDIR        "resultdir"       ///< Name of the "resultdir" configuration variable and run attribute
#define CFGVAR_REPETITION       "repetition"      ///< Name of the "repetition" configuration variable and run attribute
#define CFGVAR_SEEDSET          "seedset"         ///< Name of the "seedset" configuration variable and run attribute
#define CFGVAR_ITERATIONVARS    "iterationvars"   ///< Name of the "iterationvars" configuration variable and run attribute
#define CFGVAR_ITERATIONVARSF   "iterationvarsf"  ///< Name of the "iterationvarsf" configuration variable and run attribute
#define CFGVAR_ITERATIONVARSD   "iterationvarsd"  ///< Name of the "iterationvarsd" configuration variable and run attribute
/** @} */

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
 * @see InifileContents, cEnvir::getConfig()
 * @ingroup EnvirAndExtensions
 */
class SIM_API cConfiguration : public cObject
{
  public:
    /**
     * @brief Describes a configuration entry.
     */
    class SIM_API KeyValue {
      public:
        virtual ~KeyValue() {}
        virtual const char *getKey() const = 0;
        virtual const char *getValue() const = 0;
        virtual const char *getBaseDirectory() const = 0;
        virtual const char *getComment() const {return nullptr;}
        virtual const char *getOriginSection() const {return nullptr;}
        virtual FileLine getSourceLocation() const;
        virtual std::string str() const;
    };

    /**
     * Flags for the 'flags' argument of the getKeyValuePairs() method.
     */
    enum FilterFlags {
        FILT_ESSENTIAL_CONFIG = 1 << 0,
        FILT_GLOBAL_CONFIG = 1 << 1 | FILT_ESSENTIAL_CONFIG, // both per-run and not per-run
        FILT_PER_OBJECT_CONFIG = 1 << 2,
        FILT_CONFIG = FILT_GLOBAL_CONFIG | FILT_PER_OBJECT_CONFIG,
        FILT_PARAM = 1 << 3,
        FILT_ALL = FILT_CONFIG | FILT_PARAM
    };

  protected:
    // internal utility method
    cComponent *getContextComponent() const;

  public:
    /** @name String-based getters for configuration options */
    //@{
    /**
     * Returns the value for the given key in the configuration.
     * The key should not contain dots or wildcard characters.
     * Returns nullptr if key is not found.
     */
    virtual const char *getConfigValue(const char *key) const = 0;

    /**
     * Returns the entry for the given key in the configuration.
     * If the key is not found, a special KeyValue object is returned
     * where both key and value are nullptr.
     *
     * The lifetime of the returned object might be limited, so clients
     * should not store references to it. Copying the object is not allowed
     * either, because KeyValue is a polymorphic type (object slicing!).
     */
    virtual const KeyValue& getConfigEntry(const char *key) const = 0;

    /**
     * Returns the value for the key "<objectFullPath>.<keySuffix>" in the
     * configuration. keySuffix should not contain dots or wildcard characters.
     * Example keySuffix: "vector-recording-intervals".
     * Returns nullptr if key is not found.
     */
    virtual const char *getPerObjectConfigValue(const char *objectFullPath, const char *keySuffix) const = 0;

    /**
     * Returns the entry for the key "<objectFullPath>.<keySuffix>" in the
     * configuration. keySuffix should not contain dots or wildcard characters.
     * Example keySuffix: "vector-recording-intervals".
     * If the key is not found, a special KeyValue object is returned
     * where both key and value are nullptr.
     *
     * The lifetime of the returned object might be limited, so clients
     * should not store references to it. Copying the object is not allowed
     * either, because KeyValue is a polymorphic type (object slicing!).
     */
    virtual const KeyValue& getPerObjectConfigEntry(const char *objectFullPath, const char *keySuffix) const = 0;
    //@}

    /** @name Utility functions for parsing config entries */
    //@{
    bool parseBool(const char *s, const char *defaultValue, bool fallbackValue=false) const;
    long parseLong(const char *s, const char *defaultValue, long fallbackValue=0) const;
    double parseDouble(const char *s, const char *unit, const char *defaultValue, double fallbackValue=0) const;
    std::string parseString(const char *s, const char *defaultValue, const char *fallbackValue="") const;
    std::string parseFilename(const char *s, const char *baseDir, const char *defaultValue) const;
    std::vector<std::string> parseFilenames(const char *s, const char *baseDir, const char *defaultValue) const;
    std::string adjustPath(const char *s, const char *baseDir, const char *defaultValue) const;
    //@}

    /** @name Getters for configuration options */
    //@{
    /**
     * Returns the value of the configuration option in string form, independent of its type.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual const char *getConfigValue(cConfigOption *option, const char *fallbackValue=nullptr) const;

    /**
     * Returns the value of the configuration option without any conversion. The option's type must be CFG_CUSTOM.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual const char *getAsCustom(cConfigOption *option, const char *fallbackValue=nullptr) const;

    /**
     * Returns the value of the configuration option as bool. The option's type must be CFG_BOOL.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual bool getAsBool(cConfigOption *option, bool fallbackValue=false) const;

    /**
     * Returns the value of the configuration option as long. The option's type must be CFG_INT.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual long getAsInt(cConfigOption *option, long fallbackValue=0) const;

    /**
     * Returns the value of the configuration option as double. The option's type must be CFG_DOUBLE.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual double getAsDouble(cConfigOption *option, double fallbackValue=0) const;

    /**
     * Returns the value of the configuration option as string. The option's type must be CFG_STRING.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual std::string getAsString(cConfigOption *option, const char *fallbackValue="") const;

    /**
     * Returns the value of the configuration option as a file path. The option's type must be CFG_FILENAME.
     * If it is relative path, then it will be converted to an absolute path,
     * using the base directory (see the getBaseDirectoryFor() method).
     */
    virtual std::string getAsFilename(cConfigOption *option) const;

    /**
     * Returns the value of the configuration option as a list of file paths (file or directory names,
     * possibly containing wildcards), separated by spaces. The option's type must be CFG_FILENAMES.
     * The relative paths in the list will be converted to absolute, using the base directory
     * (see getBaseDirectoryFor() method).
     */
    virtual std::vector<std::string> getAsFilenames(cConfigOption *option) const;

    /**
     * Returns the value of the configuration option as a list of directory names, possibly
     * containing wildcards, and separated by ";" (Windows), or ":" or ";"
     * (other OSes). The option's type must be CFG_PATH.
     * The relative names in the list will be converted to absolute,
     * using the base directory (see getBaseDirectoryFor() method).
     */
    virtual std::string getAsPath(cConfigOption *option) const;
    //@}

    /** @name Getters for per-object configuration options */
    //@{
    /**
     * Returns a per-object config value in string form, independent of its type.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual const char *getPerObjectConfigValue(const char *objectFullPath, cConfigOption *option, const char *fallbackValue=nullptr) const;

    /**
     * Returns a per-object config value without any conversion. The option's type must be CFG_CUSTOM.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual const char *getAsCustom(const char *objectFullPath, cConfigOption *option, const char *fallbackValue=nullptr) const;

    /**
     * Returns a per-object config value as bool. The option's type must be CFG_BOOL.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual bool getAsBool(const char *objectFullPath, cConfigOption *option, bool fallbackValue=false) const;

    /**
     * Returns a per-object config value as long. The option's type must be CFG_INT.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual long getAsInt(const char *objectFullPath, cConfigOption *option, long fallbackValue=0) const;

    /**
     * Returns a per-object config value as double. The option's type must be CFG_DOUBLE.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual double getAsDouble(const char *objectFullPath, cConfigOption *option, double fallbackValue=0) const;

    /**
     * Returns a per-object config value as string. The option's type must be CFG_STRING.
     * fallbackValue is returned if the value is not specified in the configuration, and there is no default value.
     */
    virtual std::string getAsString(const char *objectFullPath, cConfigOption *option, const char *fallbackValue="") const;

    /**
     * Returns the per-object config value as a file path. The option's type must be CFG_FILENAME.
     * If the value is relative, then it will be converted to an absolute path, using the base directory
     * (see KeyValue::getBaseDirectory()).
     */
    virtual std::string getAsFilename(const char *objectFullPath, cConfigOption *option) const;

    /**
     * Returns the per-object config value as a list of file paths (file or directory
     * names, possibly containing wildcards), separated by spaces. The option's type must be CFG_FILENAMES.
     * The relative paths in the list will be converted to absolute, using the base directory
     * (see KeyValue::getBaseDirectory()).
     */
    virtual std::vector<std::string> getAsFilenames(const char *objectFullPath, cConfigOption *option) const;

    /**
     * Returns the per-object config value as a list of directory names, possibly
     * containing wildcards, and separated by ";" (Windows), or ":" or ";"
     * (other OSes). The option's type must be CFG_PATH.
     * The relative names in the list will be converted to absolute,
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

    /**
     * Returns the name of the primary file this configuration comes from,
     * or nullptr if the source was not a file.
     */
    virtual const char *getFileName() const = 0;
    //@}

    /** @name Configuration variables */
    //@{
    /**
     * Returns the name of the configuration represented by this object.
     * This is equivalent to getVariable(CFGVAR_CONFIGNAME).
     */
    virtual const char *getActiveConfigName();

    /**
     * Returns the run number this object represents in a parameter study.
     * This is similar to getVariable(CFGVAR_RUNNUMBER).
     */
    virtual int getActiveRunNumber();

    /**
     * Returns a map containing the names and values of the predefined variables
     * in this configuration.
     */
    virtual std::map<std::string,std::string> getPredefinedVariables() const = 0;

    /**
     * Returns a map containing the names and values of the iteration variables
     * in this configuration.
     */
    virtual std::map<std::string,std::string> getIterationVariables() const = 0;

    /**
     * Returns the value of the give iteration variable or predefined variable.
     * These are the variables that can be referred to using the "${...}" syntax
     * in the configuration. If the variable does not exist, nullptr is returned.
     *
     * Some of the predefined variables are: "configname", "runnumber", "network",
     * "processid", "datetime", "runid", "repetition", "iterationvars";
     * these names are also available as symbolic constants, see CFGVAR_CONFIGNAME
     * and other CFGVAR_xxx names.
     *
     * @see getIterationVariables(), getPredefinedVariables()
     */
    virtual const char *getVariable(const char *varname) const = 0;
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
     * key2, value2,...), where keys and values correspond to (a subset of)
     * the entries in the active configuration.
     *
     * The 'flags' parameter should take its value from the FilterFlags enum,
     * or the binary OR of several such flags: FILT_PARAM, FILT_CONFIG, etc.
     */
    virtual std::vector<const char *> getKeyValuePairs(int flags=FILT_ALL) const = 0;

    /**
     * Returns the list of suffixes from keys that match the wildcard pattern
     * objectFullPath + dot + keySuffixPattern. objectFullPath may either be
     * a concrete string without wildcards, or "**" to match anything.
     * The returned keys may be used with getPerObjectConfigValue() to obtain
     * the corresponding values. The returned suffixes may be used with
     * getPerObjectConfigValue() to obtain the corresponding values.
     */
    virtual std::vector<const char *> getMatchingPerObjectConfigKeySuffixes(const char *objectFullPath, const char *keySuffixPattern) const = 0;

    /**
     * Returns the list of entries that have not been accessed. This can be
     * useful for detecting entries in the configuration that take no effect.
     * The method tries to be clever about which entries to report, unless all=true
     * is specified. The postsimulation flag indicates whether it is invoked after
     * completing the simulation (=true) or earlier (=false).
     */
    virtual std::vector<const KeyValue*> getUnusedEntries(bool all=false, bool postsimulation=false) const {return std::vector<const KeyValue*>();}

    /**
     * Resets usage info on all config entries, i.e. mark them as having been
     * never accessed.
     */
    virtual void clearUsageInfo() {}
    //@}

};

}  // namespace omnetpp

#endif


