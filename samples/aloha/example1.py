from omnetpp.runtime import *

loadExtensionLibrary("aloha")
CodeFragments.executeAll(CodeFragments.STARTUP) # setup complete

ini = InifileContents("omnetpp.ini")
cfg = ini.extractConfig("PureAloha1")

nedLoader = cNedLoader()
nedLoader.__python_owns__ = False
nedLoader.loadNedFolder(".")

simulation = cSimulation("simulation", nedLoader)
simulation.setupNetwork(cfg)
simulation.run()
