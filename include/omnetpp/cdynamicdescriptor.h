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
#include <set>
#include <map>
#include <unordered_map>
#include <utility>
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

template<typename T> struct is_std_vector : std::false_type {};
template<typename E, typename A> struct is_std_vector<std::vector<E,A>> : std::true_type {};

template<typename T> struct is_std_list : std::false_type {};
template<typename E, typename A> struct is_std_list<std::list<E,A>> : std::true_type {};

template<typename T> struct is_std_set : std::false_type {};
template<typename E, typename C, typename A> struct is_std_set<std::set<E,C,A>> : std::true_type {};

template<typename T> struct is_std_map : std::false_type {};
template<typename K, typename V, typename C, typename A> struct is_std_map<std::map<K,V,C,A>> : std::true_type {};

template<typename T> struct is_std_unordered_map : std::false_type {};
template<typename K, typename V, typename H, typename E, typename A> struct is_std_unordered_map<std::unordered_map<K,V,H,E,A>> : std::true_type {};

template<typename T> struct is_std_pair : std::false_type {};
template<typename F, typename S> struct is_std_pair<std::pair<F,S>> : std::true_type {};

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
 * @brief Descriptor for std::vector<E>. Pretends to have a single "elements[]"
 * array field of type E.
 *
 * @ingroup Internals
 */
template<typename E>
class cStdVectorDescriptor : public cDynamicDescriptor
{
  private:
    std::string elemTypeName;
  public:
    cStdVectorDescriptor(const char *typeName)
        : cDynamicDescriptor(typeName), elemTypeName(internal::getTypeName<std::remove_pointer_t<E>>()) {}

    virtual std::string getValueAsString(any_ptr object) const override {
        std::vector<E> *v = object.get<std::vector<E>>();
        if (v->empty()) return "empty";
        std::ostringstream os;
        os << "size=" << v->size();
        return os.str();
    }

    virtual int getFieldCount() const override { return 1; }

    virtual const char *getFieldName(int field) const override {
        return field == 0 ? "elements" : nullptr;
    }

    virtual unsigned int getFieldTypeFlags(int field) const override {
        if (field != 0) return 0;
        unsigned int flags = FD_ISARRAY;
        if constexpr (std::is_pointer_v<E>) {
            flags |= FD_ISPOINTER;
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<E>>)
                flags |= FD_ISCOMPOUND | FD_ISCOBJECT;
        }
        if (!(flags & FD_ISCOMPOUND)) {
            cClassDescriptor *elemDesc = cClassDescriptor::getDescriptorFor(elemTypeName.c_str());
            if (elemDesc) {
                flags |= FD_ISCOMPOUND;
                if (elemDesc->extendsCObject())
                    flags |= FD_ISCOBJECT;
            }
        }
        return flags;
    }

    virtual const char *getFieldTypeString(int field) const override {
        return field == 0 ? elemTypeName.c_str() : nullptr;
    }

    virtual int getFieldArraySize(any_ptr object, int field) const override {
        if (field != 0) return 0;
        std::vector<E> *v = object.get<std::vector<E>>();
        return (int)v->size();
    }

    virtual void setFieldArraySize(any_ptr object, int field, int size) const override {
        if (field != 0) { cDynamicDescriptor::setFieldArraySize(object, field, size); return; }
        if constexpr (std::is_default_constructible_v<E>) {
            std::vector<E> *v = object.get<std::vector<E>>();
            v->resize(size);
        }
        else {
            cDynamicDescriptor::setFieldArraySize(object, field, size);
        }
    }

    virtual std::string getFieldValueAsString(any_ptr object, int field, int i) const override {
        if (field != 0) return "";
        std::vector<E> *v = object.get<std::vector<E>>();
        cClassDescriptor *elemDesc = cClassDescriptor::getDescriptorFor(elemTypeName.c_str());
        if constexpr (std::is_pointer_v<E>) {
            E ptr = (*v)[i];
            if (!ptr) return "nullptr";
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<E>>) {
                cClassDescriptor *desc = static_cast<const cObject*>(ptr)->getDescriptor();
                if (desc)
                    return desc->getValueAsString(toAnyPtr(static_cast<const cObject*>(ptr)));
            }
            else {
                if (elemDesc)
                    return elemDesc->getValueAsString(any_ptr(ptr));
                if constexpr (is_printable<std::remove_pointer_t<E>>::value) {
                    std::ostringstream os;
                    os << *ptr;
                    return os.str();
                }
            }
        }
        else {
            if (elemDesc)
                return elemDesc->getValueAsString(any_ptr(&(*v)[i]));
            if constexpr (is_printable<E>::value) {
                std::ostringstream os;
                os << (*v)[i];
                return os.str();
            }
        }
        return UNPRINTABLE;
    }

    virtual void setFieldValueAsString(any_ptr object, int field, int i, const char *value) const override {
        if (field != 0) { cDynamicDescriptor::setFieldValueAsString(object, field, i, value); return; }
        std::vector<E> *v = object.get<std::vector<E>>();
        cClassDescriptor *elemDesc = cClassDescriptor::getDescriptorFor(elemTypeName.c_str());
        if constexpr (std::is_pointer_v<E>) {
            E ptr = (*v)[i];
            if (!ptr) return;
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<E>>) {
                cClassDescriptor *desc = static_cast<const cObject*>(ptr)->getDescriptor();
                if (desc)
                    desc->setValueAsString(toAnyPtr(static_cast<const cObject*>(ptr)), value);
            }
            else {
                if (elemDesc) {
                    elemDesc->setValueAsString(any_ptr(ptr), value);
                    return;
                }
            }
        }
        else {
            if (elemDesc) {
                elemDesc->setValueAsString(any_ptr(&(*v)[i]), value);
                return;
            }
            if constexpr (is_extractable<E>::value) {
                std::istringstream is(value);
                is >> (*v)[i];
            }
            else {
                cDynamicDescriptor::setFieldValueAsString(object, field, i, value);
            }
        }
    }

    virtual const char *getFieldStructName(int field) const override {
        return field == 0 ? elemTypeName.c_str() : nullptr;
    }

    virtual any_ptr getFieldStructValuePointer(any_ptr object, int field, int i) const override {
        if (field != 0) return any_ptr(nullptr);
        std::vector<E> *v = object.get<std::vector<E>>();
        if constexpr (std::is_pointer_v<E>) {
            E ptr = (*v)[i];
            if (!ptr) return any_ptr(nullptr);
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<E>>)
                return toAnyPtr(static_cast<const cObject*>(ptr));
            else
                return any_ptr(ptr);
        }
        else {
            return any_ptr(&(*v)[i]);
        }
    }
};

/**
 * @brief Descriptor for std::list<E>. Pretends to have a single "elements[]"
 * array field of type E. Uses iterator-based access.
 *
 * @ingroup Internals
 */
template<typename E>
class cStdListDescriptor : public cDynamicDescriptor
{
  private:
    std::string elemTypeName;

    typename std::list<E>::iterator nthIt(std::list<E> *c, int i) const {
        auto it = c->begin();
        for (int k = 0; k < i && it != c->end(); k++) ++it;
        return it;
    }

  public:
    cStdListDescriptor(const char *typeName)
        : cDynamicDescriptor(typeName), elemTypeName(internal::getTypeName<std::remove_pointer_t<E>>()) {}

    virtual std::string getValueAsString(any_ptr object) const override {
        auto *c = object.get<std::list<E>>();
        if (c->empty()) return "empty";
        std::ostringstream os;
        os << "size=" << c->size();
        return os.str();
    }

    virtual int getFieldCount() const override { return 1; }
    virtual const char *getFieldName(int field) const override { return field == 0 ? "elements" : nullptr; }
    virtual const char *getFieldTypeString(int field) const override { return field == 0 ? elemTypeName.c_str() : nullptr; }
    virtual const char *getFieldStructName(int field) const override { return field == 0 ? elemTypeName.c_str() : nullptr; }

    virtual unsigned int getFieldTypeFlags(int field) const override {
        if (field != 0) return 0;
        unsigned int flags = FD_ISARRAY;
        if constexpr (std::is_pointer_v<E>) {
            flags |= FD_ISPOINTER;
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<E>>)
                flags |= FD_ISCOMPOUND | FD_ISCOBJECT;
        }
        if (!(flags & FD_ISCOMPOUND)) {
            cClassDescriptor *elemDesc = cClassDescriptor::getDescriptorFor(elemTypeName.c_str());
            if (elemDesc) { flags |= FD_ISCOMPOUND; if (elemDesc->extendsCObject()) flags |= FD_ISCOBJECT; }
        }
        return flags;
    }

    virtual int getFieldArraySize(any_ptr object, int field) const override {
        if (field != 0) return 0;
        return (int)object.get<std::list<E>>()->size();
    }

    virtual std::string getFieldValueAsString(any_ptr object, int field, int i) const override {
        if (field != 0) return "";
        auto *c = object.get<std::list<E>>();
        auto it = nthIt(c, i);
        if (it == c->end()) return "";
        cClassDescriptor *elemDesc = cClassDescriptor::getDescriptorFor(elemTypeName.c_str());
        if constexpr (std::is_pointer_v<E>) {
            E ptr = *it;
            if (!ptr) return "nullptr";
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<E>>) {
                cClassDescriptor *desc = static_cast<const cObject*>(ptr)->getDescriptor();
                if (desc) return desc->getValueAsString(toAnyPtr(static_cast<const cObject*>(ptr)));
            }
            else {
                if (elemDesc) return elemDesc->getValueAsString(any_ptr(ptr));
                if constexpr (is_printable<std::remove_pointer_t<E>>::value) { std::ostringstream os; os << *ptr; return os.str(); }
            }
        }
        else {
            if (elemDesc) return elemDesc->getValueAsString(any_ptr(&(*it)));
            if constexpr (is_printable<E>::value) { std::ostringstream os; os << *it; return os.str(); }
        }
        return UNPRINTABLE;
    }

    virtual void setFieldValueAsString(any_ptr object, int field, int i, const char *value) const override {
        if (field != 0) { cDynamicDescriptor::setFieldValueAsString(object, field, i, value); return; }
        auto *c = object.get<std::list<E>>();
        auto it = nthIt(c, i);
        if (it == c->end()) return;
        cClassDescriptor *elemDesc = cClassDescriptor::getDescriptorFor(elemTypeName.c_str());
        if constexpr (std::is_pointer_v<E>) {
            E ptr = *it;
            if (!ptr) return;
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<E>>) {
                cClassDescriptor *desc = static_cast<const cObject*>(ptr)->getDescriptor();
                if (desc) desc->setValueAsString(toAnyPtr(static_cast<const cObject*>(ptr)), value);
            }
            else {
                if (elemDesc) { elemDesc->setValueAsString(any_ptr(ptr), value); return; }
            }
        }
        else {
            if (elemDesc) { elemDesc->setValueAsString(any_ptr(&(*it)), value); return; }
            if constexpr (is_extractable<E>::value) { std::istringstream is(value); is >> *it; }
            else { cDynamicDescriptor::setFieldValueAsString(object, field, i, value); }
        }
    }

    virtual any_ptr getFieldStructValuePointer(any_ptr object, int field, int i) const override {
        if (field != 0) return any_ptr(nullptr);
        auto *c = object.get<std::list<E>>();
        auto it = nthIt(c, i);
        if (it == c->end()) return any_ptr(nullptr);
        if constexpr (std::is_pointer_v<E>) {
            E ptr = *it;
            if (!ptr) return any_ptr(nullptr);
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<E>>)
                return toAnyPtr(static_cast<const cObject*>(ptr));
            else
                return any_ptr(ptr);
        }
        else {
            return any_ptr(&(*it));
        }
    }
};

/**
 * @brief Descriptor for std::set<E>. Pretends to have a single "elements[]"
 * array field of type E. Elements are read-only.
 *
 * @ingroup Internals
 */
template<typename E>
class cStdSetDescriptor : public cDynamicDescriptor
{
  private:
    std::string elemTypeName;

    typename std::set<E>::const_iterator nthIt(const std::set<E> *c, int i) const {
        auto it = c->begin();
        for (int k = 0; k < i && it != c->end(); k++) ++it;
        return it;
    }

  public:
    cStdSetDescriptor(const char *typeName)
        : cDynamicDescriptor(typeName), elemTypeName(internal::getTypeName<std::remove_pointer_t<E>>()) {}

    virtual std::string getValueAsString(any_ptr object) const override {
        auto *c = object.get<std::set<E>>();
        if (c->empty()) return "empty";
        std::ostringstream os;
        os << "size=" << c->size();
        return os.str();
    }

    virtual int getFieldCount() const override { return 1; }
    virtual const char *getFieldName(int field) const override { return field == 0 ? "elements" : nullptr; }
    virtual const char *getFieldTypeString(int field) const override { return field == 0 ? elemTypeName.c_str() : nullptr; }
    virtual const char *getFieldStructName(int field) const override { return field == 0 ? elemTypeName.c_str() : nullptr; }

    virtual unsigned int getFieldTypeFlags(int field) const override {
        if (field != 0) return 0;
        unsigned int flags = FD_ISARRAY;
        if constexpr (std::is_pointer_v<E>) {
            flags |= FD_ISPOINTER;
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<E>>)
                flags |= FD_ISCOMPOUND | FD_ISCOBJECT;
        }
        if (!(flags & FD_ISCOMPOUND)) {
            cClassDescriptor *elemDesc = cClassDescriptor::getDescriptorFor(elemTypeName.c_str());
            if (elemDesc) {
                flags |= FD_ISCOMPOUND;
                if (elemDesc->extendsCObject())
                    flags |= FD_ISCOBJECT;
            }
        }
        return flags;
    }

    virtual int getFieldArraySize(any_ptr object, int field) const override {
        if (field != 0) return 0;
        return (int)object.get<std::set<E>>()->size();
    }

    virtual std::string getFieldValueAsString(any_ptr object, int field, int i) const override {
        if (field != 0) return "";
        auto *c = object.get<std::set<E>>();
        auto it = nthIt(c, i);
        if (it == c->end()) return "";
        if constexpr (std::is_pointer_v<E>) {
            E ptr = *it;
            if (!ptr) return "nullptr";
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<E>>) {
                cClassDescriptor *desc = static_cast<const cObject*>(ptr)->getDescriptor();
                if (desc) return desc->getValueAsString(toAnyPtr(static_cast<const cObject*>(ptr)));
            }
            else {
                cClassDescriptor *elemDesc = cClassDescriptor::getDescriptorFor(elemTypeName.c_str());
                if (elemDesc) return elemDesc->getValueAsString(any_ptr(ptr));
                if constexpr (is_printable<std::remove_pointer_t<E>>::value) { std::ostringstream os; os << *ptr; return os.str(); }
            }
        }
        else {
            cClassDescriptor *elemDesc = cClassDescriptor::getDescriptorFor(elemTypeName.c_str());
            if (elemDesc) return elemDesc->getValueAsString(any_ptr(const_cast<E*>(&(*it))));
            if constexpr (is_printable<E>::value) { std::ostringstream os; os << *it; return os.str(); }
        }
        return UNPRINTABLE;
    }

    virtual any_ptr getFieldStructValuePointer(any_ptr object, int field, int i) const override {
        if (field != 0) return any_ptr(nullptr);
        auto *c = object.get<std::set<E>>();
        auto it = nthIt(c, i);
        if (it == c->end()) return any_ptr(nullptr);
        if constexpr (std::is_pointer_v<E>) {
            E ptr = *it;
            if (!ptr) return any_ptr(nullptr);
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<E>>)
                return toAnyPtr(static_cast<const cObject*>(ptr));
            else
                return any_ptr(ptr);
        }
        else {
            return any_ptr(const_cast<E*>(&(*it)));
        }
    }

    virtual std::string getFieldArrayIndexString(any_ptr object, int field, int arrayIndex) const override {
        return "";
    }
};

/**
 * @brief Descriptor for std::map<K,V> and std::unordered_map<K,V>.
 * Pretends to have a single "elements[]" array field of type V.
 * Element values are shown as "key => value" format.
 *
 * @ingroup Internals
 */
template<typename MapType>
class cStdMapDescriptor : public cDynamicDescriptor
{
  private:
    using K = typename MapType::key_type;
    using V = typename MapType::mapped_type;
    std::string keyTypeName;
    std::string valueTypeName;

    typename MapType::iterator nthIt(MapType *m, int i) const {
        auto it = m->begin();
        for (int k = 0; k < i && it != m->end(); k++) ++it;
        return it;
    }

  public:
    cStdMapDescriptor(const char *typeName)
        : cDynamicDescriptor(typeName),
          keyTypeName(internal::getTypeName<std::remove_pointer_t<K>>()),
          valueTypeName(internal::getTypeName<std::remove_pointer_t<V>>()) {}

    virtual std::string getValueAsString(any_ptr object) const override {
        auto *m = object.get<MapType>();
        if (m->empty()) return "empty";
        std::ostringstream os;
        os << "size=" << m->size();
        return os.str();
    }

    virtual int getFieldCount() const override { return 1; }
    virtual const char *getFieldName(int field) const override { return field == 0 ? "elements" : nullptr; }
    virtual const char *getFieldTypeString(int field) const override { return field == 0 ? valueTypeName.c_str() : nullptr; }
    virtual const char *getFieldStructName(int field) const override { return field == 0 ? valueTypeName.c_str() : nullptr; }

    virtual unsigned int getFieldTypeFlags(int field) const override {
        if (field != 0) return 0;
        unsigned int flags = FD_ISARRAY;
        if constexpr (std::is_pointer_v<V>) {
            flags |= FD_ISPOINTER;
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<V>>)
                flags |= FD_ISCOMPOUND | FD_ISCOBJECT;
        }
        if (!(flags & FD_ISCOMPOUND)) {
            cClassDescriptor *valDesc = cClassDescriptor::getDescriptorFor(valueTypeName.c_str());
            if (valDesc) { flags |= FD_ISCOMPOUND; if (valDesc->extendsCObject()) flags |= FD_ISCOBJECT; }
        }
        return flags;
    }

    virtual int getFieldArraySize(any_ptr object, int field) const override {
        if (field != 0) return 0;
        return (int)object.get<MapType>()->size();
    }

    virtual std::string getFieldValueAsString(any_ptr object, int field, int i) const override {
        if (field != 0) return "";
        auto *m = object.get<MapType>();
        auto it = nthIt(m, i);
        if (it == m->end()) return "";
        if constexpr (std::is_pointer_v<V>) {
            V vptr = it->second;
            if (!vptr) return "nullptr";
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<V>>) {
                cClassDescriptor *desc = static_cast<const cObject*>(vptr)->getDescriptor();
                if (desc) return desc->getValueAsString(toAnyPtr(static_cast<const cObject*>(vptr)));
            }
            else {
                cClassDescriptor *valDesc = cClassDescriptor::getDescriptorFor(valueTypeName.c_str());
                if (valDesc) return valDesc->getValueAsString(any_ptr(vptr));
                if constexpr (is_printable<std::remove_pointer_t<V>>::value) { std::ostringstream os; os << *vptr; return os.str(); }
            }
        }
        else {
            cClassDescriptor *valDesc = cClassDescriptor::getDescriptorFor(valueTypeName.c_str());
            if (valDesc) return valDesc->getValueAsString(any_ptr(&it->second));
            if constexpr (is_printable<V>::value) { std::ostringstream os; os << it->second; return os.str(); }
        }
        return UNPRINTABLE;
    }

    virtual void setFieldValueAsString(any_ptr object, int field, int i, const char *value) const override {
        if (field != 0) { cDynamicDescriptor::setFieldValueAsString(object, field, i, value); return; }
        auto *m = object.get<MapType>();
        auto it = nthIt(m, i);
        if (it == m->end()) return;
        if constexpr (std::is_pointer_v<V>) {
            V vptr = it->second;
            if (!vptr) return;
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<V>>) {
                cClassDescriptor *desc = static_cast<const cObject*>(vptr)->getDescriptor();
                if (desc) desc->setValueAsString(toAnyPtr(static_cast<const cObject*>(vptr)), value);
            }
            else {
                cClassDescriptor *valDesc = cClassDescriptor::getDescriptorFor(valueTypeName.c_str());
                if (valDesc) { valDesc->setValueAsString(any_ptr(vptr), value); return; }
            }
        }
        else {
            cClassDescriptor *valDesc = cClassDescriptor::getDescriptorFor(valueTypeName.c_str());
            if (valDesc) { valDesc->setValueAsString(any_ptr(&it->second), value); return; }
            if constexpr (is_extractable<V>::value) { std::istringstream is(value); is >> it->second; }
            else { cDynamicDescriptor::setFieldValueAsString(object, field, i, value); }
        }
    }

    virtual any_ptr getFieldStructValuePointer(any_ptr object, int field, int i) const override {
        if (field != 0) return any_ptr(nullptr);
        auto *m = object.get<MapType>();
        auto it = nthIt(m, i);
        if (it == m->end()) return any_ptr(nullptr);
        if constexpr (std::is_pointer_v<V>) {
            V vptr = it->second;
            if (!vptr) return any_ptr(nullptr);
            if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<V>>)
                return toAnyPtr(static_cast<const cObject*>(vptr));
            else
                return any_ptr(vptr);
        }
        else {
            return any_ptr(&it->second);
        }
    }

    virtual std::string getFieldArrayIndexString(any_ptr object, int field, int arrayIndex) const override {
        if (field != 0) return "";
        auto *m = object.get<MapType>();
        auto it = nthIt(m, arrayIndex);
        if (it == m->end()) return "";
        std::ostringstream os;
        if constexpr (std::is_pointer_v<K>) {
            K kptr = it->first;
            if (!kptr) os << "nullptr";
            else if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<K>>) {
                cClassDescriptor *desc = static_cast<const cObject*>(kptr)->getDescriptor();
                if (desc) os << desc->getValueAsString(toAnyPtr(static_cast<const cObject*>(kptr)));
                else os << UNPRINTABLE;
            }
            else {
                cClassDescriptor *keyDesc = cClassDescriptor::getDescriptorFor(keyTypeName.c_str());
                if (keyDesc) os << keyDesc->getValueAsString(any_ptr(kptr));
                else if constexpr (is_printable<std::remove_pointer_t<K>>::value) os << *kptr;
                else os << UNPRINTABLE;
            }
        }
        else {
            cClassDescriptor *keyDesc = cClassDescriptor::getDescriptorFor(keyTypeName.c_str());
            if (keyDesc) os << keyDesc->getValueAsString(any_ptr(const_cast<K*>(&it->first)));
            else if constexpr (is_printable<K>::value) os << it->first;
            else os << UNPRINTABLE;
        }
        os << " => ";
        return os.str();
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
  private:
    std::string firstTypeName;
    std::string secondTypeName;

  public:
    cStdPairDescriptor(const char *typeName)
        : cDynamicDescriptor(typeName),
          firstTypeName(internal::getTypeName<std::remove_pointer_t<F>>()),
          secondTypeName(internal::getTypeName<std::remove_pointer_t<S>>()) {}

    virtual std::string getValueAsString(any_ptr object) const override {
        auto *p = object.get<std::pair<F,S>>();
        std::ostringstream os;
        // first
        if constexpr (std::is_pointer_v<F>) {
            F fptr = p->first;
            if (!fptr) os << "nullptr";
            else if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<F>>) {
                cClassDescriptor *desc = static_cast<const cObject*>(fptr)->getDescriptor();
                if (desc) os << desc->getValueAsString(toAnyPtr(static_cast<const cObject*>(fptr))); else os << UNPRINTABLE;
            }
            else {
                cClassDescriptor *fDesc = cClassDescriptor::getDescriptorFor(firstTypeName.c_str());
                if (fDesc) os << fDesc->getValueAsString(any_ptr(fptr));
                else if constexpr (is_printable<std::remove_pointer_t<F>>::value) os << *fptr;
                else os << UNPRINTABLE;
            }
        }
        else {
            cClassDescriptor *fDesc = cClassDescriptor::getDescriptorFor(firstTypeName.c_str());
            if (fDesc) os << fDesc->getValueAsString(any_ptr(const_cast<F*>(&p->first)));
            else if constexpr (is_printable<F>::value) os << p->first;
            else os << UNPRINTABLE;
        }
        os << " => ";
        // second
        if constexpr (std::is_pointer_v<S>) {
            S sptr = p->second;
            if (!sptr) os << "nullptr";
            else if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<S>>) {
                cClassDescriptor *desc = static_cast<const cObject*>(sptr)->getDescriptor();
                if (desc) os << desc->getValueAsString(toAnyPtr(static_cast<const cObject*>(sptr))); else os << UNPRINTABLE;
            }
            else {
                cClassDescriptor *sDesc = cClassDescriptor::getDescriptorFor(secondTypeName.c_str());
                if (sDesc) os << sDesc->getValueAsString(any_ptr(sptr));
                else if constexpr (is_printable<std::remove_pointer_t<S>>::value) os << *sptr;
                else os << UNPRINTABLE;
            }
        }
        else {
            cClassDescriptor *sDesc = cClassDescriptor::getDescriptorFor(secondTypeName.c_str());
            if (sDesc) os << sDesc->getValueAsString(any_ptr(&p->second));
            else if constexpr (is_printable<S>::value) os << p->second;
            else os << UNPRINTABLE;
        }
        return os.str();
    }

    virtual int getFieldCount() const override { return 2; }

    virtual const char *getFieldName(int field) const override {
        switch (field) { case 0: return "first"; case 1: return "second"; default: return nullptr; }
    }

    virtual const char *getFieldTypeString(int field) const override {
        switch (field) { case 0: return firstTypeName.c_str(); case 1: return secondTypeName.c_str(); default: return nullptr; }
    }

    virtual const char *getFieldStructName(int field) const override {
        switch (field) { case 0: return firstTypeName.c_str(); case 1: return secondTypeName.c_str(); default: return nullptr; }
    }

    virtual unsigned int getFieldTypeFlags(int field) const override {
        if (field != 0 && field != 1) return 0;
        unsigned int flags = 0;
        if (field == 0) {
            if constexpr (std::is_pointer_v<F>) {
                flags |= FD_ISPOINTER;
                if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<F>>)
                    flags |= FD_ISCOMPOUND | FD_ISCOBJECT;
            }
        }
        else {
            if constexpr (std::is_pointer_v<S>) {
                flags |= FD_ISPOINTER;
                if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<S>>)
                    flags |= FD_ISCOMPOUND | FD_ISCOBJECT;
            }
        }
        if (!(flags & FD_ISCOMPOUND)) {
            const char *tn = (field == 0) ? firstTypeName.c_str() : secondTypeName.c_str();
            cClassDescriptor *d = cClassDescriptor::getDescriptorFor(tn);
            if (d && d->getFieldCount() > 0) { flags |= FD_ISCOMPOUND; if (d->extendsCObject()) flags |= FD_ISCOBJECT; }
        }
        return flags;
    }

    virtual std::string getFieldValueAsString(any_ptr object, int field, int i) const override {
        auto *p = object.get<std::pair<F,S>>();
        if (field == 0) {
            if constexpr (std::is_pointer_v<F>) {
                F fptr = p->first;
                if (!fptr) return "nullptr";
                if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<F>>) {
                    cClassDescriptor *desc = static_cast<const cObject*>(fptr)->getDescriptor();
                    if (desc) return desc->getValueAsString(toAnyPtr(static_cast<const cObject*>(fptr)));
                }
                else {
                    cClassDescriptor *d = cClassDescriptor::getDescriptorFor(firstTypeName.c_str());
                    if (d) return d->getValueAsString(any_ptr(fptr));
                    if constexpr (is_printable<std::remove_pointer_t<F>>::value) { std::ostringstream os; os << *fptr; return os.str(); }
                }
            }
            else {
                cClassDescriptor *d = cClassDescriptor::getDescriptorFor(firstTypeName.c_str());
                if (d) return d->getValueAsString(any_ptr(const_cast<F*>(&p->first)));
                if constexpr (is_printable<F>::value) { std::ostringstream os; os << p->first; return os.str(); }
            }
        }
        else if (field == 1) {
            if constexpr (std::is_pointer_v<S>) {
                S sptr = p->second;
                if (!sptr) return "nullptr";
                if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<S>>) {
                    cClassDescriptor *desc = static_cast<const cObject*>(sptr)->getDescriptor();
                    if (desc) return desc->getValueAsString(toAnyPtr(static_cast<const cObject*>(sptr)));
                }
                else {
                    cClassDescriptor *d = cClassDescriptor::getDescriptorFor(secondTypeName.c_str());
                    if (d) return d->getValueAsString(any_ptr(sptr));
                    if constexpr (is_printable<std::remove_pointer_t<S>>::value) { std::ostringstream os; os << *sptr; return os.str(); }
                }
            }
            else {
                cClassDescriptor *d = cClassDescriptor::getDescriptorFor(secondTypeName.c_str());
                if (d) return d->getValueAsString(any_ptr(&p->second));
                if constexpr (is_printable<S>::value) { std::ostringstream os; os << p->second; return os.str(); }
            }
        }
        return UNPRINTABLE;
    }

    virtual void setFieldValueAsString(any_ptr object, int field, int i, const char *value) const override {
        auto *p = object.get<std::pair<F,S>>();
        if (field == 0) {
            if constexpr (std::is_pointer_v<F>) {
                F fptr = p->first;
                if (!fptr) return;
                if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<F>>) {
                    cClassDescriptor *desc = static_cast<const cObject*>(fptr)->getDescriptor();
                    if (desc) desc->setValueAsString(toAnyPtr(static_cast<const cObject*>(fptr)), value);
                }
                else {
                    cClassDescriptor *d = cClassDescriptor::getDescriptorFor(firstTypeName.c_str());
                    if (d) { d->setValueAsString(any_ptr(fptr), value); return; }
                }
            }
            else {
                cClassDescriptor *d = cClassDescriptor::getDescriptorFor(firstTypeName.c_str());
                if (d) { d->setValueAsString(any_ptr(const_cast<F*>(&p->first)), value); return; }
                if constexpr (is_extractable<F>::value) { std::istringstream is(value); is >> const_cast<F&>(p->first); return; }
            }
        }
        else if (field == 1) {
            if constexpr (std::is_pointer_v<S>) {
                S sptr = p->second;
                if (!sptr) return;
                if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<S>>) {
                    cClassDescriptor *desc = static_cast<const cObject*>(sptr)->getDescriptor();
                    if (desc) desc->setValueAsString(toAnyPtr(static_cast<const cObject*>(sptr)), value);
                }
                else {
                    cClassDescriptor *d = cClassDescriptor::getDescriptorFor(secondTypeName.c_str());
                    if (d) { d->setValueAsString(any_ptr(sptr), value); return; }
                }
            }
            else {
                cClassDescriptor *d = cClassDescriptor::getDescriptorFor(secondTypeName.c_str());
                if (d) { d->setValueAsString(any_ptr(&p->second), value); return; }
                if constexpr (is_extractable<S>::value) { std::istringstream is(value); is >> p->second; return; }
            }
        }
        cDynamicDescriptor::setFieldValueAsString(object, field, i, value);
    }

    virtual any_ptr getFieldStructValuePointer(any_ptr object, int field, int i) const override {
        auto *p = object.get<std::pair<F,S>>();
        if (field == 0) {
            if constexpr (std::is_pointer_v<F>) {
                F fptr = p->first;
                if (!fptr) return any_ptr(nullptr);
                if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<F>>)
                    return toAnyPtr(static_cast<const cObject*>(fptr));
                else
                    return any_ptr(fptr);
            }
            else {
                return any_ptr(const_cast<F*>(&p->first));
            }
        }
        if (field == 1) {
            if constexpr (std::is_pointer_v<S>) {
                S sptr = p->second;
                if (!sptr) return any_ptr(nullptr);
                if constexpr (std::is_base_of_v<cObject, std::remove_pointer_t<S>>)
                    return toAnyPtr(static_cast<const cObject*>(sptr));
                else
                    return any_ptr(sptr);
            }
            else {
                return any_ptr(&p->second);
            }
        }
        return any_ptr(nullptr);
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
        else if constexpr (internal::is_std_vector<T>::value) {
            using E = typename T::value_type;
            if constexpr (std::is_pointer_v<E>)
                ensureDescriptor<std::remove_pointer_t<E>>();
            else
                ensureDescriptor<E>();
            desc = new internal::cStdVectorDescriptor<E>(typeName);
        }
        else if constexpr (internal::is_std_list<T>::value) {
            using E = typename T::value_type;
            if constexpr (std::is_pointer_v<E>)
                ensureDescriptor<std::remove_pointer_t<E>>();
            else
                ensureDescriptor<E>();
            desc = new internal::cStdListDescriptor<E>(typeName);
        }
        else if constexpr (internal::is_std_set<T>::value) {
            using E = typename T::value_type;
            if constexpr (std::is_pointer_v<E>)
                ensureDescriptor<std::remove_pointer_t<E>>();
            else
                ensureDescriptor<E>();
            desc = new internal::cStdSetDescriptor<E>(typeName);
        }
        else if constexpr (internal::is_std_map<T>::value) {
            using K = typename T::key_type;
            using V = typename T::mapped_type;
            if constexpr (std::is_pointer_v<K>) ensureDescriptor<std::remove_pointer_t<K>>(); else ensureDescriptor<K>();
            if constexpr (std::is_pointer_v<V>) ensureDescriptor<std::remove_pointer_t<V>>(); else ensureDescriptor<V>();
            desc = new internal::cStdMapDescriptor<T>(typeName);
        }
        else if constexpr (internal::is_std_unordered_map<T>::value) {
            using K = typename T::key_type;
            using V = typename T::mapped_type;
            if constexpr (std::is_pointer_v<K>) ensureDescriptor<std::remove_pointer_t<K>>(); else ensureDescriptor<K>();
            if constexpr (std::is_pointer_v<V>) ensureDescriptor<std::remove_pointer_t<V>>(); else ensureDescriptor<V>();
            desc = new internal::cStdMapDescriptor<T>(typeName);
        }
        else if constexpr (internal::is_std_pair<T>::value) {
            using F = typename T::first_type;
            using S = typename T::second_type;
            if constexpr (std::is_pointer_v<F>) ensureDescriptor<std::remove_pointer_t<F>>(); else ensureDescriptor<F>();
            if constexpr (std::is_pointer_v<S>) ensureDescriptor<std::remove_pointer_t<S>>(); else ensureDescriptor<S>();
            desc = new internal::cStdPairDescriptor<F,S>(typeName);
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
