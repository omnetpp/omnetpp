//==========================================================================
//  OMNETPP.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Includes all other headers
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

// make this a system header and supress warnings
#if defined(__clang__)
#pragma clang system_header
#elif defined(__GNUC__)
#pragma GCC system_header
#endif

#ifndef __OMNETPP_H
#define __OMNETPP_H

#include "omnetpp/carray.h"
#include "omnetpp/cboolparimpl.h"
#include "omnetpp/cobjectfactory.h"
#include "omnetpp/ccanvas.h"
#include "omnetpp/ccommbuffer.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cdelaychannel.h"
#include "omnetpp/cdataratechannel.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/ccoroutine.h"
#include "omnetpp/cdefaultlist.h"
#include "omnetpp/cabstracthistogram.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cdoubleparimpl.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/cenum.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cexpression.h"
#include "omnetpp/chasher.h"
#include "omnetpp/cfingerprint.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cfsm.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/cgate.h"
#include "omnetpp/chistogram.h"
#include "omnetpp/chistogramstrategy.h"
#include "omnetpp/cksplit.h"
#include "omnetpp/clcg32.h"
#include "omnetpp/clistener.h"
#include "omnetpp/clog.h"
#include "omnetpp/cintparimpl.h"
#include "omnetpp/cmersennetwister.h"
#include "omnetpp/simtime.h"
#include "omnetpp/simtimemath.h"
#include "omnetpp/simtime_t.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cmessageprinter.h"
#include "omnetpp/cmsgpar.h"
#include "omnetpp/cmodelchange.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/ceventheap.h"
#include "omnetpp/cmatchexpression.h"
#include "omnetpp/cpatternmatcher.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cnedvalue.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cobject.h"
#include "omnetpp/cnamedobject.h"
#include "omnetpp/cnullenvir.h"
#include "omnetpp/cosgcanvas.h"
#include "omnetpp/cownedobject.h"
#include "omnetpp/coutvector.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cparimpl.h"
#include "omnetpp/cparsimcomm.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/cpsquare.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/cpacketqueue.h"
#include "omnetpp/cprecolldensityest.h"
#include "omnetpp/crandom.h"
#include "omnetpp/cresultfilter.h"
#include "omnetpp/cresultlistener.h"
#include "omnetpp/cresultrecorder.h"
#include "omnetpp/crng.h"
#include "omnetpp/cscheduler.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/cstddev.h"
#include "omnetpp/cstlwatch.h"
#include "omnetpp/cstringparimpl.h"
#include "omnetpp/cstringpool.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/cclassdescriptor.h"
#include "omnetpp/ctimestampedvalue.h"
#include "omnetpp/ctopology.h"
#include "omnetpp/cvarhist.h"
#include "omnetpp/cvisitor.h"
#include "omnetpp/cwatch.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cxmlparimpl.h"
#include "omnetpp/simkerneldefs.h"
#include "omnetpp/distrib.h"
#include "omnetpp/envirext.h"
#include "omnetpp/errmsg.h"
#include "omnetpp/globals.h"
#include "omnetpp/index.h"
#include "omnetpp/mersennetwister.h"
#include "omnetpp/onstartup.h"
#include "omnetpp/opp_string.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/simutil.h"
#include "omnetpp/nedsupport.h"
#include "omnetpp/packing.h"
#include "omnetpp/compat.h"
#include "omnetpp/platdep/intlimits.h"
#include "omnetpp/platdep/timeutil.h"

#ifdef AUTOIMPORT_OMNETPP_NAMESPACE
using namespace omnetpp;
#endif

#endif


