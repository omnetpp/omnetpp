//==========================================================================
// nedutil.cc - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   misc util functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "nederror.h"
#include "nedutil.h"


char *NEDSimtimeToStr(double t, char *buf)
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


double NEDStrToSimtime(const char *str)
{
    while (*str==' ' || *str=='\t') str++;
    if (*str=='\0') return -1; // empty string not accepted
    double d = NEDStrToSimtime0( str );
    return (*str=='\0') ? d : -1; // OK if whole string could be interpreted
}

double NEDStrToSimtime0(const char *&str)
{
    double simtime = 0;

    while (*str!='\0')
    {
          // read number into num and skip it
          double num;
          int len;
          while (*str==' ' || *str=='\t') str++;
          if (0==sscanf(str, "%lf%n", &num, &len)) break; // break if error
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
    return simtime;
}

