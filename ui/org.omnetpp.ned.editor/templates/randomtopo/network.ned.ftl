<@setoutput path=targetMainFile?default("")/>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>
<#if gateName==""><#assign gateName = "g"></#if>
<#assign gateName = StringUtils.makeValidIdentifier(gateName)>

<#assign nodeTypeExists = NedUtils.isVisibleType(nodeType,targetFolder)>
<#if nodeTypeExists>
import ${nodeType};
  <#assign nodeType = nodeType?replace("^.*\\.", "", "r")>
<#else>
  <#assign nodeType = StringUtils.makeValidIdentifier(nodeType)>
</#if>

<#assign channelTypeSupplied = channelType!="">
<#assign channelTypeExists = channelTypeSupplied && NedUtils.isVisibleType(channelType,targetFolder)>
<#if channelTypeExists>
import ${channelType};
  <#assign channelType = channelType?replace("^.*\\.", "", "r")>
<#else>
  <#assign channelType = StringUtils.makeValidIdentifier(channelType)>
</#if>

<#if channelTypeSupplied>
  <#assign channelSpec = " " + channelType + " <-->">
<#else>
  <#assign channelSpec = "">
</#if>

<#if !nodeTypeExists>
module ${nodeType} {
    parameters:
        @display("i=abstract/router_vs");
    gates:
        inout ${gateName}[];
    connections allowunconnected:
}
</#if>

<#if channelTypeSupplied && !channelTypeExists>
channel ${channelType} extends ned.DatarateChannel {
    parameters:
        int cost = default(0);
}
</#if>

<#assign numNodes = numNodes?number>
<#assign numLinks = numLinks?number>
<#assign seed = seed?number>
<#assign param1 = param1Times100?number / 100.0>
<#assign param2 = param2Times100?number / 100.0>

<#assign topo = LangUtils.newInstance("org.example.Topogen")>
<@do topo.setNodes(numNodes) !/>
<@do topo.setEdges(numLinks) !/>
<@do topo.setSeed(seed) !/>
<@do topo.setParam1(param1) !/>
<@do topo.setParam2(param2) !/>
<#assign neighborLists = topo.generate()>

//
// Generated network with random topology (${numNodes} nodes, ${numLinks} edges, seed=${seed}).
//
<#assign networkKeyword = iif(wizardType=="compoundmodule", "module", "network")>
${networkKeyword} ${targetTypeName} {
    submodules:
<#list 0..neighborLists.size()-1 as i>
        node${i} : ${nodeType};
</#list>
    connections:
<#list 0..neighborLists.size()-1 as i>
  <#assign neighborList = neighborLists[i] >
  <#list neighborList as neighbor>
     <#if (i < neighbor)>
        node${i}.${gateName}++ <-->${channelSpec} node${neighbor}.${gateName}++;
     </#if>
  </#list>
</#list>
}
