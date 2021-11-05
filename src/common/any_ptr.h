//==========================================================================
//   ANY_PTR.H  - part of
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

#ifndef __OMNETPP_COMMON_ANY_PTR_H
#define __OMNETPP_COMMON_ANY_PTR_H

#include <typeinfo>
#include <string>
#include <cassert>
#include "commondefs.h"
#include "commonutil.h"

namespace omnetpp {
namespace common {

/**
 * A type-safe equivalent of the void* pointer.
 *
 * any_ptr accepts a pointer of any type, and keeps the memory address
 * along with the pointer's type. The pointer can be extracted from
 * any_ptr with the exact same type it was used to store it. This was
 * achieved with the use of template methods, typeid() and std::type_info.
 */
class COMMON_API any_ptr
{
  private:
    void *ptr;
    const std::type_info *type;
  private:
    void checkType(const std::type_info& asType) const;
  public:
    // internal
    static std::string (*printerFunction)(any_ptr ptr);
  public:
    any_ptr() : any_ptr(nullptr) {}

    explicit any_ptr(std::nullptr_t) : ptr(nullptr), type(&typeid(std::nullptr_t)) {}

    template<typename T>
    explicit any_ptr(T *ptr) : ptr(ptr), type(&typeid(T*)) {}

    template<typename T>
    explicit any_ptr(const T *ptr) : any_ptr(const_cast<T*>(ptr)) {}

    any_ptr(void *ptr, const std::type_info& type) : ptr(ptr), type(&type) {}

    any_ptr(const any_ptr &other) : ptr(other.ptr), type(other.type) {}

    const any_ptr& operator=(const any_ptr& other) {ptr=other.ptr; type=other.type; return *this;}

    bool operator==(const any_ptr& other) const {return ptr==other.ptr && (ptr==nullptr || type==other.type);}
    bool operator!=(const any_ptr& other) const {return !operator==(other);}

    bool operator==(std::nullptr_t) const {return ptr==nullptr;}
    bool operator!=(std::nullptr_t) const {return ptr!=nullptr;}

    template<typename T>
    bool contains() const {return typeid(T*) == *type;}

    void *raw() const {return ptr;}
    const std::type_info& pointerType() const {return *type;}
    const char *pointerTypeName() const {return opp_typename(*type);}

    template<typename T>
    T *get() { checkType(typeid(T*)); return reinterpret_cast<T*>(ptr); }

    template<typename T>
    const T *get() const { checkType(typeid(T*)); return reinterpret_cast<const T*>(ptr);}

    std::string str() const;
};

}  // namespace common
}  // namespace omnetpp


#endif

