<#assign hFileName=newFileName?default("")?replace("\\.[a-z]*$",".h", "ri")>
<#assign ccFileName=newFileName?default("")?replace("\\.[a-z]*$",".cc", "ri")>
<@setoutput file=ccFileName/>
${bannerComment}

#include "${hFileName}"

<#if namespaceName!="">namespace ${namespaceName} {</#if>

Define_Module(${nedTypeName});

void ${nedTypeName}::initialize()
{
    // TODO - Generated method body
}

void ${nedTypeName}::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

<#if namespaceName!="">} //namespace</#if>

