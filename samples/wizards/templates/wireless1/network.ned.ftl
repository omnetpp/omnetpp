<@setoutput file=newNedFile?default("")/>
//
// Created: ${date} for project ${projectName}
//

simple WirelessNode {
    string routingProtocol;
    @display("i=misc/node_vs");
}

<#assign numNodes = numNodes?number>
<#assign gridColumns = gridColumns?number>
<#assign gridHSpacing = gridHSpacing?number>
<#assign gridVSpacing = gridVSpacing?number>
//
// Generated network: ${numNodes} nodes, ${placement} placement, ${routingProtocol} routing
//
network ${ProjectName} {
    parameters:
        //TODO: node*.routingProtocol = "${routingProtocol}";
     submodules:
<#list 0..numNodes-1 as i>
        node${i} : WirelessNode {
<#if placement=="grid">
            @display("p=${50+(i%gridColumns)*gridHSpacing},${50 + (i/gridColumns)?int * gridVSpacing}");
<#elseif placement=="random">
            @display("p=${Math.random()*600},${Math.random()*400}");
<#else>
</#if>
         }
</#list>
}
