<#-- template include for network.ned.ftl -->
<#-- TODO: generateCoordinates -->
<#if parametricNED>
network ${ProjectName}
{
    parameters:
        int n = default(${nodes});
    submodules:
        node[n]: ${nodeTypeName};
    connections:
        for i=0..n-1 {
            node[i].g++ <--> node[(i+1)%n].g++;
        }
}
<#else>
network ${ProjectName}
{
    submodules:
<#list 0..nodes-1 as i>
        node${i}: ${nodeTypeName};
</#list>
    connections:
<#list 0..nodes-1 as i>
        node${i}.g++ <--> node${(i+1)%nodes}.g++;
</#list>
}
</#if>
