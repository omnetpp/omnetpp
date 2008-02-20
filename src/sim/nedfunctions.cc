//=========================================================================
//  NEDFUNCTIONS.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <string.h>
#include "cnedfunction.h"
#include "cexception.h"
#include "opp_ctype.h"

USING_NAMESPACE

// Implementation of NED string manipulation functions.

//FIXME check units!
//TODO add: replaceunit(var, "m"); dropunit(var); convertunit(var, "m"); unitof(var)

typedef cDynamicExpression::Value Value;


// length(S) -> L
Value length(cComponent *context, Value args[], int numargs)
{
    return (long)args[0].str.size();
}
Define_NED_Function(length, "S", "L");

// contains(S, S) -> B
Value contains(cComponent *context, Value args[], int numargs)
{
    return args[0].str.find(args[1].str) != std::string::npos;
}
Define_NED_Function(contains, "SS", "B");

// substring(S, L, L) -> S
Value substring(cComponent *context, Value args[], int numargs)
{
    int size = args[0].str.size();
    int index1 = (int)args[1].dbl;
    int index2 = (int)args[2].dbl;

    if (index1 < 0 || index1 > size)
        throw cRuntimeError("substring(): index1 out of range");
    else if (index2 < 0 || index2 > size)
        throw cRuntimeError("substring(): index2 out of range");

    return args[0].str.substr(index1, index2 - index1);
}
Define_NED_Function(substring, "SLL", "S");

// startswith(S, S) -> B
Value startswith(cComponent *context, Value args[], int numargs)
{
    return args[0].str.find(args[1].str) == 0;
}
Define_NED_Function(startswith, "SS", "B");

// endswith(S, S) -> B
Value endswith(cComponent *context, Value args[], int numargs)
{
    return args[0].str.rfind(args[1].str) == args[0].str.size() - args[1].str.size();
}
Define_NED_Function(endswith, "SS", "B");

// startof(S, L) -> S
Value startof(cComponent *context, Value args[], int numargs)
{
    int length = (int)args[1].dbl;

    if (length < 0)
        throw cRuntimeError("startof(): length is negative");

    return args[0].str.substr(0, min(args[0].str.size(), length));
}
Define_NED_Function(startof, "SL", "S");

// endof(S, L) -> S
Value endof(cComponent *context, Value args[], int numargs)
{
    int length = (int)args[1].dbl;

    if (length < 0)
        throw cRuntimeError("endof(): length is negative");

    int size = args[0].str.size();
    return args[0].str.substr(max(0, size - length), size);
}
Define_NED_Function(endof, "SL", "S");

// leftof(S, L) -> S
Value leftof(cComponent *context, Value args[], int numargs)
{
    int size = args[0].str.size();
    int index = (int)args[1].dbl;

    if (index < 0 || index > size)
        throw cRuntimeError("leftof(): index out of range");

    return startof(context, args, numargs);
}
Define_NED_Function(leftof, "SL", "S");

// rightof(S, L) -> S
Value rightof(cComponent *context, Value args[], int numargs)
{
    int size = args[0].str.size();
    int index = (int)args[1].dbl;

    if (index < 0 || index > size)
        throw cRuntimeError("rightof(): index out of range");

    return args[0].str.substr(index, size);
}
Define_NED_Function(rightof, "SL", "S");

// replace(S, S, S) -> S
Value replace(cComponent *context, Value args[], int numargs)
{
    std::string str = args[0].str;
    std::string &search = args[1].str;
    std::string &replacement = args[2].str;

    int searchSize = search.size();
    int replacementSize = replacement.size();
    int index = 0;

    while ((index = str.find(search, index)) != std::string::npos) {
        str.replace(index, searchSize, replacement);
        index += replacementSize - searchSize + 1;
    }

    return str;
}
Define_NED_Function(replace, "SSS", "S");

// indexof(S, S) -> L
Value indexof(cComponent *context, Value args[], int numargs)
{
    return (long)args[0].str.find(args[1].str);
}
Define_NED_Function(indexof, "SS", "L");

// toupper(S) -> S
Value toupper(cComponent *context, Value args[], int numargs)
{
    std::string tmp = args[0].str;
    int length = tmp.length();
    for (int i=0; i<length; i++)
        tmp[i] = opp_toupper(tmp[i]);
    return tmp;
}
Define_NED_Function(toupper, "S", "S");

// tolower(S) -> S
Value tolower(cComponent *context, Value args[], int numargs)
{
    std::string tmp = args[0].str;
    int length = tmp.length();
    for (int i=0; i<length; i++)
        tmp[i] = opp_tolower(tmp[i]);
    return tmp;
}
Define_NED_Function(tolower, "S", "S");

// toint(*) -> L
Value toint(cComponent *context, Value args[], int numargs)
{
    switch (args[0].type) {
        case cDynamicExpression::Value::BOOL:
            return args[0].bl ? 1L : 0L;
        case cDynamicExpression::Value::DBL:
            return (long)floor(args[0].dbl);
        case cDynamicExpression::Value::STR:
            return atol(args[0].str.c_str());
        case cDynamicExpression::Value::XML:
            throw cRuntimeError("toint(): cannot convert xml to int");
        default:
            throw cRuntimeError("internal error: bad Value type");
    }
}
Define_NED_Function(toint, "*", "L");

// todouble(*) -> D
Value todouble(cComponent *context, Value args[], int numargs)
{
    switch (args[0].type) {
        case cDynamicExpression::Value::BOOL:
            return args[0].bl ? 1.0 : 0.0;
        case cDynamicExpression::Value::DBL:
            return args[0].dbl;
        case cDynamicExpression::Value::STR:
            return atof(args[0].str.c_str());
        case cDynamicExpression::Value::XML:
            throw cRuntimeError("todouble(): cannot convert xml to double");
        default:
            throw cRuntimeError("internal error: bad Value type");
    }
}
Define_NED_Function(todouble, "*", "D");

// tostring(*) -> S
Value tostring(cComponent *context, Value args[], int numargs)
{
    return args[0].toString();
}
Define_NED_Function(tostring, "*", "S");

// don't remove this -- see util.cc
void nedfunctions_dummy() {}

