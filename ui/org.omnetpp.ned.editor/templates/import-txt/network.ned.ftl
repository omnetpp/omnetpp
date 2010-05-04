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

<#-- read the file, and build data structure from it: -->
<#--   nodes: a list, where each item is a map with keys "name", "x", "y" -->
<#--   links: a list, where each item is a map with keys "src", "dest", "bw", "cost" -->
<#-- In this sample code, we do no cross-checks between nodes[] and links[], and -->
<#-- assume that all nodes have coordinates, and all links have both data rate and cost specified. -->
<#compress>
<#assign lines = FileUtils.readLineOrientedTextFile(fileName)>
<#assign nodes = LangUtils.newList()>
<#assign links = LangUtils.newList()>
<#assign readingWhat = "">
<#list lines as line>
  <#if line.startsWith("#") || line.trim().length()==0>
    <#-- comment line or blank line, ignore -->
  <#elseif line.startsWith("NODES:")>
    <#assign readingWhat = "NODES">
  <#elseif line.startsWith("LINKS:")>
    <#assign readingWhat = "LINKS">
  <#elseif readingWhat == "NODES">
    <#assign fields = line.split("[ \t]+")>
    <#if (fields?size != 3)> <#stop "Node lines are expected to contain exactly 3 items"> </#if>
    <#assign node = LangUtils.newMap()>
    <@do node.put("name", fields[0]) !/>
    <@do node.put("x", fields[1]) !/>
    <@do node.put("y", fields[2]) !/>
    <@do nodes.add(node) !/>
  <#elseif readingWhat == "LINKS">
    <#assign fields = line.split("[ \t]+")>
    <#if (fields?size != 4)> <#stop "Link lines are expected to contain exactly 4 items"> </#if>
    <#assign link = LangUtils.newMap()>
    <@do link.put("src", fields[0]) !/>
    <@do link.put("dest",fields[1]) !/>
    <@do link.put("bw",  fields[2]) !/>
    <@do link.put("cost",fields[3]) !/>
    <@do links.add(link) !/>
  </#if>
</#list>
</#compress>
//
// Network generated from ${fileName}
//
<#if wizardType=="compoundmodule"><#assign keyword="module"><#else><#assign keyword="network"></#if>
${keyword} ${targetTypeName} {
    submodules:
<#list nodes as node>
        ${node["name"]}: ${nodeType} { @display("p=${node["x"]},${node["y"]}"); }
</#list>
    connections:
<#list links as link>
  <#if !channelTypeSupplied>
        ${link["src"]}.${gateName}++ <--> ${link["dest"]}.${gateName}++;
  <#else>
        ${link["src"]}.${gateName}++ <--> ${channelType} { datarate=${link["bw"]}bps; cost=${link["cost"]}; } <--> ${link["dest"]}.${gateName}++;
  </#if>
</#list>
}

