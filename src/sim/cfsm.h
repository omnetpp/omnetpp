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
  Copyright (C) 1992,99 Andras Varga
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
    explicit cFSM(const char *name=NULL);
    cFSM(cFSM& vs) {setName(vs.name());operator=(vs);}
    virtual const char *className()  {return "cFSM";}
    virtual cObject *dup()   {return new cFSM(*this);}
    virtual void info(char *buf);

    virtual const char *inspectorFactoryName() {return "cFSMIFC";}
    cFSM& operator=(cFSM& vs);
    virtual void writeContents(ostream& os);
    virtual int netPack();
    virtual int netUnpack();

    // new functions
    int state()  {return _state;}
    const char *stateName() {return _statename?_statename:"";}
    int inTransientState()  {return _state<0;}
    void setState(int state, const char *stn=NULL)  {_state=state;_statename=stn;}
};

#endif

