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
<#if parametric>
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
        server[numServers]: Server {
            @display("p=,,col");
        }
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
<#else>
network ${targetTypeName}
{
  <#if numServers==""> <#assign midY = 50> <#else> <#assign midY = (numServers?number-1) * 30 + 50 > </#if>
  <#assign x = 50>
    submodules:
        source: Source {
            @display("p=${x},${midY}");
            <#assign x = x + 100>
        }
  <#if numServers="1">
        server: Queue {
            @display("p=${x},${midY}");
            <#assign x = x + 100>
        }
  <#elseif numServers="">
        server: Delay {
            @display("p=${x},${midY}");
            <#assign x = x + 100>
        }
  <#else>
        queue: PassiveQueue {
            @display("p=${x},${midY}");
            <#assign x = x + 100>
        };
    <#list 0..numServers?number-1 as i>
        server${i}: Server {
            @display("p=${x},${i*60+50}");
        }
    </#list>
    <#assign x = x + 100>
  </#if>
        sink: Sink {
            @display("p=${x},${midY}");
        }
    connections:
  <#if numServers=="1" || numServers=="">
        source.out --> server.in++;
        server.out --> sink.in++;
  <#else>
        source.out --> queue.in++;
    <#list 0..numServers?number-1 as i>
        queue.out++ --> server${i}.in++;
        server${i}.out --> sink.in++;
	</#list>
  </#if>
}
</#if>
