import sys
from omnetpp.runtime import *

# #TODO into lib? with more options
# def loadNedFiles(nedFolders):
#     nedLoader = cNedLoader()
#     if not type(nedFolders) is list:
#         nedFolders = [nedFolders]
#     for folder in nedFolders:
#         nedLoader.loadNedSourceFolder(folder)
#     nedLoader.doneLoadingNedFiles()
#     return nedLoader

loadExtensionLibrary("aloha")
CodeFragments.executeAll(CodeFragments.STARTUP) # setup complete

ini = InifileContents("omnetpp.ini") # + args?
cfg = ini.extractConfig("PureAloha1")
assert cfg.__python_owns__ == True

# v1:
# nedLoader = CmdenvSimulationHolder.loadNEDFiles(cfg)
# sim = CmdenvSimulationHolder(nedLoader)
# sim.runCmdenvSimulation(cfg)

# v2:
nedLoader = cNedLoader()
nedLoader.loadNedSourceFolder(".")

simulation = cSimulation("simulation", nedLoader)
nedLoader.__python_owns__ = False

simulation.setupNetwork(cfg)
#cfg.__python_owns__ = False

print(simulation)

root = simulation.getSystemModule()

simulation.run()

sys.exit(0)

##################################################

# run a configuration from omnetpp.ini
def runIniConfig():
    ini = InifileContents("omnetpp.ini")
    cfg = ini.extractConfig("PureAloha1")
    nedLoader = CmdenvSimulationHolder.loadNEDFiles(cfg)
    sim = CmdenvSimulationHolder(nedLoader)
    sim.runCmdenvSimulation(cfg)

def runIniConfigManualNedPath():
    nedLoader = loadNedFiles(".")
    cfg = InifileContents("omnetpp.ini").extractConfig("PureAloha1")
    sim = CmdenvSimulationHolder(nedLoader)
    sim.runCmdenvSimulation(cfg)

# run a configuration from omnetpp.ini until simulation time or cpu time limit
def runIniConfigWithTimeLimit():
    pass # currently not possible in a convenient way, due to lack of mutator methods in cInifileContents/cConfiguration

# run saimulation with repeated "simulation x seconds" calls
#...

# run a specified network with a manually assembled configuration
def runIniConfigManualNedPath():
    nedLoader = loadNedFiles(".")
    cfg = InifileContents("omnetpp.ini").extractConfig("PureAloha1")
    sim = CmdenvSimulationHolder(nedLoader)
    sim.runCmdenvSimulation(cfg)

# run parameter study
#...

# run parameter study using multiple threads
#...

# simulation-based optimization
#...

# run simulation until some condition is met (schedule custom event)
#...

# Python-based simple module
#...

# building the network in Python
#...

#runIniConfigManualNedPath()

