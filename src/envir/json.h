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

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "envirdefs.h"
#include "stringutil.h"


class ENVIR_API JsonNode
{
    public:
        virtual void printOn(std::ostream& out) = 0;
        virtual ~JsonNode() {}
    protected:
        static void writeStr(std::ostream& out, const std::string& s) {
            if (opp_needsquotes(s.c_str()))
                out << opp_quotestr(s.c_str());
            else
                out << "\"" + s + "\"";
        }
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

class ENVIR_API JsonDouble : public JsonNode
{
    private:
        double value;
    public:
        JsonDouble(double d) { value = d; }
        double get() {return value;}
        virtual void printOn(std::ostream& out) { out << value; }
};

class ENVIR_API JsonString : public JsonNode
{
    private:
        std::string value;
    public:
        JsonString(const char *s) { value = s ? s : ""; }
        JsonString(const std::string& s) { value = s; }
        std::string get() {return value;}
        virtual void printOn(std::ostream& out) { writeStr(out, value); }
};

class ENVIR_API JsonArray : public JsonNode, public std::vector<JsonNode*>
{
    public:
        virtual void printOn(std::ostream& out);
        void clear() { size_t n = size(); for (int i = 0; i < n; i++) delete (*this)[i]; }
        virtual ~JsonArray() { clear(); }
};

class ENVIR_API JsonMap : public JsonNode, public std::map<std::string, JsonNode*>
{
    public:
        void put(const char *s, JsonNode *node) { (*this)[s] = node; }
        void put(const std::string& s, JsonNode *node) { (*this)[s] = node; }
        virtual void printOn(std::ostream& out);
        void clear() { for (JsonMap::iterator it = begin(); it != end(); ++it) delete it->second; }
        virtual ~JsonMap() { clear(); }
};

inline JsonNode *jsonWrap(bool b) { return new JsonBool(b); }
inline JsonNode *jsonWrap(int i) { return new JsonLong(i); }
inline JsonNode *jsonWrap(long l) { return new JsonLong(l); }
inline JsonNode *jsonWrap(double d) { return new JsonDouble(d); }
inline JsonNode *jsonWrap(const char *s) { return new JsonString(s); }
inline JsonNode *jsonWrap(const std::string& s) { return new JsonString(s); }


#endif


