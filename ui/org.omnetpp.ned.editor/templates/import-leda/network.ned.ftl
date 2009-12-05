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

<#if !nodeTypeExists>
module ${nodeType} {
    parameters:
        @display("i=abstract/router_s");
    gates:
        inout ${gateName}[];
}
</#if>

<#if channelTypeSupplied && !channelTypeExists>
channel ${channelType} extends ned.DatarateChannel {
    parameters:
        int cost = default(0);
}
</#if>

<#-- read the file, and build data structure from it: -->
<#--   nodes: a list, where each item is a map with keys "name", "x", "y" -->
<#--   links: a list, where each item is a map with keys "src", "dest", "bw", "cost" -->

<#-- read file and split to lines -->
<#assign lines = FileUtils.readLineOrientedTextFile(fileName)>
<#assign lineIndex = 0>

<#function getLine>
  <#if (lineIndex >= lines?size)>
    <#stop "End of file reached prematurely">
  </#if>
  <#assign line = lines[lineIndex]>
  <#assign lineIndex = lineIndex + 1>
  <#if line?trim=="" || line?starts_with("#")>
    <#return getLine()>
  <#else>
    <#return line>
  </#if>
</#function>

<#-- process header -->
<#assign headerLine = getLine()?trim>
<#if headerLine != "LEDA.GRAPH">
  <#stop "Not a LEDA native file: first significant line is not LEDA.GRAPH">
</#if>
<#assign nodeParamTypes = getLine()?trim>
<#assign edgeParamTypes = getLine()?trim>
<#assign isDirectedFlag = getLine()?trim>
<#if isDirectedFlag != "-1" && isDirectedFlag != "-2">
  <#stop "Wrong file format: fourth line of header must be either -1 or -2">
</#if>
<#if isDirectedFlag != "-2">
  <#stop "This wizard only supports undirected graphs (fourth line of header should be '-2')">
</#if>

<#-- process nodes section -->
<#assign numNodes = getLine()?trim?number>
<#assign nodes = LangUtils.newList()>
<#list 1..numNodes as i>
  <#assign nodeLine = getLine()?trim>
  <#assign node = LangUtils.newMap()>
  <@do node.put("name", "node"+i) !/>
  <@do nodes.add(node)! />
  <#assign rest = StringUtils.substringAfter(nodeLine,"|{")>
  <#assign params = StringUtils.substringBefore(rest,"}|")>
  <#-- todo: extract parameters if needed -->
</#list>

<#-- process edges section -->
<#assign numEdges = getLine()?trim?number>
<#assign edges = LangUtils.newList()>
<#list 1..numEdges as i>
  <#assign edgeLine = getLine()?trim>
  <#assign fields = StringUtils.substringBefore(edgeLine, "|{").split("[ \t]+")>
  <#if (fields?size != 3)> 
    <#stop "Edge lines are expected to contain 3 items plus parameters"> 
  </#if>
  <#assign edge = LangUtils.newMap()>
  <@do edge.put("src", fields[0]) !/>
  <@do edge.put("dest",fields[1]) !/>
  <@do edges.add(edge) !/>
  <#assign rest = StringUtils.substringAfter(edgeLine,"|{")>
  <#assign params = StringUtils.substringBefore(rest,"}|")>
  <#-- todo: extract parameters if needed -->
</#list>
//
// Network generated from ${fileName}
//
<#if wizardType=="compoundmodule"><#assign keyword="module"><#else><#assign keyword="network"></#if>
${keyword} ${targetTypeName} {
    submodules:
<#list nodes as node>
        ${node["name"]}: ${nodeType};
</#list>
    connections:
<#list edges as edge>
  <#if !channelTypeSupplied>
        node${edge["src"]}.${gateName}++ <--> node${edge["dest"]}.${gateName}++;
  <#else>
        node${edge["src"]}.${gateName}++ <--> ${channelType} <--> node${edge["dest"]}.${gateName}++;
  </#if>
</#list>
}
