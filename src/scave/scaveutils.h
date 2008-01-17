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

NAMESPACE_BEGIN

SCAVE_API extern double dblNaN;
SCAVE_API extern double dblPositiveInfinity;
SCAVE_API extern double dblNegativeInfinity;

inline bool isNaN(double d) { return d != d;}
inline bool isPositiveInfinity(double d) { return d==dblPositiveInfinity; }
inline bool isNegativeInfinity(double d) { return d==dblNegativeInfinity; }

SCAVE_API bool parseInt(const char *str, int &dest);
SCAVE_API bool parseLong(const char *str, long &dest);
SCAVE_API bool parseDouble(const char *str, double &dest);
SCAVE_API bool parseSimtime(const char *str, simultime_t &dest);
SCAVE_API std::string unquoteString(const char *str);

NAMESPACE_END


#endif

