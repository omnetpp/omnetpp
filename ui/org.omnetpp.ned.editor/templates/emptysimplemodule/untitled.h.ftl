<@setoutput file=newFileName?default("")/>
${bannerComment}

#ifndef ${GUARD}
#define ${GUARD}

#include <omnetpp.h>

<#if namespace!="">namespace ${namespace} {</#if>

/**
 * TODO - Generated class
 */
class ${nedTypeName} : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

<#if namespace!="">} //namespace</#if>

#endif


