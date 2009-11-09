${bannerComment}

<#if simulationsPackage!="">package ${simulationsPackage};</#if>
<#if rootPackage!="">
import ${rootPackage}.Source;
import ${rootPackage}.Sink;
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


