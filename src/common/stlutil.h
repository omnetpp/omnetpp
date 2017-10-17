//=========================================================================
//  STLUTIL.H - part of
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

#ifndef __OMNETPP_COMMON_STLUTIL_H
#define __OMNETPP_COMMON_STLUTIL_H

// various utility functions to make STL containers more usable

#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
#include "commonutil.h"  // Assert()

namespace omnetpp {
namespace common {

template<typename T>
typename std::vector<T>& addAll(std::vector<T>& v, const std::vector<T>& w) {
    v.insert(v.end(), w.begin(), w.end());
    return v;
}

template<typename T>
typename std::set<T>& addAll(std::set<T>& s, const std::set<T>& t) {
    s.insert(t.begin(), t.end());
    return s;
}

template<typename K, typename V>
inline std::map<K,V>& addAll(std::map<K,V>& m, const std::map<K,V>& n) {
    m.insert(n.begin(), n.end());
    return m;
}

template<typename T>
typename std::vector<T>::iterator find(std::vector<T>& v, const T& a) {
    return std::find(v.begin(), v.end(), a);
}

template<typename T>
typename std::vector<T>::const_iterator find(const std::vector<T>& v, const T& a) {
    return std::find(v.begin(), v.end(), a);
}

template<typename T>
inline int count(const std::vector<T>& v, const T& a) {
    return std::count(v.begin(), v.end(), a);
}

template<typename T>
int indexOf(const std::vector<T>& v, const T& a) {
    auto it = find(v, a);
    return it == v.end() ? -1 : it - v.begin();
}

template<typename T>
inline bool contains(const std::vector<T>& v, const T& a) {
    return find(v, a) != v.end();
}

template<typename T>
inline bool contains(const std::set<T>& s, const T& a) {
    return s.find(a) != s.end();
}

template<typename K, typename V>
inline bool containsKey(const std::map<K,V>& m, const K& a) {
    return m.find(a) != m.end();
}

template<typename K, typename V>
inline bool containsKey(const std::unordered_map<K,V>& m, const K& a) {
    return m.find(a) != m.end();
}

template<typename T>
void insert(std::vector<T>& v, int pos, const T& a) {
    Assert(pos >= 0 && pos <= (int)v.size());
    v.insert(v.begin() + pos, a);
}

template<typename T>
void erase(std::vector<T>& v, int pos) {
    Assert(pos >= 0 && pos < (int)v.size());
    v.erase(v.begin() + pos);
}

template<typename T, typename A>
inline void remove(std::vector<T>& v, const A& a) {
    v.erase(std::remove(v.begin(), v.end(), a), v.end());
}

template<typename K, typename V>
inline std::vector<K> keys(const std::map<K,V>& m) {
    std::vector<K> result;
    for (auto it = m.begin(); it != m.end(); ++it)
        result.push_back(it->first);
    return result;
}

template<typename K, typename V>
inline std::vector<V> values(const std::map<K,V>& m) {
    std::vector<V> result;
    for (auto it = m.begin(); it != m.end(); ++it)
        result.push_back(it->second);
    return result;
}

template<typename T>
void sort(std::vector<T>& v) {
    std::sort(v.begin(), v.end());
}

template<typename T>
std::vector<T> sorted(const std::vector<T>& v) {
    std::vector<T> result = v;
    std::sort(result.begin(), result.end());
    return result;
}

template <typename T>
std::string to_str(const std::vector<T>& v) {
    std::stringstream out;
    out << '[';
    for (auto it = v.begin(); it != v.end(); ++it) {
        if (it != v.begin())
            out << ", ";
        out << *it;
    }
    out << "]";
    return out.str();
}

template <typename K, typename V>
std::string to_str(const std::map<K,V>& m) {
    std::stringstream out;
    out << '{';
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (it != m.begin())
            out << ", ";
        out << it->first << " -> " << it->second;
    }
    out << "}";
    return out.str();
}

} // namespace common
} // namespace omnetpp

#endif

