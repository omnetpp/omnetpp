//=========================================================================
//  UTIL.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Utility functions
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "commonutil.h"
#include "unitconversion.h"
#include "opp_ctype.h"
#include "simutil.h"
#include "cenvir.h"
#include "csimulation.h"
#include "globals.h"
#include "cexception.h"
#include "cnedmathfunction.h"
#include "cscheduler.h" // dummy()
#include "distrib.h" // dummy()

NAMESPACE_BEGIN

#ifdef USE_DOUBLE_SIMTIME
//
// This function is legacy from OMNeT++ 3.x, and it is only used if simtime_t
// is double. It will be removed in the future.
//
char *simtimeToStr(double t, char *buf)
{
   // Print simulation time into a string.
   // The result has two fields:
   //   a)  A real number meaning the sim.time expressed in seconds.
   //       Always has (at least) 8 significant digits
   //   b)  Time expressed in units such as us,ns,ms,s,m,h,d.
   //       The numbers in this field are truncated.
   // Conversion method (%f or %e) and lengths were chosen to
   // minimize fluctuations in total string length.
   // Result is typically 19,20 or 22 chars long.

   static char buf2[48];
   char *b = buf ? buf : buf2;

   if (t==0.0)
       sprintf(b,"0.0000000  ( 0.00s)");
   // Note that in the following line, a small constant is added to t
   // in order to eliminate truncation errors (like: "1.0000000e-6 (0us)")
   else if (t+=1e-16, t<1e-9)
       sprintf(b,"%8.8e ( <1ns)", t);
   else if (t<1e-6)
       sprintf(b,"%8.8e (%3dns)", t,int(t*1e9));
   else if (t<1e-3)
       sprintf(b,"%8.8e (%3dus)", t,int(t*1e6));
   else if (t<1.0)
       sprintf(b,"%.10f (%3dms)", t,int(t*1e3));
   else if (t<60.0)
       sprintf(b,"%8.8g  (%2d.%2.2ds)", t,int(t*100)/100,int(t*100)%100);
   else if (t<3600.0)
       sprintf(b,"%8.8g (%2dm %2ds)", t,int(t)/60, int(t)%60);
   else if (t<86400.0)
       sprintf(b,"%8.8g (%2dh %2dm)", t,int(long(t)/3600L), int((long(t)%3600L)/60L));
   else if (t<8640000.0)
       sprintf(b,"%8.8g (%2dd %2dh)", t,int(t/86400L), int((long(t)%86400L)/3600L));
   else
       sprintf(b,"%8.8e (%2gd)", t,floor(t/86400L));

   return b;
}

//
// This function is legacy from OMNeT++ 3.x, and it is only used if simtime_t
// is double. It will be removed in the future.
//
char *simtimeToStrShort(double t, char *buf)
{
   // Print simulation time into a string -- short version.
   static char buf2[48];
   char *b = buf ? buf : buf2;

   if (t==0.0)
       sprintf(b,"0s");
   // Note that in the following line, a small constant is added to t
   // in order to eliminate truncation errors (like: "1.0000000e-6 (0us)")
   else if (t+=1e-16, t<1e-9)
       sprintf(b,"%.5gs", t);
   else if (t<1e-6)
       sprintf(b,"%.5gns", t*1e9);
   else if (t<1e-3)
       sprintf(b,"%.5gus", t*1e6);
   else if (t<1.0)
       sprintf(b,"%.5gms", t*1e3);
   else if (t<3600.0)
       sprintf(b,"%.5gs", t);
   else if (t<86400.0)
       sprintf(b,"%dh %dm", int(long(t)/3600L), int((long(t)%3600L)/60L));
   else if (t<864000000.0)
       sprintf(b,"%dd %dh", int(t/86400L), int((long(t)%86400L)/3600L));
   else
       sprintf(b,"%.5gs", t);

   return b;
}

//
// This function is legacy from OMNeT++ 3.x, and it is only used if simtime_t
// is double. It will be removed in the future.
//
double strToSimtime(const char *str)
{
    try {
        return UnitConversion::parseQuantity(str, "s");
    }
    catch (std::exception& e) {
        throw cRuntimeError("strToSimtime(): %s", e.what());
    }
}

//
// This function is legacy from OMNeT++ 3.x, and it is only used if simtime_t
// is double. It will be removed in the future.
//
double strToSimtime0(const char *&str)
{
    const char *end = str;
    while (opp_isspace(*end))
        end++;
    if (!*end)
        return 0.0; // it was just space

    while (opp_isalnum(*end) || opp_isspace(*end) || *end=='+' || *end=='-' || *end=='.')
        end++;
    std::string tmp(str, end-str);
    str = end;

    try {
        return UnitConversion::parseQuantity(tmp.c_str(), "s");
    }
    catch (std::exception& e) {
        throw cRuntimeError("strToSimtime(): %s", e.what());
    }
}
#endif //USE_DOUBLE_SIMTIME

char *opp_strprettytrunc(char *dest, const char *src, unsigned maxlen)
{
    if (!src) {
        *dest = '\0';
        return dest;
    }
    strncpy(dest, src, maxlen);
    if (strlen(src) > maxlen) {
        dest[maxlen] = '\0';
        if (maxlen >= 3)
            dest[maxlen-1] = dest[maxlen-2] = dest[maxlen-3] = '.';
    }
    return dest;
}

//----

#define BUFLEN 512

void opp_error(OppErrorCode errorcode...)
{
    char message[BUFLEN];
    VSNPRINTF2(message, BUFLEN, errorcode, cErrorMessages::get(errorcode));
    throw cRuntimeError(errorcode, message);
}

void opp_error(const char *msgformat...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, msgformat);
    throw cRuntimeError(eUSER, message);
}

void opp_warning(OppErrorCode errorcode...)
{
    char message[BUFLEN];
    VSNPRINTF2(message, BUFLEN, errorcode, cErrorMessages::get(errorcode));

    if (!simulation.getContextModule())
    {
        // we're called from global context
        ev.printfmsg("%s.", message);
    }
    else
    {
        ev.printfmsg("Module %s: %s.", simulation.getContextModule()->getFullPath().c_str(), message);
    }
}

void opp_warning(const char *msgformat...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, msgformat);

    if (!simulation.getContextModule())
    {
        // we're called from global context
        ev.printfmsg("%s.", message);
    }
    else
    {
        ev.printfmsg("Module %s: %s.", simulation.getContextModule()->getFullPath().c_str(), message);
    }
}

void opp_terminate(OppErrorCode errorcode...)
{
    char message[BUFLEN];
    VSNPRINTF2(message, BUFLEN, errorcode, cErrorMessages::get(errorcode));
    throw cTerminationException(errorcode,message);
}

void opp_terminate(const char *msgformat...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, msgformat);
    throw cTerminationException(message);
}

#undef BUFLEN

//----

#ifdef __GNUC__
typedef std::map<std::string,std::string> StringMap;
static StringMap demangledNames;
#endif

const char *opp_typename(const std::type_info& t)
{
    if (t == typeid(std::string))
        return "std::string"; // otherwise we'd get "std::basic_string<........>"

    const char *mangledName = t.name();
    const char *s = mangledName;

#ifdef __GNUC__
    // gcc's typeinfo returns mangled name:
    //   - Foo -> "3Foo"
    //   - omnetpp::Foo -> "N7omnetpp3FooE"
    //   - omnetpp::inner::Foo -> "N7omnetpp5inner3FooE"
    //   - std::runtime_error -> "St13runtime_error"
    //   - Foo* -> "P3Foo" (P prefix means pointer)
    // http://theoryx5.uwinnipeg.ca/gnu/gcc/gxxint_15.html
    // http://www.codesourcery.com/cxx-abi/abi.html#mangling
    // libiberty/cp_demangle.c
    //
    if (*s>='0' && *s<='9')
    {
        // no namespace: just skip the leading number
        while (*s>='0' && *s<='9')
            s++;
        return s;
    }

    // if we've already seen this name, return cached result
    StringMap::const_iterator it = demangledNames.find(mangledName);
    if (it == demangledNames.end())
    {
        // not found -- demangle it and cache the result
        std::string result;
        std::string prefix, suffix;
        if (*s=='P' && *(s+1)=='K') {
            // PKx -> "const x *"
            prefix = "const ";
            suffix = " *";
            s += 2;
        }
        while (true) {
            if (*s=='P') suffix += " *";
            else if (*s=='K') suffix += " const";
            else break;
            s++;
        }

        switch (*s) {
            case 'v': result = "void"; break;
            case 'b': result = "bool"; break;
            case 's': result = "short"; break;
            case 't': result = "unsigned short"; break;
            case 'i': result = "int"; break;
            case 'j': result = "unsigned int"; break;
            case 'l': result = "long"; break;
            case 'm': result = "unsigned long"; break;
            case 'f': result = "float"; break;
            case 'd': result = "double"; break;
            case 'c': result = "char"; break;
            case 'a': result = "signed char"; break;
            case 'h': result = "unsigned char"; break;
            case 'N': {
                // mangled name contains namespace: decode it
                result.reserve(strlen(s)+8);
                s++; // skip leading 'N'
                while (*s>='0' && *s<='9') {
                    int len = (int)strtol(s, (char **)&s, 10);
                    if (!result.empty()) result += "::";
                    result.append(s, len);
                    s += len;
                }
                break;
            }
            case 'S': {
                // probably std::something, e.g. "St13runtime_error"
                switch (s[1]) {
                    // some "Sx" prefixes are special abbreviations
                    case 'a': result = "std::allocator"; break;
                    case 'b': result = "std::basic_string"; break;
                    case 's': result = "std::string"; break;
                    case 'i': result = "std::istream"; break;
                    case 'o': result = "std::ostream"; break;
                    case 'd': result = "std::iostream"; break;
                    case 't':
                        // "St" -> std::
                        s+=2;
                        result.reserve(strlen(s)+8);
                        result.append("std");
                        while (opp_isalpha(*s)) s++; // skip possible other modifiers
                        while (*s>='0' && *s<='9') {
                            int len = (int)strtol(s, (char **)&s, 10);
                            if (!result.empty()) result += "::";
                            result.append(s, len);
                            s += len;
                        }
                        break;
                }
                break;
            }
            default: {
                if (*s>='0' && *s<='9') {
                    // no namespace: just skip the leading number
                    while (*s>='0' && *s<='9')
                        s++;
                    result = s;
                }
                else {
                    // dunno how to interpret it, just return it unchanged
                    result = s;
                }
            }
        }

        demangledNames[mangledName] = prefix + result + suffix;
        it = demangledNames.find(mangledName);
    }
    return it->second.c_str();
#else
    // MSVC prepends the string with "class " (possibly other compilers too)
    if (s[0]=='c' && s[1]=='l' && s[2]=='a' && s[3]=='s' && s[4]=='s' && s[5]==' ')
        s+=6;
    else if (s[0]=='s' && s[1]=='t' && s[2]=='r' && s[3]=='u' && s[4]=='c' && s[5]=='t' && s[6]==' ')
        s+=7;
    else if (s[0]=='e' && s[1]=='n' && s[2]=='u' && s[3]=='m' && s[4]==' ')
        s+=5;
    return s;
#endif
}

//----

cContextSwitcher::cContextSwitcher(const cComponent *newContext)
{
    // save current context and switch to new
    callerContext = simulation.getContext();
    simulation.setContext(const_cast<cComponent *>(newContext));
}

cContextSwitcher::~cContextSwitcher()
{
    // restore old context
    if (!callerContext)
        simulation.setGlobalContext();
    else
        simulation.setContext(callerContext);
}

//----

static va_list dummy_va;

cMethodCallContextSwitcher::cMethodCallContextSwitcher(const cComponent *newContext) :
  cContextSwitcher(newContext)
{
}

void cMethodCallContextSwitcher::methodCall(const char *methodFmt,...)
{
    cComponent *newContext = simulation.getContext();
    if (newContext!=callerContext)
    {
        va_list va;
        va_start(va, methodFmt);
        EVCB.componentMethodBegin(callerContext, newContext, methodFmt, va, false);
        va_end(va);
    }
}

void cMethodCallContextSwitcher::methodCallSilent(const char *methodFmt,...)
{
    cComponent *newContext = simulation.getContext();
    if (newContext!=callerContext)
    {
        va_list va;
        va_start(va, methodFmt);
        EVCB.componentMethodBegin(callerContext, newContext, methodFmt, va, true);
        va_end(va);
    }
}

void cMethodCallContextSwitcher::methodCallSilent()
{
    cComponent *newContext = simulation.getContext();
    if (newContext!=callerContext)
        EVCB.componentMethodBegin(callerContext, newContext, NULL, dummy_va, true);
}

cMethodCallContextSwitcher::~cMethodCallContextSwitcher()
{
    cComponent *methodContext = simulation.getContext();
    if (methodContext!=callerContext)
        EVCB.componentMethodEnd();
}

//----

cContextTypeSwitcher::cContextTypeSwitcher(int contexttype)
{
    // save current context type and switch to new one
    savedcontexttype = simulation.getContextType();
    simulation.setContextType(contexttype);
}

cContextTypeSwitcher::~cContextTypeSwitcher()
{
    simulation.setContextType(savedcontexttype);
}

NAMESPACE_END

//----
// dummy function to force over-optimizing Unix linkers to include these symbols
// in the library

#include "cwatch.h"
#include "cstlwatch.h"
#include "clcg32.h"
#include "cmersennetwister.h"
#include "cksplit.h"
#include "cpsquare.h"
#include "cstringtokenizer.h"
#include "cxmlelement.h"
#include "cdelaychannel.h"
#include "cdataratechannel.h"
#include "cpacketqueue.h"
#include "cfsm.h"
#include "clinkedlist.h"
#include "coutvector.h"
#include "cvarhist.h"

NAMESPACE_BEGIN

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

//void _dummy_for_env();
void std_sim_descriptor_dummy();
void nedfunctions_dummy();
void _sim_dummy_func()
{
      bool bb;
      cWatch_bool w(NULL,bb);
      std::vector<int> v;
      WATCH_VECTOR(v);
      w.supportsAssignment();
      exponential(1.0);
      cSequentialScheduler sch;
      (void)sch;
      cLCG32 lcg;
      lcg.intRand();
      cMersenneTwister mt;
      mt.intRand();
      cKSplit ks;
      ks.info();
      cPSquare ps;
      ps.info();
      cStringTokenizer tok("");
      tok.nextToken();
      std_sim_descriptor_dummy();
      cXMLElement a(0,0,0);
      (void)a;
      cDelayChannel dc(NULL);
      (void)dc;
      cDatarateChannel c(NULL);
      (void)c;
      cPacketQueue pq;
      (void)pq;
      cFSM fsm;
      fsm.info();
      cLinkedList ll;
      ll.info();
      cOutVector ov;
      ov.info();
      cVarHistogram vh;
      vh.random();

      nedfunctions_dummy();
      //_dummy_for_env();
}

NAMESPACE_END

