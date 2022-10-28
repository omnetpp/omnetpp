import networkx
import routing_utils as ru
from omnetpp.runtime import *

class RandomTree(omnetpp.cModule):
    def doBuildInside(self):
        n = self.par("n").intValue()
        g = networkx.random_tree(n)
        self.buildNetworkFromGraph(g)

    def buildNetworkFromGraph(self, g):
        nodeTypeName = self.par("nodeType").stringValue()
        ru.build_networkx_graph(g, nodeTypeName, namePrefix="rte")
        # nodeType = omnetpp.cModuleType.get(nodeTypeName)
        # n = g.number_of_nodes()
        # nodes = [nodeType.create("rte"+str(i), self) for i in range(n)]
        # delay, ber, datarate = (0.001, 0, 1e6)
        # for edge in g.edges():
        #     srcNode, destNode = nodes[edge[0]], nodes[edge[1]]
        #     ru.connectNodes(srcNode, destNode, delay=delay, ber=ber, datarate=datarate)
        # for node in nodes:
        #     node.buildInside()
