//==========================================================================
//   CFSM.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Contents:
//    FSM building macros
//    class cFSM: stores the state of an FSM
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CFSM_H
#define __CFSM_H

#include "cobject.h"

//
// FSM Macros
//

//
// FSM_MAXT:
//  after this many transitions without reaching a steady state
//  we assume the FSM is in an infinite loop
//
#define FSM_MAXT  64

//
// FSM_Switch():
//   used like switch() in order to execute one state change of an FSM
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
        (__i<3 || (__i&1)==0 || (fsm).inTransientState()) &&  \
        (__i<2*FSM_MAXT || (opp_error(eINFLOOP,(fsm).stateName()),0));  \
        ((__i&1)==0 && __savedstate!=(fsm).state() &&  \
         (opp_error(eSTATECHG,(fsm).stateName()),0)),  \
         __savedstate=(fsm).state(),++__i)  \
     switch (FSM_Print(fsm,__i&1),(((fsm).state()<<1)|(__i&1)))

//
// FSM_Transient/FSM_Steady:
//   to be used in enum which declares states
//
#define FSM_Transient(state)   (-(state))
#define FSM_Steady(state)      (state)

//
// FSM_Enter/FSM_Exit:
//   to be used in "cases" within an FSM_Switch()
//
#define FSM_Enter(state)  ((state)<<1)
#define FSM_Exit(state)   (((state)<<1)|1)

//
// FSM_Goto:
//   to be used in state exit code, to transition to another state.
//
//   Uses stringize (#state), so it only works correctly if 'state'
//   is the enum name itself and not some variable that contains the
//   state code.
//
#define FSM_Goto(fsm,state)   (fsm).setState(state,#state)

//
// FSM_DEBUG:
//   #define FSM_DEBUG before #including "omnetpp.h" to enable reporting
//   all state changes to ev
//
#ifdef FSM_DEBUG
#define FSM_Print(fsm,exiting) \
    (ev << "FSM " << (fsm).name() \
        << ((exiting) ? ": exiting " : ": entering ") \
        << (fsm).stateName() << endl)
// this may also be useful as third line:
//      << ((fsm).inTransientState() ? "transient state " : "steady state ")
#else
#define FSM_Print(fsm,entering) ((void)0)
#endif


//-----------------------------------------------------
//
// cFSM: class to store the state of an FSM
//

/**
 * FIXME: 
 * FSM Macros
 * 
 * 
 * FSM_MAXT:
 *  after this many transitions without reaching a steady state
 *  we assume the FSM is in an infinite loop
 * 
 * 
 * FSM_Switch():
 *   used like switch() in order to execute one state change of an FSM
 * 
 * operation:
 * - __i counts up (starting from 1) until the FSM reaches a steady state.
 * - at __i=1,3,5,7,etc, FSM_Exit code is executed
 * - at __i=2,4,6,8,etc, FSM_Enter code is executed
 * - FSM_Enter code must not contain state change (this is verified)
 * - state changes should be encoded in FSM_Exit code
 * - infinite loops (when control never reaches steady state) are detected (FSM_MAXT)
 * 
 * 
 * FSM_Transient/FSM_Steady:
 *   to be used in enum which declares states
 * 
 * 
 * FSM_Enter/FSM_Exit:
 *   to be used in "cases" within an FSM_Switch()
 * 
 * 
 * FSM_Goto:
 *   to be used in state exit code, to transition to another state.
 * 
 *   Uses stringize (#state), so it only works correctly if 'state'
 *   is the enum name itself and not some variable that contains the
 *   state code.
 * 
 * 
 * FSM_DEBUG:
 *   #define FSM_DEBUG before #including "omnetpp.h" to enable reporting
 *   all state changes to ev
 * 
 * this may also be useful as third line:
 *      << ((fsm).inTransientState() ? "transient state " : "steady state ")
 * 
 * 
 * cFSM: class to store the state of an FSM
 * 
 */
class SIM_API cFSM : public cObject
{
  private:
    //
    // About state codes:
    //  initial state is number 0
    //  negative state codes are transient states
    //  positive state codes are steady states
    //
    int _state;
    const char *_statename;   // just a ptr to an external string
  public:

    /**
     * FIXME: 
     * About state codes:
     *  initial state is number 0
     *  negative state codes are transient states
     *  positive state codes are steady states
     * 
     */
    explicit cFSM(const char *name=NULL);

    /**
     * MISSINGDOC: cFSM:cFSM(cFSM&)
     */
    cFSM(cFSM& vs) {setName(vs.name());operator=(vs);}

    /**
     * MISSINGDOC: cFSM:char*className()
     */
    virtual const char *className() const {return "cFSM";}

    /**
     * MISSINGDOC: cFSM:cObject*dup()
     */
    virtual cObject *dup()   {return new cFSM(*this);}

    /**
     * MISSINGDOC: cFSM:void info(char*)
     */
    virtual void info(char *buf);


    /**
     * MISSINGDOC: cFSM:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cFSMIFC";}

    /**
     * MISSINGDOC: cFSM:cFSM&operator=(cFSM&)
     */
    cFSM& operator=(cFSM& vs);

    /**
     * MISSINGDOC: cFSM:void writeContents(ostream&)
     */
    virtual void writeContents(ostream& os);

    /**
     * MISSINGDOC: cFSM:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cFSM:int netUnpack()
     */
    virtual int netUnpack();

    // new functions

    /**
     * FIXME: new functions
     */
    int state()  {return _state;}

    /**
     * MISSINGDOC: cFSM:char*stateName()
     */
    const char *stateName() {return _statename?_statename:"";}

    /**
     * MISSINGDOC: cFSM:int inTransientState()
     */
    int inTransientState()  {return _state<0;}

    /**
     * MISSINGDOC: cFSM:void setState(int,char*)
     */
    void setState(int state, const char *stn=NULL)  {_state=state;_statename=stn;}
};

#endif
