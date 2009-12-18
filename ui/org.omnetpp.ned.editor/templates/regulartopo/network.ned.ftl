<@setoutput path=targetMainFile?default("")/>
${bannerComment}

// Created: ${date} for project ${rawProjectName}

<#if nedPackageName!="">package ${nedPackageName};</#if>

<#-- convert variables to numbers (some widgets return them as strings)-->
<#assign treeK = treeK?number>
<#assign treeLevels = treeLevels?number>
<#assign nodes = nodes?number>
<#assign columns = columns?number>
<#assign rows = rows?number>
<#assign networkKeyword = iif(wizardType=="compoundmodule", "module", "network")>

<#assign nodeTypeExists = NedUtils.isVisibleType(nodeType,targetFolder)>
<#if nodeTypeExists>
import ${nodeType};
  <#assign nodeType = nodeType?replace("^.*\\.", "", "r")>
<#else>
  <#assign nodeType = StringUtils.makeValidIdentifier(nodeType)>
</#if>

<#if star || wheel>
  <#assign hubTypeExists = NedUtils.isVisibleType(hubType,targetFolder)>
  <#if hubTypeExists>
import ${hubType};
    <#assign hubType = hubType?replace("^.*\\.", "", "r")>
  <#else>
    <#assign hubType = StringUtils.makeValidIdentifier(hubType)>
  </#if>
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

<#-- pull in the correct template to do the actual work -->
<#if star || ring || wheel>
   <#include "wheel.ned.fti">
<#elseif grid || torus>
   <#include "gridtorus.ned.fti">
<#elseif bintree || ktree>
   <#if bintree> <#assign treeK=2> </#if>
   <#include "ktree.ned.fti">
<#elseif fullgraph>
   <#include "fullgraph.ned.fti">
</#if>
