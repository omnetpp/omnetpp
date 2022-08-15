from opplib import *

loadExtensionLibrary("aloha")
CodeFragments.executeAll(CodeFragments.STARTUP) # setup complete

ini = InifileContents("omnetpp.ini") # + args?
cfg = ini.extractConfig("PureAloha1")
nedLoader = CmdenvSimulationHolder.loadNEDFiles(cfg)
sim = CmdenvSimulationHolder(nedLoader)
sim.runCmdenvSimulation(cfg)

