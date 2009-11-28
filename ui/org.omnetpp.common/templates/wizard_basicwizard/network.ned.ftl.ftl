<#noparse>
<@setoutput path=targetMainFile?default("")/>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>

<#-- convert variables to numbers (some widgets return them as strings) -->
<#assign columns = columns?number>
<#assign rows = rows?number>

//
// TODO Generated module
//
module Node {
    parameters:
        @display("i=misc/node_vs");
    gates:
        inout up;
        inout down;
        inout left;
        inout right;
}

//
// TODO Generated network
//
network ${targetTypeName}
{
    submodules:
<#list 0..rows-1 as i>
  <#list 0..columns-1 as j>
        node_${i}_${j}: Node;
  </#list>

</#list>
    connections allowunconnected:
<#list 0..rows-1 as i>
  <#list 0..columns-1 as j>
        <#if i!=rows-1   >node_${i}_${j}.down <--> node_${i+1}_${j}.up;</#if>
        <#if j!=columns-1>node_${i}_${j}.right <--> node_${i}_${j+1}.left;</#if>
  </#list>
</#list>
}

</#noparse>

