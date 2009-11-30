<#noparse>
<@setoutput path=targetFileName?default("")/>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>

module Module {
    submodules:
<#list submoduleNames?split(",") as name>
        <#assign typeVar = name?trim + "Type">
        ${name?trim} : ${typeVar?eval?default("Unknown")};
</#list>
}
</#noparse>
