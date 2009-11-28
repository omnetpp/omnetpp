<#include "main.fti">
<@setoutput path=simulationsFolder+"/SourceSink.ned"/>
${bannerComment}

<#if simulationsPackage!="">package ${simulationsPackage};</#if>

<#if srcPackage!=simulationsPackage>
  <#if srcPackage=="">
import Source;
import Sink;
  <#else>
import ${srcPackage}.Source;
import ${srcPackage}.Sink;
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


