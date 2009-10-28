<#-- template include for network.ned.ftl -->
<#-- TODO: generateCoordinates -->
<#if parametricNED>
network ${ProjectName}
{
    parameters:
        int k = default(${treeK});
        int levels = default(${treeLevels});
    submodules:
        node[TODO]: ${nodeTypeName};
    connections:
        //TODO
}
<#else>
network ${ProjectName}
{
    submodules:
<#list 0..treeLevels-1 as level>
  <#list 0..Math.pow(treeK, level)-1 as i>
        node_${level}_${i}: ${nodeTypeName};
  </#list>

</#list>
    connections:
<#-- connect each node to its parent -->    
<#list 1..treeLevels-1 as level>
  <#list 0..Math.pow(treeK, level)-1 as i>
        node_${level}_${i}.g++ <--> node_${level-1}_${(i/treeK)?floor}.g++;
  </#list>
  
</#list>
}
</#if>

