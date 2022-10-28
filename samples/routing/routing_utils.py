import cppyy
from omnetpp.runtime import *

def readDataFile(fileName):
    with open(fileName, "r") as f:
        return [float(s) for s in f.read().split(",")]

def build_networkx_graph(g, nodeType, namePrefix="node", parentModule=None, gateName=None, **channelArgs):
    """
    Builds a network using the topology provided in the form of a networkx.Graph.
    """
    # resolve nodeType
    if type(nodeType) == str:
        nodeType = omnetpp.cModuleType.get(nodeType)
    elif not isinstance(nodeType, omnetpp.cModuleType):
        raise TypeError(f"invalid type {type(nodeType)} for nodeType argument, should be cModuleType or string")

    # resolve parentModule
    if not parentModule:
        parentModule = omnetpp.cSimulation.getActiveSimulation().getSystemModule()
        if not parentModule:
            raise ValueError(f"simulation must already contain the top-level module")
    elif type(parentModule) == str:
        parentModule = omnetpp.cSimulation.getActiveSimulation().getModuleByPath(parentModule)
    elif not isinstance(parentModule, omnetpp.cModule):
        raise TypeError(f"invalid type {type(parentModule)} for parentModule argument, should be cModule, string or None")

    # build network
    n = g.number_of_nodes()
    nodes = [nodeType.create(namePrefix+str(i), parentModule) for i in range(n)]
    for edge in g.edges():
        srcNode, destNode = nodes[edge[0]], nodes[edge[1]]
        edgeAttrs = g.get_edge_data(*edge)
        connectNodes(srcNode, destNode, srcGateName=gateName, destGateName=gateName, **(channelArgs | edgeAttrs))
    for node in nodes:
        node.buildInside()

def connectNodes(srcNode, destNode, srcGateName=None, destGateName=None, **channelArgs):
    """
    Connects two nodes in both ways.
    """
    connectGates(srcNode, destNode, srcGate=srcGateName, destGate=destGateName, **channelArgs)
    connectGates(destNode, srcNode, destGate=destGateName, srcGate=srcGateName, **channelArgs)

def connectGates(srcNode=None, destNode=None, srcGate=None, destGate=None, srcGateIndex=None, destGateIndex=None, **channelArgs):
    """
    Connects two gates, figuring out the details.

    Parameters:

    - `srcNode`, `destNode` (cModule or string): the source and destination of the connection
    - `srcGate`, `destGate` (cGate, string or None): the gates or gate names.
    - TODO
    """
    def resolveModule(module, gate):
        if isinstance(module, omnetpp.cModule):
            return module
        elif type(module) == str:
            return omnetpp.cSimulation.getActiveSimulation().getModuleByPath(module)
        elif module is None:
            # gate parameter MUST contain a cGate object
            return gate.getOwnerModule()
        else:
            raise TypeError(f"invalid type {type(module)} for srcNode/destNode argument, should be cModule, string or None")

    srcNode = resolveModule(srcNode, srcGate)
    destNode = resolveModule(destNode, destGate)
    areSiblings = srcNode.getParentModule() == destNode.getParentModule()

    def resolveGate(node, gate, gateIndex, gateType):
        if isinstance(gate, omnetpp.cGate):
            return gate
        elif areSiblings:
            return resolveGateToConnect(node, gate, gateIndex, gateType=gateType, inside=False, expand=True)
        else:
            assert False #TODO

    srcGate = resolveGate(srcNode, srcGate, srcGateIndex, omnetpp.cGate.OUTPUT)
    destGate = resolveGate(destNode, destGate, destGateIndex, omnetpp.cGate.INPUT)

    channel = createChannel(**channelArgs) if channelArgs else cppyy.nullptr
    srcGate.connectTo(destGate, channel)

def resolveGateToConnect(module=None, gateName=None, gateIndex=None, gateType=None, inside=False, expand=True):
    """
    Utility for connectGates().
    """
    if module is None:
        raise RuntimeError("module not specified")

    gateNameSuffix = 0
    if gateName is None:
        names = module.getGateNames(gateType)
        inoutNames = module.getGateNames(omnetpp.cGate.INOUT)
        numNames = len(names) + len(inoutNames)
        if numNames == 0:
            raise RuntimeError("has no gate of the appropriate type")
        elif numNames > 1:
            raise RuntimeError("ambiguity: has more than one gate of the appropriate type")
        if names:
            gateName = names[0]
        else:
            gateName = inoutNames[0]
            gateNameSuffix = "i" if gateType == omnetpp.cGate.INPUT else "o"
        if type(gateName) == cppyy.gbl.std.string:
            gateName = str(gateName)

    if gateIndex is not None and not module.isGateVector(gateName):
        raise RuntimeError("gate index specified for scalar gate")

    if gateIndex is None and module.isGateVector(gateName):
        gateIndex = module.getOrCreateFirstUnconnectedGatePairIndex(gateName, inside, expand) if gateNameSuffix \
            else module.getOrCreateFirstUnconnectedGateIndex(gateName, gateNameSuffix, inside, expand)

    gateNameWithSuffix = gateName + "$" + gateNameSuffix if gateNameSuffix else gateName
    return module.gate(gateNameWithSuffix, gateIndex)

def createChannel(channelType=None, name=None, displayString=None, **channelParams):
    """
    Utility for connectGates().
    """
    if name is None:
        name = "channel"

    if type(channelType) == str:
        channelType = omnetpp.cChannelType.get(channelType)

    if channelType is not None:
        channel = channelType.create(name)
    elif ("datarate" in channelParams) or ("ber" in channelParams) or ("per" in channelParams):
        channel = omnetpp.cDatarateChannel.create(name)
    elif "delay" in channelParams:
        channel = omnetpp.cDelayChannel.create(name)
    else:
        channel = omnetpp.cIdealChannel.create(name)

    if displayString is not None:
        channel.setDisplayString(displayString)

    for name, value in channelParams.items():
        param = channel.par(name)
        if type(value) == str and param.getType() != omnetpp.cPar.STRING:
            param.parse(value)
        elif param.getType() == omnetpp.cPar.BOOL:
            param.setBoolValue(value)
        elif param.getType() == omnetpp.cPar.INT:
            param.setIntValue(value)
        elif param.getType() == omnetpp.cPar.DOUBLE:
            param.setDoubleValue(value)
        elif param.getType() == omnetpp.cPar.STRING:
            param.setStringValue(value)
        elif param.getType() == omnetpp.cPar.XML:
            param.setXmlValue(value)
        elif param.getType() == omnetpp.cPar.OBJECT:
            param.setObjectValue(value)
        else:
            assert False

    return channel
