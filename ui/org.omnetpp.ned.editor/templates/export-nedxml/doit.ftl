<#-- look up NED type -->
<#assign nedTypeInfoSet = nedResources.getNedTypesFromAllProjects(nedType)>
<#if nedTypeInfoSet?size==0>
  <#stop "No such NED type: ${nedType}">
</#if>  

<#-- if there are more than one such types (coming from different projects), pick one -->
<#list nedTypeInfoSet as i>
  <#assign nedTypeInfo = i>
  <#break>
</#list>

<#-- generate XML from its NedElement tree -->
<#assign nedElementTree = nedTypeInfo.getNedElement()>
<#assign NedTreeUtil = classes["org.omnetpp.ned.model.NedTreeUtil"]>
<#assign xmlText = NedTreeUtil.generateXML(nedElementTree, false, 2)>

<#-- save (TODO: ask permission to overwrite) -->
<@do FileUtils.createFile(fileName, xmlText)!/>
