<#assign hFileName=targetFileName?default("")?replace("\\.[a-z]*$",".h", "ri")>
<#assign ccFileName=targetFileName?default("")?replace("\\.[a-z]*$",".cc", "ri")>
<@setoutput path=ccFileName/>
${bannerComment}

#include "${hFileName}"

<#if namespaceName!="">namespace ${namespaceName} {</#if>

Define_Module(${targetTypeName});

void ${targetTypeName}::initialize()
{
    // TODO - Generated method body
}

void ${targetTypeName}::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

<#if namespaceName!="">} //namespace</#if>

