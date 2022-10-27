from omnetpp.runtime import *

loadExtensionLibrary("aloha")
CodeFragments.executeAll(CodeFragments.STARTUP) # setup complete

nedLoader = cNedLoader()
nedLoader.__python_owns__ = False
nedLoader.loadNedFolder(".")

ini = InifileContents("omnetpp.ini")
numRuns = ini.getNumRunsInConfig("PureAlohaExperiment")

for runNumber in range(numRuns):
    print("PureAlohaExperiment, run", runNumber)
    cfg = ini.extractConfig("PureAlohaExperiment", runNumber)
    simulation = cSimulation("simulation", nedLoader)
    cSimulation.setActiveSimulation(simulation) # either this, or simulation=None at the bottom of the loop
    simulation.setupNetwork(cfg)
    simulation.run()

