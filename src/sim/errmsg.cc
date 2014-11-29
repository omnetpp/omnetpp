//=========================================================================
//  ERRMSG.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "errmsg.h"

NAMESPACE_BEGIN


const char *cErrorMessages::get(OppErrorCode e)
{
    switch (e)
    {
      case eOK:        return "No error -- everything's fine";
      case eBACKSCHED: return "Cannot schedule message (%s)%s to the past, t=%g";
      case eBADCAST:   return "Cannot cast from type %s to %s";
      case eBADEXP:    return "Badly formed Reverse Polish expression";
      case eBADINIT:   return "setValue(): Type %s does not suit arg types";
      case eCANCEL:    return "Simulation cancelled";
      case eCANTCOPY:  return "Assignment, copying, dup() not supported by this class";
      case eCANTDUP:   return "This object cannot dup() itself";
      case eCANTPACK:  return "parsimPack()/parsimUnpack() not supported for this object";
      case eCUSTOM:    return "%s";
      case eDIMLESS:   return "Error in expression: `%s': argument(s) must be dimensionless";
      case eEBADARGS:  return "Error in expression: `%s': wrong argument type(s)";
      case eECANTCAST: return "Cannot cast result of expression to %s";
      case eENDEDOK:   return "No more events -- simulation ended";
      case eENDSIM:    return "Simulation stopped with endSimulation()";
      case eENOPARENT: return "Expression cannot be interpreted on network level";
      case eESTKOFLOW: return "Expression stack overflow";
      case eESTKUFLOW: return "Expression stack underflow";
      case eFINISH:    return "All finish() functions called, simulation ended";
      case eGATEID:    return "Invalid gate Id %d";
      case eINFLOOP:   return "FSM: infinite loop of transient states (now in state %s)";
      case eLOCKED:    return "Object is locked against modifications, try unlockProperties() on parent object";
      case eMODFIN:    return "Message sent to already terminated module `%s'";
      case eMODINI:    return "Module initialization error";
      case eNEGTIME:   return "wait(): negative delay";
      case eNEGTOUT:   return "receive()/receiveNew(): negative timeout";
      case eNONET:     return "Network definition not found";
      case eNOPARSIM:  return "Simulation kernel was compiled without parallel simulation support (WITH_PARSIM=no)";
      case eNORECV:    return "Cannot use receive..() or wait() with handleMessage()";
      case eNUMARGS:   return "Bad number of arguments, %d expected";
      case ePARAM:     return "While evaluating parameter `%s': %s";
      case ePARNOTSET: return "Parameter is not yet accessible";
      case ePARAMSNOTREADY: return "Attempt to access component parameters too early, they have not been set up yet";
      case eREALTIME:  return "CPU time limit reached -- simulation stopped";
      case eSIMTIME:   return "Simulation time limit reached -- simulation stopped";
      case eSTATECHG:  return "FSM: state changed during state entry code (now in state %s)";
      case eSTOPSIMRCVD: return "Simulation stopped by partition %d: %s";
      case eUSER:      return "Model error: %s";
      case eWRONGSIM:  return "cSimulation instance not activated before invoked (need setActiveSimulation())";
      default:         return "<missing error text>";
    }
}

NAMESPACE_END

