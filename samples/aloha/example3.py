from omnetpp.runtime import *

loadExtensionLibrary("aloha")
CodeFragments.executeAll(CodeFragments.STARTUP) # setup complete

nedLoader = cNedLoader()
nedLoader.__python_owns__ = False
nedLoader.loadNedFolder(".")

config = """
network = Aloha
**.vector-recording = false
Aloha.slotTime = 0s    # no slots
Aloha.txRate = 9.6kbps
Aloha.host[*].pkLenBits = 952b #=119 bytes, so that (with +1 byte guard) slotTime is a nice round number

**.x = uniform(0m, 1000m)
**.y = uniform(0m, 1000m)
**.animationHoldTimeOnCollision = 0s
**.idleAnimationSpeed = 1
**.transmissionEdgeAnimationSpeed = 1e-6
**.midTransmissionAnimationSpeed = 1e-1

sim-time-limit = {simTimeLimit}s
Aloha.numHosts = {numHosts}
Aloha.host[*].iaTime = exponential({iaMean}s)
"""

simTimeLimit = 5600
for numHosts in [10,15,20]:
    for iaMean in [1,2,3,4,5,7,9]:
        print(f"PureAloha numHosts={numHosts}, iaMean={iaMean}s")
        ini = InifileContents()
        ini.readText(config.format(**locals()), "<string>", ".")
        cfg = ini.extractConfig("General")

        simulation = cSimulation("simulation", nedLoader)
        cSimulation.setActiveSimulation(simulation) # either this, or simulation=None at the bottom of the loop
        simulation.setupNetwork(cfg)
        simulation.run()

