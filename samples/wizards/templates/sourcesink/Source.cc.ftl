${bannerComment}

#include "Source.h"

<#if namespace!="">namespace ${namespace} {</#if>

Define_Module(Source);

Source::Source()
{
    timerMessage = NULL;
}

Source::~Source()
{
    cancelAndDelete(timerMessage);
}

void Source::initialize()
{
    timerMessage = new cMessage("timer");
    scheduleAt(simTime(), timerMessage);
}

void Source::handleMessage(cMessage *msg)
{
    ASSERT(msg==timerMessage);

    cMessage *job = new cMessage("job");
    send(job, "out");

    scheduleAt(simTime()+par("sendInterval").doubleValue(), timerMessage);
}

<#if namespace!="">}; // namespace</#if>

