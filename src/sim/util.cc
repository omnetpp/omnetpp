//=========================================================================
//
//  UTIL.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Utility functions
//
//   Random number functions
//     randseed :
//     intrand  : random number in range 0..(2^31-1)
//     other random functions
//
//   Defines and registers functions:
//      uniform
//      exponential
//      normal
//      truncnormal
//
//   Author: Andras Varga
//           intrand(), exponential(), normal() by Gyorgy Pongor
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>   // isspace()
#include <math.h>
#include "util.h"
#include "csimul.h"  // simulation.error()
#include "macros.h"  // Define_Function()


// random number seeds:
static long seeds[NUM_RANDOM_GENERATORS];

// a hack to initialize all random number seeds to 1:
class init_generators {
   public: init_generators();
};
init_generators::init_generators() {
  for (int i=0; i<NUM_RANDOM_GENERATORS; i++)  seeds[i] = 1;
}
static init_generators InitGenerators;

void opp_randomize()
{
       randseed( time(NULL) );
}

void genk_opp_randomize(int gen_nr)
{
       genk_randseed( gen_nr, time(NULL) );
}

long randseed()
{
       return seeds[0];
}

long genk_randseed(int gen_nr)
{
       if (gen_nr<0 || gen_nr>=NUM_RANDOM_GENERATORS)
          {opp_error("Invalid random number generator %d",gen_nr);return 0;}
       return seeds[gen_nr];
}

long randseed(long seed)
{
       if (seed<=0 || seed>INTRAND_MAX)
          {opp_error("Invalid random number seed %ld",seed);return 0;}

       long res = seeds[0];
       seeds[0] = seed;
       return res;
}

long genk_randseed(int gen_nr, long seed)
{
       if (gen_nr<0 || gen_nr>=NUM_RANDOM_GENERATORS)
          {opp_error("Invalid random number generator %d",gen_nr);return 0;}
       if (seed<=0 || seed>INTRAND_MAX)
          {opp_error("Invalid random number seed %ld",seed);return 0;}

       long res = seeds[gen_nr];
       seeds[gen_nr] = seed;
       return res;
}

/*------- long int pseudorandom number generator -------------
*    Range:          1 ... 2**31-2  (INTRAND_MAX=2**31-2)
*    Period length:  2**31-2
*    Global variable used:   long int theSeed : seed
*    Method:  x=(x * 7**5) mod (2**31-1)
*    To check:  if  x[0]=1  then  x[10000]=1,043,618,065
*    Required hardware:  exactly 32-bit integer aritmetics
*    Source:  Raj Jain: The Art of Computer Systems Performance Analysis
*                (John Wiley & Sons, 1991)   Pages 441-444, 455
*---------------------------------------------------------*/
long intrand()
{
     const long int a=16807, q=127773, r=2836;
     seeds[0]=a*(seeds[0]%q) - r*(seeds[0]/q);
     if (seeds[0]<=0) seeds[0]+=INTRAND_MAX+1;
     return seeds[0];
}

long genk_intrand(int gen_nr)
{
     if (gen_nr<0 || gen_nr>=NUM_RANDOM_GENERATORS)
        {opp_error("Invalid random number generator %d",gen_nr);return 0;}

     const long int a=16807, q=127773, r=2836;
     long& seed = seeds[gen_nr];
     seed=a*(seed%q) - r*(seed/q);
     if (seed<=0) seed+=INTRAND_MAX+1;
     return seed;
}

int testrand()
{
     long oldseed = seeds[0];
     seeds[0] = 1;
     for(int i=0; i<10000; i++) intrand();
     int good = (seeds[0]==1043618065L);
     seeds[0] = oldseed;
     return good;
}

long intrand(long r)
{
     if (r>0)
         return intrand()%r;   // good if r<<MAX_LONG
     else if (r==0)
         opp_error("intrand(r) called with r=0 (cannot generate 0<=x<0 integer)");
     else
         opp_error("intrand(r) called with negative r argument");
     return 0L;
}

long genk_intrand(int gen_nr, long r)
{
     if (r>0)
         return genk_intrand(gen_nr)%r;   // good if r<<MAX_LONG
     else if (r==0)
         opp_error("genk_intrand(g,r) called with r=0 (cannot generate 0<=x<0 integer)");
     else
         opp_error("genk_intrand(g,r) called with negative r argument");
     return 0L;
}


//==========================================================================
// Distributions:

double uniform(double a, double b)
{
    return a + dblrand() * (b-a);
}

double intuniform(double a, double b)
{
    long a1 = (long)a,
         b1 = (long)b;
    return a1 + intrand() % (b1-a1+1);
}

double exponential(double p)
{
    return -p * log(dblrand());
}

double normal(double m, double d)
{
    return m + d *  sqrt(-2.0*log(dblrand()))*cos(PI*2*dblrand());
}

double truncnormal(double m, double d)
{
    double res;
    do {
         res = m + d *  sqrt(-2.0*log(dblrand()))*cos(PI*2*dblrand());
    } while(res<0);

    return res;
}

// register functions for findFunction()
Define_Function( uniform, 2 )
Define_Function( intuniform, 2 )
Define_Function( exponential, 1 )
Define_Function( normal, 2 )
Define_Function( truncnormal, 2 )

// Now their genk_ versions:

double genk_uniform(double gen_nr, double a, double b)
{
    return a + genk_dblrand((int)gen_nr) * (b-a);
}

double genk_intuniform(double gen_nr, double a, double b)
{
    long a1 = (long)a,
         b1 = (long)b;
    return a1 + genk_intrand((int)gen_nr) % (b1-a1+1);
}

double genk_exponential(double gen_nr, double p)
{
    return -p * log(genk_dblrand((int)gen_nr));
}

double genk_normal(double gen_nr, double m, double d)
{
    return m + d * sqrt(-2.0*log(genk_dblrand((int)gen_nr))) *
                   cos(PI*2*genk_dblrand((int)gen_nr));
}

double genk_truncnormal(double gen_nr, double m, double d)
{
    double res;
    do {
         res = m + d * sqrt(-2.0*log(genk_dblrand((int)gen_nr))) *
                       cos(PI*2*genk_dblrand((int)gen_nr));
    } while(res<0);

    return res;
}

// register functions for findFunction()
Define_Function( genk_uniform, 3 )
Define_Function( genk_intuniform, 3 )
Define_Function( genk_exponential, 2 )
Define_Function( genk_normal, 3 )
Define_Function( genk_truncnormal, 3 )

// functions to support expressions compiled by nedc
double min(double a, double b)      {return a<b ? a : b;}
double max(double a, double b)      {return a<b ? b : a;}
double bool_and(double a, double b) {return (a!=0) && (b!=0);}
double bool_or(double a, double b)  {return (a!=0) || (b!=0);}
double bool_xor(double a, double b) {return (a!=0) ^  (b!=0);}
double bool_not(double a)           {return a==0;}
double bin_and(double a, double b)  {return (unsigned long)a & (unsigned long)b;}
double bin_or(double a, double b)   {return (unsigned long)a | (unsigned long)b;}
double bin_xor(double a, double b)  {return (unsigned long)a ^ (unsigned long)b;}
double bin_compl(double a)          {return ~(unsigned long)a;}
double shift_left(double a, double b)   {return (unsigned long)a << (unsigned long)b;}
double shift_right(double a, double b)  {return (unsigned long)a >> (unsigned long)b;}

Define_Function( min,  2 )
Define_Function( max,  2 )
Define_Function( bool_and, 2 )
Define_Function( bool_or, 2 )
Define_Function( bool_xor, 2 )
Define_Function( bool_not, 1 )
Define_Function( bin_and, 2 )
Define_Function( bin_or, 2 )
Define_Function( bin_xor, 2 )
Define_Function( bin_compl, 1 )
Define_Function( shift_left, 2 )
Define_Function( shift_right, 2 )


//==========================================================================
//=== utility functions

char *simtimeToStr(simtime_t t, char *buf)
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
       sprintf(b,"%8.8le ( <1ns)", t);
   else if (t<1e-6)
       sprintf(b,"%8.8le (%3dns)", t,int(t*1e9));
   else if (t<1e-3)
       sprintf(b,"%8.8le (%3dus)", t,int(t*1e6));
   else if (t<1.0)
       sprintf(b,"%.10lf (%3dms)", t,int(t*1e3));
   else if (t<60.0)
       sprintf(b,"%8.8lg  (%2d.%2.2ds)", t,int(t*100)/100,int(t*100)%100);
   else if (t<3600.0)
       sprintf(b,"%8.8lg (%2dm %2ds)", t,int(t)/60, int(t)%60);
   else if (t<86400.0)
       sprintf(b,"%8.8lg (%2dh %2dm)", t,int(long(t)/3600L), int((long(t)%3600L)/60L));
   else if (t<8640000.0)
       sprintf(b,"%8.8lg (%2dd %2dh)", t,int(t/86400L), int((long(t)%86400L)/3600L));
   else
       sprintf(b,"%8.8le (%2lgd)", t,floor(t/86400L));

   return b;
}

// *
// * old version of simtimeToStr():
// *
// char *simtimeToStr(simtime_t t, char *dest)
// {                                // place result either into dest
//       static char buf[32];       //   or into a static buffer
//       if (dest==NULL) dest=buf;
//
//       if (t<600)  // < 10 minutes: 's ms us ns' format
//       {
//           double tt = (double)t + 0.5e-9; //rounding
//           int   s = (int) tt;   tt -= s;   tt *= 1000.0;
//           int  ms = (int) tt;   tt -= ms;  tt *= 1000.0;
//           int  us = (int) tt;   tt -= us;  tt *= 1000.0;
//           int  ns = (int) tt;
//           sprintf(dest,"%3ds %3dms %3dus %3dns", s,ms,us,ns); // 22 chars
//       }
//       else if (t<48L*3600L) // < 3 days: 'h m s ms' format
//       {
//           double tt = ((double)t+0.5e-3)/3600.0; //hours + rounding
//           int   h = (int) tt;   tt -= h;   tt *= 60.0;
//           int   m = (int) tt;   tt -= m;   tt *= 60.0;
//           int   s = (int) tt;   tt -= s;   tt *= 1000.0;
//           int  ms = (int) tt;
//           sprintf(dest,"%2dh %2dm %3ds %3dms", h,m,s,ms); // 18 chars
//       }
//       else // >= 3 days: 'd h m s' format
//       {
//           double tt = ((double)t+0.005)/3600.0/24.0; //days + rounding
//           long  d = (int) tt;   tt -= d;   tt *= 24.0;
//           int   h = (int) tt;   tt -= h;   tt *= 60.0;
//           int   m = (int) tt;   tt -= m;   tt *= 60.0;
//           int   s = (int) tt;   tt -= s;   tt *= 100.0;
//           int   s100 = (int) tt;
//           sprintf(dest,"%ldd %2dh %2dm %3d.%.2d", d,h,m,s,s100); // 18+ chars
//       }
//
//       return dest;
// }

simtime_t strToSimtime(const char *str)
{
    while (*str==' ' || *str=='\t') str++;
    if (*str=='\0') return -1; // empty string not accepted
    simtime_t d = strToSimtime0( str );
    return (*str=='\0') ? d : -1; // OK if whole string could be interpreted
}

simtime_t strToSimtime0(const char *&str)
{
    double simtime = 0;

    while (*str!='\0')
    {
          // read number into num and skip it
          double num;
          int len;
          while (*str==' ' || *str=='\t') str++;
          if (0==sscanf(str, "%f%n", &num, &len)) break; // break if error
          str+=len;

          // process time unit: d,h,m,s,ms,us,ns
          while (*str==' ' || *str=='\t') str++;
          if (str[0]=='n' && str[1]=='s')
                          {simtime += 1e-9*num; str+=2;}
          else if (str[0]=='u' && str[1]=='s')
                          {simtime += 1e-6*num; str+=2;}
          else if (str[0]=='m' && str[1]=='s')
                          {simtime += 1e-3*num; str+=2;}
          else if (str[0]=='s')
                          {simtime += num; str++;}
          else if (str[0]=='m')
                          {simtime += 60*num; str++;}
          else if (str[0]=='h')
                          {simtime += 3600*num; str++;}
          else if (str[0]=='d')
                          {simtime += 24*3600*num; str++;}
          else
                          {simtime += num; break;} // nothing can come after 'pure' number
    }
    return (simtime_t)simtime;
}

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

char *fastconcat(const char *s1,
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

char *indexedname(char *dest, const char *name, int index)
{
    static char buf[64];
    if (dest==NULL) dest=buf;
    sprintf(dest,"%.54s[%d]",name,index);
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
                k+=sscanf(s,"%u%n",va_arg(va,int*),&n);
                s+=n; fmt+=2;
            }
            else if (fmt[1]=='l' && fmt[2]=='d')
            {
                k+=sscanf(s,"%ld%n",va_arg(va,long*),&n);
                s+=n; fmt+=3;
            }
            else if (fmt[1]=='l' && fmt[2]=='u')
            {
                k+=sscanf(s,"%lu%n",va_arg(va,long*),&n);
                s+=n; fmt+=3;
            }
            else if (fmt[1]=='l' && fmt[2]=='g')
            {
                k+=sscanf(s,"%g%n",va_arg(va,double*),&n);
                s+=n; fmt+=3;
            }
            else
            {
                opp_error("opp_vsscanf: unsupported format '%s'",fmt);
                return k;
            }
        }
        else if (isspace(*fmt))
        {
            while (isspace(*s)) s++;
            fmt++;
        }
        else if (*fmt=='\0' || *fmt=='#')
        {
            return k;
        }
        else
        {
            opp_error("opp_vsscanf: unexpected char in format: '%s'",fmt);
            return k;
        }
    }
}

//==========================================================================

void opp_error(int errc...)
{
    va_list va;
    va_start(va, errc);
    char message[256];
    vsprintf(message,emsg[errc],va);
    va_end(va);

    simulation.error(errc,message);
}

void opp_error(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    char message[256];
    vsprintf(message,msgformat,va);
    va_end(va);

    simulation.error(eCUSTOM,message);
}

void opp_warning(int errc...)
{
    va_list va;
    va_start(va, errc);
    char message[256];
    vsprintf(message,emsg[errc],va);
    va_end(va);

    simulation.warning(errc,message);
}

void opp_warning(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    char message[256];
    vsprintf(message,msgformat,va);
    va_end(va);

    simulation.error(eCUSTOM,message);
}

void opp_terminate(int errc...)
{
    va_list va;
    va_start(va, errc);
    char message[256];
    vsprintf(message,emsg[errc],va);
    va_end(va);

    simulation.terminate(errc,message);
}

void opp_terminate(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    char message[256];
    vsprintf(message,msgformat,va);
    va_end(va);

    simulation.terminate(eCUSTOM,message);
}


//==========================================================================
// dummy function to force stupid Unix linkers to include these symbols
// in the library

#include "cwatch.h"
#include "cpacket.h"
//void _dummy_for_env();
static void _dummy_func()
{
      cPacket x;
      cPacket y(x);
      x.info(NULL);
      cWatch w(NULL,*(int*)NULL);
      w.pointer(); w.typeChar();
      //_dummy_for_env();
}
