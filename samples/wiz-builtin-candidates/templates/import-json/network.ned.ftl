<@setoutput file=newFileName?default("")/>
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

<#assign data = FileUtils.readJSONFile(fileName)>
<#assign nodes = data.get("nodes")>
<#assign links = data.get("links")>
//
// Network generated from ${fileName}
//
network ${nedTypeName} {
    submodules:
<#list nodes.keySet().toArray()?sort as node>
  <#assign nodeAttrs = nodes[node]>
  <#if nodeAttrs.containsKey("x") && nodeAttrs.containsKey("y")>
        ${node}: ${nodeType} { @display("p=${nodeAttrs["x"]},${nodeAttrs["y"]}"); }
  <#else>
        ${node}: ${nodeType};
  </#if>
</#list>
    connections:
<#list links as link>
  <#if link.containsKey("bw") || link.containsKey("cost")>
        ${link["from"]}.g++ <--> ${channelType} { <#if link.containsKey("bw")>datarate=${link["bw"]}bps; </#if><#if link.containsKey("cost")>cost=${link["cost"]}; </#if>} <--> ${link["to"]}.g++;
  <#else>
        ${link["from"]}.g++ <--> ${channelType} <--> ${link["to"]}.g++;
  </#if>
</#list>
}

