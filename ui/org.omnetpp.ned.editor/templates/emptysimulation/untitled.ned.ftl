<#if wizardType=="simulation">
  <@setoutput file=simulationFolderName+"/"+simulationName+".ned"/>
  <#assign networkName=simulationName>
<#else>
  <@setoutput file=newFileName?default("")/>
  <#assign networkName=nedTypeName?default("Untitled")>
</#if>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>

//
// TODO Auto-generated network
//
network ${networkName} 
{
}