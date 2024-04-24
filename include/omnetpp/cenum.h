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

#include <initializer_list>
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
     std::map<intval_t,std::string> valueToNameMap;
     std::map<std::string,intval_t> nameToValueMap;
     std::vector<std::string> tmpNames;

  private:
     void copy(const cEnum& other);

  public:
    // internal: helper for the Register_Enum() macro
    cEnum *registerNames(const char *nameList);

    template<typename T>
    cEnum *registerValues(const std::initializer_list<T>& values) {
        int i = 0;
        for (T value : values)
            insert((intval_t)value, tmpNames[i++].c_str());
        tmpNames.clear();
        return this;
    }

    template<typename T>
    void addPairs(const std::initializer_list<std::pair<const char*,T>> pairList) {
        for (const auto& pair : pairList)
            insert((intval_t)pair.second, pair.first);
    }

    // internal: helper for obsolete macro Register_Enum2()
    // usage example: e.bulkInsert("IDLE", IDLE, "BUSY", BUSY, nullptr);
    [[deprecated("Use Register_Enum_Custom() macro instead of Register_Enum2()")]]
    void bulkInsert(const char *name1, ...);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    explicit cEnum(const char *name=nullptr);

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
    void insert(intval_t value, const char *name);

    /**
     * Look up value and return string representation. Return
     * nullptr if not found.
     */
    const char *getStringFor(intval_t value);

    /**
     * Look up string and return numeric code. If not found, return
     * second argument (or -1).
     */
    intval_t lookup(const char *name, intval_t fallback=-1);

    /**
     * Look up string and return numeric code. Throws an error if not found.
     */
    intval_t resolve(const char *name);

    /**
     * Resolve a string and return the corresponding value in the enum type
     * given as template argument. The enum must have been registered previously
     * with one of the Register_Enum() macros.
     *
     * Example:
     *
     * ```
     * State state = cEnum::resolveName<State>("IDLE");
     * ```
     */
    template<typename E>
    static E resolveName(const char *name) {
        cEnum *e = get(opp_typename(typeid(E)));
        return static_cast<E>(e->resolve(name));
    }

    /**
     * Resolve an enum value of type E and return the corresponding string
     * representation. The enum must have been registered previously with one of
     * the Register_Enum() macros.
     *
     * Example:
     *
     * ```
     * State state = State::IDLE;
     * const char *stateName = cEnum::getNameForValue(state);
     * ```
     */
    template<typename E>
    static const char *getNameForValue(E value) {
        cEnum *e = get(opp_typename(typeid(E)));
        return e->getStringFor(static_cast<int>(value));
    }

    /**
     * Returns a map with the enum members (names as key, and numeric value map value).
     */
    const std::map<std::string,intval_t>& getNameValueMap() const {return nameToValueMap;}
    //@}

    /** @name cEnum lookup. */
    //@{
    /**
     * Returns the cEnum for the given enum name, or nullptr if not found.
     * The enum must have been registered previously with one of the
     * Register_Enum() macros.
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

