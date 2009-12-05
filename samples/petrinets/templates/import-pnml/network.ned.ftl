<@setoutput path=targetMainFile?default("")/>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>

<#-- import place/transition/arc NED types -->
<#assign arcTypeSupplied = arcType!="">
import ${placeType};
import ${transitionType};
<#if arcType!="">import ${arcType};</#if>
<#if inhibitorArcType!="">import ${inhibitorArcType};</#if>

<#assign placeType = placeType?replace("^.*\\.", "", "r")>
<#assign transitionType = transitionType?replace("^.*\\.", "", "r")>
<#assign arcType = arcType?replace("^.*\\.", "", "r")>
<#assign inhibitorArcType = inhibitorArcType?replace("^.*\\.", "", "r")>

<#assign arcSpec = iif(arcTypeSupplied, " " + arcType + " -->", "")>

<#-- actual import code begins here -->
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
<#list doc.pnml.net[0].place as place>
    <#assign name = StringUtils.makeValidIdentifier(place.name.text)>
    <@do idToName.put(place.@id?string, name)!/>
    <#assign pos = place.graphics.position>
    <#assign initialMarking = place.initialMarking.text>
        ${name}: ${placeType} { 
            <#if pos?size!=0>@display("p=${pos.@x},${pos.@y}");</#if> 
            <#if initialMarking?size!=0>numInitialTokens = ${initialMarking};</#if> 
        }
</#list>
<#list doc.pnml.net[0].transition as transition>
    <#assign name = StringUtils.makeValidIdentifier(transition.name.text)>
    <@do idToName.put(transition.@id?string, name)!/>
    <#assign pos = transition.graphics.position>
        ${name}: ${transitionType} { 
            <#if pos?size!=0>@display("p=${pos.@x},${pos.@y}");</#if> 
        }
</#list>
    connections:
<#list doc.pnml.net[0].arc as arc>
        ${idToName[arc.@source]}.out++ -->${arcSpec} ${idToName[arc.@target]}.in++; 
</#list>
}

