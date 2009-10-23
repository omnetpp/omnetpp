${bannercomment}

<#if simulationspackage!="">package ${simulationspackage};</#if>
<#if rootpackage!="">
import ${rootpackage}.Source;
import ${rootpackage}.Sink;
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


