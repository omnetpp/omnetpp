//==========================================================================
//  CCONTEXTSWITCHER.H - part of
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

#ifndef __OMNETPP_CCONTEXTSWITCHER_H
#define __OMNETPP_CCONTEXTSWITCHER_H

#include "simkerneldefs.h"
#include "csimulation.h"

namespace omnetpp {

class cObject;
class cComponent;

/**
 * @brief Denotes module class member function as callable from other modules.
 *
 * Usage: <tt>Enter_Method(fmt, arg1, arg2...);</tt>
 *
 * Example: <tt>Enter_Method("requestPacket(%d)",n);</tt>
 *
 * The macro should be put at the top of every module member function
 * that may be called from other modules. This macro arranges to
 * temporarily switch the context to the called module (the old context
 * will be restored automatically when the method returns),
 * and also lets the graphical user interface animate the method call.
 *
 * The argument(s) should specify the method name (and parameters) --
 * it will be used for the animation. The argument list works as in
 * <tt>printf()</tt>, so it is easy to include the actual parameter values.
 *
 * @see Enter_Method_Silent() macro
 * @ingroup Misc
 * @hideinitializer
 */
#define Enter_Method(...)  omnetpp::cMethodCallContextSwitcher __ctx(this); __ctx.methodCall(__VA_ARGS__)

/**
 * @brief Denotes module class member function as callable from other modules.
 *
 * This macro is similar to the Enter_Method() macro, only it does not animate
 * the call on the GUI; the call is still recorded into the the event log file.
 *
 * The macro may be called with or without arguments. When called with arguments,
 * they should be a printf-style format string, and parameters to be substituted
 * into it; the resulting string should contain the method name and the actual
 * arguments.
 *
 * Usage: <tt>Enter_Method_Silent();</tt>, <tt>Enter_Method_Silent(fmt, arg1, arg2...);</tt>
 *
 * Example: <tt>Enter_Method_Silent("getRouteFor(address=%d)",address);</tt>
 *
 * @see Enter_Method() macro
 * @ingroup Misc
 * @hideinitializer
 */
#define Enter_Method_Silent(...)  omnetpp::cMethodCallContextSwitcher __ctx(this); __ctx.methodCallSilent(__VA_ARGS__)

/**
 * @brief The constructor switches the context to the given component, and the
 * destructor restores the original context.
 *
 * @see cSimulation::getContextModule(), cSimulation::setContextModule()
 * @ingroup Internals
 */
class SIM_API cContextSwitcher
{
  protected:
    cSimulation *simulation;
    cComponent *callerContext;

  public:
    /**
     * Switches context to the given module
     */
    cContextSwitcher(const cComponent *newContext);

    /**
     * Restores the original context
     */
    ~cContextSwitcher();

    /**
     * Returns the original context
     */
    cComponent *getCallerContext() const { return callerContext; }
};

/**
 * @brief Internal class. May only be used via the Enter_Method() and Enter_Method_Silent() macros!
 * @ingroup Internals
 */
class SIM_API cMethodCallContextSwitcher : public cContextSwitcher
{
  private:
    static OPP_THREAD_LOCAL int depth;

  public:
    /**
     * Switches context to the given module
     */
    cMethodCallContextSwitcher(const cComponent *newContext);

    /**
     * Restores the original context
     */
    ~cMethodCallContextSwitcher();

    /**
     * Various ways to tell the user interface about the method call so that
     * the call can be animated, recorded into the event log, etc.
     */
    _OPP_GNU_ATTRIBUTE(format(printf, 2, 3))
    void methodCall(const char *methodFmt,...);
    _OPP_GNU_ATTRIBUTE(format(printf, 2, 3))
    void methodCallSilent(const char *methodFm,...);
    void methodCallSilent();

    /**
     * Returns the depth of Enter_Method[_Silent] calls
     */
    static int getDepth() {return depth;}
};

}  // namespace omnetpp

#endif


