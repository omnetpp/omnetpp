//=========================================================================
//
//  ERRMSG.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Contents:
//    emsg[]:  error message table
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


//
// emsg[]:
//   error messages table
//
//   Blank slots are message codes no longer in use.
//

char *emsg[] = {
  "No error -- everything's fine",                       // eOK
  "Cannot cast from type `%c' to `%c'",                  // eBADCAST
  "Indirection would create circular reference",         // eCIRCREF
  "Bad number of arguments, %d expected",                // eNUMARGS
  "Cannot find module interface `%s' needed to create module of type `%s'", // eNOMODIF
  "Error during starting up user interface",             // eUISTARTUP
  "Simulation stopped by segment on host `%s'",          // eSTOPSIMRCVD
  "No such module or module finished already",           // eNOMOD
  "Module has no gate %d",                               // eNOGATE
  "send(): Gate `%s' is an input gate",                  // eINGATE
  "No module parameter called `%s'",                     // eNOPARAM
  "Not enough memory on heap",                           // eNOMEM
  "Incausality during simulation",                       // eINCAUSAL
  "Message sent to already terminated module `%s'",      // eMODFIN
  "Transfer to nonexistent, finished or compound module",// eBADTRANSF
  "setValue(): Type `%c' does not suit arg types",       // eBADINIT
  "Something unexpected happened (internal error)",      // eUNEXP
  "Cannot use receive..() or wait() with handleMessage()", // eNORECV
  "",                                                    // eSTKLOW
  "No more events -- simulation ended",                  // eENDEDOK
  "Module initialization error",                         // eMODINI
  "Object #%d not found",                                // eNULLPTR
  "Object #%d not found",                                // eNULLREF
  "Cannot write output vector file",                     // eOUTVECT
  "Cannot write parameter change file",                  // ePARCHF
  "wait(): negative delay",                              // eNEGTIME
  "",                                                    // eMATH
  "Floating point exception",                            // eFPERR
  "receive()/receiveNew(): negative timeout",            // eNEGTOUT
  "Message cannot be delivered",                         // eNODEL
  "Simulation cancelled",                                // eCANCEL
  "Network definition not found",                        // eNONET
  "Badly formed Reverse Polish expression",              // eBADEXP
  "All finish() functions called, simulation ended",     // eFINISH
  "Simulation stopped with endSimulation()",             // eENDSIM
  "CPU time limit reached -- simulation stopped",        // eREALTIME
  "Simulation time limit reached -- simulation stopped", // eSIMTIME
  "This object cannot DUP itself",                       // eCANTDUP
  "FSM: infinite loop of transient states (now in state %s)", // eINFLOOP
  "FSM: state changed during state entry code (now in state %s)", // eSTATECHG
  "assignment, copying, dup() not supported by this class", // eCANTCOPY
  "Badly connected gate",                                // eBADGATE
  "Gate not connected to anything",                      // eNOTCONN
  "",                                                    // eBADKEY
  "",                                                    // eNOMODDEF
  "",                                                    // eNOCHANDEF
  "",                                                    // eNOSUCHNET
  "PVM: function call error in %s",                      // ePVM
  "Bad command line argument",                           // eBADARGS
  "Cannot schedule a message to the past",               // eBACKSCHED
  "receiveOn(): gate %d is output gate",                 // eOUTGATE
  "Channel error out of range [0..1]",                   // NL eCHERROR
  "Delay time less than zero",                           // NL eCHDELAY
  "Array index out of boundaries",                       // NL eARRAY
  "",  /* this msg is never printed */                   // eCUSTOM
  "Not enough memory on heap for module %s",             // eNOMEM2
  "User error: %s",                                      // eUSER
  ""
};
