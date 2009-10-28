<#-- template include for network.ned.ftl -->
<#-- TODO: generateCoordinates -->
<#if parametricNED>
network ${ProjectName}
{
    parameters:
        int n = default(${nodes});
    submodules:
        hub: ${hubTypeName};
        node[n]: ${nodeTypeName};
    connections:
        for i=0..n-1 {
            hub.g++ <--> node[i].g++;
        }
}
<#else>
network ${ProjectName}
{
    submodules:
        hub: ${hubTypeName};
<#list 0..nodes-1 as i>
        node${i}: ${nodeTypeName};
</#list>
    connections:
<#list 0..nodes-1 as i>
        hub.g++ <--> node${i}.g++;
</#list>
}
</#if>
