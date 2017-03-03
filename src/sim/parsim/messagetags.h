//=========================================================================
//  MESSAGETAGS.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2003
//          Dept. of Electrical and Computer Systems Engineering,
//          Monash University, Melbourne, Australia
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_MESSAGETAGS_H
#define __OMNETPP_MESSAGETAGS_H

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
