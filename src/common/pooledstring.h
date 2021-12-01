//=========================================================================
//  POOLEDSTRING.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_POOLEDSTRING_H
#define __OMNETPP_COMMON_POOLEDSTRING_H

#include "commondefs.h"
#include "stringpool.h"

namespace omnetpp {
namespace common {

/**
 * A basic string class backed by a static string pool.
 * Storing nullptr is OK, but avoid op== and str() then.
 */
class COMMON_API opp_staticpooledstring
{
  public:
    static StaticStringPool pool;
  private:
    const char *p;
  public:
    opp_staticpooledstring() {p = pool.EMPTY_STRING;}
    opp_staticpooledstring(const char *s) {p = pool.get(s);}
    opp_staticpooledstring(const std::string& s) {p = pool.get(s.c_str());}
    opp_staticpooledstring(const opp_staticpooledstring&) = default;
    opp_staticpooledstring(opp_staticpooledstring&&) = default;
    opp_staticpooledstring& operator=(const opp_staticpooledstring&) = default;
    opp_staticpooledstring& operator=(opp_staticpooledstring&&) = default;
    bool empty() const {return !p || !*p;}
    std::string str() const {return p;}
    const char *c_str() const {return p;}
    static const char *get(const char *s) {return pool.get(s);}
    bool operator==(const opp_staticpooledstring& ps) const {return p == ps.p;}
    bool operator!=(const opp_staticpooledstring& ps) const {return p != ps.p;}
    bool operator==(const char *s) const {return strcmp(p,s) == 0;}
    bool operator!=(const char *s) const {return strcmp(p,s) != 0;}
    bool operator==(const std::string& s) const {return strcmp(p,s.c_str()) == 0;}
    bool operator!=(const std::string& s) const {return strcmp(p,s.c_str()) != 0;}
};

inline bool operator==(const char *s, const opp_staticpooledstring& ps) {return ps == s;}
inline bool operator!=(const char *s, const opp_staticpooledstring& ps) {return ps != s;}
inline bool operator==(const std::string& s, const opp_staticpooledstring& ps) {return ps == s;}
inline bool operator!=(const std::string& s, const opp_staticpooledstring& ps) {return ps != s;}


/**
 * A basic string class backed by a reference counting string pool.
 * Storing nullptr is OK, but avoid op== and str() then.
 */
class COMMON_API opp_pooledstring
{
  public:
    static StringPool pool;
   private:
    const char *p;
  public:
    opp_pooledstring() {p = pool.EMPTY_STRING;}
    opp_pooledstring(const char *s) {p = pool.obtain(s);}
    opp_pooledstring(const std::string& s) {p = pool.obtain(s.c_str());}
    opp_pooledstring(const opp_pooledstring& ps) {p = pool.obtain(ps.p);}
    opp_pooledstring(opp_pooledstring&& ps) {p = ps.p; ps.p = nullptr;}
    ~opp_pooledstring() {if (p) pool.release(p);}
    opp_pooledstring& operator=(const opp_pooledstring& ps) {if (p != ps.p) {pool.release(p); p = pool.obtain(ps.p);} return *this;}
    opp_pooledstring& operator=(opp_pooledstring&& ps) {if (p != ps.p) {pool.release(p); p = ps.p; ps.p = nullptr;} return *this;}
    bool empty() const {return !p || !*p;}
    std::string str() const {return p;}
    const char *c_str() const {return p;}
    bool operator==(const opp_pooledstring& ps) const {return p == ps.p;}
    bool operator!=(const opp_pooledstring& ps) const {return p != ps.p;}
    bool operator==(const char *s) const {return strcmp(p,s) == 0;}
    bool operator!=(const char *s) const {return strcmp(p,s) != 0;}
    bool operator==(const std::string& s) const {return strcmp(p,s.c_str()) == 0;}
    bool operator!=(const std::string& s) const {return strcmp(p,s.c_str()) != 0;}
};

inline bool operator==(const char *s, const opp_pooledstring& ps) {return ps == s;}
inline bool operator!=(const char *s, const opp_pooledstring& ps) {return ps != s;}
inline bool operator==(const std::string& s, const opp_pooledstring& ps) {return ps == s;}
inline bool operator!=(const std::string& s, const opp_pooledstring& ps) {return ps != s;}

}  // namespace common
}  // namespace omnetpp


#endif


