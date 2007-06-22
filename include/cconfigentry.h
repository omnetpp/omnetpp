//==========================================================================
//  CCONFIGENTRY.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCONFIGENTRY_H
#define __CCONFIGENTRY_H

#include <string>
#include "cownedobject.h"


/**
 * Describes a configuration entry.
 *
 * @ingroup Internals
 */
//XXX rename to cConfigKey, cSetting? cConfigurationSetting? cSettingDeclaration? cConfigDeclaration?
class SIM_API cConfigEntry : public cNoncopyableOwnedObject
{
  public:
    /// Configuration entry types.
    enum Type {
      CFG_BOOL,
      CFG_INT,
      CFG_TIME, // note: input only: maps to CFG_DOUBLE with unit="s"
      CFG_DOUBLE,
      CFG_STRING,
      CFG_FILENAME,
      CFG_FILENAMES,
      CFG_CUSTOM
    };

    // note: entry name (e.g. "sim-time-limit") is stored in object's name field
    bool isPerObject_;         // if true, keys must be in <object-full-path>.config-name format
    bool isGlobal_;            // if true, entry may only occur in the [General] section
    Type type_;                // entry type
    std::string unit_;         // if numeric, its unit ("s") or empty string
    std::string defaultValue_; // the default value in string form
    std::string description_;  // help text

  public:
    /** @name Constructors, destructor */
    //@{
    /**
     * Constructor.
     */
    cConfigEntry(const char *name, bool isPerObject, bool isGlobal, Type type, const char *unit,
                 const char *defaultValue, const char *description);
    //@}

    /** @name Redefined cObject methods */
    //@{
    virtual std::string info() const;
    //@}

    /** @name Getter methods */
    //@{
    /**
     * Returns whether this is a per-object configuration. Per-object
     * configuration entries take the form of
     * <object-full-path>.<configname> = <value> in the inifile,
     * instead of <configname> = <value>
     */
    bool isPerObject() const {return isPerObject_;}

    /**
     * Returns whether this is a global setting. Global settings may only
     * occur in the [General] section.
     */
    bool isGlobal() const  {return isGlobal_;}

    /**
     * Data type of the entry.
     */
    Type type() const  {return type_;}

    /**
     * Returns the human-readable name of an entry data type.
     */
    static const char *typeName(Type type);

    /**
     * Returns the unit of the entry (e.g. "s" for seconds, "b" for bytes, etc),
     * or NULL if the entry does not have a unit.
     */
    const char *unit() const  {return unit_.empty() ? NULL : unit_.c_str();}

    /**
     * Returns the default value in string form, or NULL if there is no default.
     */
    const char *defaultValue() const  {return defaultValue_.empty() ? NULL : defaultValue_.c_str();}

    /**
     * Returns a brief textual description of the entry, which can be used as
     * help text or hint.
     */
    const char *description() const  {return description_.c_str();}
    //@}
};

#endif


