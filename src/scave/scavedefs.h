//=========================================================================
//  SCAVEDEFS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _SCAVEDEFS_H_
#define _SCAVEDEFS_H_

// define int64, our equivalent of Java's "long" type
#ifdef _MSC_VER
typedef __int64 int64;
#else
typedef long long int64;
#endif

#define DEFAULT_PRECISION  12

#endif


