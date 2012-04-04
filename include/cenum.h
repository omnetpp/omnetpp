//==========================================================================
//  CENUM.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cEnum : effective integer-to-string mapping
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CENUM_H
#define __CENUM_H

#include "cownedobject.h"

NAMESPACE_BEGIN

#define Register_Enum(NAME, VALUES)  \
    EXECUTE_ON_STARTUP(enums.getInstance()->add((new cEnum(#NAME))->registerNames(#VALUES)->registerValues VALUES))

/**
 * Provides string representation for enums. The class basically implements
 * effective integer-to-string and string-to-integer mapping. Primary usage
 * is to support displaying symbolic names for integer values that represent
 * some code (such as an enum or \#define).
 *
 * @ingroup Internals
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

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    explicit cEnum(const char *name=NULL);

    /**
     * FIXME document properly:
     * arglist: (enumname, string1, value1, string2, value2, ..., NULL)
     *
     * Example:
     * <pre>
     * enum State { IDLE=0, BUSY };
     * cEnum stateEnum("state", "IDLE", IDLE, "BUSY", BUSY, NULL);
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
    virtual ~cEnum();

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
    virtual cEnum *dup() const  {return new cEnum(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string info() const;
    //@}

    /** @name Insertion and lookup. */
    //@{
    /**
     * Add an item to the enum. If that numeric code exist, overwrite it.
     */
    void insert(int value, const char *name);

    /**
     * Look up value and return string representation. Return
     * NULL if not found.
     */
    const char *getStringFor(int value);

    /**
     * Look up string and return numeric code. If not found, return
     * second argument (or -1).
     */
    int lookup(const char *name, int fallback=-1);
    //@}

    /**
     * Returns a textual representation of this enum.
     */
    std::string str() const;

    /**
     * Finds a registered enum by name. Returns NULL if not found.
     */
    static cEnum *find(const char *name);

    /**
     * Returns the enum with the given name. Throws an error if not found.
     */
    static cEnum *get(const char *name);
};

NAMESPACE_END


#endif

