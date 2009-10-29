<#-- template include for network.ned.ftl -->
<#-- TODO: generateCoordinates -->
<#if parametricNED>
   <#if treeK==2>
network ${ProjectName}
{
    parameters:
        int levels = default(${treeLevels});  // 1 = root only
    submodules:
        node[2^levels-1]: ${nodeTypeName};
    connections:
        for i=1..sizeof(node)-1 {
            node[i].g++ <--> node[floor((i-1)/2)].g++;  
        }
}
  <#else>
network ${ProjectName}
{
    parameters:
        int k = default(${treeK}); // tree branching factor
        int levels = default(${treeLevels});  // 1 = root only
    submodules:
        node[(k^levels-1) / (k-1)]: ${nodeTypeName};
    connections:
        for i=1..sizeof(node)-1 {
            node[i].g++ <--> node[floor((i-1)/k).g++;
        }
}
  </#if>
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

