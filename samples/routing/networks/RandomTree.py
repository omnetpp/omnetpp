import networkx
from omnetpp.runtime import *

class RandomTree(omnetpp.cModule):
    def doBuildInside(self):
        n = self.par("n").intValue()
        g = networkx.random_tree(n)
        self.buildNetworkFromGraph(g)

    def buildNetworkFromGraph(self, g):
        nodeTypeName = self.par("nodeType").stringValue()
        nodeType = omnetpp.cModuleType.get(nodeTypeName)
        n = g.number_of_nodes()
        nodes = [nodeType.create("rte"+str(i), self) for i in range(n)]
        delay, ber, datarate = (0.001, 0, 1e6)
        for edge in g.edges():
            srcNode = nodes[edge[0]]
            destNode = nodes[edge[1]]
            srcGateIndex = srcNode.getOrCreateFirstUnconnectedGatePairIndex("port", False, True)
            destGateIndex = destNode.getOrCreateFirstUnconnectedGatePairIndex("port", False, True)
            self.connect(srcNode, srcGateIndex, destNode, destGateIndex, delay, ber, datarate)
            self.connect(destNode, destGateIndex, srcNode, srcGateIndex, delay, ber, datarate)
        for node in nodes:
            node.buildInside()

    def connect(self, srcNode, srcGateIndex, destNode, destGateIndex, delay, ber, datarate):
        srcGate = srcNode.gate("port$o", srcGateIndex)
        destGate = destNode.gate("port$i", destGateIndex)
        channel = omnetpp.cDatarateChannel.create("channel")
        channel.setDelay(delay)
        channel.setBitErrorRate(ber)
        channel.setDatarate(datarate)
        srcGate.connectTo(destGate, channel)


