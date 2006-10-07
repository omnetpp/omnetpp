//=========================================================================
//  MESSAGETAGS.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MESSAGETAGS_H
#define __MESSAGETAGS_H

//
// message tags:
//
enum {
     TAG_SETUP_LINKS = 1000,
     TAG_RUNNUMBER,
     TAG_CMESSAGE,
     TAG_NULLMESSAGE,
     TAG_CMESSAGE_WITH_NULLMESSAGE,
     TAG_TERMINATIONEXCEPTION,
     TAG_EXCEPTION
};

#endif
