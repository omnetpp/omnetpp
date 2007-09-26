//=========================================================================
//  SCAVEUTILS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _SCAVEUTILS_H_
#define _SCAVEUTILS_H_

#include <string>
#include "scavedefs.h"

extern double dblNaN;
extern double dblPositiveInfinity;
extern double dblNegativeInfinity;

inline bool isNaN(double d) { return d != d;}
inline bool isPositiveInfinity(double d) { return d==dblPositiveInfinity; }
inline bool isNegativeInfinity(double d) { return d==dblNegativeInfinity; }

bool parseInt(const char *str, int &dest);
bool parseLong(const char *str, long &dest);
bool parseDouble(const char *str, double &dest);
bool parseSimtime(const char *str, simultime_t &dest);
std::string unquoteString(const char *str);
#endif

