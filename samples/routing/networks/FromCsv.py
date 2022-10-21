import numpy as np
import pandas as pd
from omnetpp.runtime import *

class FromCsv(omnetpp.cModule):
    def doBuildInside(self):
        csvFile = self.par("csvFile").stringValue()
        nodeTypeName = self.par("nodeType").stringValue()
        df = pd.read_csv(csvFile, skipinitialspace=True)
        nodeType = omnetpp.cModuleType.get(nodeTypeName)
        nodes = dict()
        for name in np.unique(df[['src', 'dest']].values):
            nodes[name] = nodeType.create(name, self)
        for row in df.itertuples():
            srcNode = nodes[row.src]
            destNode = nodes[row.dest]
            srcGateIndex = srcNode.getOrCreateFirstUnconnectedGatePairIndex("port", False, True)
            destGateIndex = destNode.getOrCreateFirstUnconnectedGatePairIndex("port", False, True)
            self.connect(srcNode, srcGateIndex, destNode, destGateIndex, row.delay, row.ber, row.datarate)
            self.connect(destNode, destGateIndex, srcNode, srcGateIndex, row.delay, row.ber, row.datarate)
        for node in nodes.values():
            node.buildInside()

    def connect(self, srcNode, srcGateIndex, destNode, destGateIndex, delay, ber, datarate):
        srcGate = srcNode.gate("port$o", srcGateIndex)
        destGate = destNode.gate("port$i", destGateIndex)
        channel = omnetpp.cDatarateChannel.create("channel")
        channel.par("delay").parse(str(delay))
        channel.par("ber").parse(str(ber))
        channel.par("datarate").parse(str(datarate))
        srcGate.connectTo(destGate, channel)


