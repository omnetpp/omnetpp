<#if makeSrcAndSimulationsFolders>
  <@setoutput file="simulations/SourceSink.ned"/>
<#else>
  <#assign simulationsPackage=rootPackage>
</#if>
${bannerComment}

<#if simulationsPackage!="">
package ${simulationsPackage};
</#if>

<#if rootPackage!=simulationsPackage>
  <#if rootPackage==""> 
import Source;
import Sink;
  <#else>
import ${rootPackage}.Source;
import ${rootPackage}.Sink;
  </#if>
</#if>

//
// Sample network, consisting of a source and a sink.
//
network SourceSink
{
    submodules:
        source: Source;
        sink: Sink;
    connections:
        source.out --> sink.in;
}


