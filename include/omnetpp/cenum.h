//==========================================================================
//  CENUM.H - part of
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

#ifndef __OMNETPP_CENUM_H
#define __OMNETPP_CENUM_H

#include "cownedobject.h"

namespace omnetpp {

/**
 * @brief Provides string representation for enums.
 *
 * The class basically implements efficient integer-to-string and
 * string-to-integer mapping. The primary usage is to support
 * displaying symbolic names for integer enum values.
 *
 * @ingroup Misc
 */
class SIM_API cEnum : public cOwnedObject
{
  private:
     std::map<int,std::string> valueToNameMap;
     std::map<std::string,int> nameToValueMap;
     std::vector<std::string> tmpNames;

  private:
     void copy(const cEnum& other);

  public:
    // internal: helper for the Register_Enum() macro
    cEnum *registerNames(const char *nameList);
    // internal: helper for the Register_Enum() macro
    cEnum *registerValues(int first, ...);
    cEnum *registerValues() {return this;}  // for empty enum

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    explicit cEnum(const char *name=nullptr);

    /**
     * Constructor that allows adding several values to the enum, in a way
     * similar to bulkInsert(). The argument list begins with the object name,
     * which should be followed by an alternating list of names and values,
     * terminated by a nullptr: name1, value1, name2, value2, ..., nullptr.
     *
     * Example:
     * <pre>
     * enum State { IDLE=0, BUSY };
     * cEnum stateEnum("state", "IDLE", IDLE, "BUSY", BUSY, nullptr);
     * </pre>
     */
    cEnum(const char *name, const char *str, ...);

    /**
     * Copy constructor.
     */
    cEnum(const cEnum& cenum);

    /**
     * Destructor.
     */
    virtual ~cEnum() {}

    /**
     * Assignment operator. The name member is not copied;
     * see cOwnedObject's operator=() for more details.
     */
    cEnum& operator=(const cEnum& list);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cEnum *dup() const override  {return new cEnum(*this);}

    /**
     * Returns a textual representation of this enum.
     */
    virtual std::string str() const override;
    //@}

    /** @name Insertion and lookup. */
    //@{
    /**
     * Add an item to the enum. If that numeric code exist, overwrite it.
     */
    void insert(int value, const char *name);

    /**
     * Adds several values to the enum. The argument list should be an
     * alternating list of names and values, terminated by a nullptr:
     * name1, value1, name2, value2, ..., nullptr.
     *
     * Example:
     * <pre>
     * cEnum stateEnum("state");
     * stateEnum.bulkInsert("IDLE", IDLE, "BUSY", BUSY, nullptr);
     * </pre>
     */
    void bulkInsert(const char *name1, ...);

    /**
     * Look up value and return string representation. Return
     * nullptr if not found.
     */
    const char *getStringFor(int value);

    /**
     * Look up string and return numeric code. If not found, return
     * second argument (or -1).
     */
    int lookup(const char *name, int fallback=-1);

    /**
     * Look up string and return numeric code. Throws an error if not found.
     */
    int resolve(const char *name);

    /**
     * Returns a map with the enum members (names as key, and numeric value map value).
     */
    std::map<std::string,int> getNameValueMap() const {return nameToValueMap;}
    //@}

    /** @name cEnum lookup. */
    //@{
    /**
     * Returns the cEnum for the given enum name, or nullptr if not found.
     * The enum must have been registered previously with the Register_Enum()
     * macro.
     */
    static cEnum *find(const char *enumName, const char *contextNamespace=nullptr);

    /**
     * Like find(), but throws an error if the object was not found.
     */
    static cEnum *get(const char *enumName, const char *contextNamespace=nullptr);
    //@}

};

}  // namespace omnetpp


#endif

