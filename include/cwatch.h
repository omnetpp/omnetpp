//==========================================================================
//  CWATCH.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cWatchBase etc: make primitive types, structs etc inspectable
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CWATCH_H
#define __CWATCH_H

#include <iostream>
#include <sstream>
#include "cobject.h"


/**
 * Utility class to make primitive types and non-cObject objects
 * inspectable in Tkenv.
 *
 * Users rarely need to create cWatch objects directly, they rather use
 * the WATCH() and LWATCH() macros.
 *
 * @ingroup SimSupport
 */
class SIM_API cWatchBase : public cObject
{
  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Initialize the shell to hold the given variable.
     */
    cWatchBase(const char *name)  : cObject(name) {}

    /**
     * Copy constructor not supported: it will raise an error via dup().
     */
    cWatchBase(const cWatchBase& v) : cObject(v.name()) {operator=(v);}

    /**
     * Assignment not supported: it will raise an error.
     */
    cWatchBase& operator=(const cWatchBase&) {copyNotSupported();return *this;}

    /**
     * dup() not supported: it will raise an error.
     */
    virtual cObject *dup() const {copyNotSupported(); return NULL;}
    //@}

    /** @name New methods */
    //@{
    /**
     * Tells if changing the variable's value via assign() is supported.
     */
    virtual bool supportsAssignment() const = 0;

    /**
     * Changes the watched variable's value. May only be called if
     * supportsAssignment() returns true.
     */
    virtual void assign(const char *s) {}
    //@}
};


/**
 * Template Watch class, for any type that supports operator<<.
 */
template<typename T>
class SIM_API cGenericReadonlyWatch : public cWatchBase
{
  private:
    const T& r;
  public:
    cGenericReadonlyWatch(const char *name, const T& x) : cWatchBase(name), r(x) {}
    virtual const char *className() const {return opp_typename(typeid(T));}
    virtual bool supportsAssignment() const {return false;}
    virtual std::string info() const
    {
        std::stringstream out;
        out << r;
        return out.str();
    }
};


/**
 * Template Watch class, for any type that supports operator<<,
 * and operator>> for assignment.
 */
template<typename T>
class SIM_API cGenericAssignableWatch : public cWatchBase
{
  private:
    T& r;
  public:
    cGenericAssignableWatch(const char *name, T& x) : cWatchBase(name), r(x) {}
    virtual const char *className() const {return opp_typename(typeid(T));}
    virtual bool supportsAssignment() const {return true;}
    virtual std::string info() const
    {
        std::stringstream out;
        out << r;
        return out.str();
    }
    virtual void assign(const char *s)
    {
        std::stringstream in(s);
        in >> r;
    }
};

/**
 * Watch class, specifically for bool.
 */
class SIM_API cWatch_bool : public cWatchBase
{
  private:
    bool& r;
  public:
    cWatch_bool(const char *name, bool& x) : cWatchBase(name), r(x) {}
    virtual const char *className() const {return "bool";}
    virtual bool supportsAssignment() const {return true;}
    virtual std::string info() const
    {
        return r ? "true" : "false";
    }
    virtual void assign(const char *s)
    {
        r = *s!='0' && *s!='n' && *s!='N' && *s!='f' && *s!='F';
    }
};

/**
 * Watch class, specifically for char.
 */
class SIM_API cWatch_char : public cWatchBase
{
  private:
    char& r;
  public:
    cWatch_char(const char *name, char& x) : cWatchBase(name), r(x) {}
    virtual const char *className() const {return "char";}
    virtual bool supportsAssignment() const {return true;}
    virtual std::string info() const
    {
        std::stringstream out;
        out << "'" << r << "' (" << int(r) << ")";
        return out.str();
    }
    virtual void assign(const char *s)
    {
        if (s[0]=='\'')
            r = s[1];
        else
            r = atoi(s);
    }
};

/**
 * Watch class, specifically for std::string.
 */
class SIM_API cWatch_stdstring : public cWatchBase
{
  private:
    std::string& r;
  public:
    cWatch_stdstring(const char *name, std::string& x) : cWatchBase(name), r(x) {}
    virtual const char *className() const {return "std::string";}
    virtual bool supportsAssignment() const {return true;}
    virtual std::string info() const
    {
        std::stringstream out;
        out << "\"" << r << "\"";
        return out.str();
    }
    virtual void assign(const char *s)
    {
        r = s;
    }
};

/**
 * Watch class, specifically for objects subclassed from cPolymorphic.
 */
class SIM_API cWatch_cPolymorphic : public cWatchBase
{
  private:
    cPolymorphic *p;
  public:
    cWatch_cPolymorphic(const char *name, cPolymorphic *ptr) : cWatchBase(name), p(ptr) {}
    cWatch_cPolymorphic(const char *name, cPolymorphic& ref) : cWatchBase(name), p(&ref) {}
    virtual const char *className() const {return p ? p->className() : "n/a";}
    virtual std::string info() const {return p ? p->info() : "<null>";}
    virtual bool supportsAssignment() const {return false;}
    virtual cStructDescriptor *createDescriptor() {return p ? p->createDescriptor() : NULL;}
};


inline cWatchBase *createWatch(const char *varname, short& d) {
    return new cGenericAssignableWatch<short>(varname, d);
}

inline cWatchBase *createWatch(const char *varname, unsigned short& d) {
    return new cGenericAssignableWatch<unsigned short>(varname, d);
}

inline cWatchBase *createWatch(const char *varname, int& d) {
    return new cGenericAssignableWatch<int>(varname, d);
}

inline cWatchBase *createWatch(const char *varname, unsigned int& d) {
    return new cGenericAssignableWatch<unsigned int>(varname, d);
}

inline cWatchBase *createWatch(const char *varname, long& d) {
    return new cGenericAssignableWatch<long>(varname, d);
}

inline cWatchBase *createWatch(const char *varname, unsigned long& d) {
    return new cGenericAssignableWatch<unsigned long>(varname, d);
}

inline cWatchBase *createWatch(const char *varname, float& d) {
    return new cGenericAssignableWatch<float>(varname, d);
}

inline cWatchBase *createWatch(const char *varname, double& d) {
    return new cGenericAssignableWatch<double>(varname, d);
}

inline cWatchBase *createWatch(const char *varname, bool& d) {
    return new cWatch_bool(varname, d);
}

inline cWatchBase *createWatch(const char *varname, char& d) {
    return new cWatch_char(varname, d);
}

inline cWatchBase *createWatch(const char *varname, unsigned char& d) {
    return new cWatch_char(varname, *(char *)&d);
}

inline cWatchBase *createWatch(const char *varname, signed char& d) {
    return new cWatch_char(varname, *(char *)&d);
}

inline cWatchBase *createWatch(const char *varname, std::string& v) {
    return new cWatch_stdstring(varname, v);
}

inline cWatchBase *createWatch(const char *varname, cPolymorphic *ptr) {
    return new cWatch_cPolymorphic(varname, ptr);
}

inline cWatchBase *createWatch(const char *varname, cPolymorphic& ref) {
    return new cWatch_cPolymorphic(varname, ref);
}

template<typename T>
inline cWatchBase *createWatch_genericAssignable(const char *varname, T& d) {
    return new cGenericAssignableWatch<T>(varname, d);
}

template<typename T>
inline cWatchBase *createWatch_genericReadonly(const char *varname, T& d) {
    return new cGenericReadonlyWatch<T>(varname, d);
}


#define WATCH(v)     createWatch(#v,(v))
#define WATCH2(v)    createWatch_genericReadonly(#v,(v))
#define WATCH2_RW(v) createWatch_genericAssignable(#v,(v))


#endif


