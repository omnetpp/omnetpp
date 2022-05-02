import re
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
cppyy.include("cmdenv/cmdenv.h")
cppyy.include("envir/inifilereader.h")
cppyy.include("envir/sectionbasedconfig.h")

# setup omnetpp namespace
from cppyy.gbl import omnetpp
cStaticFlag = omnetpp.cStaticFlag
SimTime = omnetpp.SimTime
CodeFragments = omnetpp.CodeFragments
cSimulation = omnetpp.cSimulation
cEvent = omnetpp.cEvent
Cmdenv = omnetpp.cmdenv.Cmdenv
InifileReader = omnetpp.envir.InifileReader
SectionBasedConfiguration = omnetpp.envir.SectionBasedConfiguration

# define a modified Cmdenv
class PythonCmdenv(Cmdenv):
    def loadNEDFiles(self):
        pass

# startup
#SimTime.setScaleExp(-12)
static_flag = cStaticFlag()
static_flag.__python_owns__ = False
CodeFragments.executeAll(CodeFragments.EARLY_STARTUP)

# Long version:
# iniReader = InifileReader()
# iniReader.readFile("omnetpp.ini")
# configuration = SectionBasedConfiguration()
# configuration.setConfigurationReader(iniReader)
# iniReader.__python_owns__ = False

# Shorter:
# configuration = SectionBasedConfiguration()
# configuration.readFile("omnetpp.ini")

configuration = SectionBasedConfiguration("omnetpp.ini")

cppyy.load_library("libaloha" + libsuffix)  #TODO should use omnet's own lib loaded which does EARLY_STARTUP too
CodeFragments.executeAll(CodeFragments.EARLY_STARTUP)
args = [ "<progname>", "-c", "PureAloha1" ]
extra_config_options = { "cpu-time-limit" : "1s" }
configuration.setCommandLineConfigOptions(extra_config_options, ".")

environment = PythonCmdenv()
#environment.loggingEnabled = False
simulation = cSimulation("simulation", environment.getEnvir())
environment.__python_owns__ = False
cSimulation.setActiveSimulation(simulation)  # TODO should be done inside methods like doOneEvent()
simulation.loadNedSourceFolder(omnetpp_root + "/samples/aloha")
simulation.doneLoadingNedFiles()  # TODO clumsy -- cannot this be implicit?

# run
returncode = environment.run(args, configuration)

# shutdown
cSimulation.setActiveSimulation(cppyy.nullptr)
simulation.__python_owns__ = False
CodeFragments.executeAll(CodeFragments.SHUTDOWN)
del static_flag
