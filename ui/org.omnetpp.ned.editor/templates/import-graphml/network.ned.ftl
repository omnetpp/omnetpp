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
    connections allowunconnected:
}
</#if>

<#if channelTypeSupplied && !channelTypeExists>
channel ${channelType} extends ned.DatarateChannel {
    parameters:
        int cost = default(0);
}
</#if>

<#-- We regard all data elements within nodes/edges as module/channel parameters -->
<#function getParameters doc nodeOrEdge separator>
  <#if !importDisplayString && !importParams>
    <#return "">
  </#if>
  <#local result = "">
  <#local displayX = "">
  <#local displayY = "">
  <#local dataList = nodeOrEdge.getElementsByTagName("data")>
  <#if dataList.getLength()==0>
    <#return ""><#-- important, as #list 0..-1 counts down instead of skipping... -->
  </#if>
  <#list 0..dataList.getLength()-1 as i>
    <#local data = dataList.item(i)>
    <#local keyId = data.getAttribute("key")>
    <#local key = doc.getElementById(keyId)>
    <#local attrName = key.getAttribute("attr.name")>
    <#local attrType = key.getAttribute("attr.type")>
    <#local value = data.getTextContent()>
    <#if importDisplayString && attrName == "x">
       <#local displayX = value>
    <#elseif importDisplayString && attrName == "y">
       <#local displayY = value>
    <#elseif importParams>
      <#if attrType=="string"><#local value = '"' + value + '"'></#if>
      <#local result = result + attrName + " = " + value + ";" + separator>
    </#if>
  </#list>
  <#if displayX != "" || displayY != "">
    <#local result = '@display("p=' + displayX + ',' + displayY + '");' + separator + result>
  </#if>
  <#return result?trim>
</#function>

<#-- read the file -->
<#assign doc = FileUtils.readXMLFile(fileName)>
<#-- make doc.getElementById() work -->
<#assign all = doc.getElementsByTagName("*")>
<#list 0..all.getLength()-1 as i>
  <#assign element = all.item(i)>
  <#if element.hasAttribute("id")>
    <@do element.setIdAttribute("id", true)!/>
  </#if>
</#list>
<#-- check if we can support this doc -->
<#assign graphs = doc.getElementsByTagName("graph")>
<#if (graphs.getLength()==0)>
  <#stop "Document contains no graph element.">
</#if>
<#if (graphs.getLength()>1)>
  <#stop "Document contains nested graphs, which are not supported by this importer.">
</#if>
<#if graphs.item(0).hasAttribute("edgedefault") && graphs.item(0).getAttribute("edgedefault")=="directed">
  <#stop "Document contains a directed graph, which is not supported by this importer.">
</#if>
<#if doc.getElementsByTagName("hyperedge").getLength()!=0>
  <#stop "Document contains a hyperedges, which are not supported by this importer.">
</#if>
<#-- now the real thing -->
<#assign nodes = doc.getElementsByTagName("node")>
<#assign edges = doc.getElementsByTagName("edge")>
//
// Network generated from ${fileName}
//
<#if wizardType=="compoundmodule"><#assign keyword="module"><#else><#assign keyword="network"></#if>
${keyword} ${targetTypeName} {
    submodules:
<#if nodes.getLength()!=0>
  <#list 0..nodes.getLength()-1 as i>
    <#assign node = nodes.item(i)>
    <#assign params = getParameters(doc, node, "\n            ")>
    <#if params?trim=="">
        ${node.getAttribute("id")}: ${nodeType};
    <#else>
        ${node.getAttribute("id")}: ${nodeType} {
            ${params}
        }
    </#if>
  </#list>
</#if>
    connections:
<#if edges.getLength()!=0>
  <#list 0..edges.getLength()-1 as i>
    <#assign edge = edges.item(i)>
    <#if edge.hasAttribute("directed") && edge.getAttribute("directed")=="true">
      <#stop "Document contains a directed edge, which is not supported by this importer.">
    </#if>
    <#assign params = getParameters(doc, node, " ")>
    <#if !channelTypeSupplied>
        ${edge.getAttribute("source")}.${gateName}++ <--> ${edge.getAttribute("target")}.${gateName}++;
    <#elseif params?trim=="">
        ${edge.getAttribute("source")}.${gateName}++ <--> ${channelType} <--> ${edge.getAttribute("target")}.${gateName}++;
    <#else>
        ${edge.getAttribute("source")}.${gateName}++ <--> ${channelType} {${params}} <--> ${edge.getAttribute("target")}.${gateName}++;
    </#if>
  </#list>
</#if>
}

