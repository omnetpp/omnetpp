//=========================================================================
//  UTIL.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Utility functions
//
//   Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "opp_ctype.h"
#include "util.h"
#include "cenvir.h"
#include "csimulation.h"
#include "globals.h"
#include "cexception.h"
#include "cfunction.h"
#include "cscheduler.h" // dummy()
#include "distrib.h" // dummy()

NAMESPACE_BEGIN

// functions to support expressions compiled by nedtool

double min(double a, double b)      {return a<b ? a : b;}
double max(double a, double b)      {return a<b ? b : a;}

Define_Function( min,  2 )
Define_Function( max,  2 )


//----
// utility functions

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
    //XXX in 3.x, it returned -1 on error, now it throws exception: TBD check invocations!!!!
    return UnitConversion::parseQuantity(str, "s");
}

//
// This function is legacy from OMNeT++ 3.x, and it is only used if simtime_t
// is double. It will be removed in the future.
//
double strToSimtime0(const char *&str)
{
    //XXX in 3.x, it returned -1 on error, now it throws exception: TBD check invocations!!!!
    const char *end = str;
    while (opp_isspace(*end))
        end++;
    if (!*end)
        return 0.0; // it was just space

    while (opp_isalnum(*end) || opp_isspace(*end) || *end=='+' || *end=='-' || *end=='.')
        end++;
    std::string tmp(str, end-str);
    str = end;
    return UnitConversion::parseQuantity(tmp.c_str(), "s");
}
#endif //USE_DOUBLE_SIMTIME

char *opp_strdup(const char *s)
{
    if (!s || !s[0]) return NULL;
    char *p = new char[ strlen(s)+1 ];
    if (p) strcpy(p,s);
    return p;
}

char *opp_strcpy(char *s1, const char *s2)
{
    if (s1 && s2)       return strcpy(s1,s2);
    else if (!s1 && s2) return NULL;
    else {s1[0]='\x0';  return s1;}
}

int opp_strcmp(const char *s1, const char *s2)
{
    // case-sensitive comparison
    if (s1 && s2)       return strcmp(s1,s2);
    else if (!s1 && s2) return -1;
    else if (s1 && !s2) return 1;
    else                return 0;
}

bool opp_strmatch(const char *s1, const char *s2)
{
    // case-sensitive comparison until
    // the length of the shorter string
    if (!s1 || !s2) {
         return true;
    } else {
         int l = Min(strlen(s1), strlen(s2));
         return strncmp( s1, s1, l)==0;
    }
}

int opp_strlen(const char *s)
{
    if (!s) return 0;
    return strlen(s);
}

char *opp_concat(const char *s1,
                 const char *s2,
                 const char *s3,
                 const char *s4)
{
    // concatenate strings into static buffer
    static char buf[256];
    char *dest=buf;
    if (s1) while( *s1 && dest!=buf+255 ) *dest++ = *s1++;
    if (s2) while( *s2 && dest!=buf+255 ) *dest++ = *s2++;
    if (s3) while( *s3 && dest!=buf+255 ) *dest++ = *s3++;
    if (s4) while( *s4 && dest!=buf+255 ) *dest++ = *s4++;
    *dest = 0;
    return buf;
}

char *opp_mkindexedname(char *dest, const char *name, int index)
{
    static char buf[64];
    if (dest==NULL) dest=buf;
    sprintf(dest,"%.54s[%d]",name,index);
    return dest;
}

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

int opp_vsscanf(const char *s, const char *fmt, va_list va)
{
    // A simplified vsscanf implementation, solely for cStatistic::freadvarsf.
    // Only recognizes %d, %u, %ld, %g and whitespace. '#' terminates scanning

    int k=0;
    while (1)
    {
        if (*fmt=='%')
        {
            int n;
            if (fmt[1]=='d')
            {
                k+=sscanf(s,"%d%n",va_arg(va,int*),&n);
                s+=n; fmt+=2;
            }
            else if (fmt[1]=='u')
            {
                k+=sscanf(s,"%u%n",va_arg(va,unsigned int*),&n);
                s+=n; fmt+=2;
            }
            else if (fmt[1]=='l' && fmt[2]=='d')
            {
                k+=sscanf(s,"%ld%n",va_arg(va,long*),&n);
                s+=n; fmt+=3;
            }
            else if (fmt[1]=='l' && fmt[2]=='u')
            {
                k+=sscanf(s,"%lu%n",va_arg(va,unsigned long*),&n);
                s+=n; fmt+=3;
            }
            else if (fmt[1]=='l' && fmt[2]=='g')
            {
                k+=sscanf(s,"%lg%n",va_arg(va,double*),&n);
                s+=n; fmt+=3;
            }
            else if (fmt[1]=='g')
            {
                k+=sscanf(s,"%lg%n",va_arg(va,double*),&n);
                s+=n; fmt+=2;
            }
            else
            {
                throw cRuntimeError("opp_vsscanf: unsupported format '%s'",fmt);
            }
        }
        else if (opp_isspace(*fmt))
        {
            while (opp_isspace(*s)) s++;
            fmt++;
        }
        else if (*fmt=='\0' || *fmt=='#')
        {
            return k;
        }
        else
        {
            throw cRuntimeError("opp_vsscanf: unexpected char in format: '%s'",fmt);
        }
    }
}

//----

void opp_error(ErrorCode errorcode...)
{
    va_list va;
    va_start(va, errorcode);
    char message[512];
    vsprintf(message, cErrorMessages::get(errorcode), va);
    va_end(va);

    throw cRuntimeError(errorcode, message);
}

void opp_error(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    char message[512];
    vsprintf(message, msgformat, va);
    va_end(va);

    throw cRuntimeError(eUSER, message);
}

void opp_warning(ErrorCode errorcode...)
{
    va_list va;
    va_start(va, errorcode);
    char message[512];
    vsprintf(message, cErrorMessages::get(errorcode),va);
    va_end(va);

    if (!simulation.contextModule())
    {
        // we're called from global context
        ev.printfmsg("%s.", message);
    }
    else
    {
        ev.printfmsg("Module %s: %s.", simulation.contextModule()->fullPath().c_str(), message);
    }
}

void opp_warning(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    char message[512];
    vsprintf(message,msgformat,va);
    va_end(va);

    if (!simulation.contextModule())
    {
        // we're called from global context
        ev.printfmsg("%s.", message);
    }
    else
    {
        ev.printfmsg("Module %s: %s.", simulation.contextModule()->fullPath().c_str(), message);
    }
}

void opp_terminate(ErrorCode errorcode...)
{
    va_list va;
    va_start(va, errorcode);
    char message[512];
    vsprintf(message, cErrorMessages::get(errorcode),va);
    va_end(va);

    throw cTerminationException(errorcode,message);
}

void opp_terminate(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    char message[512];
    vsprintf(message,msgformat,va);
    va_end(va);

    throw cTerminationException(message);
}

//----

#ifdef __GNUC__
typedef std::map<std::string,std::string> StringMap;
static StringMap demangledNames;
#endif

const char *opp_typename(const std::type_info& t)
{
    if (t == typeid(std::string))
        return "std::string"; // otherwise we'd get "std::basic_string<........>"

    const char *s = t.name();

#ifdef __GNUC__
    // gcc's typeinfo returns mangled name:
    //   - Foo -> "3Foo"
    //   - omnetpp::Foo -> "N7omnetpp3FooE"
    //   - omnetpp::inner::Foo -> "N7omnetpp5inner3FooE"
    //
    if (*s>='0' && *s<='9') 
    {
        // no namespace: just skip the leading number
        while (*s>='0' && *s<='9') 
            s++;
        return s;
    }
    else if (*s=='N') 
    {
        // mangled name contains namespace: decode it and cache the result
        const char *mangledName = s;
        StringMap::const_iterator it = demangledNames.find(mangledName);
        if (it == demangledNames.end())
        {
            std::string result;
            result.reserve(strlen(s)+8);
            s++; // skip leading 'N'
            while (*s>='0' && *s<='9') {
                int len = (int)strtol(s, (char **)&s, 10);
                if (!result.empty()) result += "::";
                result.append(s, len);
                s += len;
            }
            demangledNames[mangledName] = result;
            it = demangledNames.find(mangledName);
        }
        return it->second.c_str();
    }
    else {
        // dunno how to interpret it, just return it unchanged
        return s;
    }
#else
    // MSVC prepends the string with "class " (possibly other compilers too)
    if (s[0]=='c' && s[1]=='l' && s[2]=='a' && s[3]=='s' && s[4]=='s' && s[5]==' ') 
        s+=6;
    return s;
#endif
}

//----

cContextSwitcher::cContextSwitcher(cComponent *newContext)
{
    // save current context and switch to new
    callerContext = simulation.context();
    simulation.setContext(newContext);
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

cMethodCallContextSwitcher::cMethodCallContextSwitcher(cComponent *newContext, bool notifyEnvir) :
  cContextSwitcher(newContext)
{
    if (notifyEnvir)
        EVCB.componentMethodBegin(callerContext, newContext, "");
}

void cMethodCallContextSwitcher::methodCall(const char *fmt,...)
{
    cComponent *methodContext = simulation.context();
    if (methodContext!=callerContext)
    {
        static char buf[MAX_METHODCALL];
        va_list va;
        va_start(va, fmt);
        vsprintf(buf,fmt,va);
        va_end(va);

        EVCB.componentMethodBegin(callerContext, methodContext, buf);
    }
}

cMethodCallContextSwitcher::~cMethodCallContextSwitcher()
{
    cComponent *methodContext = simulation.context();
    if (methodContext!=callerContext)
        EVCB.componentMethodEnd();
}

//----

cContextTypeSwitcher::cContextTypeSwitcher(int ctxtype)
{
    // save current context type and switch to new one
    contexttype = simulation.contextType();
    simulation.setContextType(ctxtype);
}

cContextTypeSwitcher::~cContextTypeSwitcher()
{
    simulation.setContextType(contexttype);
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
#include "cstrtokenizer.h"
#include "cxmlelement.h"
#include "cbasicchannel.h"

USING_NAMESPACE

//void _dummy_for_env();
void std_sim_descriptor_dummy();
void _sim_dummy_func()
{
      bool bb;
      cWatch_bool w(NULL,bb);
      std::vector<int> v;
      WATCH_VECTOR(v);
      w.supportsAssignment();
      exponential(1.0);
      cSequentialScheduler sch;
      sch.setSimulation(NULL);
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
      cBasicChannel c(NULL);
      //_dummy_for_env();
}

