//=========================================================================
//  STRINGPOOL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_STRINGPOOL_H
#define __OMNETPP_COMMON_STRINGPOOL_H

#include <cstring>
#include <unordered_set>
#include <unordered_map>
#include "commondefs.h"

namespace omnetpp {
namespace common {

/**
 * Hash function for strings. This one is djb2 by Dan Bernstein,
 * from http://www.cse.yorku.ca/~oz/hash.html
 */
inline std::size_t opp_hash(unsigned char *str)
{
    std::size_t hash = 5381;
    while (int c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

/**
 * For saving memory on the storage of (largely) constant strings that occur in
 * many instances. (See Flyweight GoF pattern.)
 *
 * Note: this variant does not do reference counting, so strings do not need
 * to be released. The downside is that they will only be deallocated in the
 * stringpool object's destructor. Nullptr is also accepted.
 */
class COMMON_API StaticStringPool
{
  protected:
    struct str_hash {
        std::size_t operator()(const char *s) const { return opp_hash((unsigned char *)s); }
    };
    struct str_eq {
        bool operator()(const char *lhs, const char *rhs) const { return strcmp(lhs, rhs) == 0; }
    };
    std::unordered_set<const char *,str_hash,str_eq> pool;

  public:
    static const char * const EMPTY_STRING;

  public:
    StaticStringPool() {}
    ~StaticStringPool();
    const char *get(const char *s);
    bool contains(const char *s) const;
    void clear();
};

/**
 * Reference-counted pooled storage for strings.
 *
 * obtain() and release() calls must be balanced. Nullptr and empty string
 * can also be added (and they bypass reference counting).
 */
class COMMON_API StringPool
{
  protected:
    struct str_hash {
        std::size_t operator()(const char *s) const { return opp_hash((unsigned char *)s); }
    };
    struct str_eq {
        bool operator()(const char *lhs, const char *rhs) const { return strcmp(lhs, rhs) == 0; }
    };
    std::unordered_map<const char *,int,str_hash, str_eq> pool; // map<string,refcount>
    bool alive = true; // useful when stringpool is a global variable

  public:
    static const char * const EMPTY_STRING;

  public:
    StringPool() {}
    ~StringPool();
    const char *obtain(const char *s);
    const char *peek(const char *s) const;
    void release(const char *s);
    void dump() const;
};

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
    opp_pooledstring& operator=(opp_pooledstring&& ps) {pool.release(p); p = ps.p; ps.p = nullptr; return *this;}
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


