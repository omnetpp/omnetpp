# invoke from LLDB with: command script import <OMNETPP_ROOT>/python/omnetpp/lldb/formatters/omnetpp.py

import lldb
from decimal import *

def cDisplayString_SummaryProvider(value, internal_dict):
    return value.GetChildMemberWithName("assembledString").GetSummary()[1:-1]

def simtime_t_SummaryProvider(value, internal_dict):
    t = value.GetChildMemberWithName("t").GetValueAsSigned()
    if (t == 0):
        return "0s"
    scaleexp = value.GetTarget().FindFirstGlobalVariable("omnetpp::SimTime::scaleexp").GetValueAsSigned()
    s = Decimal(str(t) + 'E' + str(scaleexp)).normalize()
    return str(s) + "s"

def cNamedObject_SummaryProvider(value, internal_dict):
    return value.GetChildMemberWithName("name").GetSummary()[1:-1]

def cOwnedObject_SummaryProvider(value, internal_dict):
    return value.GetChildMemberWithName("name").GetSummary()[1:-1] + " (owned by " + \
        value.GetChildMemberWithName("owner").GetSummary() + ")"

def cModule_SummaryProvider(value, internal_dict):
    return value.GetChildMemberWithName("fullPath").GetSummary()[1:-1] + \
    ": " + value.GetChildMemberWithName("componentType").GetSummary()[1:-1]

def cComponentType_SummaryProvider(value, internal_dict):
    return value.GetChildMemberWithName("qualifiedName").GetSummary()

def cComponent_SummaryProvider(value, internal_dict):
    return value.GetChildMemberWithName("name").GetSummary()[1:-1] + \
        ": " + value.GetChildMemberWithName("componentType").GetSummary()[1:-1]

def cEvent_SummaryProvider(value, internal_dict):
    return value.GetChildMemberWithName("name").GetSummary()[1:-1] + \
        " @" + value.GetChildMemberWithName("arrivalTime").GetSummary() + \
        " pri:" + value.GetChildMemberWithName("priority").GetValue()

def cMessage_SummaryProvider(value, internal_dict):
    return cEvent_SummaryProvider(value, internal_dict) + \
        " kind:" + value.GetChildMemberWithName("messageKind").GetValue()

def cPacket_SummaryProvider(value, internal_dict):
    return cMessage_SummaryProvider(value, internal_dict) + \
        " len:" + value.GetChildMemberWithName("bitLength").GetValue() + "bit" + \
        " dur:" + value.GetChildMemberWithName("duration").GetSummary()

def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".simtime_t_SummaryProvider omnetpp::simtime_t")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cDisplayString_SummaryProvider omnetpp::cDisplayString")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cNamedObject_SummaryProvider omnetpp::cNamedObject")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cOwnedObject_SummaryProvider omnetpp::cOwnedObject")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cOwnedObject_SummaryProvider omnetpp::cNoncopyableOwnedObject")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cOwnedObject_SummaryProvider omnetpp::cDefaultOwner")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cComponentType_SummaryProvider omnetpp::cComponentType")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cComponentType_SummaryProvider omnetpp::cModuleType")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cComponentType_SummaryProvider omnetpp::cDynamicModuleType")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cComponent_SummaryProvider omnetpp::cComponent")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cModule_SummaryProvider omnetpp::cModule")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cModule_SummaryProvider omnetpp::cSimpleModule")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cEvent_SummaryProvider omnetpp::cEvent")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cMessage_SummaryProvider omnetpp::cMessage")
    debugger.HandleCommand("type summary add -w omnetpp -F " + __name__ + ".cPacket_SummaryProvider omnetpp::cPacket")
    debugger.HandleCommand("type category enable omnetpp")