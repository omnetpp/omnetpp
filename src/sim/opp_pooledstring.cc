//==========================================================================
//  OPP_POOLEDSTRING.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/opp_pooledstring.h"
#include "common/pooledstring.h"

using namespace omnetpp::common;

namespace omnetpp {

static StaticStringPool& staticPool = common::opp_staticpooledstring::pool;

opp_staticpooledstring::opp_staticpooledstring()
{
    p = staticPool.EMPTY_STRING;
}

opp_staticpooledstring::opp_staticpooledstring(const char *s)
{
    p = staticPool.get(s);
}

const char *opp_staticpooledstring::get(const char *s)
{
    return staticPool.get(s);
}

//---

static StringPool& pool = common::opp_pooledstring::pool;

opp_pooledstring::opp_pooledstring()
{
    p = pool.EMPTY_STRING;
}

opp_pooledstring::opp_pooledstring(const char *s)
{
    p = pool.obtain(s);
}

opp_pooledstring::~opp_pooledstring()
{
    if (p)
        pool.release(p);
}

opp_pooledstring& opp_pooledstring::operator=(const opp_pooledstring& ps)
{
    if (p != ps.p) {
        pool.release(p);
        p = pool.obtain(ps.p);
    }
    return *this;
}

opp_pooledstring& opp_pooledstring::operator=(opp_pooledstring&& ps)
{
    if (ps.p != p) {
        pool.release(p);
        p = ps.p;
        ps.p = nullptr;
    }
    return *this;
}

}  // namespace omnetpp

