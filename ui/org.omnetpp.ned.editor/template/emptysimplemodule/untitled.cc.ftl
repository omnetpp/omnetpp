<@setoutput file=newFileName?default("")/>
${bannerComment}

#include "${nedTypeName}.h"

<#if namespace!="">namespace ${namespace} {</#if>

Define_Module(${nedTypeName});

void ${nedTypeName}::initialize()
{
    // TODO - Generated method body
}

void ${nedTypeName}::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

<#if namespace!="">} //namespace</#if>

