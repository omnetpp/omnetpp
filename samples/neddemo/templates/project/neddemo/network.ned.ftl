//
// Created on ${date} for project ${projectname}
//

simple Node {
    gates:
        inout g[];
}

TODO handle: staticNED flag
TODO handle: generateCoordinates

<#-- convert variables to numbers (some widgets return them as strings)-->
<#assign treeK = treeK?number>
<#assign treeLevels = treeLevels?number>
<#assign nodes = nodes?number>
<#assign columns = columns?number>
<#assign rows = rows?number>

<#-- pull in the correct template to do the actual work -->
<#if star>
   <#include "star.inc.ftl">
<#elseif ring>
   <#include "ring.inc.ftl">
<#elseif grid || torus>
   <#include "gridtorus.inc.ftl">
<#elseif bintree || ktree>
   <#include "ktree.inc.ftl">
</#if>
