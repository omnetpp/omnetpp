<@setoutput path=srcFolder+"/Source.h"/>
${bannerComment}

#ifndef __${PROJECTNAME}_SOURCE_H
#define __${PROJECTNAME}_SOURCE_H

#include <omnetpp.h>

using namespace omnetpp;

<#if namespace!="">namespace ${namespace} {</#if>

/**
 * Generates messages; see NED file for more info.
 */
class Source : public cSimpleModule
{
  private:
    cMessage *timerMessage;

  public:
     Source();
     virtual ~Source();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

<#if namespace!="">}; // namespace</#if>

#endif

