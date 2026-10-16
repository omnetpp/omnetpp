//==========================================================================
//  CDYNAMICDESCRIPTOR.H - part of
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

#ifndef __OMNETPP_CDYNAMICDESCRIPTOR_H
#define __OMNETPP_CDYNAMICDESCRIPTOR_H

#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <forward_list>
#include <deque>
#include <array>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>
#include <utility>
#include <iterator>
#include <type_traits>
#include "cclassdescriptor.h"
#include "simutil.h"
#include "globals.h"

namespace omnetpp {

namespace internal {

// --- Type traits ---

template<typename T, typename = void>
struct is_printable : std::false_type {};
template<typename T>
struct is_printable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>> : std::true_type {};

template<typename T, typename = void>
struct is_extractable : std::false_type {};
template<typename T>
struct is_extractable<T, std::void_t<decltype(std::declval<std::istream&>() >> std::declval<T&>())>> : std::true_type {};

// standard library containers supported by cStdContainerDescriptor
template<typename T> struct is_std_sequence : std::false_type {};  // elements are accessed by index
template<typename E, typename A> struct is_std_sequence<std::vector<E,A>> : std::true_type {};
template<typename E, typename A> struct is_std_sequence<std::list<E,A>> : std::true_type {};
template<typename E, typename A> struct is_std_sequence<std::forward_list<E,A>> : std::true_type {};
template<typename E, typename A> struct is_std_sequence<std::deque<E,A>> : std::true_type {};
template<typename E, size_t N> struct is_std_sequence<std::array<E,N>> : std::true_type {};

template<typename T> struct is_std_set : std::false_type {};  // elements are read-only
template<typename E, typename C, typename A> struct is_std_set<std::set<E,C,A>> : std::true_type {};
template<typename E, typename C, typename A> struct is_std_set<std::multiset<E,C,A>> : std::true_type {};
template<typename E, typename H, typename Q, typename A> struct is_std_set<std::unordered_set<E,H,Q,A>> : std::true_type {};
template<typename E, typename H, typename Q, typename A> struct is_std_set<std::unordered_multiset<E,H,Q,A>> : std::true_type {};

template<typename T> struct is_std_map : std::false_type {};  // elements are the mapped values, labeled with the keys
template<typename K, typename V, typename C, typename A> struct is_std_map<std::map<K,V,C,A>> : std::true_type {};
template<typename K, typename V, typename C, typename A> struct is_std_map<std::multimap<K,V,C,A>> : std::true_type {};
template<typename K, typename V, typename H, typename Q, typename A> struct is_std_map<std::unordered_map<K,V,H,Q,A>> : std::true_type {};
template<typename K, typename V, typename H, typename Q, typename A> struct is_std_map<std::unordered_multimap<K,V,H,Q,A>> : std::true_type {};

template<typename T> struct is_std_adaptor : std::false_type {};  // container adaptors: their underlying container is displayed, read-only
template<typename E, typename C> struct is_std_adaptor<std::queue<E,C>> : std::true_type {};
template<typename E, typename C> struct is_std_adaptor<std::stack<E,C>> : std::true_type {};
template<typename E, typename C, typename P> struct is_std_adaptor<std::priority_queue<E,C,P>> : std::true_type {};

// gives access to the underlying container (protected member "c") of a container adaptor
template<typename A>
struct AdaptorAccess : A {
    static typename A::container_type *getContainer(A *adaptor) { return &(adaptor->*(&AdaptorAccess::c)); }
};

template<typename T, bool isAdaptor = is_std_adaptor<T>::value>
struct underlying_container { using type = T; };
template<typename T>
struct underlying_container<T, true> { using type = typename T::container_type; };

template<typename T> struct is_std_pair : std::false_type {};
template<typename F, typename S> struct is_std_pair<std::pair<F,S>> : std::true_type {};

template<typename C, bool isMap = is_std_map<C>::value>
struct container_element { using type = typename C::value_type; };
template<typename C>
struct container_element<C, true> { using type = typename C::mapped_type; };

template<typename C, typename = void>
struct has_size : std::false_type {};
template<typename C>
struct has_size<C, std::void_t<decltype(std::declval<const C&>().size())>> : std::true_type {};

template<typename C, typename = void>
struct has_resize : std::false_type {};
template<typename C>
struct has_resize<C, std::void_t<decltype(std::declval<C&>().resize(0))>> : std::true_type {};

// Returns the type name for T. Note: it must be unique, as descriptors are
// registered and looked up by name.
template<typename T>
std::string getTypeName() {
    return opp_typename(typeid(T));
}

/**
 * @brief Base class for dynamically created descriptors. Provides defaults
 * for all pure virtual methods of cClassDescriptor (zero fields, empty properties).
 *
 * @ingroup Internals
 */
class SIM_API cDynamicDescriptor : public cClassDescriptor
{
  public:
    cDynamicDescriptor(const char *className, const char *baseClassName = nullptr);
    virtual ~cDynamicDescriptor() {}

    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(any_ptr object, int field) const override;
    virtual void setFieldArraySize(any_ptr object, int field, int size) const override;
    virtual std::string getFieldValueAsString(any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(any_ptr object, int field, int i, const char *value) const override;
    virtual cValue getFieldValue(any_ptr object, int field, int i) const override;
    virtual void setFieldValue(any_ptr object, int field, int i, const cValue& value) const override;
    virtual const char *getFieldStructName(int field) const override;
    virtual any_ptr getFieldStructValuePointer(any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(any_ptr object, int field, int i, any_ptr ptr) const override;
};

/**
 * @brief Descriptor for primitive/simple types that support operator<< and/or operator>>.
 * Has no fields; provides getValueAsString()/setValueAsString() via stream operators.
 *
 * @ingroup Internals
 */
template<typename T>
class cPrimitiveTypeDescriptor : public cDynamicDescriptor
{
  public:
    cPrimitiveTypeDescriptor(const char *typeName) : cDynamicDescriptor(typeName) {}

    virtual std::string getValueAsString(any_ptr object) const override {
        T *p = object.get<T>();
        if constexpr (std::is_same_v<T, char> || std::is_same_v<T, signed char>) {
            std::ostringstream os;
            os << "'" << ((unsigned char)*p < 32 ? ' ' : (char)*p) << "' (" << (int)*p << ")";
            return os.str();
        }
        else if constexpr (std::is_same_v<T, unsigned char>) {
            std::ostringstream os;
            os << "'" << (*p < ' ' ? ' ' : (char)*p) << "' (" << (unsigned)*p << ")";
            return os.str();
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            std::ostringstream os;
            os << "\"" << *p << "\"";
            return os.str();
        }
        else if constexpr (is_printable<T>::value) {
            std::ostringstream os;
            os << *p;
            return os.str();
        }
        return UNPRINTABLE;
    }

    virtual void setValueAsString(any_ptr object, const char *value) const override {
        T *p = object.get<T>();
        if constexpr (std::is_same_v<T, char> || std::is_same_v<T, signed char>) {
            if (value[0] == '\'')
                *p = value[1];
            else
                *p = (char)atoi(value);
        }
        else if constexpr (std::is_same_v<T, unsigned char>) {
            if (value[0] == '\'')
                *p = value[1];
            else
                *p = (unsigned char)atoi(value);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            size_t len = strlen(value);
            if (len >= 2 && value[0] == '"' && value[len-1] == '"')
                *p = std::string(value + 1, len - 2);
            else
                *p = value;
        }
        else if constexpr (is_extractable<T>::value) {
            std::istringstream is(value);
            is >> *p;
        }
        else {
            cClassDescriptor::setValueAsString(object, value);
        }
    }
};

/**
 * @brief Operations on a single value of type E (a container element or a pair
 * member) for the descriptors of standard library types. E may be a pointer;
 * null pointers are displayed as "nullptr". cObjects are accessed via their own
 * descriptors (cObject::getDescriptor()), other values via the descriptor registered
 * for their type (see ensureDescriptor()), or else via operator<< and operator>>.
 *
 * @ingroup Internals
 */
template<typename E>
class ValueAccess
{
  private:
    using T = std::remove_cv_t<std::remove_pointer_t<E>>;  // type of the value (the pointee for pointers)
    static constexpr bool isCObject = std::is_base_of_v<cObject, T>;

    // returns the address of the value; nullptr for null pointers
    static T *getValuePointer(const E& e) {
        if constexpr (std::is_pointer_v<E>)
            return const_cast<T*>(e);
        else
            return const_cast<T*>(&e);
    }

    static any_ptr toAny(T *p) {
        if constexpr (isCObject)
            return toAnyPtr(static_cast<const cObject*>(p));
        else
            return any_ptr(p);
    }

    static cClassDescriptor *getDescriptor(T *p) {
        if constexpr (isCObject)
            return p->getDescriptor();
        else
            return cClassDescriptor::getDescriptorFor(getTypeName().c_str());
    }

  public:
    static const std::string& getTypeName() {
        static const std::string typeName = internal::getTypeName<T>();
        return typeName;
    }

    static unsigned int getTypeFlags() {
        unsigned int flags = 0;
        if constexpr (std::is_pointer_v<E>)
            flags |= cClassDescriptor::FD_ISPOINTER;
        if constexpr (isCObject)
            flags |= cClassDescriptor::FD_ISCOMPOUND | cClassDescriptor::FD_ISCOBJECT;
        else {
            cClassDescriptor *desc = cClassDescriptor::getDescriptorFor(getTypeName().c_str());
            if (desc && desc->getFieldCount() > 0)
                flags |= cClassDescriptor::FD_ISCOMPOUND;
        }
        return flags;
    }

    static std::string getValueAsString(const E& e) {
        T *p = getValuePointer(e);
        if (!p)
            return "nullptr";
        if (cClassDescriptor *desc = getDescriptor(p))
            return desc->getValueAsString(toAny(p));
        if constexpr (!isCObject && is_printable<T>::value) {
            std::ostringstream os;
            os << *p;
            return os.str();
        }
        return cClassDescriptor::UNPRINTABLE;
    }

    // returns false if not possible (no value or no way to parse it)
    static bool setValueAsString(E& e, const char *value) {
        T *p = getValuePointer(e);
        if (!p)
            return false;
        if (cClassDescriptor *desc = getDescriptor(p)) {
            desc->setValueAsString(toAny(p), value);
            return true;
        }
        if constexpr (!isCObject && is_extractable<T>::value) {
            std::istringstream is(value);
            is >> *p;
            return true;
        }
        return false;
    }

    static any_ptr getStructValuePointer(const E& e) {
        T *p = getValuePointer(e);
        return p ? toAny(p) : any_ptr(nullptr);
    }
};

/**
 * @brief Descriptor for standard library containers and container adaptors.
 * Pretends to have a single "elements[]" array field. For maps, the elements
 * are the mapped values, and their array index strings are the keys. Elements
 * of sets are read-only. For container adaptors (std::queue, std::stack,
 * std::priority_queue), the elements of the underlying container are shown
 * (read-only) in its own order: front to back for std::queue, bottom to top
 * for std::stack, heap order for std::priority_queue.
 *
 * Elements are accessed via iterators, so accessing the ith element of a
 * non-random-access container takes O(i) time.
 *
 * @ingroup Internals
 */
template<typename T>
class cStdContainerDescriptor : public cDynamicDescriptor
{
  private:
    static constexpr bool isAdaptor = is_std_adaptor<T>::value;
    using C = typename underlying_container<T>::type;  // the container whose elements are shown
    static constexpr bool isMap = is_std_map<C>::value;
    static constexpr bool isReadOnly = is_std_set<C>::value || isAdaptor;
    using E = typename container_element<C>::type;  // mapped_type for maps, otherwise value_type

    // true for std::vector<bool>, whose elements are not addressable: operator[]
    // and iterators return a proxy object instead of a reference
    static constexpr bool hasProxyElements = !std::is_reference_v<typename C::reference>;

    static int getSize(const C *c) {
        if constexpr (has_size<C>::value)
            return (int)c->size();
        else
            return (int)std::distance(c->begin(), c->end());  // e.g. std::forward_list
    }

    template<typename Iterator>
    static auto& getElement(Iterator it) {
        if constexpr (isMap)
            return it->second;
        else
            return *it;
    }

    static C *getContainer(any_ptr object) {
        if constexpr (isAdaptor)
            return AdaptorAccess<T>::getContainer(object.get<T>());
        else
            return object.get<T>();
    }

    // returns the iterator to the ith element, or end() if i is out of range
    static auto getIterator(C *c, int i) {
        return (i >= 0 && i < getSize(c)) ? std::next(c->begin(), i) : c->end();
    }

  public:
    cStdContainerDescriptor(const char *typeName) : cDynamicDescriptor(typeName) {}

    virtual std::string getValueAsString(any_ptr object) const override {
        C *c = getContainer(object);
        if (c->begin() == c->end())
            return "empty";
        return "size=" + std::to_string(getSize(c));
    }

    virtual int getFieldCount() const override { return 1; }
    virtual const char *getFieldName(int field) const override { return field == 0 ? "elements" : nullptr; }
    virtual const char *getFieldTypeString(int field) const override { return field == 0 ? ValueAccess<E>::getTypeName().c_str() : nullptr; }
    virtual const char *getFieldStructName(int field) const override { return field == 0 ? ValueAccess<E>::getTypeName().c_str() : nullptr; }
    virtual unsigned int getFieldTypeFlags(int field) const override { return field == 0 ? FD_ISARRAY | ValueAccess<E>::getTypeFlags() : 0; }

    virtual int getFieldArraySize(any_ptr object, int field) const override {
        return field == 0 ? getSize(getContainer(object)) : 0;
    }

    virtual void setFieldArraySize(any_ptr object, int field, int size) const override {
        if constexpr (has_resize<C>::value && std::is_default_constructible_v<E> && !isAdaptor) {
            if (field == 0) {
                getContainer(object)->resize(size);
                return;
            }
        }
        cDynamicDescriptor::setFieldArraySize(object, field, size);
    }

    virtual std::string getFieldArrayIndexString(any_ptr object, int field, int i) const override {
        if constexpr (isMap) {
            C *c = getContainer(object);
            auto it = getIterator(c, i);
            if (field != 0 || it == c->end())
                return "";
            return ValueAccess<typename C::key_type>::getValueAsString(it->first) + " => ";
        }
        else if constexpr (is_std_set<C>::value)
            return "";  // set elements have no meaningful index
        else
            return cDynamicDescriptor::getFieldArrayIndexString(object, field, i);
    }

    virtual std::string getFieldValueAsString(any_ptr object, int field, int i) const override {
        C *c = getContainer(object);
        auto it = getIterator(c, i);
        if (field != 0 || it == c->end())
            return "";
        if constexpr (hasProxyElements) {
            E element = *it;
            return ValueAccess<E>::getValueAsString(element);
        }
        else
            return ValueAccess<E>::getValueAsString(getElement(it));
    }

    virtual void setFieldValueAsString(any_ptr object, int field, int i, const char *value) const override {
        if constexpr (!isReadOnly) {
            C *c = getContainer(object);
            auto it = getIterator(c, i);
            if (field == 0 && it != c->end()) {
                bool done;
                if constexpr (hasProxyElements) {
                    E element = *it;
                    done = ValueAccess<E>::setValueAsString(element, value);
                    if (done)
                        *it = element;
                }
                else
                    done = ValueAccess<E>::setValueAsString(getElement(it), value);
                if (done)
                    return;
            }
        }
        cDynamicDescriptor::setFieldValueAsString(object, field, i, value);
    }

    virtual any_ptr getFieldStructValuePointer(any_ptr object, int field, int i) const override {
        C *c = getContainer(object);
        auto it = getIterator(c, i);
        if (field != 0 || it == c->end())
            return any_ptr(nullptr);
        if constexpr (hasProxyElements)
            return any_ptr(nullptr);  // elements are not addressable (nor compound)
        else
            return ValueAccess<E>::getStructValuePointer(getElement(it));
    }
};

/**
 * @brief Descriptor for std::pair<F,S>. Has two fields: "first" and "second".
 *
 * @ingroup Internals
 */
template<typename F, typename S>
class cStdPairDescriptor : public cDynamicDescriptor
{
  public:
    cStdPairDescriptor(const char *typeName) : cDynamicDescriptor(typeName) {}

    virtual std::string getValueAsString(any_ptr object) const override {
        auto *p = object.get<std::pair<F,S>>();
        return ValueAccess<F>::getValueAsString(p->first) + " => " + ValueAccess<S>::getValueAsString(p->second);
    }

    virtual int getFieldCount() const override { return 2; }

    virtual const char *getFieldName(int field) const override {
        switch (field) { case 0: return "first"; case 1: return "second"; default: return nullptr; }
    }

    virtual const char *getFieldTypeString(int field) const override {
        switch (field) {
            case 0: return ValueAccess<F>::getTypeName().c_str();
            case 1: return ValueAccess<S>::getTypeName().c_str();
            default: return nullptr;
        }
    }

    virtual const char *getFieldStructName(int field) const override { return getFieldTypeString(field); }

    virtual unsigned int getFieldTypeFlags(int field) const override {
        switch (field) {
            case 0: return ValueAccess<F>::getTypeFlags();
            case 1: return ValueAccess<S>::getTypeFlags();
            default: return 0;
        }
    }

    virtual std::string getFieldValueAsString(any_ptr object, int field, int i) const override {
        auto *p = object.get<std::pair<F,S>>();
        switch (field) {
            case 0: return ValueAccess<F>::getValueAsString(p->first);
            case 1: return ValueAccess<S>::getValueAsString(p->second);
            default: return "";
        }
    }

    virtual void setFieldValueAsString(any_ptr object, int field, int i, const char *value) const override {
        auto *p = object.get<std::pair<F,S>>();
        bool done = false;
        if (field == 0)
            done = ValueAccess<F>::setValueAsString(const_cast<F&>(p->first), value);
        else if (field == 1)
            done = ValueAccess<S>::setValueAsString(p->second, value);
        if (!done)
            cDynamicDescriptor::setFieldValueAsString(object, field, i, value);
    }

    virtual any_ptr getFieldStructValuePointer(any_ptr object, int field, int i) const override {
        auto *p = object.get<std::pair<F,S>>();
        switch (field) {
            case 0: return ValueAccess<F>::getStructValuePointer(p->first);
            case 1: return ValueAccess<S>::getStructValuePointer(p->second);
            default: return any_ptr(nullptr);
        }
    }
};

}  // namespace internal


/**
 * @brief Returns the cClassDescriptor for type T. If a descriptor is already
 * registered, returns it; otherwise creates and registers an appropriate one.
 *
 * For primitive types, creates a descriptor that uses operator<< / operator>>
 * for string conversion. For STL containers, creates a descriptor with an
 * "elements[]" array field. For std::pair, creates a descriptor with
 * "first" and "second" fields.
 *
 * For cObject subclasses, no descriptor is created: if none is registered for
 * the class, nullptr is returned. (Objects use the descriptor of their nearest
 * base class then, see cObject::getDescriptor(); registering one under the
 * name of the class would override that for all objects of the class.)
 */
template<typename T>
cClassDescriptor *ensureDescriptor();

namespace internal {

// ensures the descriptor for a container element or pair member type (for pointers, the pointee type)
template<typename E>
void ensureValueDescriptor()
{
    ensureDescriptor<std::remove_pointer_t<E>>();
}

}  // namespace internal

template<typename T>
cClassDescriptor *ensureDescriptor()
{
    // any_ptr strips const, so descriptors must be created for the unqualified type
    if constexpr (!std::is_same_v<T, std::remove_cv_t<T>>)
        return ensureDescriptor<std::remove_cv_t<T>>();
    else {
        static std::string typeNameStr = internal::getTypeName<T>();
        const char *typeName = typeNameStr.c_str();
        cClassDescriptor *desc = cClassDescriptor::getDescriptorFor(typeName);
        if (desc)
            return desc;

        if constexpr (std::is_base_of_v<cObject, T>) {
            return nullptr;
        }
        else if constexpr (internal::is_std_sequence<T>::value || internal::is_std_set<T>::value || internal::is_std_adaptor<T>::value) {
            internal::ensureValueDescriptor<typename T::value_type>();
            desc = new internal::cStdContainerDescriptor<T>(typeName);
        }
        else if constexpr (internal::is_std_map<T>::value) {
            internal::ensureValueDescriptor<typename T::key_type>();
            internal::ensureValueDescriptor<typename T::mapped_type>();
            desc = new internal::cStdContainerDescriptor<T>(typeName);
        }
        else if constexpr (internal::is_std_pair<T>::value) {
            internal::ensureValueDescriptor<typename T::first_type>();
            internal::ensureValueDescriptor<typename T::second_type>();
            desc = new internal::cStdPairDescriptor<typename T::first_type, typename T::second_type>(typeName);
        }
        else {
            desc = new internal::cPrimitiveTypeDescriptor<T>(typeName);
        }
        internal::classDescriptors.getInstance()->add(desc);
        return desc;
    }
}

}  // namespace omnetpp

#endif
