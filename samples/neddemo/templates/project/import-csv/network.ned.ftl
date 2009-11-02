<#if generateNodeTypeDecl>
module ${nodeType} {
    parameters:
        @display("i=abstract/router_s");
    gates:
        inout g[];
}
</#if>

<#if generateChannelTypeDecl>
channel ${channelType} extends ned.DatarateChannel {
    parameters:
        int cost = default(0);
}
</#if>

<#assign linksData = FileUtils.readCSVFile(linksFile)>
<#assign nodes = LangUtils.newMap()>
<#list linksData as link>
  <#if (link?size >= 2)>     
    <#if nodes.put(link[0],"")></#if><#--FIXME dummy if!-->
    <#if nodes.put(link[1],"")></#if><#--FIXME dummy if!-->
  </#if>
</#list>

<#if nodesFile != "">
  <#assign nodesData = FileUtils.readCSVFile(nodesFile)>
  <#list nodesData as node>
  <#if (node?size == 3)>     
    <#if nodes.put(node[0],node[1]+","+node[2])></#if> <#--FIXME dummy if!-->
  <#elseif (node?size == 0 || (node?size == 1 && node[0] == ""))>
  <#else>     
    <#stop "Unexpected number of items on line >>>" + LangUtils.toString(node) + "<<< (expected 2, 3, or 4)" + node?size>
  </#if>
</#list>
</#if>
//
// Network generated from ${linksFile} <#if nodesFile!="">and ${nodesFile}</#if>
//
network ${networkName} {
    submodules:
<#list nodes.keySet().toArray()?sort as node>
        ${node}: ${nodeType};  // ${nodes.get(node)}
</#list>
    connections:
<#list linksData as link>
  <#if (link?size == 4)>     
        ${link[0]} <--> ${channelType} { datarate=${link[2]}bps; cost=${link[3]}; } <--> ${link[1]};
  <#elseif (link?size == 3)>     
        ${link[0]} <--> ${channelType} { datarate=${link[2]}bps; } <--> ${link[1]};
  <#elseif (link?size == 2)>     
        ${link[0]} <--> ${channelType} <--> ${link[1]};
  <#elseif (link?size == 0 || (link?size == 1 && link[0] == ""))>
  <#else>     
        <#stop "Unexpected number of items on line >>>" + LangUtils.toString(link) + "<<< (expected 2, 3, or 4)" + link?size>
  </#if>
</#list>
}

