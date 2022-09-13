import cppyy

omnetpp_root = "/home/andras/projects/omnetpp-dev"
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

# initialization boilerplate
static_flag = cStaticFlag()
static_flag.__python_owns__ = False
CodeFragments.executeAll(CodeFragments.EARLY_STARTUP)

import atexit

def cleanup():
    # shutdown boilerplate
    print("bye")
    #CodeFragments.executeAll(CodeFragments.SHUTDOWN)
    #global static_flag
    #del static_flag

atexit.register(cleanup)



