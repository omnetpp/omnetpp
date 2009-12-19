<#noparse>

<#-- print all template variables -->
<#list creationContext.getVariables()?keys?sort as key>
   <#assign value = creationContext.getVariable(key)>
   ${key} ==> ${LangUtils.toString(value)} (${LangUtils.getClass(value).getSimpleName()})
</#list>

</#noparse>

