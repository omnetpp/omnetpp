//==========================================================================
//  CSTLWATCH.H - part of
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

#ifndef __OMNETPP_CSTLWATCH_H
#define __OMNETPP_CSTLWATCH_H

#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include "cownedobject.h"
#include "cwatch.h"

namespace omnetpp {

//
// Internal class
//
class SIM_API cStdVectorWatcherBase : public cWatchBase
{
  private:
    mutable cClassDescriptor *desc;
  public:
    cStdVectorWatcherBase(const char *name) : cWatchBase(name) {desc = nullptr;}

    virtual std::string str() const override;
    virtual bool supportsAssignment() const override {return false;}

    virtual const char *getElemTypeName() const = 0;
    virtual int size() const = 0;
    virtual std::string at(int i) const = 0;
    virtual cClassDescriptor *getDescriptor() const override;
};


//
// Internal class
//
template<class T>
class cStdVectorWatcher : public cStdVectorWatcherBase
{
  protected:
    std::vector<T>& v;
    std::string classname;
  public:
    cStdVectorWatcher(const char *name, std::vector<T>& var) : cStdVectorWatcherBase(name), v(var) {
        classname = std::string("std::vector<")+opp_typename(typeid(T))+">";
    }
    const char *getClassName() const override {return classname.c_str();}
    virtual const char *getElemTypeName() const override {return opp_typename(typeid(T));}
    virtual int size() const override {return v.size();}
    virtual std::string at(int i) const override {std::stringstream out; out << v[i]; return out.str();}
};

template <class T>
void createStdVectorWatcher(const char *varname, std::vector<T>& v)
{
    new cStdVectorWatcher<T>(varname, v);
}


//
// Internal class
//
template<class T>
class cStdPointerVectorWatcher : public cStdVectorWatcher<T>
{
  public:
    cStdPointerVectorWatcher(const char *name, std::vector<T>& var) : cStdVectorWatcher<T>(name, var) {}
    virtual std::string at(int i) const {std::stringstream out; out << *(this->v[i]); return out.str();}
};

template <class T>
void createStdPointerVectorWatcher(const char *varname, std::vector<T>& v)
{
    new cStdPointerVectorWatcher<T>(varname, v);
}

//
// Internal class
//
template<class T>
class cStdListWatcher : public cStdVectorWatcherBase
{
  protected:
    std::list<T>& v;
    std::string classname;
    mutable typename std::list<T>::iterator it;
    mutable int itPos;
  public:
    cStdListWatcher(const char *name, std::list<T>& var) : cStdVectorWatcherBase(name), v(var) {
        itPos=-1;
        classname = std::string("std::list<")+opp_typename(typeid(T))+">";
    }
    const char *getClassName() const override {return classname.c_str();}
    virtual const char *getElemTypeName() const override {return opp_typename(typeid(T));}
    virtual int size() const override {return v.size();}
    virtual std::string at(int i) const override {
        // std::list doesn't support random access iterator and iteration is slow,
        // so we have to use a trick, knowing that Qtenv will call this function with
        // i=0, i=1, etc...
        if (i==0) {
            it=v.begin(); itPos=0;
        } else if (i==itPos+1 && it!=v.end()) {
            ++it; ++itPos;
        } else {
            it=v.begin();
            for (int k=0; k<i && it!=v.end(); k++) ++it;
            itPos=i;
        }
        if (it==v.end()) {
            return std::string("out of bounds");
        }
        return atIt();
    }
    virtual std::string atIt() const {
        std::stringstream out;
        out << (*it);
        return out.str();
    }
};

template <class T>
void createStdListWatcher(const char *varname, std::list<T>& v)
{
    new cStdListWatcher<T>(varname, v);
}


//
// Internal class
//
template<class T>
class cStdPointerListWatcher : public cStdListWatcher<T>
{
  public:
    cStdPointerListWatcher(const char *name, std::list<T>& var) : cStdListWatcher<T>(name, var) {}
    virtual std::string atIt() const {
        std::stringstream out;
        out << (**this->it);
        return out.str();
    }
};

template <class T>
void createStdPointerListWatcher(const char *varname, std::list<T>& v)
{
    new cStdPointerListWatcher<T>(varname, v);
}


//
// Internal class
//
template<class T>
class cStdSetWatcher : public cStdVectorWatcherBase
{
  protected:
    std::set<T>& v;
    std::string classname;
    mutable typename std::set<T>::iterator it;
    mutable int itPos;
  public:
    cStdSetWatcher(const char *name, std::set<T>& var) : cStdVectorWatcherBase(name), v(var) {
        itPos=-1;
        classname = std::string("std::set<")+opp_typename(typeid(T))+">";
    }
    const char *getClassName() const override {return classname.c_str();}
    virtual const char *getElemTypeName() const override {return opp_typename(typeid(T));}
    virtual int size() const override {return v.size();}
    virtual std::string at(int i) const override {
        // std::set doesn't support random access iterator and iteration is slow,
        // so we have to use a trick, knowing that Qtenv will call this function with
        // i=0, i=1, etc...
        if (i==0) {
            it=v.begin(); itPos=0;
        } else if (i==itPos+1 && it!=v.end()) {
            ++it; ++itPos;
        } else {
            it=v.begin();
            for (int k=0; k<i && it!=v.end(); k++) ++it;
            itPos=i;
        }
        if (it==v.end()) {
            return std::string("out of bounds");
        }
        return atIt();
    }
    virtual std::string atIt() const {
        std::stringstream out;
        out << (*it);
        return out.str();
    }
};

template <class T>
void createStdSetWatcher(const char *varname, std::set<T>& v)
{
    new cStdSetWatcher<T>(varname, v);
}


//
// Internal class
//
template<class T>
class cStdPointerSetWatcher : public cStdSetWatcher<T>
{
  public:
    cStdPointerSetWatcher(const char *name, std::set<T>& var) : cStdSetWatcher<T>(name, var) {}
    virtual std::string atIt() const {
        std::stringstream out;
        out << (**this->it);
        return out.str();
    }
};

template <class T>
void createStdPointerSetWatcher(const char *varname, std::set<T>& v)
{
    new cStdPointerSetWatcher<T>(varname, v);
}


//
// Internal class
//
template<class KeyT, class ValueT, class CmpT>
class cStdMapWatcher : public cStdVectorWatcherBase
{
  protected:
    std::map<KeyT,ValueT,CmpT>& m;
    mutable typename std::map<KeyT,ValueT,CmpT>::iterator it;
    mutable int itPos;
    std::string classname;
  public:
    cStdMapWatcher(const char *name, std::map<KeyT,ValueT,CmpT>& var) : cStdVectorWatcherBase(name), m(var) {
        itPos=-1;
        classname = std::string("std::map<")+opp_typename(typeid(KeyT))+","+opp_typename(typeid(ValueT))+">";
    }
    const char *getClassName() const override {return classname.c_str();}
    virtual const char *getElemTypeName() const override {return "pair<,>";}
    virtual int size() const override {return m.size();}
    virtual std::string at(int i) const override {
        // std::map doesn't support random access iterator and iteration is slow,
        // so we have to use a trick, knowing that Qtenv will call this function with
        // i=0, i=1, etc...
        if (i==0) {
            it=m.begin(); itPos=0;
        } else if (i==itPos+1 && it!=m.end()) {
            ++it; ++itPos;
        } else {
            it=m.begin();
            for (int k=0; k<i && it!=m.end(); k++) ++it;
            itPos=i;
        }
        if (it==m.end()) {
            return std::string("out of bounds");
        }
        return atIt();
    }
    virtual std::string atIt() const {
        std::stringstream out;
        out << it->first << " ==> " << it->second;
        return out.str();
    }
};

template <class KeyT, class ValueT, class CmpT>
void createStdMapWatcher(const char *varname, std::map<KeyT,ValueT,CmpT>& m)
{
    new cStdMapWatcher<KeyT,ValueT,CmpT>(varname, m);
}


//
// Internal class
//
template<class KeyT, class ValueT, class CmpT>
class cStdPointerMapWatcher : public cStdMapWatcher<KeyT,ValueT,CmpT>
{
  public:
    cStdPointerMapWatcher(const char *name, std::map<KeyT,ValueT,CmpT>& var) : cStdMapWatcher<KeyT,ValueT,CmpT>(name, var) {}
    virtual std::string atIt() const {
        std::stringstream out;
        out << this->it->first << "  ==>  " << *(this->it->second);
        return out.str();
    }
};

template<class KeyT, class ValueT, class CmpT>
void createStdPointerMapWatcher(const char *varname, std::map<KeyT,ValueT,CmpT>& m)
{
    new cStdPointerMapWatcher<KeyT,ValueT,CmpT>(varname, m);
}


/**
 * @ingroup WatchMacros
 * @{
 */

/**
 * @brief Makes std::vectors inspectable in Qtenv. See also WATCH_PTRVECTOR().
 *
 * @hideinitializer
 */
#define WATCH_VECTOR(variable)     omnetpp::createStdVectorWatcher(#variable,(variable))

/**
 * @brief Makes std::vectors storing pointers inspectable in Qtenv. See also WATCH_VECTOR().
 *
 * @hideinitializer
 */
#define WATCH_PTRVECTOR(variable)  omnetpp::createStdPointerVectorWatcher(#variable,(variable))

/**
 * @brief Makes std::lists inspectable in Qtenv. See also WATCH_PTRLIST().
 *
 * @hideinitializer
 */
#define WATCH_LIST(variable)       omnetpp::createStdListWatcher(#variable,(variable))

/**
 * @brief Makes std::lists storing pointers inspectable in Qtenv. See also WATCH_LIST().
 *
 * @hideinitializer
 */
#define WATCH_PTRLIST(variable)    omnetpp::createStdPointerListWatcher(#variable,(variable))

/**
 * @brief Makes std::sets inspectable in Qtenv. See also WATCH_PTRSET().
 *
 * @hideinitializer
 */
#define WATCH_SET(variable)        omnetpp::createStdSetWatcher(#variable,(variable))

/**
 * @brief Makes std::sets storing pointers inspectable in Qtenv. See also WATCH_SET().
 *
 * @hideinitializer
 */
#define WATCH_PTRSET(variable)     omnetpp::createStdPointerSetWatcher(#variable,(variable))

/**
 * @brief Makes std::maps inspectable in Qtenv. See also WATCH_PTRMAP().
 *
 * @hideinitializer
 */
#define WATCH_MAP(m)               omnetpp::createStdMapWatcher(#m,(m))

/**
 * @brief Makes std::maps storing pointers inspectable in Qtenv. See also WATCH_MAP().
 *
 * @hideinitializer
 */
#define WATCH_PTRMAP(m)            omnetpp::createStdPointerMapWatcher(#m,(m))

/** @} */

}  // namespace omnetpp


#endif

