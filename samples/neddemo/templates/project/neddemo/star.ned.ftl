<#-- template include for network.ned.ftl -->
<#if generateNodeTypeDecl>
module ${nodeTypeName} {
    parameters:
        @display("i=misc/node_vs");
    gates:
        inout g[];
}
</#if>

<#if generateHubNodeTypeDecl>
module ${hubNodeTypeName} {
    parameters:
        @display("i=misc/node_vs");
    gates:
        inout g[];
}
</#if>

<#if generateChannelTypeDecl && channelTypeName!="">
channel ${channelTypeName} extends ned.DatarateChannel {
    parameters:
        int cost = default(0);
}
</#if>

<#-- TODO: generateCoordinates -->
<#if parametricNED>
network ${networkName}
{
    parameters:
        int n = default(${nodes});
    submodules:
        hub: ${hubNodeTypeName};
        node[n]: ${nodeTypeName};
    connections:
        for i=0..n-1 {
            hub.g++ <-->${channelSpec} node[i].g++;
        }
}
<#else>
network ${networkName}
{
    submodules:
        hub: ${hubNodeTypeName};
<#list 0..nodes-1 as i>
        node${i}: ${nodeTypeName};
</#list>
    connections:
<#list 0..nodes-1 as i>
        hub.g++ <-->${channelSpec} node${i}.g++;
</#list>
}
</#if>
