//==========================================================================
//  CHECKANDCAST.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CHECKANDCAST_H
#define __OMNETPP_CHECKANDCAST_H

#include <typeinfo>
#include "simkerneldefs.h"
#include "cownedobject.h"
#include "cexception.h"
#include "cmodule.h"
#include "csimulation.h"

NAMESPACE_BEGIN

template<class P, class T>
void check_and_cast_failure(T *p, P ret)
{
    const cObject *o = dynamic_cast<const cObject *>(p);
    if (o)
        throw cRuntimeError("check_and_cast(): cannot cast (%s*)%s to type '%s'",
                o->getClassName(),
                o->getOwner()==simulation.getContextModule() ? o->getFullName() : o->getFullPath().c_str(),
                opp_typename(typeid(P)));
    else
        throw cRuntimeError("check_and_cast(): cannot cast '%s*' to type '%s'",
                opp_typename(typeid(T)),
                opp_typename(typeid(P)));
}

/**
 * Cast a pointer to the given pointer type P, and throw exception if fails.
 * The method calls dynamic_cast\<P\>(p) where P is a type you supplied;
 * if the result is NULL (which indicates incompatible types), an exception
 * is thrown.
 *
 * In the following example, DHCPPacket is a subclass of cMessage, and we want
 * to assert that the message received is actually a DHCPPacket. If not,
 * the simulation stops with an error message as the result of the exception.
 * <pre>
 *   cMessage *msg = ...;
 *   DHCPPacket *pk = check_and_cast\<DHCPPacket *\>(msg);
 * </pre>
 *
 * @ingroup Functions
 */
template<class P, class T>
P check_and_cast(T *p)
{
    if (!p)
        throw cRuntimeError("check_and_cast(): cannot cast NULL pointer to type '%s'",opp_typename(typeid(P)));
    P ret = dynamic_cast<P>(p);
    if (!ret)
        check_and_cast_failure(p, ret);
    return ret;
}

/**
 * A variant of check_and_cast\<\>() that also allows NULL pointer as input.
 *
 * @ingroup Functions
 */
template<class P, class T>
P check_and_cast_nullable(T *p)
{
    if (!p)
        return NULL;
    P ret = dynamic_cast<P>(p);
    if (!ret)
        check_and_cast_failure(p, ret);
    return ret;
}

NAMESPACE_END

#endif

