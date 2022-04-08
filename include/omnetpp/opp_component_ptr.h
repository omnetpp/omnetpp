//==========================================================================
//   OPP_COMPONENT_PTR.H  -  header for
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

#ifndef __OMNETPP_OPP_COMPONENT_PTR_H
#define __OMNETPP_OPP_COMPONENT_PTR_H

#include "ccomponent.h"
#include "cexception.h"

namespace omnetpp {

/**
 * A smart pointer that points to a component (module or channel) object, and
 * automatically becomes nullptr when the referenced object is deleted.
 *
 * This is a non-owning ("weak") pointer, i.e. the pointer going out of scope
 * has no effect on the referenced object.
 *
 * It is implemented as a template class to provide type safety to users
 * by letting them store typed pointers in it instead of "raw" cComponent
 * pointers.
 *
 * The class also contains protection against accidental dereference of
 * null pointers. Only the pointer cast operator (operator T *) is willing
 * to return nullptr, other members refuse to do so and throw an exception
 * instead.
 *
 * Note 1: T is often a subclass of cComponent, but that is not necessary.
 * In particular, T may be an unrelated (but polymorphic) type, as long as
 * actual objects passed into the smart pointer can be dynamically cast
 * to cComponent. For example, given an unrelated abstract class IQueue,
 * a opp_component_ptr<IQueue> smart pointer may be used with objects
 * that inherit from both IQueue and cComponent.
 *
 * Note 2: This class always stores pointers to mutable (non-const) objects.
 * Making a const opp_component_ptr only makes it impossible to re-set the
 * pointer to refer to another object, but it is still possible to extract
 * mutable pointers from it. This interface is consistent with C++ smart
 * pointers like std::unique_ptr and std::shared_ptr.
 *
 * @ingroup Misc
 */
template <typename T>
class SIM_API opp_component_ptr
{
  private:
    T *ptr = nullptr;

    void registerPtr() {
        if (ptr) {
            if (cComponent *component = dynamic_cast<cComponent*>(ptr))
                component->registerSelfPointer(ptr);
            else
                throw cRuntimeError("opp_component_ptr<%s>: Pointer cannot be dynamic_cast to cComponent", opp_typename(typeid(T)));
        }
    }

    void deregisterPtr() {
        if (ptr)
            dynamic_cast<cComponent*>(ptr)->deregisterSelfPointer(ptr);
    }

    void checkNull() const {
        if (ptr == nullptr)
            throw cRuntimeError("opp_component_ptr<%s>: Cannot dereference nullptr", opp_typename(typeid(T)));
    }

  public:
    /**
     * Initializes the pointer to nullptr.
     */
    opp_component_ptr() {}

    /**
     * Initializes the pointer to the given value. The pointer must be able
     * to be dynamic_cast to a cComponent pointer.
     */
    opp_component_ptr(T *ptr) : ptr(ptr) {registerPtr();}

    /**
     * Initializes a pointer to reference the same object (or nullptr) as
     * the argument.
     */
    opp_component_ptr(const opp_component_ptr<T>& other) : ptr(other.ptr) {registerPtr();}

    /**
     * Destructor. It has no effect on the referenced object.
     */
    ~opp_component_ptr() {deregisterPtr();}

    /**
     * Returns a reference to the referenced object. Throws an error if the
     * stored pointer is nullptr.
     */
    T& operator*() const {checkNull(); return *ptr;}

    /**
     * Allows member access on the referenced object. Throws an error if the
     * stored pointer is nullptr.
     */
    T *operator->() const {checkNull(); return ptr;}

    /**
     * Returns a pointer to the referenced object, or nullptr.
     */
    operator T *() const {return ptr;}

    /**
     * Returns true if the stored pointer is not a nullptr, and false if it is.
     */
    explicit operator bool() const {return ptr != nullptr;}

    /**
     * Returns true if this object references the same object (or nullptr)
     * as the argument.
     */
    bool operator==(const T *ptr) {return ptr == this->ptr;}

    /**
     * Returns true if this object references the same object (or nullptr)
     * as the argument.
     */
    bool operator==(const opp_component_ptr<T>& other) {return ptr == other.ptr;}

    /**
     * Changes this object to reference the object given as the argument.
     * A nullptr argument is also allowed.
     */
    void operator=(T *newPtr) {
        if (ptr != newPtr) {
            deregisterPtr();
            ptr = newPtr;
            registerPtr();
        }
    }

    /**
     * Changes this object to reference the same object as the one given in
     * the argument.
     */
    void operator=(const opp_component_ptr<T>& ref) {*this = ref.ptr;}

    /**
     * Returns a pointer to the referenced object. Throws an error if the
     * stored pointer is nullptr.
     */
    T *get() const {checkNull(); return ptr;}

    /**
     * Returns a pointer to the referenced object, or nullptr.
     */
    T *getNullable() const {return ptr;}
};

}  // namespace omnetpp

#endif

