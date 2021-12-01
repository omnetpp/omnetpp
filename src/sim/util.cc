//=========================================================================
//  UTIL.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Utility functions
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctime>
#include <cstdio>
#include <cstring>
#include <cmath>
#include "common/commonutil.h"
#include "common/unitconversion.h"
#include "common/opp_ctype.h"
#include "omnetpp/simutil.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/globals.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cscheduler.h"  // dummy()
#include "omnetpp/distrib.h"  // dummy()

using namespace omnetpp::common;

namespace omnetpp {

void intCastError(const std::string& num, const char *errmsg)
{
    throw cRuntimeError(errmsg ? errmsg : "Overflow casting %s to the target integer type", num.c_str());
}

void intCastError(const std::string& num, const cObject *context, const char *errmsg)
{
    throw cRuntimeError(context, errmsg ? errmsg : "Overflow casting %s to the target integer type", num.c_str());
}


const char *opp_typename(const std::type_info& t)
{
    return common::opp_typename(t);
}

}  // namespace omnetpp

//----
// dummy function to force over-optimizing Unix linkers to include these symbols
// in the library

#include "omnetpp/cwatch.h"
#include "omnetpp/cstlwatch.h"
#include "omnetpp/clcg32.h"
#include "omnetpp/cmersennetwister.h"
#include "omnetpp/chistogram.h"
#include "omnetpp/cksplit.h"
#include "omnetpp/cpsquare.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cdelaychannel.h"
#include "omnetpp/cdataratechannel.h"
#include "omnetpp/cpacketqueue.h"
#include "omnetpp/cfsm.h"
#include "omnetpp/coutvector.h"
#include "omnetpp/cfingerprint.h"

using namespace omnetpp::common;

namespace omnetpp {

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

//void _dummy_for_env();
void std_sim_descriptor_dummy();
void nedfunctions_dummy();
void _sim_dummy_func()
{
    bool bb = false;
    cRNG *rng = nullptr;
    cWatch_bool w(nullptr, bb);
    std::vector<int> v;
    WATCH_VECTOR(v);
    w.supportsAssignment();
    exponential(rng, 1.0);
    cSequentialScheduler sch;
    (void)sch;
    cLCG32 lcg;
    lcg.intRand();
    cMersenneTwister mt;
    mt.intRand();
    cKSplit ks;
    ks.str();
    cPSquare ps;
    ps.str();
    cStringTokenizer tok("");
    tok.nextToken();
    std_sim_descriptor_dummy();
    cXMLElement a(nullptr, nullptr);
    (void)a;
    cDelayChannel dc(nullptr);
    (void)dc;
    cDatarateChannel c(nullptr);
    (void)c;
    cPacketQueue pq;
    (void)pq;
    cFSM fsm;
    fsm.str();
    cOutVector ov;
    ov.str();
    cHistogram h;
    h.draw();

    nedfunctions_dummy();
    //_dummy_for_env();
}

}  // namespace omnetpp

