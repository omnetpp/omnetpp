<#assign hFileName=newFileName?default("")?replace("\\.[a-z]*$",".h", "ri")>
<@setoutput file=hFileName/>
${bannerComment}

#ifndef __${PROJECTNAME}_${nedTypeName?upper_case}_H_
#define __${PROJECTNAME}_${nedTypeName?upper_case}_H_

#include <omnetpp.h>

<#if namespaceName!="">namespace ${namespaceName} {</#if>

/**
 * TODO - Generated class
 */
class ${nedTypeName} : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

<#if namespaceName!="">} //namespace</#if>

#endif


