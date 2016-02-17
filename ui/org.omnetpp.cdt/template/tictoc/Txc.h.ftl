<@setoutput path=srcFolder+"/Txc.h"/>
${bannerComment}

#ifndef __${PROJECTNAME}_TCX_H
#define __${PROJECTNAME}_TCX_H

#include <omnetpp.h>

using namespace omnetpp;

<#if namespace!="">namespace ${namespace} {</#if>

/**
 * Implements the Txc simple module. See the NED file for more information.
 */
class Txc : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

<#if namespace!="">}; // namespace</#if>

#endif

