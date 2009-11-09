<@setoutput file=newFileName?default("")/>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>

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

<#-- read links file and collect nodes from it -->
<#assign linksData = FileUtils.readCSVFile(linksFile, linksFileIgnoreFirstLine, true, true)>
<#assign nodes = LangUtils.newMap()>
<#list linksData as link>
  <#if (link?size >= 2)>
    <#-- about the "dummy=something?default" construct, see http://osdir.com/ml/web.freemarker.user/2003-06/msg00026.html -->
    <#assign dummy = nodes.put(link[0],"")?default(0)>
    <#assign dummy = nodes.put(link[1],"")?default(0)>
  </#if>
</#list>

<#-- read nodes file and store x,y coordinates -->
<#if nodesFile != "">
  <#assign nodesData = FileUtils.readCSVFile(nodesFile, nodesFileIgnoreFirstLine, true, true)>
  <#list nodesData as node>
  <#if (node?size == 3)>
    <#assign dummy = nodes.put(node[0], node[1]+","+node[2])?default(0)>
  <#else>
    <#stop "Unexpected number of items on line >>>" + LangUtils.toString(node) + "<<< (expected 2, 3, or 4)">
  </#if>
</#list>
</#if>
//
// Network generated from ${linksFile} <#if nodesFile!="">and ${nodesFile}</#if>
//
network ${nedTypeName} {
    submodules:
<#list nodes.keySet().toArray()?sort as node>
  <#if nodes[node]=="">
        ${node}: ${nodeType};
  <#else>
        ${node}: ${nodeType} { @display("p=${nodes[node]}"); }
  </#if>
</#list>
    connections:
<#list linksData as link>
  <#if (link?size == 4)>
        ${link[0]}.g++ <--> ${channelType} { datarate=${link[2]}bps; cost=${link[3]}; } <--> ${link[1]}.g++;
  <#elseif (link?size == 3)>
        ${link[0]}.g++ <--> ${channelType} { datarate=${link[2]}bps; } <--> ${link[1]}.g++;
  <#elseif (link?size == 2)>
        ${link[0]}.g++ <--> ${channelType} <--> ${link[1]}.g++;
  <#else>
        <#stop "Unexpected number of items on line >>>" + LangUtils.toString(link) + "<<< (expected 2, 3, or 4)" + link?size>
  </#if>
</#list>
}

