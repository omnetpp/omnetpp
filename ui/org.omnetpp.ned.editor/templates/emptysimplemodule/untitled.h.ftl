<#assign hFileName=targetFileName?default("")?replace("\\.[a-z]*$",".h", "ri")>
<@setoutput path=hFileName/>
${bannerComment}

#ifndef __${PROJECTNAME}_${targetTypeName?upper_case}_H_
#define __${PROJECTNAME}_${targetTypeName?upper_case}_H_

#include <omnetpp.h>

<#if namespaceName!="">namespace ${namespaceName} {</#if>

/**
 * TODO - Generated class
 */
class ${targetTypeName} : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

<#if namespaceName!="">} //namespace</#if>

#endif


