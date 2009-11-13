<@setoutput file=newFileName?default("")/>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>

<#if !isEmpty>
//
// TODO auto-generated type
//
<#if isSimple>
simple ${nedTypeName}
{
}
<#elseif isModule>
module ${nedTypeName}
{
}
<#elseif isNetwork>
network ${nedTypeName}
{
}
<#elseif isChannel>
channel ${nedTypeName}
{
}
<#elseif isModuleinterface>
moduleinterface ${nedTypeName}
{
}
<#elseif isChannelinterface>
channelinterface ${nedTypeName}
{
}
<#else>
  <#stop "no NED type selected">
</#if>
</#if>

