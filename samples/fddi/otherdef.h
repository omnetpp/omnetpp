//==========================================================================
//  OTHERDEF.H  -  Definitions for
//                 Discrete System Simulation in OMNeT++
//
//  Simulated system: TUB Northern&Southern FDDI Rings
//
//  Author: Gabor.Lencse@hit.bme.hu
//==========================================================================

//--------------------------------------------------------------*
//  Copyright (C) 1996,97 Gabor Lencse,
//  Technical University of Budapest, Dept. of Telecommunications,
//  Stoczek u.2, H-1111 Budapest, Hungary.
//
//  This file is distributed WITHOUT ANY WARRANTY. See the file
//  `license' for details on this and other legal matters.
//--------------------------------------------------------------*/

#ifndef __OTHERDEF_H
#define __OTHERDEF_H


#define BOOL int

#if defined(__BORLANDC__) && __BCPLUSPLUS__<=0x310 && defined(__MSDOS__)
  #define GENERATOR_HEAPSIZE   2048
  #define STAT_HEAPSIZE        2048
  #define SINK_HEAPSIZE        1024
  #define MONITOR_HEAPSIZE     2048
  #define LOADCONTROL_HEAPSIZE 1024
#else
  #define GENERATOR_HEAPSIZE   16384
  #define STAT_HEAPSIZE        16384
  #define SINK_HEAPSIZE        16384
  #define MONITOR_HEAPSIZE     16384
  #define LOADCONTROL_HEAPSIZE 16384
#endif

#define LOAD_CONTROL_SELFMSG 95 /* LoadControl self message */

// RingID to prefix translation
#define RING_0_PREFIX "nr"
#define RING_1_PREFIX "sr"

#endif
