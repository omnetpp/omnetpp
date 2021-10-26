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

#ifndef __OMNETPP_H
#define __OMNETPP_H

// make this a system header and suppress warnings
#if defined(__clang__)
#pragma clang system_header
#elif defined(__GNUC__)
#pragma GCC system_header
#endif

#include "omnetpp/simkerneldefs.h"
#include "omnetpp/cobject.h"
#include "omnetpp/cabstracthistogram.h"
#include "omnetpp/carray.h"
#include "omnetpp/cboolparimpl.h"
#include "omnetpp/ccanvas.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cclassdescriptor.h"
#include "omnetpp/ccommbuffer.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/ccontextswitcher.h"
#include "omnetpp/ccoroutine.h"
#include "omnetpp/cdataratechannel.h"
#include "omnetpp/csoftowner.h"
#include "omnetpp/cdelaychannel.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cdoubleparimpl.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/cenum.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/ceventheap.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cexpression.h"
#include "omnetpp/cfingerprint.h"
#include "omnetpp/cfsm.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/cgate.h"
#include "omnetpp/chasher.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/chistogram.h"
#include "omnetpp/chistogramstrategy.h"
#include "omnetpp/cintparimpl.h"
#include "omnetpp/cksplit.h"
#include "omnetpp/clcg32.h"
#include "omnetpp/clistener.h"
#include "omnetpp/clog.h"
#include "omnetpp/cmatchexpression.h"
#include "omnetpp/cmersennetwister.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cmessageprinter.h"
#include "omnetpp/cmodelchange.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cmsgpar.h"
#include "omnetpp/cnamedobject.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnullenvir.h"
#include "omnetpp/cobjectfactory.h"
#include "omnetpp/compat.h"
#include "omnetpp/cosgcanvas.h"
#include "omnetpp/coutvector.h"
#include "omnetpp/cownedobject.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/cpacketqueue.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cparimpl.h"
#include "omnetpp/cparsimcomm.h"
#include "omnetpp/cpatternmatcher.h"
#include "omnetpp/cprecolldensityest.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/cpsquare.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/crandom.h"
#include "omnetpp/cresultfilter.h"
#include "omnetpp/cresultlistener.h"
#include "omnetpp/cresultrecorder.h"
#include "omnetpp/crng.h"
#include "omnetpp/cscheduler.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/cstatisticbuilder.h"
#include "omnetpp/cstddev.h"
#include "omnetpp/cstlwatch.h"
#include "omnetpp/cstringparimpl.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/ctimestampedvalue.h"
#include "omnetpp/ctopology.h"
#include "omnetpp/cvalue.h"
#include "omnetpp/cvaluearray.h"
#include "omnetpp/cvaluemap.h"
#include "omnetpp/cvalueholder.h"
#include "omnetpp/cvisitor.h"
#include "omnetpp/cwatch.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cxmlparimpl.h"
#include "omnetpp/distrib.h"
#include "omnetpp/envirext.h"
#include "omnetpp/errmsg.h"
#include "omnetpp/globals.h"
#include "omnetpp/index.h"
#include "omnetpp/mersennetwister.h"
#include "omnetpp/onstartup.h"
#include "omnetpp/opp_component_ptr.h"
#include "omnetpp/opp_string.h"
#include "omnetpp/opp_pooledstring.h"
#include "omnetpp/packing.h"
#include "omnetpp/platdep/intlimits.h"
#include "omnetpp/platdep/timeutil.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/resultfilters.h"
#include "omnetpp/resultrecorders.h"
#include "omnetpp/simtime_t.h"
#include "omnetpp/simtime.h"
#include "omnetpp/simtimemath.h"
#include "omnetpp/simutil.h"
#include "omnetpp/stringutil.h"
#include "omnetpp/sim_std_m.h"

#ifdef AUTOIMPORT_OMNETPP_NAMESPACE
using namespace omnetpp;
#endif

#endif


