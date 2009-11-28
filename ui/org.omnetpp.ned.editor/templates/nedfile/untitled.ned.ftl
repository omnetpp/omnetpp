<@setoutput path=targetFileName?default("")/>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>

<#if !isEmpty>
//
// TODO auto-generated type
//
<#if isSimple>
simple ${targetTypeName}
{
}
<#elseif isModule>
module ${targetTypeName}
{
}
<#elseif isNetwork>
network ${targetTypeName}
{
}
<#elseif isChannel>
channel ${targetTypeName}
{
}
<#elseif isModuleinterface>
moduleinterface ${targetTypeName}
{
}
<#elseif isChannelinterface>
channelinterface ${targetTypeName}
{
}
<#else>
  <#stop "no NED type selected">
</#if>
</#if>

