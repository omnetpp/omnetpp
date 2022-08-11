from opplib import *

# setup: load ini,ned,libs,etc.
ini = InifileContents("omnetpp.ini")

globalCfg = ini.extractGlobalConfig()
SimTime.configure(globalCfg)
cCoroutine.configure(globalCfg)

loadExtensionLibrary("aloha")
CodeFragments.executeAll(CodeFragments.STARTUP) # setup complete

nedLoader = CmdenvSimulationHolder.loadNEDFiles(globalCfg)

# run simulation
# TODO { "cpu-time-limit" : "1s" }
sim = CmdenvSimulationHolder(nedLoader)
cfg = ini.extractConfig("PureAloha1")
sim.runCmdenvSimulation(cfg)

