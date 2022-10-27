from omnetpp.runtime import *
import math
import cppyy
import multiprocessing
import scipy.optimize
import matplotlib.pyplot as plt

loadExtensionLibrary("aloha")
CodeFragments.executeAll(CodeFragments.STARTUP) # setup complete

nedLoader = cNedLoader()
nedLoader.__python_owns__ = False
nedLoader.loadNedSourceFolder(".")

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

def computeAlohaUtilization(numHosts, iaMean, simTimeLimit):
    print(f"PureAloha numHosts={numHosts}, iaMean={iaMean}s", end="")
    ini = InifileContents()
    ini.readText(config.format(**locals()), "<string>", ".")
    cfg = ini.extractConfig("General")

    simulation = cSimulation("simulation", nedLoader)
    simulation.setupNetwork(cfg)

    probe = installChannelUtilizationProbe(simulation)

    simulation.run()

    channelUtilization = probe.getLastValue()
    if math.isnan(channelUtilization):
        channelUtilization = 0
    print(" --> channelUtilization = ", channelUtilization)
    return channelUtilization

def installChannelUtilizationProbe(simulation):
    # @statistic[channelUtilization](source="timeavg(receive)"; record=last;...);
    serverModule = simulation.getModuleByPath("server")
    signal = omnetpp.cComponent.registerSignal("receive")
    statisticProperty = serverModule.getProperties().get("statistic", "channelUtilization")

    timeAverageFilter = omnetpp.TimeAverageFilter()
    timeAverageFilter.__python_owns__ = False
    ctx = omnetpp.cResultFilter.Context()
    ctx.component = serverModule
    ctx.attrsProperty = statisticProperty
    timeAverageFilter.init(ctx)

    lastValueRecorder = omnetpp.LastValueRecorder()
    lastValueRecorder.__python_owns__ = False
    ctx = omnetpp.cResultRecorder.Context()
    ctx.component = serverModule
    ctx.statisticName = "channelUtilization"
    ctx.recordingMode = "last"
    ctx.attrsProperty = statisticProperty
    ctx.manualAttrs = cppyy.nullptr
    lastValueRecorder.init(ctx)

    serverModule.subscribe(signal, timeAverageFilter)
    timeAverageFilter.addDelegate(lastValueRecorder)
    return lastValueRecorder

def findOptimalIaTime(numHosts):
    # usage hint: scipy.optimize.minimize(math.cos, 1.0) yields x = 3.1415973
    def to_minimize(iaMean):
        return -computeAlohaUtilization(numHosts, iaMean[0], simTimeLimit=1000)
    result = scipy.optimize.minimize(to_minimize, [200.0], bounds=[(0.01, None)], method='Nelder-Mead')
    print(result)
    return result.x[0]

numHosts = (5, 10, 15, 20, 30, 40, 60, 80, 100, 150, 200, 250, 300, 400, 600, 800)
#numHosts = (5, 10, 15)

with multiprocessing.Pool() as p:
    optimalIaMeanValues = p.map(findOptimalIaTime, numHosts)

plt.plot(numHosts, optimalIaMeanValues, marker='.')
plt.show()
