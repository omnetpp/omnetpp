<#include "main.fti">
<@setoutput file=srcFolder+"/Sink.ned"/>
${bannerComment}

<#if srcPackage!="">package ${srcPackage};</#if>

//
// Consumes received messages and collects statistics
//
simple Sink
{
    parameters:
        @display("i=block/sink");
    gates:
        input in;
}

