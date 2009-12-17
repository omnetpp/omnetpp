<#if wizardType=="project" || wizardType=="simulation">
[General]
network = ${targetTypeName}

*.source.interArrivalTime = ${interArrivalTime}
  <#if population!="">
*.source.numJobs = ${population}
  </#if>
  <#if numServers=="1">
*.server.serviceTime = ${serviceTime}
    <#if systemCapacity!="">
*.server.capacity = ${systemCapacity}
    </#if>
    <#if discipline!="FIFO">
*.server.fifo = false
    </#if>
  <#elseif numServers=="">
*.server.delay =  ${serviceTime}
  <#else>
*.server[*].serviceTime = ${serviceTime}
    <#if parametric>
${targetTypeName}.numServers = ${numServers}
    </#if>
    <#if systemCapacity!="">
*.queue.capacity = ${systemCapacity} - sizeof(server)
    </#if>
  </#if>
</#if>
