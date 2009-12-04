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

<#assign doc = FileUtils.readXMLFile(fileName)>
<#assign nodes = doc.getElementsByTagName("node")>
<#assign links = doc.getElementsByTagName("link")>
//
// Network generated from ${fileName}
//
<#if wizardType=="compoundmodule"><#assign keyword="module"><#else><#assign keyword="network"></#if>
${keyword} ${targetTypeName} {
    submodules:
<#list 0..nodes.getLength()-1 as i>
  <#assign node = nodes.item(i)>
  <#if node.hasAttribute("x") && node.hasAttribute("y")>
        ${node.getAttribute("name")}: ${nodeType} { @display("p=${node.getAttribute("x")},${node.getAttribute("y")}"); }
  <#else>
        ${node.getAttribute("name")}: ${nodeType};
  </#if>
</#list>
    connections:
<#list 0..links.getLength()-1 as i>
  <#assign link = links.item(i)>
  <#if !channelTypeSupplied>
        ${link.getAttribute("src")}.${gateName}++ <--> ${link.getAttribute("dest")}.${gateName}++;
  <#elseif link.hasAttribute("bw") || link.hasAttribute("cost")>
        ${link.getAttribute("src")}.${gateName}++ <--> ${channelType} { <#if link.hasAttribute("bw")>datarate=${link.getAttribute("bw")}bps; </#if><#if link.hasAttribute("cost")>cost=${link.getAttribute("cost")}; </#if>} <--> ${link.getAttribute("dest")}.${gateName}++;
  <#else>
        ${link.getAttribute("src")}.${gateName}++ <--> ${channelType} <--> ${link.getAttribute("dest")}.${gateName}++;
  </#if>
</#list>
}

