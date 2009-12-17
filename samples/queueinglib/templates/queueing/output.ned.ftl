<@setoutput path=targetMainFile?default("network.ned")/>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>

import org.omnetpp.queueing.Source;
import org.omnetpp.queueing.Queue;
import org.omnetpp.queueing.Sink;
import org.omnetpp.queueing.PassiveQueue;
import org.omnetpp.queueing.Server;
import org.omnetpp.queueing.Delay;

//
// TODO Generated network
//
network ${targetTypeName}
{
<#if numServers!="1" && numServers!="">
    parameters:
        int numServers = default(1);
</#if>
    submodules:
        source: Source;
<#if numServers="1">
        server: Queue;
<#elseif numServers="">
        server: Delay;
<#else>
        queue: PassiveQueue;
        server[numServers]: Server;
</#if>
        sink: Sink;
    connections:
<#if numServers=="1" || numServers=="">
        source.out --> server.in++;
        server.out --> sink.in++;
<#else>
        source.out --> queue.in++;
        for i = 0..sizeof(server)-1 {
	        queue.out++ --> server[i].in++;
	    server[i].out --> sink.in++;
        }
</#if>
}
