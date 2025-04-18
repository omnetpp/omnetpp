//==========================================================================
//  CWATCH.H - part of
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

#ifndef __OMNETPP_CWATCH_H
#define __OMNETPP_CWATCH_H

#include <iostream>
#include <sstream>
#include <functional>
#include "cownedobject.h"
#include "cclassdescriptor.h"

namespace omnetpp {


/**
 * @brief Helper class to make primitive types and non-cOwnedObject objects
 * inspectable in Qtenv. To be used only via the WATCH, WATCH_PTR,
 * WATCH_OBJ, WATCH_VECTOR etc macros.
 *
 * @ingroup Internals
 */
class SIM_API cWatchBase : public cNoncopyableOwnedObject
{
  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Initialize the shell to hold the given variable.
     */
    cWatchBase(const char *name)  : cNoncopyableOwnedObject(name) {}
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
 * @brief Template Watch class, for any type that supports operator<<.
 * @ingroup Internals
 */
template<typename T>
class cGenericReadonlyWatch : public cWatchBase
{
  private:
    const T& r;
  public:
    cGenericReadonlyWatch(const char *name, const T& x) : cWatchBase(name), r(x) {}
    virtual const char *getClassName() const override {return omnetpp::opp_typename(typeid(T));}
    virtual bool supportsAssignment() const override {return false;}
    virtual std::string str() const override
    {
        std::stringstream out;
        out << r;
        return out.str();
    }
};


/**
 * @brief Template Watch class, for any type that supports operator<<,
 * and operator>> for assignment.
 * @ingroup Internals
 */
template<typename T>
class cGenericAssignableWatch : public cWatchBase
{
  private:
    T& r;
  public:
    cGenericAssignableWatch(const char *name, T& x) : cWatchBase(name), r(x) {}
    virtual const char *getClassName() const override {return omnetpp::opp_typename(typeid(T));}
    virtual bool supportsAssignment() const override {return true;}
    virtual std::string str() const override
    {
        std::stringstream out;
        out << r;
        return out.str();
    }
    virtual void assign(const char *s) override
    {
        std::stringstream in(s);
        in >> r;
    }
};

/**
 * @brief Watch class, specifically for bool.
 * @ingroup Internals
 */
class SIM_API cWatch_bool : public cWatchBase
{
  private:
    bool& r;
  public:
    cWatch_bool(const char *name, bool& x) : cWatchBase(name), r(x) {}
    virtual const char *getClassName() const override {return "bool";}
    virtual bool supportsAssignment() const override {return true;}
    virtual std::string str() const override
    {
        return r ? "true" : "false";
    }
    virtual void assign(const char *s) override
    {
        r = *s!='0' && *s!='n' && *s!='N' && *s!='f' && *s!='F';
    }
};

/**
 * @brief Watch class, specifically for char.
 * @ingroup Internals
 */
class SIM_API cWatch_char : public cWatchBase
{
  private:
    char& r;
  public:
    cWatch_char(const char *name, char& x) : cWatchBase(name), r(x) {}
    virtual const char *getClassName() const override {return "char";}
    virtual bool supportsAssignment() const override {return true;}
    virtual std::string str() const override
    {
        std::stringstream out;
        out << "'" << ((unsigned char)r<32 ? ' ' : r) << "' (" << int(r) << ")";
        return out.str();
    }
    virtual void assign(const char *s) override
    {
        if (s[0]=='\'')
            r = s[1];
        else
            r = atoi(s);
    }
};

/**
 * @brief Watch class, specifically for unsigned char.
 * @ingroup Internals
 */
class SIM_API cWatch_uchar : public cWatchBase
{
  private:
    unsigned char& r;
  public:
    cWatch_uchar(const char *name, unsigned char& x) : cWatchBase(name), r(x) {}
    virtual const char *getClassName() const override {return "unsigned char";}
    virtual bool supportsAssignment() const override {return true;}
    virtual std::string str() const override
    {
        std::stringstream out;
        out << "'" << (char)(r<' ' ? ' ' : r) << "' (" << int(r) << ")";
        return out.str();
    }
    virtual void assign(const char *s) override
    {
        if (s[0]=='\'')
            r = s[1];
        else
            r = atoi(s);
    }
};

/**
 * @brief Watch class, specifically for std::string.
 * @ingroup Internals
 */
class SIM_API cWatch_stdstring : public cWatchBase
{
  private:
    std::string& r;
  public:
    cWatch_stdstring(const char *name, std::string& x) : cWatchBase(name), r(x) {}
    virtual const char *getClassName() const override {return "std::string";}
    virtual bool supportsAssignment() const override {return true;}
    virtual std::string str() const override;
    virtual void assign(const char *s) override;
};

/**
 * @brief Watch class, specifically for objects subclassed from cObject.
 * @ingroup Internals
 */
class SIM_API cWatch_cObject : public cWatchBase
{
    friend class cWatchProxyDescriptor;
  private:
    cObject& r;
    cClassDescriptor *desc; // for this watch object, not the one being watched
    std::string typeName;
  public:
    cWatch_cObject(const char *name, const char *typeName, cObject& ref);
    ~cWatch_cObject() { dropAndDelete(desc); }
    virtual const char *getClassName() const override {return typeName.c_str();}
    virtual std::string str() const override {return std::string("-> ") + r.getClassName() + ")" + r.getFullName() + " " + r.str();}
    virtual bool supportsAssignment() const override {return false;}
    virtual cClassDescriptor *getDescriptor() const override {return desc;}
    virtual void forEachChild(cVisitor *visitor) override;
    cObject *getObjectPtr() const {return &r;}
};

/**
 * @brief Watch class, specifically for pointers to objects subclassed from cObject.
 * @ingroup Internals
 */
class SIM_API cWatch_cObjectPtr : public cWatchBase
{
    friend class cWatchProxyDescriptor;
  private:
    cObject *&rp;
    cClassDescriptor *desc; // for this watch object, not the one being watched
    std::string typeName;
  public:
    cWatch_cObjectPtr(const char *name, const char* typeName, cObject *&ptr);
    ~cWatch_cObjectPtr() { dropAndDelete(desc); }
    virtual const char *getClassName() const override {return typeName.c_str();}
    virtual std::string str() const override {return rp ? ( std::string("-> (") + rp->getClassName() + ")" + rp->getFullName() + " " + rp->str()) : "<null>";}
    virtual bool supportsAssignment() const override {return false;}
    virtual cClassDescriptor *getDescriptor() const override {return desc;}
    virtual void forEachChild(cVisitor *visitor) override;
    cObject *getObjectPtr() const {return rp;}
};

/**
 * @brief Watch class for computed expressions. The watch will display the result of
 * calling an encapsulated function.
 * @ingroup Internals
 */
template<typename T>
class cComputedExpressionWatch : public cWatchBase
{
  private:
    std::function<T()> f;
  public:
    cComputedExpressionWatch(const char *name, std::function<T()> f) : cWatchBase(name), f(f) {}
    virtual const char *getClassName() const override {return omnetpp::opp_typename(typeid(T));}
    virtual bool supportsAssignment() const override {return false;}
    virtual std::string str() const override { std::stringstream out; out << f(); return out.str(); }
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
    return new cWatch_uchar(varname, d);
}

inline cWatchBase *createWatch(const char *varname, signed char& d) {
    return new cWatch_char(varname, *(char *)&d);
}

inline cWatchBase *createWatch(const char *varname, std::string& v) {
    return new cWatch_stdstring(varname, v);
}

// this is the fallback, if none of the above match
template<typename T>
inline cWatchBase *createWatch(const char *varname, T& d) {
    return new cGenericReadonlyWatch<T>(varname, d);
}

// to be used if T also has op>> defined
template<typename T>
inline cWatchBase *createWatch_genericAssignable(const char *varname, T& d) {
    return new cGenericAssignableWatch<T>(varname, d);
}

// for objects
inline cWatchBase *createWatch_cObject(const char *varname, const char *typeName, cObject& obj) {
    return new cWatch_cObject(varname, typeName, obj);
}

template<typename T>
inline cWatchBase *createComputedExpressionWatch(const char *name, std::function<T()> f) {
    return new cComputedExpressionWatch<T>(name, f);
}

// for pointers to objects.
// NOTE: this is a bit tricky. C++ thinks that (cObject*&) and
// (SomeDerivedType*&) are unrelated, so we have to force the cast
// in the WATCH_PTR() macro. But to stay type-safe, we include a 3rd arg
// of type cObject*: the compiler has to be able to cast that
// implicitly from SomeDerivedType* -- this way we do not accept pointers
// that are REALLY unrelated.
inline cWatchBase *createWatch_cObjectPtr(const char *varname, const char *typeName, cObject *&refp, cObject *p) {
    ASSERT(refp==p);
    return new cWatch_cObjectPtr(varname, typeName, refp);
}


/**
 * @ingroup WatchMacros
 * @{
 */

/**
 * @brief Makes primitive types and types with operator<< inspectable in Qtenv.
 * See also WATCH_RW(), WATCH_PTR(), WATCH_OBJ(), WATCH_VECTOR(),
 * WATCH_PTRVECTOR(), etc. macros.
 *
 * @hideinitializer
 */
#define WATCH(variable)  omnetpp::createWatch(#variable,(variable))

/**
 * @brief Makes types with operator\<\< and operator\>\> inspectable in Qtenv.
 * operator\>\> is used to enable changing the variable's value interactively.
 *
 * @hideinitializer
 */
#define WATCH_RW(variable)  omnetpp::createWatch_genericAssignable(#variable,(variable))

/**
 * @brief Makes classes derived from cObject inspectable in Qtenv.
 * See also WATCH_PTR().
 *
 * @hideinitializer
 */
#define WATCH_OBJ(variable)  omnetpp::createWatch_cObject(#variable, omnetpp::opp_typename(typeid(variable)), (variable))

/**
 * @brief Makes pointers to objects derived from cObject inspectable in Qtenv.
 * See also WATCH_OBJ().
 *
 * @hideinitializer
 */
#define WATCH_PTR(variable)  omnetpp::createWatch_cObjectPtr(#variable, omnetpp::opp_typename(typeid(variable)), (cObject*&)(variable),(variable))

/**
 * @brief Makes the result of a formula or calculation inspectable in Qtenv
 * without requiring a separate variable. The expression is evaluated as often
 * as needed, providing real-time monitoring of derived metrics. Unlike WATCH
 * which monitors single variables, WATCH_EXPR can display the result of
 * operations combining multiple variables, function calls, or any valid
 * expression.
 *
 * The macro works by creating a lambda function. Note that local variables will
 * be captured by value (i.e. their current values will be used.) See also
 * WATCH_LAMBDA() which gives you more flexibility.
 *
 * Example: WATCH_EXPR("totalPks", numTransmitted + queue.length() + numDropped)
 *
 * @hideinitializer
 */
#define WATCH_EXPR(name, expression)  omnetpp::createComputedExpressionWatch(name, std::function([=]() {return (expression);}))

/**
 * @brief Makes the result of a lambda function inspectable in Qtenv. This is a
 * more flexible (but also more verbose) version of WATCH_EXPR().
 *
 * Example: WATCH_LAMBDA("totalPks", [this]() { return numTransmitted + queue.length() + numDropped; })
 *
 * @hideinitializer
 */
#define WATCH_LAMBDA(name, lambdaFunction)  omnetpp::createComputedExpressionWatch(name, std::function(lambdaFunction))

/** @} */

}  // namespace omnetpp


#endif


