//==========================================================================
//  JSON.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __JSON_H
#define __JSON_H

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "envirdefs.h"
#include "stringutil.h"
#include "simtime.h"


class ENVIR_API JsonNode
{
    public:
        virtual void printOn(std::ostream& out) = 0;
        virtual ~JsonNode() {}
};

class ENVIR_API JsonNull : public JsonNode
{
    public:
        virtual void printOn(std::ostream& out) { out << "null"; }
};

class ENVIR_API JsonBool : public JsonNode
{
    private:
        bool value;
    public:
        JsonBool(bool b) { value = b; }
        bool get() {return value;}
        virtual void printOn(std::ostream& out) { out << (value ? "true" : "false"); }
};

class ENVIR_API JsonLong : public JsonNode
{
    private:
        long value;
    public:
        JsonLong(long l) { value = l; }
        long get() {return value;}
        virtual void printOn(std::ostream& out) { out << value; }
};

class ENVIR_API JsonInt64 : public JsonNode
{
    private:
        int64_t value;
    public:
        JsonInt64(int64_t l) { value = l; }
        int64_t get() {return value;}
        virtual void printOn(std::ostream& out) { out << value; }
};

class ENVIR_API JsonDouble : public JsonNode
{
    private:
        double value;
    public:
        JsonDouble(double d) { value = d; }
        double get() {return value;}
        virtual void printOn(std::ostream& out) { out << value; }
};

class ENVIR_API JsonSimTime : public JsonNode
{
    private:
        SimTime value;
    public:
        JsonSimTime(SimTime t) { value = t; }
        const SimTime& get() {return value;}
        virtual void printOn(std::ostream& out) { out << "\"" << value << "\""; }
};

class ENVIR_API JsonString : public JsonNode
{
    private:
        std::string value;
    public:
        JsonString(const char *s) { value = s ? s : ""; }
        JsonString(const std::string& s) { value = s; }
        const std::string& get() {return value;}
        virtual void printOn(std::ostream& out);
};

class ENVIR_API JsonConstantString : public JsonNode
{
    private:
        const char *value;
    public:
        JsonConstantString(const char *s) { value = s; } // note: stores the pointer -- string must persist until this object is disposed!
        const char *get() {return value;}
        virtual void printOn(std::ostream& out);
};

class ENVIR_API JsonArray : public JsonNode, public std::vector<JsonNode*>
{
    public:
        virtual void printOn(std::ostream& out);
        void clear() { size_t n = size(); for (unsigned int i = 0; i < n; i++) delete (*this)[i]; }
        virtual ~JsonArray() { clear(); }
};

// key must be a string constant
class ENVIR_API JsonObject : public JsonNode, public std::vector<std::pair<const char *, JsonNode*> >
{
    public:
        void put(const char *s, JsonNode *node) { push_back(std::pair<const char *, JsonNode*>(s, node)); }
        virtual void printOn(std::ostream& out);
        void clear() { for (JsonObject::iterator it = begin(); it != end(); ++it) delete it->second; }
        virtual ~JsonObject() { clear(); }
};

// key can be any string (it is copied)
class ENVIR_API JsonObject2 : public JsonNode, public std::vector<std::pair<std::string, JsonNode*> >
{
    public:
        void put(const std::string& s, JsonNode *node) { push_back(std::pair<std::string, JsonNode*>(s, node)); }
        virtual void printOn(std::ostream& out);
        void clear() { for (JsonObject2::iterator it = begin(); it != end(); ++it) delete it->second; }
        virtual ~JsonObject2() { clear(); }
};

inline JsonNode *jsonWrap(bool b) { return new JsonBool(b); }
inline JsonNode *jsonWrap(int i) { return new JsonLong(i); }
inline JsonNode *jsonWrap(long l) { return new JsonLong(l); }
#ifndef INT64_IS_SAME_AS_LONG
inline JsonNode *jsonWrap(int64_t l) { return new JsonInt64(l); }
#endif
inline JsonNode *jsonWrap(double d) { return new JsonDouble(d); }
inline JsonNode *jsonWrap(SimTime t) { return new JsonSimTime(t); }
inline JsonNode *jsonWrap(const char *s) { return new JsonConstantString(s); }
inline JsonNode *jsonWrap(const std::string& s) { return new JsonString(s); }

#endif


