<#if wizardType=="simulation">
  <@setoutput file=simulationFolderName+"/"+simulationName+".ned"/>
  <#assign networkName=simulationName>
<#elseif wizardType=="project">
  <@setoutput file=ProjectName+".ned"/>
  <#assign networkName=ProjectName>
<#else>
  <@setoutput file=newFileName?default("")/>
  <#assign networkName=nedTypeName?default("Untitled")>
</#if>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>

<#if !NedUtils.isVisibleType(nodeType,targetFolder)>
<#assign nodeType = StringUtils.makeValidIdentifier(nodeType)>
module ${nodeType} {
    parameters:
        @display("i=abstract/router_s");
    gates:
        inout g[];
}
</#if>

<#if !NedUtils.isVisibleType(channelType,targetFolder)>
<#assign channelType = StringUtils.makeValidIdentifier(channelType)>
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
    <@do nodes.put(link[0],"") !/>
    <@do nodes.put(link[1],"") !/>
  </#if>
</#list>

<#-- read nodes file and store x,y coordinates -->
<#if nodesFile != "">
  <#assign nodesData = FileUtils.readCSVFile(nodesFile, nodesFileIgnoreFirstLine, true, true)>
  <#list nodesData as node>
  <#if (node?size == 3)>
    <@do nodes.put(node[0], node[1]+","+node[2]) !/>
  <#else>
    <#stop "Unexpected number of items on line >>>" + LangUtils.toString(node) + "<<< (expected 2, 3, or 4)">
  </#if>
</#list>
</#if>
//
// Network generated from ${linksFile} <#if nodesFile!="">and ${nodesFile}</#if>
//
network ${networkName} {
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

