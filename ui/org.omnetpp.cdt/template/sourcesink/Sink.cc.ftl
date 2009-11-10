<#include "main.fti">
<@setoutput file=srcFolder+"/Sink.cc"/>
${bannerComment}

#include "Sink.h"

<#if namespace!="">namespace ${namespace} {</#if>

Define_Module(Sink);

void Sink::initialize()
{
    lastArrival = simTime();
    iaTimeHistogram.setName("interarrival times");
    arrivalsVector.setName("arrivals");
    arrivalsVector.setInterpolationMode(cOutVector::NONE);
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t d = simTime() - lastArrival;
    EV << "Received " << msg->getName() << endl;
    delete msg;

    iaTimeHistogram.collect(d);
    arrivalsVector.record(1);

    lastArrival = simTime();
}

void Sink::finish()
{
    recordStatistic(&iaTimeHistogram);
}

<#if namespace!="">}; // namespace</#if>

