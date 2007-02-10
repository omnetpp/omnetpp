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
class SIM_API cConfigEntry : public cNoncopyableOwnedObject
{
  public:
    /// Configuration entry types.
    enum Type {
      CFG_BOOL,
      CFG_INT,
      CFG_DOUBLE, // note: former "time" type is now double with unit="s"
      CFG_STRING,
      CFG_FILENAME,
      CFG_FILENAMES,
      CFG_CUSTOM
    };

    // note: entry name (e.g. "sim-time-limit") is stored in object's name field
    std::string section_;      // e.g. "General"
    bool isGlobal_;            // if true, it cannot occur in [Run X] sections
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
    cConfigEntry(const char *name, const char *section, bool isGlobal, Type type,
                 const char *unit, const char *defaultValue, const char *description) :
        cNoncopyableOwnedObject(name)
    {
        section_ = section;
        isGlobal_ = isGlobal;
        type_ = type;
        unit_ = unit ? unit : "";
        defaultValue_ = defaultValue ? defaultValue : "";
        description_ = description ? description : "";
    }

    /**
     * Destructor.
     */
    virtual ~cConfigEntry() {}
    //@}

    /** @name Getter methods */
    //@{
    //XXX comments
    const char *section()  {return section_.c_str();}
    bool isGlobal()  {return isGlobal_;}
    Type type()  {return type_;}
    const char *unit()  {return unit_.empty() ? NULL : unit_.c_str();}
    const char *defaultValue()  {return defaultValue_.empty() ? NULL : defaultValue_.c_str();
    const char *description()  {return description_.c_str();}
    //@}
};

#endif


