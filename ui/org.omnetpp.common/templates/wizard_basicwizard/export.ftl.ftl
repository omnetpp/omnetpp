<#if supportExport>
<#noparse>
<#-- collect output in the 'content' variable -->
<#assign content>
NED types in open projects (exported by ${author}, on ${date}):
<#list nedResources.getNedTypesFromAllProjects() as nedTypeInfo>
  ${nedTypeInfo.getName()} (${nedTypeInfo.getFullyQualifiedName()}), in ${nedTypeInfo.getNedFile().getFullPath().toString()})  
</#list>
</#assign>

<#-- save it -->
<@do FileUtils.createFile(fileName, content)!/>
</#noparse>
</#if>
