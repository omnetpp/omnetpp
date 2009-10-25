${bannerComment}

<#if rootPackage!="">package ${rootPackage};</#if>

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

