<@setoutput path=targetMainFile?default("")/>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>

<#assign NodeModel = classes["freemarker.ext.dom.NodeModel"]>
<#assign fileLocation = FileUtils.asFile(fileName).getLocation().toString()>
<#assign doc = NodeModel.parse(FileUtils.asExternalFile(FileUtils.asExternalFile(fileLocation)))>

//
// Network generated from ${fileName}
//
<#if wizardType=="compoundmodule"><#assign keyword="module"><#else><#assign keyword="network"></#if>
${keyword} ${targetTypeName} {
    submodules:
<#assign idToName = LangUtils.newMap()>
<#list doc.pnml.net.place as place>
    <#assign name = StringUtils.makeValidIdentifier(place.name.text)>
    <@do idToName.put(place.@id?string, name)!/>
    <#assign pos = place.graphics.position>
    <#assign initialMarking = place.initialMarking.text>
        ${name}: Place { 
            <#if pos?size!=0>@display("p=${pos.@x},${pos.@y}");</#if> 
            <#if initialMarking?size!=0>numTokens = ${initialMarking};</#if> 
        }
</#list>
<#list doc.pnml.net.transition as transition>
    <#assign name = StringUtils.makeValidIdentifier(transition.name.text)>
    <@do idToName.put(transition.@id?string, name)!/>
    <#assign pos = transition.graphics.position>
        ${name}: Transition { 
            <#if pos?size!=0>@display("p=${pos.@x},${pos.@y}");</#if> 
        }
</#list>
    connections:
<#list doc.pnml.net.arc as arc>
        ${idToName[arc.@source]}.out++ --> ${idToName[arc.@target]}.in++; 
</#list>
}

