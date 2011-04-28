<@setoutput path=srcFolder+"/Sink.ned"/>
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

