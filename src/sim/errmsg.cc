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
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "omnetpp/errmsg.h"

namespace omnetpp {


const char *cErrorMessages::get(OppErrorCode e)
{
    switch (e) {
        case E_OK:        return "No error -- everything's fine";
        case E_BACKSCHED: return "Cannot schedule message (%s)%s to the past, t=%g";
        case E_BADCAST:   return "Cannot cast from type %s to %s";
        case E_BADEXP:    return "Badly formed Reverse Polish expression";
        case E_BADINIT:   return "setValue(): Type %s does not suit arg types";
        case E_CANCEL:    return "Simulation cancelled";
        case E_CANTCOPY:  return "Assignment, copying and duplication are not supported by this class";
        case E_CANTDUP:   return "This object cannot dup() itself";
        case E_CANTPACK:  return "parsimPack()/parsimUnpack() not supported for this object";
        case E_CUSTOM:    return "%s";
        case E_DIMLESS:   return "`%s' expects dimensionless arguments";
        case E_EBADARGS:  return "Wrong argument type for `%s'";
        case E_ECANTCAST: return "Cannot cast result of expression to %s";
        case E_ENDEDOK:   return "No more events -- simulation ended";
        case E_ENDSIM:    return "Simulation stopped with endSimulation()";
        case E_ENOPARENT: return "Expression cannot be interpreted on network level";
        case E_ESTKOFLOW: return "Expression stack overflow";
        case E_ESTKUFLOW: return "Expression stack underflow";
        case E_FINISH:    return "All finish() functions called, simulation ended";
        case E_GATEID:    return "Invalid gate Id %d";
        case E_INFLOOP:   return "FSM: infinite loop of transient states (now in state %s)";
        case E_LOCKED:    return "Object is locked against modifications, try unlockProperties() on parent object";
        case E_MISSINGTAG:return "Mandatory tag `%s' missing from message or packet";
        case E_MODFIN:    return "Message sent to already terminated module `%s'";
        case E_MODINI:    return "Module initialization error";
        case E_NEGTIME:   return "wait(): negative delay";
        case E_NEGTOUT:   return "receive()/receiveNew(): negative timeout";
        case E_NONET:     return "Network definition not found";
        case E_NOPARSIM:  return "Simulation kernel was compiled without parallel simulation support (WITH_PARSIM=no)";
        case E_NORECV:    return "Cannot use receive..() or wait() with handleMessage()";
        case E_NUMARGS:   return "Bad number of arguments, %d expected";
        case E_PARAM:     return "While evaluating parameter `%s': %s";
        case E_PARNOTSET: return "Parameter is not yet accessible";
        case E_PARAMSNOTREADY: return "Attempt to access component parameters too early, they have not been set up yet";
        case E_REALTIME:  return "CPU time limit reached -- simulation stopped";
        case E_SIMTIME:   return "Simulation time limit reached -- simulation stopped";
        case E_STATECHG:  return "FSM: state changed during state entry code (now in state %s)";
        case E_STOPSIMRCVD: return "Simulation stopped by partition %d: %s";
        case E_USER:      return "Model error: %s";
        case E_WRONGSIM:  return "cSimulation instance not activated before invoked (need setActiveSimulation())";
        default:         return "<missing error text>";
    }
}

}  // namespace omnetpp

