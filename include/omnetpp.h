//==========================================================================
//   OMNETPP.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Includes all other headers
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_H
#define __OMNETPP_H

#include "defs.h"
#include "util.h"
#include "opp_string.h"
#include "random.h"
#include "distrib.h"
#include "cexception.h"
#include "errmsg.h"
#include "cobject.h"
#include "cdefaultlist.h"
#include "csimul.h"
#include "macros.h"
#include "ctypes.h"
#include "carray.h"
#include "cqueue.h"
#include "cllist.h"
#include "globals.h"
#include "cwatch.h"
#include "cpar.h"
#include "cgate.h"
#include "cmessage.h"
#include "cpacket.h"
#include "cmsgheap.h"
#include "cmodule.h"
#include "csimplemodule.h"
#include "cstat.h"
#include "cdensity.h"
#include "chist.h"
#include "cvarhist.h"
#include "cpsquare.h"
#include "cksplit.h"
#include "coutvect.h"
#include "cdetect.h"
#include "ctopo.h"
#include "cfsm.h"
#include "protocol.h"
#include "cenvir.h"
#include "cenum.h"
#include "cstruct.h"
#include "onstartup.h"
#include "cchannel.h"
#include "cdispstr.h"
#include "envirext.h"
#include "cscheduler.h"
#include "compat.h"
#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

#endif
