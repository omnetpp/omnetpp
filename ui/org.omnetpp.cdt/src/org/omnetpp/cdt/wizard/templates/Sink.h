${bannercomment}

#ifndef __${PROJECTNAME}_SINK_H
#define __${PROJECTNAME}_SINK_H

#include <omnetpp.h>

#if($namespace!="")namespace ${namespace} {#end

/**
 * Message sink; see NED file for more info.
 */
class Sink : public cSimpleModule
{
  private:
    // state
    simtime_t lastArrival;

    // statistics
    cDoubleHistogram iaTimeHistogram;
    cOutVector arrivalsVector;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#if($namespace!="")}; // namespace#end

#endif


