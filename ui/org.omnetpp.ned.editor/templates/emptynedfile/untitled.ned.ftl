<@setoutput path=targetFileName?default("")/>
<#-- needed to prevent an empty file -->
<#if bannerComment==""><#assign bannerComment="//\n// TODO copyright info\n//\n"></#if>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>
