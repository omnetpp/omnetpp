//==========================================================================
//   CFSM.H  -  header for
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

#ifndef __OMNETPP_CFSM_H
#define __OMNETPP_CFSM_H

#include "cownedobject.h"

namespace omnetpp {

/**
 * @addtogroup FSM
 * @{
 */

/**
 * @brief After this many transitions without reaching a steady state
 * we assume the FSM is in an infinite loop.
 */
#define FSM_MAXT  64

/**
 * @brief Implements a Finite State Machine. FSM state is stored in
 * an object of type cFSM.
 *
 * There are two kinds of states: transient and steady. At each
 * execution of the FSM_Switch() statement, the FSM transitions out
 * of the current (steady) state, potentially undergoes a series of
 * state changes to transient states, and arrives at another
 * steady state.
 *
 * The actual FSM is embedded in an FSM_Switch(), which has cases for
 * entering and leaving each state:
 *
 * ```
 * FSM_Switch(fsm)
 * {
 *     case FSM_Exit(state1):
 *         //...
 *         break;
 *     case FSM_Enter(state1):
 *         //...
 *         break;
 *     case FSM_Exit(state2):
 *         //...
 *         break;
 *     case FSM_Enter(state2):
 *         //...
 *         break;
 *     //...
 * }
 * ```
 *
 * States are declared in enums, using the FSM_Transient() and FSM_Steady() macros.
 *
 * State transitions are done via calls to FSM_Goto(), which simply stores the
 * new state in the cFSM object.
 *
 * @hideinitializer
 * @see cFSM, FSM_Transient, FSM_Steady, FSM_Goto, FSM_Debug, FSM_Print
 */
//
// operation:
// - __i counts up (starting from 1) until the FSM reaches a steady state.
// - at __i=1,3,5,7,etc, FSM_Exit code is executed
// - at __i=2,4,6,8,etc, FSM_Enter code is executed
// - FSM_Enter code must not contain state change (this is verified)
// - state changes should be encoded in FSM_Exit code
// - infinite loops (when control never reaches steady state) are detected (FSM_MAXT)
//
#define FSM_Switch(fsm)  \
   for (int __i=1, __savedstate;  \
        (__i<3 || (__i&1)==0 || (fsm).isInTransientState()) &&  \
        (__i<2*FSM_MAXT || (throw cRuntimeError(E_INFLOOP,(fsm).getStateName()),0));  \
        ((__i&1)==0 && __savedstate!=(fsm).getState() &&  \
         (throw cRuntimeError(E_STATECHG,(fsm).getStateName()),0)),  \
         __savedstate=(fsm).getState(),++__i)  \
     switch (FSM_Print(fsm,__i&1),(((fsm).getState()*2)|(__i&1)))

/**
 * @brief Declares a transient state; to be used in enum which declares states.
 *
 * Example:
 * ```
 * enum {
 *    INIT = 0,
 *    SLEEP = FSM_Steady(1),
 *    ACTIVE = FSM_Steady(2),
 *    SEND = FSM_Transient(1),
 * };
 * ```
 *
 * The numbers in parens must be unique within the state type and they are
 * used for constructing numeric IDs for the states.
 *
 * @hideinitializer
 * @see FSM_Steady, FSM_Switch
 */
#define FSM_Transient(state)   (-(state))

/**
 * @brief Declares a steady state; to be used in enum which declares states.
 *
 * See example in FSM_Transient.
 *
 * @hideinitializer
 * @see FSM_Transient, FSM_Switch
 */
#define FSM_Steady(state)      (state)

/**
 * @brief Within an FSM_Switch() case branch, declares code to be executed
 * on entering the given state.
 *
 * Calls to FSM_Goto() are not allowed within a state's Enter block.
 *
 * @hideinitializer
 * @see FSM_Switch
 */
#define FSM_Enter(state)  (2*(state))

/**
 * @brief Within an FSM_Switch() case branch, declares code to be executed
 * on exiting the given state.
 *
 * @hideinitializer
 * @see FSM_Switch
 */
#define FSM_Exit(state)   (2*(state)|1)

/**
 * @brief To be used in state exit code, to transition to another state.
 *
 * Uses stringize (\#state), so it only works correctly if 'state'
 * is the enum name itself and not some variable that contains the
 * state code.
 *
 * @hideinitializer
 * @see FSM_Switch
 */
#define FSM_Goto(fsm,state)   (fsm).setState(state,#state)

#ifdef FSM_DEBUG
/**
 * @brief #define FSM_DEBUG before #including "omnetpp.h" to enable reporting
 * all state changes to EV.
 *
 * @hideinitializer
 * @see FSM_Switch
 */
#define FSM_Print(fsm,exiting) \
    (EV << "FSM " << (fsm).getName() \
        << ((exiting) ? ": leaving state  " : ": entering state ") \
        << (fsm).getStateName() << endl)
// this may also be useful as third line:
//      << ((fsm).isInTransientState() ? "transient state " : "steady state ")
#else
#define FSM_Print(fsm,entering) ((void)0)
#endif

/** @} */

//-----------------------------------------------------

/**
 * @brief Store the state of an FSM. This class is used in conjunction with
 * the FSM_Switch() and other FSM_ macros.
 *
 * @ingroup FSM
 * @see FSM_Switch, FSM_Transient, FSM_Steady, FSM_Enter, FSM_Exit, FSM_Goto
 */
class SIM_API cFSM : public cOwnedObject
{
  private:
    //
    // About state codes:
    //  initial state is number 0
    //  negative state codes are transient states
    //  positive state codes are steady states
    //
    int state = 0;
    const char *stateName = "INIT";   // just a ptr to an external string

  private:
    void copy(const cFSM& other);
    virtual void parsimPack(cCommBuffer *) const override {throw cRuntimeError(this, E_CANTPACK);}
    virtual void parsimUnpack(cCommBuffer *) override {throw cRuntimeError(this, E_CANTPACK);}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cFSM(const char *name=nullptr);

    /**
     * Copy constructor.
     */
    cFSM(const cFSM& other) : cOwnedObject(other) {copy(other);}

    /**
     * Assignment operator. The name member is not copied;
     * see cOwnedObject's operator=() for more details.
     */
    cFSM& operator=(const cFSM& vs);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cFSM *dup() const override  {return new cFSM(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;
    //@}

    /** @name FSM functions. */
    //@{

    /**
     * Returns the state the FSM is currently in.
     */
    int getState() const  {return state;}

    /**
     * Returns the name of the state the FSM is currently in.
     */
    const char *getStateName() const {return stateName?stateName:"";}

    /**
     * Returns true if the FSM is currently in a transient state.
     */
    int isInTransientState() const  {return state<0;}

    /**
     * Sets the state of the FSM. This method is usually invoked through
     * the FSM_Goto() macro.
     *
     * The first arg is the state code. The second arg is the name of the state.
     * setState() assumes this is pointer to a string literal (the string is
     * not copied, only the pointer is stored).
     *
     * @see FSM_Goto
     */
    void setState(int state, const char *stateName=nullptr)  {this->state=state;this->stateName=stateName;}
    //@}
};

}  // namespace omnetpp


#endif
