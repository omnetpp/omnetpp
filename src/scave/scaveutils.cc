//=========================================================================
//  INDEXFILE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

#include <stdlib.h>
#include <string.h>
#include "scaveutils.h"

static double zero =0;

bool parseInt(const char *s, int &dest)
{
    char *e;
    dest = (int)strtol(s,&e,10);
    return !*e;
}

bool parseLong(const char *s, long &dest)
{
    char *e;
    dest = strtol(s,&e,10);
    return !*e;
}

bool parseDouble(const char *s, double& dest)
{
    char *e;
    dest = strtod(s,&e);
    if (!*e)
    {
        return true;
    }
    if (strstr(s,"INF") || strstr(s, "inf"))
    {
        dest = 1/zero;  // +INF or -INF
        if (*s=='-') dest = -dest;
        return true;
    }
    return false;
}
