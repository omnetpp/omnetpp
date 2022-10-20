import os
import cppyy

omnetpp_root = os.getenv("__omnetpp_root_dir")

if not omnetpp_root:
    raise RuntimeError("Missing environment variable -- please source the OMNeT++ 'setenv' script")

libsuffix = "_dbg"

# setup omnetpp include path
cppyy.add_include_path(omnetpp_root + "/include")
cppyy.add_include_path(omnetpp_root + "/src")

# setup omnetpp library path
cppyy.add_library_path(omnetpp_root + "/lib")

# load omnetpp libraries
cppyy.load_library("liboppcmdenv" + libsuffix)
cppyy.load_library("liboppcommon" + libsuffix)
cppyy.load_library("liboppenvir" + libsuffix)
cppyy.load_library("liboppeventlog" + libsuffix)
cppyy.load_library("libopplayout" + libsuffix)
cppyy.load_library("liboppnedxml" + libsuffix)
#cppyy.load_library("liboppqtenv-osg" + libsuffix)
#cppyy.load_library("liboppqtenv" + libsuffix)
#cppyy.load_library("liboppscave" + libsuffix)
cppyy.load_library("liboppsim" + libsuffix)

# include omnetpp header files
cppyy.include("omnetpp.h")
cppyy.include("envir/args.h")
cppyy.include("envir/fsutils.h")
cppyy.include("envir/inifilecontents.h")
cppyy.include("cmdenv/cmdenvsimholder.h")
cppyy.include("sim/netbuilder/cnedloader.h")

# setup omnetpp namespace
from cppyy.gbl import omnetpp

cStaticFlag = omnetpp.cStaticFlag
SimTime = omnetpp.SimTime
cCoroutine = omnetpp.cCoroutine
CodeFragments = omnetpp.CodeFragments
cNedLoader = omnetpp.cNedLoader
cSimulation = omnetpp.cSimulation

CmdenvSimulationHolder = omnetpp.cmdenv.CmdenvSimulationHolder
ArgList = omnetpp.envir.ArgList
InifileContents = omnetpp.envir.InifileContents
loadExtensionLibrary = omnetpp.envir.loadExtensionLibrary

def py_disown_arg(clazz, method_name, arg_index):
    orig_method = getattr(clazz, method_name)
    def f(self, *args):
        orig_method(self, *args)
        try:
            args[arg_index].__python_owns__ = False
        except:
            pass
    setattr(clazz, method_name, f)

def py_own_result(clazz, method_name):
    orig_method = getattr(clazz, method_name)
    def f(self, *args):
        result = orig_method(self, *args)
        try:
            result.__python_owns__ = True
        except:
            pass
        return result
    setattr(clazz, method_name, f)


#py_disown_arg(cppyy.gbl.omnetpp.cSimulation, "setupNetwork", 0)
py_disown_arg(cppyy.gbl.omnetpp.cSimulation, "setSystemModule", 0)
py_disown_arg(cppyy.gbl.omnetpp.cSimulation, "setNedLoader", 0)
py_disown_arg(cppyy.gbl.omnetpp.cSimulation, "setScheduler", 0)
py_disown_arg(cppyy.gbl.omnetpp.cSimulation, "setFES", 0)
py_disown_arg(cppyy.gbl.omnetpp.cSimulation, "setRngManager", 0)
py_disown_arg(cppyy.gbl.omnetpp.cSimulation, "setFingerprintCalculator", 0)

py_disown_arg(cppyy.gbl.omnetpp.cSimpleModule, "send", 0)
py_disown_arg(cppyy.gbl.omnetpp.cSimpleModule, "sendDelayed", 0)
py_disown_arg(cppyy.gbl.omnetpp.cSimpleModule, "sendDirect", 0)
py_disown_arg(cppyy.gbl.omnetpp.cSimpleModule, "scheduleAt", 0)
py_disown_arg(cppyy.gbl.omnetpp.cSimpleModule, "scheduleAfter", 0)
py_disown_arg(cppyy.gbl.omnetpp.cSimpleModule, "rescheduleAt", 0)
py_disown_arg(cppyy.gbl.omnetpp.cSimpleModule, "rescheduleAfter", 0)

py_own_result(cppyy.gbl.omnetpp.cSimpleModule, "receive")
py_own_result(cppyy.gbl.omnetpp.cSimpleModule, "cancelEvent")

py_disown_arg(cppyy.gbl.omnetpp.cMessage, "addPar", 0)
py_disown_arg(cppyy.gbl.omnetpp.cMessage, "addObject", 0)
py_own_result(cppyy.gbl.omnetpp.cMessage, "removeObject")

py_disown_arg(cppyy.gbl.omnetpp.cPacket, "encapsulate", 0)
py_own_result(cppyy.gbl.omnetpp.cPacket, "decapsulate")

py_disown_arg(cppyy.gbl.omnetpp.cQueue, "insert", 0)
py_disown_arg(cppyy.gbl.omnetpp.cQueue, "insertAfter", 1)
py_disown_arg(cppyy.gbl.omnetpp.cQueue, "insertBefore", 1)
py_own_result(cppyy.gbl.omnetpp.cQueue, "remove")
py_own_result(cppyy.gbl.omnetpp.cQueue, "pop")

py_disown_arg(cppyy.gbl.omnetpp.cPacketQueue, "insert", 0)
py_disown_arg(cppyy.gbl.omnetpp.cPacketQueue, "insertAfter", 1)
py_disown_arg(cppyy.gbl.omnetpp.cPacketQueue, "insertBefore", 1)
py_own_result(cppyy.gbl.omnetpp.cPacketQueue, "remove")
py_own_result(cppyy.gbl.omnetpp.cPacketQueue, "pop")

py_disown_arg(cppyy.gbl.omnetpp.cArray, "add", 0)
py_disown_arg(cppyy.gbl.omnetpp.cArray, "addAt", 1)
py_own_result(cppyy.gbl.omnetpp.cArray, "remove")

py_disown_arg(cppyy.gbl.omnetpp.cEventHeap, "insert", 0)
py_disown_arg(cppyy.gbl.omnetpp.cEventHeap, "putBackFirst", 0)
py_own_result(cppyy.gbl.omnetpp.cEventHeap, "removeFirst")
py_own_result(cppyy.gbl.omnetpp.cEventHeap, "remove")

py_disown_arg(cppyy.gbl.omnetpp.cGate, "connectTo", 1)

py_own_result(cppyy.gbl.omnetpp.envir.InifileContents, "extractConfig")

# initialization boilerplate
static_flag = cStaticFlag()
static_flag.__python_owns__ = False
CodeFragments.executeAll(CodeFragments.EARLY_STARTUP)

import atexit

def cleanup():
    # shutdown boilerplate
    #print("bye")
    #CodeFragments.executeAll(CodeFragments.SHUTDOWN)
    #global static_flag
    #del static_flag
    pass

atexit.register(cleanup)



