<@setoutput path=targetMainFile?default("")/>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>
<#if gateName==""><#assign gateName = "g"></#if>
<#assign gateName = StringUtils.makeValidIdentifier(gateName)>

<#if !NedUtils.isVisibleType(nodeType,targetFolder)>
<#assign nodeType = StringUtils.makeValidIdentifier(nodeType)>
module ${nodeType} {
    parameters:
        @display("i=abstract/router_vs");
    gates:
        inout ${gateName}[];
}
</#if>

<#if !NedUtils.isVisibleType(channelType,targetFolder)>
<#assign channelType = StringUtils.makeValidIdentifier(channelType)>
channel ${channelType} extends ned.DatarateChannel {
    parameters:
        int cost = default(0);
}
</#if>

<#assign numNodes = numNodes?number>
<#assign seed = seed?number>

<#assign topo = LangUtils.newInstance("Nearest3")>
<@do topo.setNodes(numNodes) !/>
<@do topo.setSeed(seed) !/>

<#assign graph = topo.generate()>

<#assign nodeX = graph.nodeX>
<#assign nodeY = graph.nodeY>
<#assign numEdges = graph.numEdges>
<#assign edgeSrc = graph.edgeSrc>
<#assign edgeDest = graph.edgeDest>

//
// Generated network (${numNodes} nodes, seed=${seed})
//	
<#assign networkKeyword = iif(wizardType=="compoundmodule", "module", "network")>
${networkKeyword} ${targetTypeName} {
    submodules:
<#list 0..numNodes-1 as i>
  <#if addCoordinates>
        node${i} : ${nodeType} { @display("p=${nodeX[i]},${nodeY[i]}"); };
  <#else>
        node${i} : ${nodeType};
  </#if>
</#list>
    connections:
<#list 0..numEdges-1 as i>
        node${edgeSrc[i]}.${gateName}++ <--> ${channelType} <--> node${edgeDest[i]}.${gateName}++;
</#list>
}
