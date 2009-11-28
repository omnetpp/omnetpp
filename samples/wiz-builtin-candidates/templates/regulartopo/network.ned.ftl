<@setoutput path=targetFileName?default("")/>
${bannerComment}

// Created: ${date} for project ${rawProjectName}

<#if nedPackageName!="">package ${nedPackageName};</#if>

<#-- convert variables to numbers (some widgets return them as strings)-->
<#assign treeK = treeK?number>
<#assign treeLevels = treeLevels?number>
<#assign nodes = nodes?number>
<#assign columns = columns?number>
<#assign rows = rows?number>

<#if channelType != "">
  <#assign channelSpec = " " + channelType + " <-->">
<#else>
  <#assign channelSpec = "">
</#if>

<#-- pull in the correct template to do the actual work -->
<#if star>
   <#include "star.ned.fti">
<#elseif ring>
   <#include "ring.ned.fti">
<#elseif grid || torus>
   <#include "gridtorus.ned.fti">
<#elseif bintree || ktree>
   <#if bintree> <#assign treeK=2> </#if>
   <#include "ktree.ned.fti">
</#if>
