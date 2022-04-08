//==========================================================================
//  CCONFIGOPTION.H - part of
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

#ifndef __OMNETPP_CCONFIGOPTION_H
#define __OMNETPP_CCONFIGOPTION_H

#include <string>
#include "cownedobject.h"

namespace omnetpp {

/**
 * @brief Describes a configuration option.
 *
 * @see Register_GlobalConfigOption(), Register_PerRunConfigOption(),
 *      Register_GlobalConfigOptionU(), Register_PerRunConfigOptionU(),
 *      Register_PerObjectConfigOption(), Register_PerObjectConfigOptionU()
 *
 * @ingroup Misc
 */
class SIM_API cConfigOption : public cNoncopyableOwnedObject
{
  public:
    /// Configuration option data types.
    enum Type {
      CFG_BOOL,
      CFG_INT,
      CFG_DOUBLE,
      CFG_STRING,
      CFG_FILENAME,
      CFG_FILENAMES,
      CFG_PATH,
      CFG_CUSTOM
    };

    /// Configuration option kinds.
    enum ObjectKind {
        KIND_NONE,
        KIND_COMPONENT,
        KIND_CHANNEL,
        KIND_MODULE,
        KIND_SIMPLE_MODULE,
        KIND_UNSPECIFIED_TYPE, // for 'typename' option (object is submodule/channel declared with 'like')
        KIND_PARAMETER,
        KIND_STATISTIC,
        KIND_SCALAR,
        KIND_VECTOR,
        KIND_OTHER
    };

    // note: option name (e.g. "sim-time-limit") is stored in object's name field
    bool isPerObject_;         // if true, entries must be in <object-full-path>.config-name format
    bool isGlobal_;            // if true, entries may only occur in the [General] section
    ObjectKind objectKind;     // kind of the object if isPerObject is true, KIND_NONE otherwise
    Type type;                 // option data type
    std::string unit;          // if numeric, its unit ("s") or empty string
    std::string defaultValue;  // the default value in string form
    std::string description;   // help text

  public:
    /** @name Constructors, destructor */
    //@{
    /**
     * Constructor for non per-object options.
     */
    cConfigOption(const char *name, bool isGlobal, Type type, const char *unit,
               const char *defaultValue, const char *description);

    /**
     * Constructor for per-object options.
     */
    cConfigOption(const char *name, ObjectKind kind, Type type, const char *unit,
               const char *defaultValue, const char *description);
    //@}

    /** @name Redefined cObject methods */
    //@{
    virtual std::string str() const override;
    //@}

    /** @name Getter methods */
    //@{
    /**
     * Returns whether this is a per-object configuration. Per-object
     * configuration entries take the form of
     * \<object-full-path\>.\<configname\> = \<value\> in the inifile,
     * instead of \<configname\> = \<value\>
     */
    bool isPerObject() const {return isPerObject_;}

    /**
     * Returns whether this is a global setting. Global settings may only
     * occur in the [General] section.
     */
    bool isGlobal() const  {return isGlobal_;}

    /**
     * Returns the object kind for per-object configuration options,
     * KIND_NONE otherwise.
     */
    ObjectKind getObjectKind() const {return objectKind;}

    /**
     * Returns the human-readable name of a per-object option object kind.
     */
    static const char *getObjectKindName(ObjectKind kind);

    /**
     * Data type of the option.
     */
    Type getType() const  {return type;}

    /**
     * Returns the human-readable name of an option data type.
     */
    static const char *getTypeName(Type type);

    /**
     * Returns the unit of the option (e.g. "s" for seconds, "b" for bytes, etc),
     * or nullptr if the option does not have a unit.
     */
    const char *getUnit() const  {return unit.empty() ? nullptr : unit.c_str();}

    /**
     * Returns the default value in string form, or nullptr if there is no default.
     */
    const char *getDefaultValue() const  {return defaultValue.empty() ? nullptr : defaultValue.c_str();}

    /**
     * Returns a brief textual description of the option, which can be used as
     * help text or hint.
     */
    const char *getDescription() const  {return description.c_str();}
    //@}

    /** @name Lookup */
    //@{
    /**
     * Finds a configuration option by name. Returns nullptr if not found.
     */
    static cConfigOption *find(const char *name);

    /**
     * Finds a configuration option by name. Throws an error if not found.
     */
    static cConfigOption *get(const char *name);
    //@}
};

}  // namespace omnetpp


#endif


