import numpy as np
import pandas as pd
from omnetpp.runtime import *
import routing_utils as ru

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
            ru.connectNodes(nodes[row.src], nodes[row.dest], delay=row.delay, ber=row.ber, datarate=row.datarate)
        for node in nodes.values():
            node.buildInside()


