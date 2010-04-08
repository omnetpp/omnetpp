<@setoutput path=targetFileName?default("")/>
${bannerComment}
<#if bannerComment=="">//
// TODO documentation
//</#if>

<#if nedPackageName!="">package ${nedPackageName};</#if>
