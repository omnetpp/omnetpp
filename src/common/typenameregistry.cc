//==========================================================================
//  TYPENAMEREGISTRY.CC - part of
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

#include <cstdlib>
#include <regex>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif
#include "typenameregistry.h"
#include "stringutil.h"
#include "stringtokenizer.h"
#include "exception.h"

namespace omnetpp {
namespace common {

const char *TypeNameRegistry::getTypeName(const std::type_info& type)
{
    auto it = typeNames.find(type);
    if (it != typeNames.end())
        return it->second.c_str();

    std::string demangledName = demangle(type);
    std::string typeName;
    try {
        typeName = normalize(demangledName);
    }
    catch (std::exception& e) {
        throw opp_runtime_error("opp_typename(): Cannot normalize type name '%s': %s", demangledName.c_str(), e.what());
    }

    // Different types must not get the same name (class descriptors are looked up by name).
    // Normalization is meant to preserve that, but check it anyway. (Note: types in anonymous
    // namespaces of different translation units may also have identical names, but that is
    // not caused by normalization.)
    auto [it2, isNewName] = demangledNameByTypeName.emplace(typeName, demangledName);
    if (!isNewName && it2->second != demangledName)
        throw opp_runtime_error("opp_typename(): Types '%s' and '%s' both normalize to '%s'",
                it2->second.c_str(), demangledName.c_str(), typeName.c_str());

    return typeNames.emplace(type, typeName).first->second.c_str();
}

std::string TypeNameRegistry::demangle(const std::type_info& type)
{
#ifdef _MSC_VER
    // MSVC's type_info::name() returns the demangled name
    return cleanUpMsvcName(type.name());
#else
    int status;
    char *demangled = abi::__cxa_demangle(type.name(), nullptr, nullptr, &status);
    if (status != 0 || demangled == nullptr)
        throw opp_runtime_error("opp_typename(): Cannot demangle type name '%s'", type.name());
    std::string result = demangled;
    free(demangled);
    return result;
#endif
}

std::string TypeNameRegistry::cleanUpMsvcName(const std::string& name)
{
    // Brings a name returned by MSVC's type_info::name() closer to the format
    // of abi::__cxa_demangle(): removes the "class", "struct", "enum" and
    // "union" keywords, and replaces "`anonymous namespace'" with
    // "(anonymous namespace)". (The apostrophe in the latter would also
    // confuse opp_findmatchingparen().)
    static const std::regex keywords("\\b(class|struct|enum|union) ");
    std::string result = std::regex_replace(name, keywords, "");
    return opp_replacesubstring(result, "`anonymous namespace'", "(anonymous namespace)", true);
}

std::string TypeNameRegistry::normalize(const std::string& demangledName)
{
    // Normalizes a demangled type name:
    //  - inline namespaces of the standard library (std::__cxx11, std::__1) are removed;
    //  - trailing template arguments of standard library containers that are equal to
    //    their defaults (e.g. std::allocator<T>) are omitted. Only exact matches are
    //    omitted, so different types still get different names;
    //  - std::basic_string<char, ...> is written as std::string;
    //  - template arguments are separated with ", ", and ">>" is not split.
    // Otherwise, the formatting of the demangler is kept (e.g. "char const*").

    // e.g. "std::__cxx11::list" -> "std::list" (libstdc++), "std::__1::list" -> "std::list" (libc++)
    std::string result = opp_replacesubstring(demangledName, "std::__cxx11::", "std::", true);
    result = opp_replacesubstring(result, "std::__1::", "std::", true);
    return normalizeTemplateArgs(result);
}

std::string TypeNameRegistry::normalizeTemplateArgs(const std::string& s)
{
    // process template argument lists recursively, and copy the rest verbatim
    static const char *NAME_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_:";
    std::string result;
    size_t pos = 0;
    while (true) {
        size_t open = s.find('<', pos);
        if (open == std::string::npos) {
            result.append(s, pos);
            return result;
        }
        const char *closePtr = opp_findmatchingparen(s.c_str() + open);
        if (closePtr == nullptr)
            throw opp_runtime_error("Unmatched '<' at position %d in '%s'", (int)open, s.c_str());
        size_t close = closePtr - s.c_str();
        result.append(s, pos, open - pos);
        size_t nameStart = result.find_last_not_of(NAME_CHARS) + 1;  // note: npos+1 == 0
        std::string name = result.substr(nameStart);
        result.resize(nameStart);
        std::vector<std::string> args = splitTemplateArgs(s.substr(open + 1, close - open - 1));
        for (std::string& arg : args)
            arg = normalizeTemplateArgs(arg);
        result += makeTemplateId(name, args);
        pos = close + 1;
    }
}

std::vector<std::string> TypeNameRegistry::splitTemplateArgs(const std::string& s)
{
    // split at commas, except inside nested template args, function parameter lists, etc.
    StringTokenizer tokenizer(s.c_str(), ",", StringTokenizer::HONOR_PARENS | StringTokenizer::KEEP_EMPTY);
    tokenizer.setParenthesisChars("()<>[]{}");
    try {
        return tokenizer.asVector();
    }
    catch (std::exception& e) {
        throw opp_runtime_error("Cannot split template argument list '%s': %s", s.c_str(), e.what());
    }
}

std::string TypeNameRegistry::makeTemplateId(const std::string& name, std::vector<std::string> args)
{
    if (name == "std::basic_string" && args == std::vector<std::string>{"char", "std::char_traits<char>", "std::allocator<char>"})
        return "std::string";

    // default values of the template arguments of standard library containers, given the preceding arguments
    if (opp_stringbeginswith(name.c_str(), "std::") && !args.empty()) {
        const std::string& a0 = args[0];
        const std::string a1 = args.size() > 1 ? args[1] : "";
        std::string allocator = "std::allocator<" + a0 + ">";
        std::string pairAllocator = "std::allocator<std::pair<" + a0 + " const, " + a1 + ">>";
        std::string less = "std::less<" + a0 + ">";
        std::string hash = "std::hash<" + a0 + ">";
        std::string equalTo = "std::equal_to<" + a0 + ">";

        std::vector<std::string> defaults; // indexed like args; "" means no default
        std::string n = name.substr(5);
        if (n == "vector" || n == "list" || n == "deque" || n == "forward_list")
            defaults = {"", allocator};
        else if (n == "set" || n == "multiset")
            defaults = {"", less, allocator};
        else if (n == "map" || n == "multimap")
            defaults = {"", "", less, pairAllocator};
        else if (n == "unordered_set" || n == "unordered_multiset")
            defaults = {"", hash, equalTo, allocator};
        else if (n == "unordered_map" || n == "unordered_multimap")
            defaults = {"", "", hash, equalTo, pairAllocator};
        else if (n == "queue" || n == "stack")
            defaults = {"", "std::deque<" + a0 + ">"};
        else if (n == "priority_queue")
            defaults = {"", "std::vector<" + a0 + ">", less};
        else if (n == "unique_ptr")
            defaults = {"", "std::default_delete<" + a0 + ">"};

        while (args.size() > 1 && args.size() <= defaults.size() && args.back() == defaults[args.size()-1])
            args.pop_back();
    }

    return name + "<" + opp_join(args, ", ") + ">";
}

}  // namespace common
}  // namespace omnetpp
